/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Factory information configuration registers
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

NRF_FICR_Type NRF_FICR_regs;

static void nhw_54l_ficr_init(void) {
  memset(&NRF_FICR_regs, 0xFF, sizeof(NRF_FICR_regs));

  NRF_FICR_regs.INFO.DEVICEID[0] = (bs_random_uint32() & 0xFFFFFF00) + bsim_args_get_global_device_nbr();
  NRF_FICR_regs.INFO.DEVICEID[1] = bs_random_uint32();
  //NRF_FICR_regs.INFO.PART = 0x5415;
  NRF_FICR_regs.INFO.RRAM = 0x5F4; /*1524 KB*/
  //NRF_FICR_regs.INFO.DEVICETYPE = 0;
  for (int i = 0; i < 4; i++) {
    NRF_FICR_regs.ER[i] = bs_random_uint32();
    NRF_FICR_regs.IR[i] = bs_random_uint32();
  }
  NRF_FICR_regs.DEVICEADDRTYPE = 0;
  NRF_FICR_regs.DEVICEADDR[0] = bs_random_uint32();
  NRF_FICR_regs.DEVICEADDR[1] = bs_random_uint32();
}

NSI_TASK(nhw_54l_ficr_init, HW_INIT, 100);
