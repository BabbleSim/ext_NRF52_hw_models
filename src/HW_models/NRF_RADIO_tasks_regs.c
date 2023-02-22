/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes the RADIO TASKS registers side-effecting functions
 * The actual tasks logic does not below here.
 */

#include "NRF_RADIO.h"
#include "bs_tracing.h"

void nrf_radio_regw_sideeffects_TASKS_BCSTART() {
  if (NRF_RADIO_regs.TASKS_BCSTART) {
    NRF_RADIO_regs.TASKS_BCSTART = 0;
    nrf_radio_tasks_bcstart();
  }
}

void nrf_radio_regw_sideeffects_TASKS_BCSTOP() {
  if (NRF_RADIO_regs.TASKS_BCSTOP) {
    NRF_RADIO_regs.TASKS_BCSTOP = 0;
    nrf_radio_tasks_bcstop();
  }
}

void nrf_radio_regw_sideeffects_TASKS_RXEN(){
  if ( NRF_RADIO_regs.TASKS_RXEN ){
    NRF_RADIO_regs.TASKS_RXEN = 0;
    nrf_radio_tasks_rxen();
  }
}

void nrf_radio_regw_sideeffects_TASKS_TXEN(){
  if ( NRF_RADIO_regs.TASKS_TXEN ){
    NRF_RADIO_regs.TASKS_TXEN = 0;
    nrf_radio_tasks_txen();
  }
}

void nrf_radio_regw_sideeffects_TASKS_DISABLE(){
  if ( NRF_RADIO_regs.TASKS_DISABLE ){
    NRF_RADIO_regs.TASKS_DISABLE = 0;
    nrf_radio_tasks_disable();
  }
}

void nrf_radio_regw_sideeffects_TASKS_RSSISTART() {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTART ){
    NRF_RADIO_regs.TASKS_RSSISTART = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTART register is not supported by the model yet\n");
  }
}

void nrf_radio_regw_sideeffects_TASKS_RSSISTOP() {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTOP ){
    NRF_RADIO_regs.TASKS_RSSISTOP = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTOP register is not supported by the model yet\n");
  }
}
