/*
 * Copyright (c) 2020 Oticon A/S
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_aar.h"
#include "bs_tracing.h"
#include "NRF_AAR.h"

void nrf_aar_int_enable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_aar_regw_sideeffects_INTENSET();
}

void nrf_aar_int_disable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_aar_regw_sideeffects_INTENCLR();
}

void nrf_aar_task_trigger(NRF_AAR_Type * p_reg, nrf_aar_task_t task)
{
  if (task == NRF_AAR_TASK_START) {
    p_reg->TASKS_START = 1;
    nrf_aar_regw_sideeffects_TASKS_START();
  } else if (task == NRF_AAR_TASK_STOP) {
    p_reg->TASKS_STOP = 1;
    nrf_aar_regw_sideeffects_TASKS_STOP();
  }  else {
    bs_trace_error_line_time("Not supported task started in nrf_aar\n");
  }
}
