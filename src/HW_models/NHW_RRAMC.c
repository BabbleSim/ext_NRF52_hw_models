/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * NRF54L
 *
 * RRAMC - Resistive random access memory controller
 * &
 * UICR - User information configuration registers
 *
 * A very simple model, which creates a memory mapped RRAMC and UICR area
 * backed by either a file on disk or host RAM.
 * The RRAM (and UICR) is memory is not mapped in the same addresses as in
 * real HW. Accesses need to be done relative to nhw_RRAMC_get_RRAM_base_address()
 * (or NRF_UICR_BASE for the UICR)
 *
 * Note: The SICR is not yet included
 *
 * Notes for the RRAM(C):
 *
 *  * Reads and writes are instantaneous.
 *
 *  * READY and READYNEXT are reset to 1 and never cleared.
 *
 *  * Task, events and interrupts are not connected.
 *
 *  * Writes are assumed always enabled and write buffering disabled.
 *    i.e. CONFIG, READYNEXTTIMEOUT are ignored,
 *    writes are immediately reflected in the RRAM,
 *    BUFSTATUS.EMPTY is reset to 1 and never cleared.
 *
 *  * There is no model of the optional immutable boot region write protection
 *    (all RRAM can be written)
 *
 *  * Reads do not go thru the CACHE
 *
 *  * There is no models of the RRAM and controller power states
 *    (POWER.* are ignored)
 *
 *  * There is no models of bit errors, or ECC corrections
 *
 *  * ACCESSERRORADDR is not modeled (it is never set after reset)
 *
 *  * Access port protection is not modeled. The UICR can always be erased.
 *
 *
 * Notes for the UICR
 *   * The APPROTECT, SECUREAPPROTECT, AUXAPPROTECT, ERASEPROTECT
 *     and BOOTCONF are ignored.
 *     Their equivalent functionality is not implemented
 *
 *
 * Implementation notes:
 *  This file includes models for a nrf54L RRAMController, the RRAM itself
 *  and of an empty UICR.
 *  And instantiates 1 set for the nrf54L15
 *  (and initializes them at startup and frees them on exit),
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
#include "NHW_RRAMC.h"
#include "NHW_NVM_backend.h"

NRF_UICR_Type *NRF_UICR_regs_p[NHW_RRAMC_UICR_TOTAL_INST];
NRF_RRAMC_Type *NRF_RRAMC_regs_p[NHW_RRAMC_UICR_TOTAL_INST];
NRF_RRAMC_Type NRF_RRAMC_regs[NHW_RRAMC_UICR_TOTAL_INST] = {0};

struct hw_rramc_st_t {
  NRF_RRAMC_Type *RRAMC_regs;
  nvm_storage_state_t rram_st;
  nvm_storage_state_t uicr_st;

  uint rram_start_addr;
  uint rram_size;
  uint uicr_size;
};

static struct hw_rramc_st_t hw_rramc_st[NHW_RRAMC_UICR_TOTAL_INST];

struct nvmc_args_t {
  struct nhw_nvm_st_args_t uicr[NHW_RRAMC_UICR_TOTAL_INST];
  struct nhw_nvm_st_args_t rram[NHW_RRAMC_UICR_TOTAL_INST];
} nvmc_args;

/**
 * Initialize the RRAM(C) and UICR models
 */
