/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * NVMC - Non-volatile memory controller
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/nvmc.html?cp=5_1_0_3_2
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/nvmc.html?cp=4_0_0_6_20
 * &
 * UICR - User information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/uicr.html?cp=5_1_0_3_4
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/uicr/doc/uicr.html?cp=4_0_0_4_3_2
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/uicr.network/doc/uicr.network.html?cp=4_0_0_5_3_1
 *
 * Notes for the NVMC:
 *  * The CPU is not stalled while doing flash operations, "while executing from flash"
 *    as we do not know what code would have been in flash or RAM, code is just
 *    assumed only in RAM for simplicity.
 *
 *  * The model will not prevent you from writing too many times (more then n_write)
 *    the same address between erases
 *
 *  * For nrf52s, the spec does not specify how much earlier READYNEXT is,
 *    and for nrf53 it is not mentioned in the documentation,
 *    so this model just sets it when the previous operation is done (together with READY)
 *
 *  * The partial erase timer is fully accurate. Meaning, a partial erase always
 *    takes exactly ERASEPAGEPARTIALCFG ms.
 *
 *  * The instruction cache does not have any kind of model.
 *    The ICACHECNF, IHIT, IMISS registers are not connected to anything,
 *    writes to them are ignored, reads read 0 (or whatever was written to them last)
 *
 *  * Access port protection is not modeled. The UICR can always be erased.
 *
 *  * There is no modeling of the power failure protection
 *
 *  * (nRF53) CONFIGNS and CONFIG have the same abilities (including enabling a partial erase)
 *
 *  * (nRF53) There is no handling or differentiation between secure and non secure areas
 *
 *  * (nRF53) All registers are accessible from all SW (there is no differentiation
 *    between secure and not secure registers)
 *
 *  * (nRF53) CONFIGNS is ignored so far. As we lack an SPU model,
 *    all Flash is assumed to be "secure" from the point of view that
 *    its access mode is only controller by CONFIG.
 *
 * Notes for the UICR
 *   * The PSELRESET[], APPROTECT, NFCPINS, DEBUGCTRL & REGOUT0 registers are ignored
 *     Their equivalent functionality is not implemented
 *
 * Possible future improvements:
 *  * Provide use/wear statistics per page and address.
 *    * Allow accumulating those statistics between runs (save stats to file)
 *  * Warn users if an address is written more than n_write between erases
 *
 *
 * Implementation notes:
 *  This file includes models for a nrf52 and 53 flash and NVMController and of an empty
 *  UICR.
 *  And instantiates 1 set for the nrf52 and 2 sets (one per MCU) for the nrf53,
 *  (and initializes them at startup and frees them on exit),
 *  Note that this is partly described in the configuration (NHW_config.h), and partly
 *  "hardcoded" here for the target type (around command line parameters)
 */

#include <string.h>
#include <stdint.h>
#include "bs_tracing.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "bs_oswrap.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_NVMC.h"
#include "NHW_NVM_backend.h"

NRF_UICR_Type *NRF_UICR_regs_p[NHW_NVMC_UICR_TOTAL_INST];
NRF_NVMC_Type *NRF_NVMC_regs_p[NHW_NVMC_UICR_TOTAL_INST];
NRF_NVMC_Type NRF_NVMC_regs[NHW_NVMC_UICR_TOTAL_INST] = {0};
static bs_time_t Timer_NVMC = TIME_NEVER; //Time when the next flash operation will be completed

enum flash_op_t {flash_idle = 0, flash_write, flash_erase, flash_erase_partial, flash_erase_uicr, flash_erase_all};

struct hw_nvmc_st_t {
  bs_time_t timer;
  NRF_NVMC_Type *NVMC_regs;
  nvm_storage_state_t flash_st;
  nvm_storage_state_t uicr_st;
  enum flash_op_t flash_op;
  uint32_t erase_page;
  bs_time_t *time_under_erase; //[flash_n_pages];
  bool *page_erased; //[flash_n_pages];

