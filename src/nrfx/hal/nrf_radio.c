/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_radio.h"
#include "bs_tracing.h"
#include "NRF_RADIO.h"

void nrf_radio_task_trigger(NRF_RADIO_Type * p_reg, nrf_radio_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

#define CASE_CALL_SIDEEFFECT(x) \
  case NRF_RADIO_TASK_##x :\
    nrf_radio_regw_sideeffects_TASKS_##x();\
    break

  switch (task) {
    CASE_CALL_SIDEEFFECT(TXEN);
    CASE_CALL_SIDEEFFECT(RXEN);
    CASE_CALL_SIDEEFFECT(START);
    CASE_CALL_SIDEEFFECT(STOP);
    CASE_CALL_SIDEEFFECT(DISABLE);
    CASE_CALL_SIDEEFFECT(RSSISTART);
    CASE_CALL_SIDEEFFECT(RSSISTOP);
    CASE_CALL_SIDEEFFECT(BCSTART);
    CASE_CALL_SIDEEFFECT(BCSTOP);
    CASE_CALL_SIDEEFFECT(EDSTART);
    CASE_CALL_SIDEEFFECT(EDSTOP);
    CASE_CALL_SIDEEFFECT(CCASTART);
    CASE_CALL_SIDEEFFECT(CCASTOP);
    default:
      bs_trace_error_line_time("%s: Not supported task %i started\n", __func__, task);
      break;
  }
}

void nrf_radio_int_enable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_radio_regw_sideeffects_INTENSET();
}

void nrf_radio_int_disable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_radio_regw_sideeffects_INTENCLR();
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
