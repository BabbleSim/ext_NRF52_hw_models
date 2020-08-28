/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ccm.h"
#include "bs_tracing.h"
#include "NRF_AES_CCM.h"

void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg, nrf_ccm_task_t task)
{
  if ( task == NRF_CCM_TASK_KSGEN ) {
    p_reg->TASKS_KSGEN = 1;
    nrf_ccm_regw_sideeffects_TASKS_KSGEN();
  } else if ( task == NRF_CCM_TASK_STOP ) {
    p_reg->TASKS_STOP = 1;
    nrf_ccm_regw_sideeffects_TASKS_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ccm\n");
  }
}

void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_ccm_regw_sideeffects_INTENSET();
}

void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_ccm_regw_sideeffects_INTENCLR();
}

