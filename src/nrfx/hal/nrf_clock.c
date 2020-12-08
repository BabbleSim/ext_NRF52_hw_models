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
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

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

uint32_t nrf_clock_int_enable_check(NRF_CLOCK_Type const * p_reg, uint32_t mask)
{
  /* Note that unlike the real NRF HW, INTENCLR is always
   * reset to 0 by the HW models */
  return p_reg->INTENSET & mask;
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
  } else {
    bs_trace_warning_line_time("Not supported task started in nrf_clock, %d\n", task);
  }
}


#if defined(DPPI_PRESENT)
void nrf_clock_subscriber_add(nrf_clock_task_t task, uint8_t channel)
{
  switch(task) {
    case NRF_CLOCK_TASK_HFCLKSTART:
      nrf_dppi_subscriber_add(channel, nrf_clock_TASKS_HFCLKSTART);
      break;
    case NRF_CLOCK_TASK_HFCLKSTOP:
      nrf_dppi_subscriber_add(channel, nrf_clock_TASKS_HFCLKSTOP);
      break;
    case NRF_CLOCK_TASK_LFCLKSTART:
      nrf_dppi_subscriber_add(channel, nrf_clock_TASKS_LFCLKSTART);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_CLOCK: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_clock_subscriber_remove(nrf_clock_task_t task)
{
  switch(task) {
    case NRF_CLOCK_TASK_HFCLKSTART:
      nrf_dppi_subscriber_remove(nrf_clock_TASKS_HFCLKSTART);
      break;
    case NRF_CLOCK_TASK_HFCLKSTOP:
      nrf_dppi_subscriber_remove(nrf_clock_TASKS_HFCLKSTOP);
      break;
    case NRF_CLOCK_TASK_LFCLKSTART:
      nrf_dppi_subscriber_remove(nrf_clock_TASKS_LFCLKSTART);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_CLOCK: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

void nrf_clock_subscribe_set(NRF_CLOCK_Type * p_reg,
                             nrf_clock_task_t task,
                             uint8_t          channel)
{
  *((volatile uint32_t *) ((uint8_t *)p_reg+ (uint32_t)task + 0x80uL)) =
          ((uint32_t)channel | CLOCK_SUBSCRIBE_HFCLKSTART_EN_Msk);
  nrf_clock_subscriber_add(task, channel);
}

void nrf_clock_subscribe_clear(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *)p_reg + (uint32_t)task + 0x80uL)) = 0;
  nrf_clock_subscriber_remove(task);
}

#endif // defined(DPPI_PRESENT)
