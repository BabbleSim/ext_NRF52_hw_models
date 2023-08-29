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

extern NRF_RNG_Type NRF_RNG_regs;

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

void nrf_rng_event_clear(NRF_RNG_Type * p_reg, nrf_rng_event_t rng_event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)rng_event)) = 0x0UL;
  nrf_rng_regw_sideeffects_EVENTS_all();
}

#if defined(DPPI_PRESENT)

static void nrf_rng_subscribe_common(NRF_RNG_Type * p_reg,
                                     nrf_rng_task_t task)
{
  if (task == NRF_RNG_TASK_START) {
      nrf_rng_regw_sideeffects_SUBSCRIBE_START(0);
  } else if ( task == NRF_RNG_TASK_STOP ) {
      nrf_rng_regw_sideeffects_SUBSCRIBE_STOP(0);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to an not-supported task in the nrf_rng (%i)\n",
                               task);
  }
}

void nrf_rng_subscribe_set(NRF_RNG_Type * p_reg,
                           nrf_rng_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_rng_subscribe_common(p_reg, task);
}

void nrf_rng_subscribe_clear(NRF_RNG_Type * p_reg,
                                               nrf_rng_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_rng_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
