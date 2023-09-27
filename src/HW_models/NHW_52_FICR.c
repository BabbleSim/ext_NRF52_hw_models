/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Factory information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ficr.html?cp=5_1_0_3_3
 */

/*
 * TOLOW: we start with most registers cleared, and a few evident ones set to some values.
 * It could be interesting to let people load them from command line
 * for example by specifying a file with their values. and/or by specifying particular ones
 * with command line options; something like -deviceaddr=<bt_address>
 */

#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include <string.h>
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "weak_stubs.h"

NRF_FICR_Type NRF_FICR_regs;

static void nhw_52_ficr_init(void) {
  memset(&NRF_FICR_regs, 0xFF, sizeof(NRF_FICR_regs));

#define FLASH_PAGE_SIZE (4*1024)
#define FLASH_N_PAGES    128
#define FLASH_SIZE (FLASH_N_PAGES*FLASH_PAGE_SIZE)

  NRF_FICR_regs.CODEPAGESIZE = FLASH_PAGE_SIZE;
  NRF_FICR_regs.CODESIZE     = FLASH_N_PAGES;

  NRF_FICR_regs.DEVICEID[0] = (bs_random_uint32() & 0xFFFFFF00) + bsim_args_get_global_device_nbr();
  NRF_FICR_regs.DEVICEID[1] = bs_random_uint32();
  NRF_FICR_regs.DEVICEADDRTYPE = 0;
  NRF_FICR_regs.DEVICEADDR[0] = bs_random_uint32();
  NRF_FICR_regs.DEVICEADDR[1] = bs_random_uint32();

  for (int i = 0 ; i < 4; i++) {
    NRF_FICR_regs.ER[i] = bs_random_uint32();
    NRF_FICR_regs.IR[i] = bs_random_uint32();
  }

  NRF_FICR_regs.INFO.PART = 0x52833;
  NRF_FICR_regs.INFO.VARIANT = 0x41414230;
  NRF_FICR_regs.PRODTEST[0] = 0xBB42319F;
  NRF_FICR_regs.PRODTEST[1] = 0xBB42319F;
  NRF_FICR_regs.PRODTEST[2] = 0xBB42319F;
}

NSI_TASK(nhw_52_ficr_init, HW_INIT, 100);
