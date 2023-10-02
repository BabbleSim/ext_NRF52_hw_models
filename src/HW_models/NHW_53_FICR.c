/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Factory information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/ficr/doc/ficr.html?cp=4_0_0_4_3_1
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/ficr.network/doc/ficr.network.html?cp=4_0_0_5_3_0
 */

/*
 * TOLOW: we start with most registers cleared, and a few evident ones set to some values.
 * It could be interesting to let people load them from command line
 * for example by specifying a file with their values. and/or by specifying particular ones
 * with command line options; something like -deviceaddr=<bt_address>
 */

#include <string.h>
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "weak_stubs.h"

static NRF_FICR_APP_Type NRF_FICR_APP_regs;
static NRF_FICR_NET_Type NRF_FICR_NET_regs;

void *NRF_FICR_regs_p[] = {(void*)&NRF_FICR_APP_regs, (void*)&NRF_FICR_NET_regs};

static void nhw_53_ficr_init(void) {
  memset(&NRF_FICR_APP_regs, 0xFF, sizeof(NRF_FICR_APP_regs));
  memset(&NRF_FICR_NET_regs, 0xFF, sizeof(NRF_FICR_NET_regs));

  NRF_FICR_APP_regs.INFO.DEVICEID[0] = (bs_random_uint32() & 0xFFFFFF00) + bsim_args_get_global_device_nbr();
  NRF_FICR_APP_regs.INFO.DEVICEID[1] = bs_random_uint32();
  NRF_FICR_APP_regs.INFO.PART = 0x5340;
  NRF_FICR_APP_regs.INFO.FLASH = 0x400; /*1 MB*/
  NRF_FICR_APP_regs.INFO.CODEPAGESIZE = 0x1000;
  NRF_FICR_APP_regs.INFO.CODESIZE = 256;
  NRF_FICR_APP_regs.INFO.DEVICETYPE = 0;

  NRF_FICR_NET_regs.INFO.DEVICEID[0] = NRF_FICR_APP_regs.INFO.DEVICEID[0];
  NRF_FICR_NET_regs.INFO.DEVICEID[1] = NRF_FICR_APP_regs.INFO.DEVICEID[1];
  NRF_FICR_NET_regs.INFO.PART = 0x5340;
  NRF_FICR_NET_regs.INFO.FLASH = 0x100; /*256 KB*/
  NRF_FICR_NET_regs.INFO.CODEPAGESIZE = 0x800;
  NRF_FICR_NET_regs.INFO.CODESIZE = 128;
  NRF_FICR_NET_regs.INFO.DEVICETYPE = 0;
  for (int i = 0; i < 4; i++) {
    NRF_FICR_NET_regs.ER[i] = bs_random_uint32();
    NRF_FICR_NET_regs.IR[i] = bs_random_uint32();
  }
  NRF_FICR_NET_regs.DEVICEADDRTYPE = 0;
  NRF_FICR_NET_regs.DEVICEADDR[0] = bs_random_uint32();
  NRF_FICR_NET_regs.DEVICEADDR[1] = bs_random_uint32();
}

NSI_TASK(nhw_53_ficr_init, HW_INIT, 100);
