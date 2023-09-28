/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_temp.h"
#include "bs_tracing.h"
#include "NHW_TEMP.h"

void nrf_temp_task_trigger(NRF_TEMP_Type * p_reg, nrf_temp_task_t temp_task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)temp_task)) = 0x1UL;

  if ( temp_task == NRF_TEMP_TASK_START ) {
    nhw_TEMP_regw_sideeffects_TASKS_START();
  } else if ( temp_task == NRF_TEMP_TASK_STOP ) {
    nhw_TEMP_regw_sideeffects_TASKS_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in %s\n", __func__);
  }
}

void nrf_temp_int_enable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
  NRF_TEMP_regs.INTENSET = mask;
  nhw_TEMP_regw_sideeffects_INTENSET();
}

void nrf_temp_int_disable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
  NRF_TEMP_regs.INTENCLR = mask;
  nhw_TEMP_regw_sideeffects_INTENCLR();
}

void nrf_temp_event_clear(NRF_TEMP_Type * p_reg, nrf_temp_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_TEMP_regw_sideeffects_EVENTS_all(0);
}

#if defined(DPPI_PRESENT)

static void nrf_temp_subscribe_common(NRF_TEMP_Type * p_reg,
                                     nrf_temp_task_t task)
{
  if (task == NRF_TEMP_TASK_START) {
      nhw_TEMP_regw_sideeffects_SUBSCRIBE_START(0);
  } else if ( task == NRF_TEMP_TASK_STOP ) {
      nhw_TEMP_regw_sideeffects_SUBSCRIBE_STOP(0);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to an not-supported task in the nrf_temp (%i)\n",
                               task);
  }
}

void nrf_temp_subscribe_set(NRF_TEMP_Type * p_reg,
                           nrf_temp_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_temp_subscribe_common(p_reg, task);
}

void nrf_temp_subscribe_clear(NRF_TEMP_Type * p_reg,
                             nrf_temp_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_temp_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
