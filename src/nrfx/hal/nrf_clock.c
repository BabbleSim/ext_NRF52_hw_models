/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_clock.h"
#include "bs_tracing.h"
#include "NRF_CLOCK.h"

void nrf_clock_int_enable(NRF_CLOCK_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_clock_reqw_sideeffects_INTENSET();
}

void nrf_clock_int_disable(NRF_CLOCK_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_clock_reqw_sideeffects_INTENCLR();
}

void nrf_clock_task_trigger(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task)
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
  } else if (task == NRF_CLOCK_TASK_LFCLKSTOP) {
    NRF_CLOCK_regs.TASKS_LFCLKSTOP = 1;
    nrf_clock_reqw_sideeffects_TASKS_LFCLKSTOP();
#if NRF_CLOCK_HAS_CALIBRATION
  } else if (task == NRF_CLOCK_TASK_CAL) {
    NRF_CLOCK_regs.TASKS_CAL = 1;
    nrf_clock_reqw_sideeffects_TASKS_CAL();
#endif
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
  } else if (task == NRF_CLOCK_TASK_CTSTART) {
    NRF_CLOCK_regs.TASKS_CTSTART = 1;
    nrf_clock_reqw_sideeffects_TASKS_CTSTART();
  } else if (task == NRF_CLOCK_TASK_CTSTOP) {
    NRF_CLOCK_regs.TASKS_CTSTOP = 1;
    nrf_clock_reqw_sideeffects_TASKS_CTSTOP();
#endif
  } else {
    bs_trace_warning_line_time("Not supported task started in nrf_clock, %d\n", task);
  }
}

NRF_STATIC_INLINE void nrf_clock_event_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_clock_regw_sideeffects_EVENTS_all();
}

