/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * NVMC - Non-volatile memory controller
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/nvmc.html?cp=5_1_0_3_2
 * &
 * UICR - User information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/uicr.html?cp=5_1_0_3_4
 *
 * Notes for the NVMC:
 *  * The CPU is not stalled while doing flash operations, "while executing from flash"
 *    as we do not know what code would have been in flash or RAM, code is just
 *    assumed only in RAM for simplicity.
 *
 *  * The model will not prevent you from writing too many times (more then n_write)
 *    the same address between erases
 *
 *  * The spec does not specify how much earlier READYNEXT, so this model just sets
 *    it when the previous operation is done (together with READY)
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
 * Notes for the UICR
 *   * The PSELRESET[], APPROTECT, NFCPINS, DEBUGCTRL & REGOUT0 registers are ignored
 *     Their equivalent functionality is not implemented
 *
 * Possible future improvements:
 *  * Provide use/wear statistics per page and address.
 *    * Allow accumulating those statistics between runs (save stats to file)
 *  * Warn users if an address is written more than n_write between erases
 */

/*
 * Notes for 53:
 *  * CONFIGNS and CONFIG have the same abilities (including enabling a partial erase)
 *  * There is no handling or differentiation between secure and non secure areas
 *  * All registers are accessible from all SW (there is no differentiation between secure and not secure registers)
 *  * There is no modelling of the protection features (Access port protection)
 *
 * Main 52-53 diffs:
 *    Erase (full or partial) by writing 0xFF..FF to first word of page instead of starting erase
 *    UICR can only be erased with eraseall (not by writing 0xFF on its first word, no ERASEUICR register)
 *    2 instances of flash and UICR
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
#include "NHW_NVM_common.h"

NRF_UICR_Type *NRF_UICR_regs_p;
NRF_NVMC_Type NRF_NVMC_regs = {0};
static bs_time_t Timer_NVMC = TIME_NEVER; //Time when the next flash operation will be completed

static nvm_storage_state_t flash_st;
static nvm_storage_state_t uicr_st;
static enum flash_op_t flash_op;
static uint32_t erase_address;
static bs_time_t time_under_erase[FLASH_N_PAGES];
static bool page_erased[FLASH_N_PAGES];

static bs_time_t flash_t_eraseall  = 173000;
static bs_time_t flash_t_erasepage =  87500;
static bs_time_t flash_t_write     =     42;
static double    flash_partial_erase_factor = 1.0; //actual tERASEPAGEPARTIAL,acc for this given device

struct nvmc_args_t {
  struct nhw_nvm_st_args_t uicr;
  struct nhw_nvm_st_args_t flash;
  bool flash_erase_warnings;
} nvmc_args;

/**
 * Initialize the NVMC and UICR models
 */
static void nrfhw_nvmc_uicr_init(void) {
  memset(&NRF_NVMC_regs, 0x00, sizeof(NRF_NVMC_regs));
  NRF_NVMC_regs.READY = 1;
  NRF_NVMC_regs.READYNEXT = 1;
  NRF_NVMC_regs.ERASEPAGEPARTIALCFG = 0x0000000A;

  flash_op = flash_idle;
  Timer_NVMC = TIME_NEVER;
  nsi_hws_find_next_event();

  nhw_nvm_init_storage(&flash_st, &nvmc_args.flash,
                       FLASH_SIZE, "flash");
  // We book more than the actual flash area, to avoid segfaults if
  // somebody tries to access the HW control registers
  nhw_nvm_init_storage(&uicr_st, &nvmc_args.uicr,
                       sizeof(NRF_UICR_Type), "UICR");

  NRF_UICR_regs_p = (NRF_UICR_Type *)uicr_st.storage;

  //Reset the partial erase tracking
  for (int i = 0; i < FLASH_N_PAGES; i++) {
    time_under_erase[i] = 0;
    page_erased[i] = true;
  }
  if (nvmc_args.flash_erase_warnings) {
    for (int i = 0; i < FLASH_SIZE/4; i+=4) {
      if (*(uint32_t*)(flash_st.storage + i) != 0) {
        page_erased[i/FLASH_PAGE_SIZE] = false;
        //Jump to next page start:
        i = (i + FLASH_PAGE_SIZE)/FLASH_PAGE_SIZE*FLASH_PAGE_SIZE;
      }
    }
  }
}

NSI_TASK(nrfhw_nvmc_uicr_init, HW_INIT, 100);

/**
 * Clean up the NVMC and UICR model before program exit
 */
static void nrfhw_nvmc_uicr_clean_up(void) {
  nhw_nvm_clear_storage(&flash_st);
  nhw_nvm_clear_storage(&uicr_st);
}