  uint flash_start_addr;
  uint flash_size;
  uint flash_page_size;
  uint flash_n_pages;
  uint uicr_size;
};

static struct hw_nvmc_st_t hw_nvmc_st[NHW_NVMC_UICR_TOTAL_INST];

struct nvmc_args_t {
  struct nhw_nvm_st_args_t uicr[NHW_NVMC_UICR_TOTAL_INST];
  struct nhw_nvm_st_args_t flash[NHW_NVMC_UICR_TOTAL_INST];
  bool flash_erase_warnings;
} nvmc_args;

static void nhw_nvmc_find_next_event(void) {
  Timer_NVMC = hw_nvmc_st[0].timer;

  for (int i = 1; i < NHW_NVMC_UICR_TOTAL_INST; i++) {
    if (hw_nvmc_st[i].timer < Timer_NVMC) {
      Timer_NVMC = hw_nvmc_st[i].timer;
    }
  }
  nsi_hws_find_next_event();
}

/**
 * Initialize the NVMC and UICR models
 */
static void nhw_nvmc_uicr_init(void)
{
  uint flash_start_addr[NHW_NVMC_UICR_TOTAL_INST] = NHW_FLASH_START_ADDR;
  uint flash_page_sizes[NHW_NVMC_UICR_TOTAL_INST] = NHW_FLASH_PAGESIZE;
  uint flash_n_pages[NHW_NVMC_UICR_TOTAL_INST] = NHW_FLASH_N_PAGES;
  uint uicr_size[NHW_NVMC_UICR_TOTAL_INST] = NHW_UICR_SIZE;

  memset(NRF_NVMC_regs, 0x00, sizeof(NRF_NVMC_regs));

  for (int inst = 0; inst < NHW_NVMC_UICR_TOTAL_INST; inst++) {
    struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
    this->NVMC_regs = &NRF_NVMC_regs[inst];
    NRF_NVMC_regs_p[inst] = &NRF_NVMC_regs[inst];

    NRF_NVMC_regs[inst].READY = 1;
    NRF_NVMC_regs[inst].READYNEXT = 1;
    NRF_NVMC_regs[inst].ERASEPAGEPARTIALCFG = 0x0000000A;

    this->flash_op = flash_idle;
    this->timer = TIME_NEVER;

    this->flash_start_addr = flash_start_addr[inst];
    this->flash_n_pages = flash_n_pages[inst];
    this->flash_page_size = flash_page_sizes[inst];
    this->flash_size = this->flash_n_pages * this->flash_page_size;

    nhw_nvm_init_storage(&this->flash_st, &nvmc_args.flash[inst],
                         this->flash_size, "flash");

    this->uicr_size = uicr_size[inst];
    nhw_nvm_init_storage(&this->uicr_st, &nvmc_args.uicr[inst],
                         this->uicr_size, "UICR");

    NRF_UICR_regs_p[inst] = (NRF_UICR_Type *)this->uicr_st.storage;

    this->time_under_erase = (bs_time_t *)bs_calloc(this->flash_n_pages, sizeof(bs_time_t));
    this->page_erased = (bool *)bs_malloc(this->flash_n_pages * sizeof(bool));
    memset(this->page_erased, true, this->flash_n_pages * sizeof(bool));

    if (nvmc_args.flash_erase_warnings) {
      for (int i = 0; i < this->flash_size; i+=4) {
        if (*(uint32_t*)(this->flash_st.storage + i) != 0) {
          int page_size = this->flash_page_size;
          this->page_erased[i/page_size] = false;
          //Jump to next page start:
          i = (i + page_size)/page_size*page_size;
        }
      }
    }
  }
  nhw_nvmc_find_next_event();
}

NSI_TASK(nhw_nvmc_uicr_init, HW_INIT, 100);

