/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_rng.h"
#include "bs_tracing.h"
#include "NRF_RNG.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

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

#if defined(DPPI_PRESENT)
void nrf_rng_subscriber_add(nrf_rng_task_t task, uint8_t channel)
{
  switch(task)
  {
    case NRF_RNG_TASK_START:
      nrf_dppi_subscriber_add(channel, nrf_rng_task_start);
      break;
    case NRF_RNG_TASK_STOP:
      nrf_dppi_subscriber_add(channel, nrf_rng_task_stop);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RNG: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_rng_subscriber_remove(nrf_rng_task_t task)
{
  switch(task)
  {
    case NRF_RNG_TASK_START:
      nrf_dppi_subscriber_remove(nrf_rng_task_start);
      break;
    case NRF_RNG_TASK_STOP:
      nrf_dppi_subscriber_remove(nrf_rng_task_stop);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RNG: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

void nrf_rng_subscribe_set(NRF_RNG_Type * p_reg,
                           nrf_rng_task_t task,
                           uint8_t        channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | RNG_SUBSCRIBE_START_EN_Msk);
  nrf_rng_subscriber_add(task, channel);
}


void nrf_rng_subscribe_clear(NRF_RNG_Type * p_reg,
                             nrf_rng_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_rng_subscriber_remove(task);
}
#endif // defined(DPPI_PRESENT)
