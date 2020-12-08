/*
 * Copyright (c) 2020 Oticon A/S
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_aar.h"
#include "bs_tracing.h"
#include "NRF_AAR.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

void nrf_aar_int_enable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_aar_regw_sideeffects_INTENSET();
}

void nrf_aar_int_disable(NRF_AAR_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_aar_regw_sideeffects_INTENCLR();
}

void nrf_aar_task_trigger(NRF_AAR_Type * p_reg, nrf_aar_task_t task)
{
  if (task == NRF_AAR_TASK_START) {
    p_reg->TASKS_START = 1;
    nrf_aar_regw_sideeffects_TASKS_START();
  } else if (task == NRF_AAR_TASK_STOP) {
    p_reg->TASKS_STOP = 1;
    nrf_aar_regw_sideeffects_TASKS_STOP();
  }  else {
    bs_trace_error_line_time("Not supported task started in nrf_aar\n");
  }
}

#if defined(DPPI_PRESENT)
void nrf_aar_subscriber_add(nrf_aar_task_t task, uint8_t channel)
{
  switch(task)
  {
    case NRF_AAR_TASK_START:
      nrf_dppi_subscriber_add(channel, nrf_aar_TASK_START);
      break;
    case NRF_AAR_TASK_STOP:
      nrf_dppi_subscriber_add(channel, nrf_aar_TASK_STOP);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_AAR: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_aar_subscriber_remove(nrf_aar_task_t task)
{
  switch(task)
  {
    case NRF_AAR_TASK_START:
      nrf_dppi_subscriber_remove(nrf_aar_TASK_START);
      break;
    case NRF_AAR_TASK_STOP:
      nrf_dppi_subscriber_remove(nrf_aar_TASK_STOP);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_AAR: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

NRF_STATIC_INLINE void nrf_aar_subscribe_set(NRF_AAR_Type * p_reg,
                                             nrf_aar_task_t task,
                                             uint8_t          channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | AAR_SUBSCRIBE_START_EN_Msk);
  nrf_aar_subscriber_add(task, channel);
}

NRF_STATIC_INLINE void nrf_aar_subscribe_clear(NRF_AAR_Type * p_reg,
                                               nrf_aar_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_aar_subscriber_remove(task);
}

#endif // defined(DPPI_PRESENT)