/**
 * Clean up the NVMC and UICR model before program exit
 */
static void nhw_nvmc_uicr_clean_up(void) {
  for (int inst = 0; inst < NHW_NVMC_UICR_TOTAL_INST; inst++) {
    struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
    nhw_nvm_clear_storage(&this->flash_st);
    nhw_nvm_clear_storage(&this->uicr_st);

    free(this->time_under_erase);
    this->time_under_erase = NULL;

    free(this->page_erased);
    this->page_erased = NULL;
  }
}

NSI_TASK(nhw_nvmc_uicr_clean_up, ON_EXIT_PRE, 100);

/*
 * Complete the actual erase of a flash page
 */
static void nhw_nvmc_complete_erase(uint inst) {
  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
  uint erase_page = this->erase_page;
  uint base_address = erase_page*this->flash_page_size;

  memset(&this->flash_st.storage[base_address], 0xFF, this->flash_page_size);

  this->time_under_erase[erase_page] = 0;
  this->page_erased[erase_page] = true;
}

/*
 * End of a partial erase (the full erase may not be yet completed)
 */
static void nhw_nvmc_complete_erase_partial(uint inst){
  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
  if (this->time_under_erase[this->erase_page] >= NHW_NVMC_FLASH_T_ERASEPAGE) {
    nhw_nvmc_complete_erase(inst);
  }
}

/*
 * Complete the actual erase of the UICR page
 */
static void nhw_nvmc_complete_erase_uicr(uint inst){
  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
  (void)memset(this->uicr_st.storage, 0xFF, this->uicr_st.size);
}

/*
 * Complete the actual erase of all flash and UICR pages
 */
static void nhw_nvmc_complete_erase_all(uint inst){
  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
  nhw_nvmc_complete_erase_uicr(inst);
  (void)memset(this->flash_st.storage, 0xFF, this->flash_st.size);

  memset(this->time_under_erase, 0, this->flash_n_pages*sizeof(bs_time_t));
  memset(this->page_erased, true, this->flash_n_pages*sizeof(bool));
}

/**
 * Time has come when the programmed flash operation has finished
 */
static void nhw_nvmc_timer_triggered(void){

  for (int inst = 0; inst < NHW_NVMC_UICR_TOTAL_INST; inst++) {
    struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];
    if (this->timer != Timer_NVMC) {
      continue;
    }
    switch (this->flash_op) {
      case flash_write:
        //Nothing left to be done
        break;
      case flash_erase:
        nhw_nvmc_complete_erase(inst);
        break;
      case flash_erase_partial:
        nhw_nvmc_complete_erase_partial(inst);
        break;
      case flash_erase_uicr:
        nhw_nvmc_complete_erase_uicr(inst);
        break;
      case flash_erase_all:
        nhw_nvmc_complete_erase_all(inst);
        break;
      default:
        bs_trace_error_time_line("%s: timer triggered with no pending "
            "operation (%i)\n", __func__, this->flash_op);
        break;
    }

    this->flash_op = flash_idle;

    NRF_NVMC_regs[inst].READY = 1;
    NRF_NVMC_regs[inst].READYNEXT = 1;
    this->timer = TIME_NEVER;
  }

  nhw_nvmc_find_next_event();
}

NSI_HW_EVENT(Timer_NVMC, nhw_nvmc_timer_triggered, 50);

bs_time_t nhw_nvmc_time_to_ready(uint inst) {
  if (NRF_NVMC_regs[inst].READY) {
    return 0;
  } else {
    return hw_nvmc_st[inst].timer - nsi_hws_get_time();
  }
}

#define ERASE_ENABLED_CHECK(i, x) \
  if ((NRF_NVMC_regs[i].CONFIG & NVMC_CONFIG_WEN_Msk) != NVMC_CONFIG_WEN_Een) { \
    bs_trace_warning_line_time("%s: %s while erase is not enabled in "       \
        "NVMC[%i].CONFIG (%u), it will be ignored\n",                        \
        __func__, x, i, NRF_NVMC_regs[i].CONFIG);                            \
    return;                                                                  \
  }