NSI_TASK(nrfhw_nvmc_uicr_clean_up, ON_EXIT_PRE, 100);

/*
 * Complete the actual erase of a flash page
 */
static void nrfhw_nvmc_complete_erase(void){
  uint base_address = erase_address/FLASH_PAGE_SIZE*FLASH_PAGE_SIZE;

  memset(&flash_st.storage[base_address], 0xFF, FLASH_PAGE_SIZE);

  time_under_erase[erase_address/FLASH_PAGE_SIZE] = 0;
  page_erased[erase_address/FLASH_PAGE_SIZE] = true;
}

/*
 * End of a partial erase (the full erase may not be yet completed)
 */
static void nrfhw_nvmc_complete_erase_partial(void){
  if (time_under_erase[erase_address/FLASH_PAGE_SIZE] >= flash_t_erasepage) {
    nrfhw_nvmc_complete_erase();
  }
}

/*
 * Complete the actual erase of the UICR page
 */
static void nrfhw_nvmc_complete_erase_uicr(void){
  (void)memset(uicr_st.storage, 0xFF, uicr_st.size);
}

/*
 * Complete the actual erase of all flash and UICR page
 */
static void nrfhw_nvmc_complete_erase_all(void){
  nrfhw_nvmc_complete_erase_uicr();
  (void)memset(flash_st.storage, 0xFF, flash_st.size);
  for (int i = 0; i < FLASH_N_PAGES; i++) {
    time_under_erase[i] = 0;
    page_erased[i] = true;
  }
}

/**
 * Time has come when the programmed flash operation has finished
 */
static void nrfhw_nvmc_timer_triggered(void){

  switch (flash_op) {
    case flash_write:
      //Nothing left to be done
      break;
    case flash_erase:
      nrfhw_nvmc_complete_erase();
      break;
    case flash_erase_partial:
      nrfhw_nvmc_complete_erase_partial();
      break;
    case flash_erase_uicr:
      nrfhw_nvmc_complete_erase_uicr();
      break;
    case flash_erase_all:
      nrfhw_nvmc_complete_erase_all();
      break;
    default:
      bs_trace_error_time_line("%s: timer triggered with no pending "
          "operation (%i)\n", __func__, flash_op);
      break;
  }

  flash_op = flash_idle;

  NRF_NVMC_regs.READY = 1;
  NRF_NVMC_regs.READYNEXT = 1;

  Timer_NVMC = TIME_NEVER;
  nsi_hws_find_next_event();
}

NSI_HW_EVENT(Timer_NVMC, nrfhw_nvmc_timer_triggered, 50);

bs_time_t nrfhw_nvmc_time_to_ready(void) {
  if (NRF_NVMC_regs.READY) {
    return 0;
  } else {
    return Timer_NVMC - nsi_hws_get_time();
  }
}

#define ERASE_ENABLED_CHECK(x) \
  if ((NRF_NVMC_regs.CONFIG & NVMC_CONFIG_WEN_Msk) != NVMC_CONFIG_WEN_Een) { \
    bs_trace_warning_line_time("%s: %s while erase is not enabled in "       \
        "CONFIG (%u), it will be ignored\n",                                 \
        __func__, x, NRF_NVMC_regs.CONFIG);                                  \
    return;                                                                  \
  }

#define BUSY_CHECK(x) \
  if (flash_op != flash_idle) {                                      \
    bs_trace_warning_line_time("%s: %s while the flash was already"  \
          " busy (%i), it will be ignored\n",                        \
          __func__, x, flash_op);                                    \
          return;                                                    \
  }

#define CHECK_ADDRESS_INRANGE(x,y) \
  if (x >= FLASH_SIZE) {   \
    bs_trace_error_time_line("%s : Attempted to %s an address (%u) " \
        "outside of the flash area\n", __func__, y, x); \
  }

#define CHECK_ADDRESS_INRANGE_UICR(x,y) \
  if (x >= UICR_SIZE) {   \
    bs_trace_error_time_line("%s : Attempted to %s an address (%u) " \
        "outside of the uicr area\n", __func__, y, x); \
  }


#define CHECK_ERASE_ADDRESS(x) \
  CHECK_ADDRESS_INRANGE(x, "erase"); \
  if (x % FLASH_PAGE_SIZE != 0) {   \
    bs_trace_warning_time_line("%s : Address (%u) is not at the start of the page " \
        "just the page it contains will be erased\n", __func__, x); \
  }

