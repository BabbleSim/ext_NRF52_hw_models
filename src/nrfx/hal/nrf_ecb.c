/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ecb.h"
#include "bs_tracing.h"
#include "NRF_AES_ECB.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task)
{
  if ( task == NRF_ECB_TASK_STARTECB ) {
    p_reg->TASKS_STARTECB = 1;
    nrf_ecb_regw_sideeffects_TASKS_STARTECB();
  } else if ( task == NRF_ECB_TASK_STOPECB ) {
    p_reg->TASKS_STOPECB = 1;
    nrf_ecb_regw_sideeffects_TASKS_STOPECB();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ecb\n");
  }

}

void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_ecb_regw_sideeffects_INTENSET();
}

void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_ecb_regw_sideeffects_INTENCLEAR();
}

#if defined(DPPI_PRESENT)
void nrf_ecb_subscriber_add(nrf_ecb_task_t task, uint8_t channel)
{
  switch(task)
  {
    case NRF_ECB_TASK_STARTECB:
      nrf_dppi_subscriber_add(channel, NRF_ECB_TASK_STARTECB);
      break;
    case NRF_ECB_TASK_STOPECB:
      nrf_dppi_subscriber_add(channel, nrf_ecb_TASK_STOPECB);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_ECB: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_ecb_subscriber_remove(nrf_ecb_task_t task)
{
  switch(task)
  {
    case NRF_ECB_TASK_STARTECB:
      nrf_dppi_subscriber_remove(nrf_ecb_TASK_STARTECB);
      break;
    case NRF_ECB_TASK_STOPECB:
      nrf_dppi_subscriber_remove(nrf_ecb_TASK_STOPECB);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_ECB: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

void nrf_ecb_subscribe_set(NRF_ECB_Type * p_reg,
                           nrf_ecb_task_t task,
                           uint8_t        channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
          ((uint32_t)channel | ECB_SUBSCRIBE_STARTECB_EN_Msk);
  nrf_ecb_subscriber_add(task, channel);
}

void nrf_ecb_subscribe_clear(NRF_ECB_Type * p_reg,
                             nrf_ecb_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_ecb_subscriber_remove(task);
}

#endif // defined(DPPI_PRESENT)
