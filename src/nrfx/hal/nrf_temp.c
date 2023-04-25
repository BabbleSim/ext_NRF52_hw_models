/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_temp.h"
#include "bs_tracing.h"
#include "NRF_TEMP.h"

void nrf_temp_task_trigger(NRF_TEMP_Type * p_reg, nrf_temp_task_t temp_task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)temp_task)) = 0x1UL;

  if ( temp_task == NRF_TEMP_TASK_START ) {
    NRF_TEMP_regs.TASKS_START = 1;
    nrf_temp_regw_sideeffects_TASK_START();
  } else if ( temp_task == NRF_TEMP_TASK_STOP ) {
    NRF_TEMP_regs.TASKS_STOP = 1;
    nrf_temp_regw_sideeffects_TASK_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in %s\n", __func__);
  }
}

void nrf_temp_int_enable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
  NRF_TEMP_regs.INTENSET = mask;
  nrf_temp_regw_sideeffects_INTENSET();
}

void nrf_temp_int_disable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
  NRF_TEMP_regs.INTENCLR = mask;
  nrf_temp_regw_sideeffects_INTENCLEAR();
}
