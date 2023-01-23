/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "NRF_FICR.h"
#include <string.h>

/*
 * Factory information configuration registers
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ficr.html?cp=4_1_0_3_3
 */

//TOLOW: we start with all registers just as 0. It could be interesting to let people load them from command line
// for example by specifying a file with their values. and/or by specifying particular ones with command line options
// something like -deviceaddr=<bt_address>
NRF_FICR_Type NRF_FICR_regs;

void nrf_ficr_init(){
  memset(&NRF_FICR_regs, 0, sizeof(NRF_FICR_regs));
}

void nrf_ficr_clean_up(){

}
