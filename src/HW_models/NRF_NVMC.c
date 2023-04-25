/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "bs_tracing.h"
#include "bs_cmd_line.h"
#include "bs_oswrap.h"
#include "NRF_NVMC.h"
#include "NRF_HW_model_top.h"
#include "time_machine_if.h"
#include "weak_stubs.h"

/*
 * NVMC — Non-volatile memory controller
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/nvmc.html?cp=5_1_0_3_2
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

NRF_UICR_Type *NRF_UICR_regs_p;
NRF_NVMC_Type NRF_NVMC_regs = {0};
bs_time_t Timer_NVMC = TIME_NEVER; //Time when the next flash operation will be completed

typedef struct {
  uint8_t *storage;
  const char *file_path;
  const char *type_s;
  int fd;
  size_t size;
  bool erase_at_start;
  bool rm_at_exit;
  bool in_ram;
} storage_state_t;

static storage_state_t flash_st;
static storage_state_t uicr_st;
static enum flash_op_t {flash_idle = 0, flash_write, flash_erase, flash_erase_partial, flash_erase_uicr, flash_erase_all} flash_op;
static uint32_t erase_address;
static bs_time_t time_under_erase[FLASH_N_PAGES];

static bs_time_t flash_t_eraseall  = 173000;
static bs_time_t flash_t_erasepage =  87500;
static bs_time_t flash_t_write     =     42;
static double    flash_partial_erase_factor = 1.0; //actual tERASEPAGEPARTIAL,acc for this given device

struct nvmc_args_t {
  char *uicr_file;
  bool uicr_erase;
  bool uicr_rm;
  bool uicr_in_ram;
  char *flash_file;
  bool flash_erase;
  bool flash_rm;
  bool flash_in_ram;
} nvmc_args;

static void nvmc_initialize_data_storage();
static void nvmc_clear_storage();
static void nvmc_register_cmd_args();

void nrfhw_nvmc_uicr_pre_init(void){
  nvmc_register_cmd_args();
}

/**
 * Initialize the NVMC and UICR models
 */
void nrfhw_nvmc_uicr_init(){
  memset(&NRF_NVMC_regs, 0x00, sizeof(NRF_NVMC_regs));
  NRF_NVMC_regs.READY = 1;
  NRF_NVMC_regs.READYNEXT = 1;
  NRF_NVMC_regs.ERASEPAGEPARTIALCFG = 0x0000000A;

  flash_op = flash_idle;
  Timer_NVMC = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();

  flash_st.file_path      = nvmc_args.flash_file;
  flash_st.erase_at_start = nvmc_args.flash_erase;
  flash_st.rm_at_exit     = nvmc_args.flash_rm;
  flash_st.in_ram         = nvmc_args.flash_in_ram;
  flash_st.size           = FLASH_SIZE;
  flash_st.type_s         = "flash";

  uicr_st.file_path      = nvmc_args.uicr_file;
  uicr_st.erase_at_start = nvmc_args.uicr_erase;
  uicr_st.rm_at_exit     = nvmc_args.uicr_rm;
  uicr_st.in_ram         = nvmc_args.uicr_in_ram;
  uicr_st.size           = sizeof(NRF_UICR_Type);
  // We book more than the actual flash area, to avoid segfaults if
  // somebody tries to access the HW control registers
  uicr_st.type_s         = "UICR";

  nvmc_initialize_data_storage(&flash_st);
  nvmc_initialize_data_storage(&uicr_st);
  NRF_UICR_regs_p = (NRF_UICR_Type *)uicr_st.storage;
}

/**
 * Clean up the NVMC and UICR model before program exit
 */
void nrfhw_nvmc_uicr_clean_up(){
  nvmc_clear_storage(&flash_st);
  nvmc_clear_storage(&uicr_st);
}

/*
 * Complete the actual erase of a flash page
 */
static void nrfhw_nvmc_complete_erase(void){
  uint base_address = erase_address/FLASH_PAGE_SIZE*FLASH_PAGE_SIZE;

  memset(&flash_st.storage[base_address], 0xFF, FLASH_PAGE_SIZE);

  time_under_erase[erase_address/FLASH_PAGE_SIZE] = 0;
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
}

