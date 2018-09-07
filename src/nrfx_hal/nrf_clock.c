/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_clock.h"
#include "bs_tracing.h"
#include "NRF_CLOCK.h"

void nrf_clock_int_enable(uint32_t int_mask)
{
  NRF_CLOCK->INTENSET = int_mask;
  nrf_clock_reqw_sideeffects_INTENSET();
}

void nrf_clock_int_disable(uint32_t int_mask)
{
  NRF_CLOCK->INTENCLR = int_mask;
  nrf_clock_reqw_sideeffects_INTENCLR();
}

void nrf_clock_task_trigger(nrf_clock_task_t task)
{
  if (task == NRF_CLOCK_TASK_HFCLKSTART) {
    NRF_CLOCK_regs.TASKS_HFCLKSTART = 1;
    nrf_clock_reqw_sideeffects_TASKS_HFCLKSTART();
  } else if (task == NRF_CLOCK_TASK_HFCLKSTOP) {
    NRF_CLOCK_regs.TASKS_HFCLKSTOP = 1;
    nrf_clock_reqw_sideeffects_TASKS_HFCLKSTOP();
  } else if (task == NRF_CLOCK_TASK_LFCLKSTART) {
    NRF_CLOCK_regs.TASKS_LFCLKSTART = 1;
    nrf_clock_reqw_sideeffects_TASKS_LFCLKSTART();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_clock\n");
  }
}