#define ERASEPARTIAL_ENABLED_CHECK(i, x) \
  if ((NRF_NVMC_regs[i].CONFIG & NVMC_CONFIG_WEN_Msk) != NVMC_CONFIG_WEN_PEen) { \
    bs_trace_warning_line_time("%s: %s while partial erase is not enabled "  \
        "in NVMC[%i].CONFIG (%u), it will be ignored\n",                     \
        __func__, x, i, NRF_NVMC_regs[i].CONFIG);                            \
    return;                                                                  \
  }

#define BUSY_CHECK(i, x) \
  if ( hw_nvmc_st[i].flash_op != flash_idle) {                       \
    bs_trace_warning_line_time("%s: %s while the flash(%i) was already"  \
          " busy (%i), it will be ignored\n",                        \
          __func__, x, i, hw_nvmc_st[inst].flash_op);                \
          return;                                                    \
  }

#define CHECK_ERASE_ADDRESS(i, x) \
  if ((x < hw_nvmc_st[i].flash_start_addr)   \
      || (x >= hw_nvmc_st[i].flash_start_addr + hw_nvmc_st[i].flash_size)) { \
    bs_trace_error_time_line("%s : Attempted to erase an address (%u) "      \
        "outside of the flash (%i) area\n", __func__, x, i);                 \
  } \
  if (x % hw_nvmc_st[i].flash_page_size != 0) {   \
    bs_trace_warning_time_line("%s : Address (%u) is not at the start of the page " \
        "just the page it contains will be erased\n", __func__, x); \
  }

#define CHECK_PARTIAL_ERASE(i, addr, type) \
  if (nvmc_args.flash_erase_warnings && hw_nvmc_st[i].time_under_erase[addr/hw_nvmc_st[i].flash_page_size] > 0){ \
    bs_trace_warning_line_time("%s: %s in partially erased address (%i, %u, "\
         "time_under_erase: %"PRItime" < %"PRItime")\n", \
         __func__, type, i, addr, \
         hw_nvmc_st[i].time_under_erase[addr/hw_nvmc_st[i].flash_page_size], \
         NHW_NVMC_FLASH_T_ERASEPAGE); \
  }

#define OUT_OF_FLASH_ERROR(addr) \
   bs_trace_error_time_line("%s: Attempted access outside of flash and UICR areas (0x%X)\n",\
    __func__, address)

void nhw_nvmc_erase_page(uint inst, uint32_t address){
  ERASE_ENABLED_CHECK(inst, "ERASEPAGE");
  BUSY_CHECK(inst, "ERASEPAGE");
  CHECK_ERASE_ADDRESS(inst, address);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  this->flash_op = flash_erase;
  NRF_NVMC_regs[inst].READY = 0;
  NRF_NVMC_regs[inst].READYNEXT = 0;
  this->erase_page = (address - this->flash_start_addr) / this->flash_page_size;
  this->timer = nsi_hws_get_time() + NHW_NVMC_FLASH_T_ERASEPAGE;
  nhw_nvmc_find_next_event();
}

#if (NHW_NVMC_HAS_ERASEREGS)
/* Note ERASEPCR1 is an alias to ERASEPAGE (same register) */
void nhw_nvmc_regw_sideeffects_ERASEPAGE(uint inst) {
  nhw_nvmc_erase_page(inst, NRF_NVMC_regs[inst].ERASEPAGE);
}

void nhw_nvmc_regw_sideeffects_ERASEPCR0(uint inst) {
  nhw_nvmc_erase_page(inst, NRF_NVMC_regs[inst].ERASEPCR0);
}

