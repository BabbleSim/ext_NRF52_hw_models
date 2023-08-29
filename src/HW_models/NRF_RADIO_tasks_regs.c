/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes the RADIO TASKS registers side-effecting functions
 * The actual tasks logic does not below here.
 */

#include "NHW_types.h"
#include "NRF_RADIO.h"
#include "bs_tracing.h"


#define RADIO_TASK_SIDEFFECT_FUNC(x) \
void nrf_radio_regw_sideeffects_TASKS_##x() { \
  if (NRF_RADIO_regs.TASKS_##x ) { \
    NRF_RADIO_regs.TASKS_##x = 0; \
    nrf_radio_tasks_##x(); \
  }\
}

RADIO_TASK_SIDEFFECT_FUNC(TXEN)
RADIO_TASK_SIDEFFECT_FUNC(RXEN)
RADIO_TASK_SIDEFFECT_FUNC(START)
RADIO_TASK_SIDEFFECT_FUNC(STOP)
RADIO_TASK_SIDEFFECT_FUNC(DISABLE)

void nrf_radio_regw_sideeffects_TASKS_RSSISTART(void) {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTART ){
    NRF_RADIO_regs.TASKS_RSSISTART = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTART register is not supported by the model yet\n");
  }
}

void nrf_radio_regw_sideeffects_TASKS_RSSISTOP(void) {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTOP ){
    NRF_RADIO_regs.TASKS_RSSISTOP = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTOP register is not supported by the model yet\n");
  }
}

RADIO_TASK_SIDEFFECT_FUNC(BCSTART)
RADIO_TASK_SIDEFFECT_FUNC(BCSTOP)
RADIO_TASK_SIDEFFECT_FUNC(EDSTART)
RADIO_TASK_SIDEFFECT_FUNC(EDSTOP)
RADIO_TASK_SIDEFFECT_FUNC(CCASTART)
RADIO_TASK_SIDEFFECT_FUNC(CCASTOP)