/**
 * Time has come when the programmed flash operation has finished
 */
void nrfhw_nvmc_timer_triggered(void){

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
  nrf_hw_find_next_timer_to_trigger();
}

bs_time_t nrfhw_nvmc_time_to_ready(void) {
  if (NRF_NVMC_regs.READY) {
    return 0;
  } else {
    return Timer_NVMC - tm_get_hw_time();
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
  if (time_under_erase[addr/FLASH_PAGE_SIZE] > 0){ \
    bs_trace_warning_line_time("%s: %s in partially erased address (%u)\n", \
         __func__, type, addr); \
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
  Timer_NVMC = tm_get_hw_time() + flash_t_erasepage;
  nrf_hw_find_next_timer_to_trigger();
}

/* Note ERASEPCR1 is an alias to ERASEPAGE (same register) */
void nrfhw_nvmc_regw_sideeffects_ERASEPAGE(){
  nrfhw_nvmc_erase_page(NRF_NVMC_regs.ERASEPAGE);
}

void nrf_nvmc_regw_sideeffects_ERASEPCR0(){
  nrfhw_nvmc_erase_page(NRF_NVMC_regs.ERASEPCR0);
}

void nrfhw_nvmc_regw_sideeffects_ERASEUICR(){
  NRF_NVMC_regs.ERASEUICR &= 1;

  if (NRF_NVMC_regs.ERASEUICR) {
    NRF_NVMC_regs.ERASEUICR = 0;
    ERASE_ENABLED_CHECK("ERASEUICR");
    BUSY_CHECK("ERASEUICR");
    flash_op = flash_erase_uicr;
    NRF_NVMC_regs.READY = 0;
    NRF_NVMC_regs.READYNEXT = 0;
    Timer_NVMC = tm_get_hw_time() + flash_t_erasepage;
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrfhw_nvmc_regw_sideeffects_ERASEALL(){
  NRF_NVMC_regs.ERASEALL &= 1;

  if (NRF_NVMC_regs.ERASEALL) {
    NRF_NVMC_regs.ERASEALL = 0;
    ERASE_ENABLED_CHECK("ERASEALL");
    BUSY_CHECK("ERASEALL");
    flash_op = flash_erase_all;
    NRF_NVMC_regs.READY = 0;
    NRF_NVMC_regs.READYNEXT = 0;
    Timer_NVMC = tm_get_hw_time() + flash_t_eraseall;
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrfhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(){
  ERASE_ENABLED_CHECK("ERASEPARTIAL");
  BUSY_CHECK("ERASEPARTIAL");
  erase_address = NRF_NVMC_regs.ERASEPAGEPARTIAL;
  CHECK_ERASE_ADDRESS(erase_address);

  flash_op = flash_erase_partial;
  NRF_NVMC_regs.READY = 0;
  NRF_NVMC_regs.READYNEXT = 0;

  bs_time_t duration = flash_partial_erase_factor * NRF_NVMC_regs.ERASEPAGEPARTIALCFG * 1000;
  time_under_erase[erase_address/FLASH_PAGE_SIZE] += duration;
  Timer_NVMC = tm_get_hw_time() + duration;
  nrf_hw_find_next_timer_to_trigger();
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

  Timer_NVMC = tm_get_hw_time() + flash_t_write;
  nrf_hw_find_next_timer_to_trigger();
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
uint32_t nrfhw_nmvc_flash_read_word(uint32_t address) {

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

/**
 * Before exiting the program, do whatever is
 * necessary for a given storage (free'ing heap,
 * closing file descriptors, etc)
 */
static void nvmc_clear_storage(storage_state_t *st){
  if (st->in_ram == true) {
    if (st->storage != NULL) {
      free(st->storage);
    }
    return;
  }

  if ((st->storage != MAP_FAILED) && (st->storage != NULL)) {
    munmap(st->storage, st->size);
  }

  if (st->fd != -1) {
    close(st->fd);
  }

  if ((st->rm_at_exit == true) && (st->file_path != NULL)) {
    /* We try to remove the file but do not error out if we can't */
    (void) remove(st->file_path);
  }
}

/**
 * At boot, do whatever is necessary for a given storage
 * (allocate memory, open files etc. )
 */
static void nvmc_initialize_data_storage(storage_state_t *st){
  struct stat f_stat;
  int rc;

  st->fd = -1;
  st->storage = NULL;

  if (st->in_ram == true) {
    st->storage = (uint8_t *)bs_malloc(st->size);

  } else {

    st->fd = open(st->file_path, O_RDWR | O_CREAT, (mode_t)0600);
    if (st->fd == -1) {
      bs_trace_error_line("%s: Failed to open %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    rc = fstat(st->fd, &f_stat);
    if (rc) {
      bs_trace_error_line("%s: Failed to get status of %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    if (ftruncate(st->fd, st->size) == -1) {
      bs_trace_error_line("%s: Failed to resize %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    st->storage = mmap(NULL, st->size, PROT_WRITE | PROT_READ, MAP_SHARED, st->fd, 0);
    if (st->storage == MAP_FAILED) {
      bs_trace_error_line("%s: Failed to mmap %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }
  }

  if ((st->erase_at_start == true) || (st->in_ram == true) || (f_stat.st_size == 0)) {
    /* Erase the memory unit by pulling all bits to the configured erase value */
    (void)memset(st->storage, 0xFF, st->size);
  }
}

static void arg_uicr_file_found(char *argv, int offset){
  nvmc_args.uicr_in_ram = false;
}

static void arg_flash_file_found(char *argv, int offset){
  nvmc_args.flash_in_ram = false;
}

static void arg_uicr_in_ram_found(char *argv, int offset){
  nvmc_args.uicr_in_ram = true;
}

static void arg_flash_in_ram_found(char *argv, int offset){
  nvmc_args.flash_in_ram = true;
}

static void nvmc_register_cmd_args(void){

  nvmc_args.uicr_in_ram = true;
  nvmc_args.flash_in_ram = true;

  static bs_args_struct_t args_struct_toadd[] = {
  { .is_switch = true,
    .option = "uicr_erase",
    .type = 'b',
    .dest = (void*)&nvmc_args.uicr_erase,
    .descript = "Reset all UICR registers to their erase values (0xFF) at boot"
  },
  { .option ="uicr_file",
    .name = "path",
    .type = 's',
    .dest = (void*)&nvmc_args.uicr_file,
    .call_when_found = arg_uicr_file_found,
    .descript = "Path to binary file where the UICR registers are stored (if set, toggles uicr_in_ram to false)"
  },
  { .is_switch = true,
    .option ="uicr_rm",
    .type = 'b',
    .dest = (void*)&nvmc_args.uicr_rm,
    .descript = "Remove the UICR file when terminating the execution (default no)"
  },
  { .is_switch = true,
    .option ="uicr_in_ram",
    .type = 'b',
    .call_when_found = arg_uicr_in_ram_found,
    .descript = "(default)  Instead of a file, keep the UICR content in RAM. If this is "
          "set uicr_erase, uicr_file & uicr_rm are ignored, and the UICR content is always reset at startup"
  },
  { .is_switch = true,
    .option ="flash_erase",
    .type = 'b',
    .dest = (void*)&nvmc_args.flash_erase,
    .descript = "Reset the flash whole flash to its erase values (0xFF) at boot"
  },
  { .option ="flash_file",
    .name = "path",
    .type = 's',
    .dest = (void*)&nvmc_args.flash_file,
    .call_when_found = arg_flash_file_found,
    .descript = "Path to binary file where the flash content is stored (if set, toggles flash_in_ram to false)"
  },
  { .option ="flash",
    .name = "path",
    .type = 's',
    .dest = (void*)&nvmc_args.flash_file,
    .call_when_found = arg_flash_file_found,
    .descript = "Alias for flash_file"
  },
  { .is_switch = true,
    .option = "flash_rm",
    .type = 'b',
    .dest = (void*)&nvmc_args.flash_rm,
    .descript = "Remove the flash file when terminating the execution (default no)"
  },
  { .is_switch = true,
    .option = "flash_in_ram",
    .type = 'b',
    .call_when_found = arg_flash_in_ram_found,
    .descript = "(default) Instead of a file, keep the flash content in RAM. If this is "
           "set flash_erase, flash_file & flash_rm are ignored, and the flash content is always reset at startup"
  },
  ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}