static void nhw_RRAMC_uicr_init(void)
{
  uint rram_start_addr[NHW_RRAMC_UICR_TOTAL_INST] = NHW_RRAM_START_ADDR;
  uint rram_size[NHW_RRAMC_UICR_TOTAL_INST] = NHW_RRAM_SIZE;
  uint uicr_size[NHW_RRAMC_UICR_TOTAL_INST] = NHW_UICR_SIZE;

  memset(NRF_RRAMC_regs, 0x00, sizeof(NRF_RRAMC_regs));

  for (int inst = 0; inst < NHW_RRAMC_UICR_TOTAL_INST; inst++) {
    struct hw_rramc_st_t *this = &hw_rramc_st[inst];
    this->RRAMC_regs = &NRF_RRAMC_regs[inst];
    NRF_RRAMC_regs_p[inst] = &NRF_RRAMC_regs[inst];

    NRF_RRAMC_regs[inst].READY = 1;
    NRF_RRAMC_regs[inst].READYNEXT = 1;
    NRF_RRAMC_regs[inst].EVENTS_READY = 1;
    NRF_RRAMC_regs[inst].EVENTS_READYNEXT = 1;
    NRF_RRAMC_regs[inst].ACCESSERRORADDR = 0x00FFFFFF;
    NRF_RRAMC_regs[inst].BUFSTATUS.WRITEBUFEMPTY = 1;
    NRF_RRAMC_regs[inst].READYNEXTTIMEOUT = 0x00000080;
    NRF_RRAMC_regs[inst].POWER.CONFIG = 0x00000100;

    this->rram_start_addr = rram_start_addr[inst];
    this->rram_size = rram_size[inst];

    nhw_nvm_init_storage(&this->rram_st, &nvmc_args.rram[inst],
                         this->rram_size, "RRAM");

    this->uicr_size = uicr_size[inst];
    nhw_nvm_init_storage(&this->uicr_st, &nvmc_args.uicr[inst],
                         this->uicr_size, "UICR");

    NRF_UICR_regs_p[inst] = (NRF_UICR_Type *)this->uicr_st.storage;
  }
}

NSI_TASK(nhw_RRAMC_uicr_init, HW_INIT, 100);

/**
 * Clean up the RRAM(C) and UICR model before program exit
 */
static void nhw_RRAMC_uicr_clean_up(void) {
  for (int inst = 0; inst < NHW_RRAMC_UICR_TOTAL_INST; inst++) {
    struct hw_rramc_st_t *this = &hw_rramc_st[inst];
    nhw_nvm_clear_storage(&this->rram_st);
    nhw_nvm_clear_storage(&this->uicr_st);
  }
}

NSI_TASK(nhw_RRAMC_uicr_clean_up, ON_EXIT_PRE, 100);

/*
 * Do the actual erase of the UICR area
 */
static void nhw_RRAMC_erase_uicr(uint inst){
  struct hw_rramc_st_t *this = &hw_rramc_st[inst];
  (void)memset(this->uicr_st.storage, 0xFF, this->uicr_st.size);
}

/*
 * Do the actual erase of all RRAM and UICR areas
 */
static void nhw_RRAMC_erase_all(uint inst){
  struct hw_rramc_st_t *this = &hw_rramc_st[inst];
  nhw_RRAMC_erase_uicr(inst);
  (void)memset(this->rram_st.storage, 0xFF, this->rram_st.size);
}

void nhw_RRAMC_regw_sideeffects_ERASEALL(uint inst) {
  NRF_RRAMC_regs[inst].ERASE.ERASEALL &= 1;

  if (NRF_RRAMC_regs[inst].ERASE.ERASEALL) {
    NRF_RRAMC_regs[inst].ERASE.ERASEALL = 0;
    nhw_RRAMC_erase_all(inst);
  }
}

#define OUT_OF_RRAM_ERROR(addr) \
   bs_trace_error_time_line("%s: Attempted access outside of RRAM and UICR areas (0x%X)\n",\
    __func__, address)

/*
 * Check if an address is  in range of any RRAM or UICR
 * and if so, set the inst, storage and offset accordingly.
 *
 * Where address is an address in either a "hard" RRAM address
 * (meaning in the 0x00..00 range for nrf52, or also in the 0x010..00 range for nrf53)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_*_BASE)
 *
 * If the address is not in range, the function does not return, but produces an error
 */