#define CHECK_PARTIAL_ERASE(addr, type) \
  if (nvmc_args.flash_erase_warnings && time_under_erase[addr/FLASH_PAGE_SIZE] > 0){ \
    bs_trace_warning_line_time("%s: %s in partially erased address (%u, "\
         "time_under_erase: %"PRItime" < %"PRItime")\n", \
         __func__, type, addr, time_under_erase[addr/FLASH_PAGE_SIZE], flash_t_erasepage); \
  }

#define OUT_OF_FLASH_ERROR(addr) \
   bs_trace_error_time_line("%s: Attempted access outside of flash and UICR areas (0x%X)\n",\
    __func__, address)

static void nrfhw_nvmc_erase_page(uint32_t address){
  ERASE_ENABLED_CHECK("ERASEPAGE");
  BUSY_CHECK("ERASEPAGE");
  CHECK_ERASE_ADDRESS(address);
  flash_op = flash_erase;
  NRF_NVMC_regs.READY = 0;
  NRF_NVMC_regs.READYNEXT = 0;
  erase_address = address;
  Timer_NVMC = nsi_hws_get_time() + flash_t_erasepage;
  nsi_hws_find_next_event();
}

/* Note ERASEPCR1 is an alias to ERASEPAGE (same register) */
void nrfhw_nvmc_regw_sideeffects_ERASEPAGE(void) {
  nrfhw_nvmc_erase_page(NRF_NVMC_regs.ERASEPAGE);
}

void nrf_nvmc_regw_sideeffects_ERASEPCR0(void) {
  nrfhw_nvmc_erase_page(NRF_NVMC_regs.ERASEPCR0);
}

void nrfhw_nvmc_regw_sideeffects_ERASEUICR(void) {
  NRF_NVMC_regs.ERASEUICR &= 1;

  if (NRF_NVMC_regs.ERASEUICR) {
    NRF_NVMC_regs.ERASEUICR = 0;
    ERASE_ENABLED_CHECK("ERASEUICR");
    BUSY_CHECK("ERASEUICR");
    flash_op = flash_erase_uicr;
    NRF_NVMC_regs.READY = 0;
    NRF_NVMC_regs.READYNEXT = 0;
    Timer_NVMC = nsi_hws_get_time() + flash_t_erasepage;
    nsi_hws_find_next_event();
  }
}

void nrfhw_nvmc_regw_sideeffects_ERASEALL(void) {
  NRF_NVMC_regs.ERASEALL &= 1;

  if (NRF_NVMC_regs.ERASEALL) {
    NRF_NVMC_regs.ERASEALL = 0;
    ERASE_ENABLED_CHECK("ERASEALL");
    BUSY_CHECK("ERASEALL");
    flash_op = flash_erase_all;
    NRF_NVMC_regs.READY = 0;
    NRF_NVMC_regs.READYNEXT = 0;
    Timer_NVMC = nsi_hws_get_time() + flash_t_eraseall;
    nsi_hws_find_next_event();
  }
}

void nrfhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(void) {
  ERASE_ENABLED_CHECK("ERASEPARTIAL");
  BUSY_CHECK("ERASEPARTIAL");
  erase_address = NRF_NVMC_regs.ERASEPAGEPARTIAL;
  CHECK_ERASE_ADDRESS(erase_address);

  flash_op = flash_erase_partial;
  NRF_NVMC_regs.READY = 0;
  NRF_NVMC_regs.READYNEXT = 0;

  bs_time_t duration = flash_partial_erase_factor * NRF_NVMC_regs.ERASEPAGEPARTIALCFG * 1000;
  if (page_erased[erase_address/FLASH_PAGE_SIZE] == false) {
    time_under_erase[erase_address/FLASH_PAGE_SIZE] += duration;
  }
  Timer_NVMC = nsi_hws_get_time() + duration;
  nsi_hws_find_next_event();
}

static bool addr_in_uicr(uint32_t address){
  return (address >= (uintptr_t)NRF_UICR_regs_p)
      && (address < (uintptr_t)NRF_UICR_regs_p + UICR_SIZE);
}

void nrfhw_nmvc_write_word(uint32_t address, uint32_t value){
  BUSY_CHECK("write");
  if ((address & 3) != 0){
    bs_trace_error_line_time("%s: write to non word aligned address %u, "
            "this would have hard-faulted in real HW\n",
            __func__, address);
  }

  if ((NRF_NVMC_regs.CONFIG & NVMC_CONFIG_WEN_Msk) != NVMC_CONFIG_WEN_Wen) {
    bs_trace_warning_line_time("%s: write while write is not enabled in "
        "CONFIG (%u), it will be ignored\n",
        __func__, NRF_NVMC_regs.CONFIG);
    return;
  }

  if (address < FLASH_SIZE) {
    CHECK_PARTIAL_ERASE(address, "write");
    page_erased[address/FLASH_PAGE_SIZE] = false;
    /*
     * Writing to flash clears to 0 bits which were one, but does not
     * set to 1 bits which are 0.
     */
    *(uint32_t*)&flash_st.storage[address] &= value;
  }
  else if (addr_in_uicr(address))
  {
    address = address - (uintptr_t)NRF_UICR_regs_p;
    *(uint32_t*)&uicr_st.storage[address] &= value;
  } else {
    OUT_OF_FLASH_ERROR(address);
  }

  flash_op = flash_write;
  NRF_NVMC_regs.READY = 0;
  NRF_NVMC_regs.READYNEXT = 0;

  Timer_NVMC = nsi_hws_get_time() + flash_t_write;
  nsi_hws_find_next_event();
}