void nhw_nvmc_regw_sideeffects_ERASEUICR(uint inst) {
  NRF_NVMC_regs[inst].ERASEUICR &= 1;

  if (NRF_NVMC_regs[inst].ERASEUICR) {
    NRF_NVMC_regs[inst].ERASEUICR = 0;
    ERASE_ENABLED_CHECK(inst, "ERASEUICR");
    BUSY_CHECK(inst, "ERASEUICR");
    struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

    this->flash_op = flash_erase_uicr;
    NRF_NVMC_regs[inst].READY = 0;
    NRF_NVMC_regs[inst].READYNEXT = 0;
    this->timer = nsi_hws_get_time() + NHW_NVMC_FLASH_T_ERASEPAGE;
    nhw_nvmc_find_next_event();
  }
}
#endif

void nhw_nvmc_regw_sideeffects_ERASEALL(uint inst) {
  NRF_NVMC_regs[inst].ERASEALL &= 1;

  if (NRF_NVMC_regs[inst].ERASEALL) {
    NRF_NVMC_regs[inst].ERASEALL = 0;
    ERASE_ENABLED_CHECK(inst, "ERASEALL");
    BUSY_CHECK(inst, "ERASEALL");
    struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

    this->flash_op = flash_erase_all;
    NRF_NVMC_regs[inst].READY = 0;
    NRF_NVMC_regs[inst].READYNEXT = 0;
    this->timer = nsi_hws_get_time() + NHW_NVMC_FLASH_T_ERASEALL;
    nhw_nvmc_find_next_event();
  }
}

void nhw_nvmc_erase_page_partial(uint inst, uint32_t address) {
#if defined(NVMC_CONFIG_WEN_PEen)
  ERASEPARTIAL_ENABLED_CHECK(inst, "ERASEPARTIAL");
#else
  ERASE_ENABLED_CHECK(inst, "ERASEPARTIAL");
#endif
  BUSY_CHECK(inst, "ERASEPARTIAL");
  CHECK_ERASE_ADDRESS(inst, address);
  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  this->erase_page = (address - this->flash_start_addr)/this->flash_page_size;

  this->flash_op = flash_erase_partial;
  NRF_NVMC_regs[inst].READY = 0;
  NRF_NVMC_regs[inst].READYNEXT = 0;

  bs_time_t duration = NHW_NVMC_FLASH_PARTIAL_ERASE_FACTOR * NRF_NVMC_regs[inst].ERASEPAGEPARTIALCFG * 1000;

  if (this->page_erased[this->erase_page] == false) {
    this->time_under_erase[this->erase_page] += duration;
  }
  this->timer = nsi_hws_get_time() + duration;
  nhw_nvmc_find_next_event();
}

#if (NHW_NVMC_HAS_ERASEREGS)
void nhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(uint inst) {
  nhw_nvmc_erase_page_partial(inst, NRF_NVMC_regs[inst].ERASEPAGEPARTIAL);
}
#endif

/*
 * Check if an address is  in range of any flash or UICR
 * and if so, set the inst, storage and offset accordingly.
 *
 * Where address is an address in either a "hard" flash address
 * (meaning in the 0x00..00 range for nrf52, or also in the 0x010..00 range for nrf53)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_BASE/NRF_UICR_S/NS_BASE)
 *
 * If the address is not in range, the function does not return, but produces an error
 */
static void nhw_nvmc_address_location(uint32_t address, uint *inst, nvm_storage_state_t **storage, int *offset){
  for (int i = 0; i < NHW_NVMC_UICR_TOTAL_INST; i ++) {
    *inst = i;
    if ((address >= hw_nvmc_st[i].flash_start_addr)   \
        && (address < hw_nvmc_st[i].flash_start_addr + hw_nvmc_st[i].flash_size)) {
      *storage = &hw_nvmc_st[i].flash_st;
      *offset = address - hw_nvmc_st[i].flash_start_addr;
      return;
    }
    if ((address >= (uint32_t)hw_nvmc_st[i].uicr_st.storage)   \
        && (address < (uint32_t)hw_nvmc_st[i].uicr_st.storage + hw_nvmc_st[i].uicr_size)) {
      *storage = &hw_nvmc_st[i].uicr_st;
      *offset = address - (uint32_t)hw_nvmc_st[i].uicr_st.storage;
      return;
    }
  }
  OUT_OF_FLASH_ERROR(address);
}