static void nhw_RRAMC_address_location(uint32_t address, uint *inst, nvm_storage_state_t **storage, int *offset){
  for (int i = 0; i < NHW_RRAMC_UICR_TOTAL_INST; i ++) {
    *inst = i;
    if ((address >= hw_rramc_st[i].rram_start_addr)   \
        && (address < hw_rramc_st[i].rram_start_addr + hw_rramc_st[i].rram_size)) {
      *storage = &hw_rramc_st[i].rram_st;
      *offset = address - hw_rramc_st[i].rram_start_addr;
      return;
    }
    if ((address >= (uint32_t)hw_rramc_st[i].uicr_st.storage)   \
        && (address < (uint32_t)hw_rramc_st[i].uicr_st.storage + hw_rramc_st[i].uicr_size)) {
      *storage = &hw_rramc_st[i].uicr_st;
      *offset = address - (uint32_t)hw_rramc_st[i].uicr_st.storage;
      return;
    }
  }
  OUT_OF_RRAM_ERROR(address);
}

/*
 * Write to a RRAM array (RRAM or UICR)
 *
 * Where address is an address in either a "hard" RRAM address
 * (meaning in the 0x00..00 range)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_*_BASE)
 */
void nhw_RRAMC_write_word(uint32_t address, uint32_t value){
  if ((address & 3) != 0){
    bs_trace_error_line_time("%s: write to non word aligned address %u, "
            "this would have hard-faulted in real HW\n",
            __func__, address);
  }

  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_RRAMC_address_location(address, &inst, &backend, &offset);

  *(uint32_t*)&backend->storage[offset] = value;
}

/**
 * Read from the RRAM array (RRAM or UICR)
 *
 * Where address is an address in either a "hard" RRAM address
 * (meaning in the 0x00..00 range)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_*_BASE)
 *
 * This operation is instantaneous in simulation.
 */
uint32_t nhw_RRAMC_read_word(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_RRAMC_address_location(address, &inst, &backend, &offset);

  return *(uint32_t*)&backend->storage[offset];
}

uint16_t nhw_RRAMC_read_halfword(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_RRAMC_address_location(address, &inst, &backend, &offset);

  return *(uint16_t*)&backend->storage[offset];
}

uint8_t nhw_RRAMC_read_byte(uint32_t address) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_RRAMC_address_location(address, &inst, &backend, &offset);

  return *(uint8_t*)&backend->storage[offset];
}

/**
 * memcpy from the RRAM/UICR array from <address> <size> bytes.
 *
 * Where address is an address in either a "hard" RRAM address
 * (meaning in the 0x00..00 range)
 * Or a "soft" address inside an UICR range (meaning an address gotten as an offset from
 * NRF_UICR_*_BASE)
 *
 * This is a convenience function in the model.
 * It can be used from DMA controller models or the like.
 *
 * This operation is instantaneous in simulation.
 * In real HW it is as "fast" as all those AHB accesses
 * can be dispatched by the interconnect and handled by
 * the RRAMC peripheral
 */
void nhw_RRAMC_read_buffer(void *dest, uint32_t address, size_t size) {
  int offset;
  nvm_storage_state_t *backend;
  uint inst;

  nhw_RRAMC_address_location(address, &inst, &backend, &offset);

  if (offset + size >= backend->size) {
    OUT_OF_RRAM_ERROR(address + size);
  }

  (void)memcpy(dest, &backend->storage[offset], size);
}

void* nhw_RRAMC_get_RRAM_base_address(uint inst) {
	return (void*)hw_rramc_st[inst].rram_st.storage;
}

NVM_BACKEND_PARAMS_CALLBACS(uicr, uicr[0])
NVM_BACKEND_PARAMS_CALLBACS(rram, rram[0])

static void RRAMC_register_cmd_args(void){
  for (int i = 0; i < NHW_RRAMC_UICR_TOTAL_INST; i++) {
    nvmc_args.uicr[i].in_ram = true;
    nvmc_args.rram[i].in_ram = true;
  }

  static bs_args_struct_t args_struct_toadd[] = {
    NVM_BACKEND_PARAMS(uicr, uicr[0], UICR),
    NVM_BACKEND_PARAMS(rram, rram[0], FLASH),
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(RRAMC_register_cmd_args, PRE_BOOT_1, 100);
