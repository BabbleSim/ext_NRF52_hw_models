/*
 * Copyright (c) 2020 Oticon A/S
 * Copyright (c) 2020-2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_aar.h"
#include "bs_tracing.h"
#include "NHW_AAR.h"

void nrf_aar_int_enable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nhw_AAR_regw_sideeffects_INTENSET();
}

void nrf_aar_int_disable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nhw_AAR_regw_sideeffects_INTENCLR();
}

void nrf_aar_task_trigger(NRF_AAR_Type * p_reg, nrf_aar_task_t task)
{
  *(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task) = 1;
  if (task == NRF_AAR_TASK_START) {
    nhw_AAR_regw_sideeffects_TASKS_START();
  } else if (task == NRF_AAR_TASK_STOP) {
    nhw_AAR_regw_sideeffects_TASKS_STOP();
  }  else {
    bs_trace_error_line_time("Not supported task started in nrf_aar\n");
  }
}

void nrf_aar_event_clear(NRF_AAR_Type * p_reg, nrf_aar_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_AAR_regw_sideeffects_EVENTS_all(0);
}

#if defined(DPPI_PRESENT)

static void nrf_aar_subscribe_common(NRF_AAR_Type * p_reg,
                                     nrf_aar_task_t task)
{
  if (task == NRF_AAR_TASK_START) {
      nhw_AAR_regw_sideeffects_SUBSCRIBE_START(0);
  } else if ( task == NRF_AAR_TASK_STOP ) {
      nhw_AAR_regw_sideeffects_SUBSCRIBE_STOP(0);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to an not-supported task in the nrf_aar (%i)\n",
                               task);
  }
}

void nrf_aar_subscribe_set(NRF_AAR_Type * p_reg,
                           nrf_aar_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_aar_subscribe_common(p_reg, task);
}

void nrf_aar_subscribe_clear(NRF_AAR_Type * p_reg,
                             nrf_aar_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_aar_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