/*
 * Write to a flash array (flash or uicr)
 *
 * Where address is an address in either a "hard" flash address
 * (meaning in the 0x00..00 range for nrf52, or also in the 0x010..00 range for nrf53)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_BASE/NRF_UICR_S/NS_BASE)
 */
void nhw_nmvc_write_word(uint32_t address, uint32_t value){
  if ((address & 3) != 0){
    bs_trace_error_line_time("%s: write to non word aligned address %u, "
            "this would have hard-faulted in real HW\n",
            __func__, address);
  }

  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_nvmc_address_location(address, &inst, &backend, &offset);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  BUSY_CHECK(inst, "write");

#if (NHW_NVMC_HAS_ERASEREGS == 0)
  if (backend == &this->flash_st) {
    if ((NRF_NVMC_regs[inst].CONFIG & NVMC_CONFIG_WEN_Msk) == NVMC_CONFIG_WEN_Een) {
      if ((value == 0xFFFFFFFF) && (offset % this->flash_page_size == 0)) {
        nhw_nvmc_erase_page(inst, address);
        return;
      }
    }
    if ((NRF_NVMC_regs[inst].CONFIG & NVMC_CONFIG_WEN_Msk) == NVMC_CONFIG_WEN_PEen) {
      if ((value == 0xFFFFFFFF) && (offset % this->flash_page_size == 0)) {
        nhw_nvmc_erase_page_partial(inst, address);
        return;
      }
    }
  }
#endif

  if ((NRF_NVMC_regs[inst].CONFIG & NVMC_CONFIG_WEN_Msk) != NVMC_CONFIG_WEN_Wen) {
    bs_trace_warning_line_time("%s: write while write is not enabled in "
        "CONFIG (%u), it will be ignored\n",
        __func__, NRF_NVMC_regs[inst].CONFIG);
    return;
  }

  if (backend == &this->flash_st) {
    CHECK_PARTIAL_ERASE(inst, offset, "write");
    this->page_erased[offset/this->flash_page_size] = false;
  }

  /*
   * Writing to flash clears to 0 bits which were one, but does not
   * set to 1 bits which are 0.
   */
  *(uint32_t*)&backend->storage[offset] &= value;

  this->flash_op = flash_write;
  NRF_NVMC_regs[inst].READY = 0;
  NRF_NVMC_regs[inst].READYNEXT = 0;

  this->timer = nsi_hws_get_time() + NHW_NVMC_FLASH_T_WRITE;
  nhw_nvmc_find_next_event();
}

/**
 * Read from the flash array (flash or uicr)
 *
 * Where address is an address in either a "hard" flash address
 * (meaning in the 0x00..00 range for nrf52, or also in the 0x010..00 range for nrf53)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_BASE/NRF_UICR_S/NS_BASE)
 *
 * This models the embedded CPU accessing the flash array for read
 *
 * This operation is instantaneous in simulation.
 * In real HW it is "fast"
 */
uint32_t nhw_nmvc_read_word(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_nvmc_address_location(address, &inst, &backend, &offset);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  if (backend == &this->flash_st) {
    CHECK_PARTIAL_ERASE(inst, offset, "read");
  }

  return *(uint32_t*)&backend->storage[offset];
}

uint16_t nhw_nmvc_read_halfword(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_nvmc_address_location(address, &inst, &backend, &offset);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  if (backend == &this->flash_st) {
    CHECK_PARTIAL_ERASE(inst, offset, "read");
  }

  return *(uint16_t*)&backend->storage[offset];
}

