/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "NRF_FICR.h"
#include "NRF_NVMC.h"
#include <string.h>
#include "bs_rand_main.h"
#include "weak_stubs.h"

/*
 * Factory information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ficr.html?cp=4_1_0_3_3
 */

/*
 * TOLOW: we start with most registers cleared, and a few evident ones set to some values.
 * It could be interesting to let people load them from command line
 * for example by specifying a file with their values. and/or by specifying particular ones
 * with command line options; something like -deviceaddr=<bt_address>
 */

NRF_FICR_Type NRF_FICR_regs;

void nrf_ficr_init(){
  memset(&NRF_FICR_regs, 0xFF, sizeof(NRF_FICR_regs));

  NRF_FICR_regs.CODEPAGESIZE = FLASH_PAGE_SIZE;
  NRF_FICR_regs.CODESIZE     = FLASH_N_PAGES;

  NRF_FICR_regs.DEVICEID[0] = (bs_random_uint32() & 0xFFFFFF00) + get_device_nbr();
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

void nrf_ficr_clean_up(){

}
