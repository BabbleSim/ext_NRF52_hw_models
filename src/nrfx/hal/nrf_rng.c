/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_rng.h"
#include "bs_tracing.h"
#include "NRF_RNG.h"

void nrf_rng_task_trigger(NRF_RNG_Type * p_reg, nrf_rng_task_t rng_task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)rng_task)) = 0x1UL;

  if ( rng_task == NRF_RNG_TASK_START ) {
    NRF_RNG_regs.TASKS_START = 1;
    nrf_rng_regw_sideeffects_TASK_START();
  } else if ( rng_task == NRF_RNG_TASK_STOP ) {
    NRF_RNG_regs.TASKS_STOP = 1;
    nrf_rng_regw_sideeffects_TASK_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_rng\n");
  }
}

void nrf_rng_int_enable(NRF_RNG_Type * p_reg, uint32_t mask)
{
  NRF_RNG_regs.INTENSET = mask;
  nrf_rng_regw_sideeffects_INTENSET();
}

void nrf_rng_int_disable(NRF_RNG_Type * p_reg, uint32_t mask)
{
  NRF_RNG_regs.INTENCLR = mask;
  nrf_rng_regw_sideeffects_INTENCLEAR();
}
