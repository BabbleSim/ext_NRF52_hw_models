/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_radio.h"
#include "bs_tracing.h"
#include "NRF_RADIO.h"

void nrf_radio_task_trigger(NRF_RADIO_Type * p_reg, nrf_radio_task_t task)
{
  if ( task == NRF_RADIO_TASK_TXEN ) {
    p_reg->TASKS_TXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_TXEN();
  } else if ( task == NRF_RADIO_TASK_RXEN ) {
    p_reg->TASKS_RXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_RXEN();
  } else if ( task == NRF_RADIO_TASK_DISABLE ) {
    p_reg->TASKS_DISABLE = 1;
    nrf_radio_regw_sideeffects_TASKS_DISABLE();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_radio\n");
  }
}

void nrf_radio_int_enable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_radio_regw_sideeffects_INTENSET();
}

void nrf_radio_bcc_set(NRF_RADIO_Type * p_reg, uint32_t radio_bcc)
{
  p_reg->BCC = radio_bcc;
  nrf_radio_regw_sideeffects_BCC();
}

void nrf_radio_power_set(NRF_RADIO_Type * p_reg, bool radio_power)
{
  p_reg->POWER = (uint32_t) radio_power;
  nrf_radio_regw_sideeffects_POWER();
}
