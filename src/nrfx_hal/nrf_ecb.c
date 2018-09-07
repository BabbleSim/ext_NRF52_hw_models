/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_ecb.h"
#include "bs_tracing.h"
#include "NRF_AES_ECB.h"

void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task)
{
  if ( task == NRF_ECB_TASK_STARTECB ) {
    p_reg->TASKS_STARTECB = 1;
    nrf_ecb_regw_sideeffects_TASKS_STARTECB();
  } else if ( task == NRF_ECB_TASK_STOPECB ) {
    p_reg->TASKS_STOPECB = 1;
    nrf_ecb_regw_sideeffects_TASKS_STOPECB();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ecb\n");
  }

}

void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_ecb_regw_sideeffects_INTENSET();
}
