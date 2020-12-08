/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ccm.h"
#include "bs_tracing.h"
#include "NRF_AES_CCM.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg, nrf_ccm_task_t task)
{
  if ( task == NRF_CCM_TASK_KSGEN ) {
    p_reg->TASKS_KSGEN = 1;
    nrf_ccm_regw_sideeffects_TASKS_KSGEN();
  } else if ( task == NRF_CCM_TASK_STOP ) {
    p_reg->TASKS_STOP = 1;
    nrf_ccm_regw_sideeffects_TASKS_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ccm\n");
  }
}

void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_ccm_regw_sideeffects_INTENSET();
}

void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_ccm_regw_sideeffects_INTENCLR();
}

#if defined(DPPI_PRESENT)

void nrf_ccm_subscriber_add(nrf_ccm_task_t task, uint8_t channel)
{
  switch(task) 
  {
    case NRF_CCM_TASK_KSGEN:
      nrf_dppi_subscriber_add(channel, nrf_ccm_TASK_KSGEN);
      break;
    case NRF_CCM_TASK_CRYPT:
      nrf_dppi_subscriber_add(channel, nrf_ccm_TASK_CRYPT);
      break;
    case NRF_CCM_TASK_STOP:
      nrf_dppi_subscriber_add(channel, nrf_ccm_TASK_STOP);
      break;
#if defined(CCM_RATEOVERRIDE_RATEOVERRIDE_Pos)
    case NRF_CCM_TASK_RATEOVERRIDE:
      nrf_dppi_subscriber_add(channel, nrf_ccm_TASK_RATEOVERRIDE);
      break;
#endif
    default:
      bs_trace_warning_line_time(
        "NRF_CCM: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_ccm_subscriber_remove(nrf_ccm_task_t task)
{
  switch(task) 
  {
    case NRF_CCM_TASK_KSGEN:
      nrf_dppi_subscriber_remove(nrf_ccm_TASK_KSGEN);
      break;
    case NRF_CCM_TASK_CRYPT:
      nrf_dppi_subscriber_remove(nrf_ccm_TASK_CRYPT);
      break;
    case NRF_CCM_TASK_STOP:
      nrf_dppi_subscriber_remove(nrf_ccm_TASK_STOP);
      break;
#if defined(CCM_RATEOVERRIDE_RATEOVERRIDE_Pos)
    case NRF_CCM_TASK_RATEOVERRIDE:
      nrf_dppi_subscriber_remove(nrf_ccm_TASK_RATEOVERRIDE);
      break;
#endif
    default:
      bs_trace_warning_line_time(
        "NRF_CCM: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

void nrf_ccm_subscribe_set(NRF_CCM_Type * p_reg,
                           nrf_ccm_task_t task,
                           uint8_t        channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | CCM_SUBSCRIBE_KSGEN_EN_Msk);
  nrf_ccm_subscriber_add(task, channel);
}

void nrf_ccm_subscribe_clear(NRF_CCM_Type * p_reg,
                             nrf_ccm_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_ccm_subscriber_remove(task);
}

#endif // defined(DPPI_PRESENT)