/**
 * Read from the flash array with offset <address>
 * (Note that the flash array starts at address 0x0 in real HW)
 *
 * This models the embedded CPU accessing the flash array for read
 *
 * This operation is instantaneous in simulation.
 * In real HW it is "fast"
 */
uint32_t nrfhw_nmvc_read_word(uint32_t address) {

  if (address < FLASH_SIZE)
  {
    CHECK_PARTIAL_ERASE(address, "read");
    return *(uint32_t*)&flash_st.storage[address];
  }
  else if (addr_in_uicr(address))
  {
    address = address - (uintptr_t)NRF_UICR_regs_p;
    return *(uint32_t*)&uicr_st.storage[address];
  } else {
    OUT_OF_FLASH_ERROR(address);
    return 0;
  }
}

uint16_t nrfhw_nmvc_read_halfword(uint32_t address) {
  if (address < FLASH_SIZE)
  {
    CHECK_PARTIAL_ERASE(address, "read");
    return *(uint16_t*)&flash_st.storage[address];
  }
  else if (addr_in_uicr(address))
  {
    address = address - (uintptr_t)NRF_UICR_regs_p;
    return *(uint16_t*)&uicr_st.storage[address];
  } else {
    OUT_OF_FLASH_ERROR(address);
    return 0;
  }
}

uint8_t nrfhw_nmvc_read_byte(uint32_t address) {
  if (address < FLASH_SIZE)
  {
    CHECK_PARTIAL_ERASE(address, "read");
    return *(uint8_t*)&flash_st.storage[address];
  }
  else if (addr_in_uicr(address))
  {
    address = address - (uintptr_t)NRF_UICR_regs_p;
    return *(uint8_t*)&uicr_st.storage[address];
  } else {
    OUT_OF_FLASH_ERROR(address);
    return 0;
  }
}

/**
 * memcpy from the flash/uicr array from <address> <size> bytes.
 * (Note that the flash array starts at address 0x0 in real HW)
 *
 * This is a convenience function in the model.
 * It can be used from DMA controller models or the like.
 *
 * This operation is instantaneous in simulation.
 * In real HW it is as "fast" as all those AHB accesses
 * can be dispatched by the interconnect and handled by
 * the NVMC peripheral
 */
void nrfhw_nmvc_read_buffer(void *dest, uint32_t address, size_t size) {

  if (address < FLASH_SIZE)
  {
    CHECK_ADDRESS_INRANGE(address + size - 1, "read");
    for (uint32_t i = address; i < address + size ; i+= FLASH_PAGE_SIZE) {
      CHECK_PARTIAL_ERASE(i, "read");
    }
    (void)memcpy(dest, &flash_st.storage[address], size);
  }
  else if (addr_in_uicr(address))
  {
    address = address - (uintptr_t)NRF_UICR_regs_p;
    CHECK_ADDRESS_INRANGE_UICR(address + size - 1, "read");
    (void)memcpy(dest, &uicr_st.storage[address], size);
  } else {
    OUT_OF_FLASH_ERROR(address);
  }
}

void* nrfhw_nmvc_flash_get_base_address(void){
	return (void*)&flash_st.storage;
}

NVM_BACKEND_PARAMS_CALLBACS(uicr)
NVM_BACKEND_PARAMS_CALLBACS(flash)

static void nvmc_register_cmd_args(void){

  nvmc_args.uicr.in_ram = true;
  nvmc_args.flash.in_ram = true;

  static bs_args_struct_t args_struct_toadd[] = {
    NVM_BACKEND_PARAMS(uicr, "UICR"),
    NVM_BACKEND_PARAMS(flash, "FLASH"),
    { .is_switch = true,
      .option = "flash_erase_warnings",
      .type = 'b',
      .dest = (void*)&nvmc_args.flash_erase_warnings,
      .descript = "Give warnings when accessing partially erased pages"
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(nvmc_register_cmd_args, PRE_BOOT_1, 100);
