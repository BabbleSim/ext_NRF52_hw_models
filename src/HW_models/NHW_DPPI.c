/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <stdint.h>
#include "NHW_DPPI.h"

/*
 * Subscribe a peripheral to a DPPI channel
 *
 * dppi_inst selects the DPPI instance
 * channel_nbr the channel in that DPPI
 * callback is the function which will be called when the DPPI event is received
 * param is an arbitrary void pointer, which will be passed to the callback
 */
void nhw_dppi_channel_subscribe(uint dppi_inst, uint channel_nbr, dppi_callback_t callback, void *param) {

}

void nhw_dppi_event_signal(uint dppi_inst, uint channel_nbr) {

}

/*
 * If the publish_register is enabled,
 * call nhw_dppi_event_signal() for the configured CHIDX
 */
void nhw_dppi_event_signal_if(uint dppi_inst, uint32_t publish_reg) {
  uint chidx;

  if (publish_reg & (0x1UL << 31) == 0){
    return;
  }

  chidx = publish_reg & 0xFFUL;

  nhw_dppi_event_signal(dppi_inst, chidx);
}