uint8_t nhw_nmvc_read_byte(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_nvmc_address_location(address, &inst, &backend, &offset);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  if (backend == &this->flash_st) {
    CHECK_PARTIAL_ERASE(inst, offset, "read");
  }

  return *(uint8_t*)&backend->storage[offset];
}

/**
 * memcpy from the flash/uicr array from <address> <size> bytes.
 *
 * Where address is an address in either a "hard" flash address
 * (meaning in the 0x00..00 range for nrf52, or also in the 0x010..00 range for nrf53)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_BASE/NRF_UICR_S/NS_BASE)
 *
 * This is a convenience function in the model.
 * It can be used from DMA controller models or the like.
 *
 * This operation is instantaneous in simulation.
 * In real HW it is as "fast" as all those AHB accesses
 * can be dispatched by the interconnect and handled by
 * the NVMC peripheral
 */
void nhw_nmvc_read_buffer(void *dest, uint32_t address, size_t size) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_nvmc_address_location(address, &inst, &backend, &offset);

  struct hw_nvmc_st_t *this = &hw_nvmc_st[inst];

  if ((backend == &this->flash_st) && (nvmc_args.flash_erase_warnings)) {
    for (uint32_t i = offset; i < offset + size ; i+= this->flash_page_size) {
      CHECK_PARTIAL_ERASE(inst, i, "read");
    }
  }

  if (offset + size >= backend->size) {
    OUT_OF_FLASH_ERROR(address + size);
  }

  (void)memcpy(dest, &backend->storage[offset], size);
}

void* nrfhw_nmvc_flash_get_base_address(uint inst){
	return (void*)&hw_nvmc_st[inst].flash_st.storage;
}

#if defined(NRF52833)
NVM_BACKEND_PARAMS_CALLBACS(uicr, uicr[0])
NVM_BACKEND_PARAMS_CALLBACS(flash, flash[0])
#elif defined(NRF5340)
NVM_BACKEND_PARAMS_CALLBACS(flash, flash[1])
NVM_BACKEND_PARAMS_CALLBACS(uicr_app, uicr[0])
NVM_BACKEND_PARAMS_CALLBACS(flash_app, flash[0])
NVM_BACKEND_PARAMS_CALLBACS(uicr_net, uicr[1])
NVM_BACKEND_PARAMS_CALLBACS(flash_net, flash[1])
#endif

static void nvmc_register_cmd_args(void){
  for (int i = 0; i < NHW_NVMC_UICR_TOTAL_INST; i++) {
    nvmc_args.uicr[i].in_ram = true;
    nvmc_args.flash[i].in_ram = true;
  }

  static bs_args_struct_t args_struct_toadd[] = {
#if defined(NRF52833)
    NVM_BACKEND_PARAMS(uicr, uicr[0], UICR),
    NVM_BACKEND_PARAMS(flash, flash[0], FLASH),
#elif defined(NRF5340)
    NVM_BACKEND_PARAMS(uicr_app, uicr[0], UICR_APP),
    NVM_BACKEND_PARAMS(flash_app, flash[0], FLASH_APP),
    NVM_BACKEND_PARAMS(uicr_net, uicr[1], UICR_NET),
    NVM_BACKEND_PARAMS(flash_net, flash[1], FLASH_NET),
    { .is_switch = true,
      .option = "flash_*",
      .manual = true,
      .type = 'b',
      .descript = "All plain flash options are aliases for the network core flash"
    },
    NVM_BACKEND_PARAMS(flash, flash[1], FLASH_NET),
#endif
    { .is_switch = true,
      .option = "flash_erase_warnings",
      .type = 'b',
      .dest = (void*)&nvmc_args.flash_erase_warnings,
      .descript = "Give warnings when accessing partially erased pages in any flash"
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(nvmc_register_cmd_args, PRE_BOOT_1, 100);
