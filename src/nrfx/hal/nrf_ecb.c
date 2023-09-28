/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ecb.h"
#include "bs_tracing.h"
#include "NHW_AES_ECB.h"

void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  if ( task == NRF_ECB_TASK_STARTECB ) {
    nhw_ECB_regw_sideeffects_TASKS_STARTECB();
  } else if ( task == NRF_ECB_TASK_STOPECB ) {
    nhw_ECB_regw_sideeffects_TASKS_STOPECB();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ecb\n");
  }
}

void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nhw_ECB_regw_sideeffects_INTENSET();
}

void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nhw_ECB_regw_sideeffects_INTENCLR();
}

void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_ECB_regw_sideeffects_EVENTS_all(0);
}

#if defined(DPPI_PRESENT)

static void nrf_ecb_subscribe_common(NRF_ECB_Type * p_reg,
                                     nrf_ecb_task_t task)
{
  if (task == NRF_ECB_TASK_STARTECB) {
      nhw_ECB_regw_sideeffects_SUBSCRIBE_STARTECB(0);
  } else if ( task == NRF_ECB_TASK_STOPECB ) {
      nhw_ECB_regw_sideeffects_SUBSCRIBE_STOPECB(0);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to an not-supported task in the nrf_ecb (%i)\n",
                               task);
  }
}

void nrf_ecb_subscribe_set(NRF_ECB_Type * p_reg,
                           nrf_ecb_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_ecb_subscribe_common(p_reg, task);
}

void nrf_ecb_subscribe_clear(NRF_ECB_Type * p_reg,
                             nrf_ecb_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_ecb_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
