/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_radio.h"
#include "bs_tracing.h"
#include "NRF_RADIO.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

void nrf_radio_task_trigger(NRF_RADIO_Type * p_reg, nrf_radio_task_t task)
{
  if ( task == NRF_RADIO_TASK_TXEN ) {
    p_reg->TASKS_TXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_TXEN();
  } else if ( task == NRF_RADIO_TASK_RXEN ) {
    p_reg->TASKS_RXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_RXEN();
  } else if ( task == NRF_RADIO_TASK_DISABLE ) {
    p_reg->TASKS_DISABLE = 1;
    nrf_radio_regw_sideeffects_TASKS_DISABLE();
  } else if ( task == NRF_RADIO_TASK_RSSISTOP ) {
    p_reg->TASKS_RSSISTOP = 1;
    nrf_radio_regw_sideeffects_TASKS_RSSISTOP();
  } else if ( task == NRF_RADIO_TASK_RSSISTART ) {
    p_reg->TASKS_RSSISTART = 1;
    nrf_radio_regw_sideeffects_TASKS_RSSISTART();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_radio\n");
  }
}

void nrf_radio_int_enable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nrf_radio_regw_sideeffects_INTENSET();
}

void nrf_radio_int_disable(NRF_RADIO_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nrf_radio_regw_sideeffects_INTENCLR();
}

void nrf_radio_bcc_set(NRF_RADIO_Type * p_reg, uint32_t radio_bcc)
{
  p_reg->BCC = radio_bcc;
  nrf_radio_regw_sideeffects_BCC();
}

void nrf_radio_power_set(NRF_RADIO_Type * p_reg, bool radio_power)
{
  p_reg->POWER = (uint32_t) radio_power;
  nrf_radio_regw_sideeffects_POWER();
}

#if defined(DPPI_PRESENT)
void nrf_radio_subscriber_add(nrf_radio_task_t task, uint8_t channel)
{
  switch(task)
  {
    case NRF_RADIO_TASK_TXEN:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_txen);
      break;
    case NRF_RADIO_TASK_RXEN:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_rxen);
      break;
    case NRF_RADIO_TASK_START:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_start);
      break;
    case NRF_RADIO_TASK_STOP:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_stop);
      break;
    case NRF_RADIO_TASK_DISABLE:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_disable);
      break;
    case NRF_RADIO_TASK_RSSISTART:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_rssistart);
      break;
    case NRF_RADIO_TASK_RSSISTOP:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_rssistop);
      break;
    case NRF_RADIO_TASK_BCSTART:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_bcstart);
      break;
    case NRF_RADIO_TASK_BCSTOP:
      nrf_dppi_subscriber_add(channel, nrf_radio_tasks_bcstop);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RADIO: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_radio_subscriber_remove(nrf_radio_task_t task)
{
  switch(task)
  {
    case NRF_RADIO_TASK_TXEN:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_txen);
      break;
    case NRF_RADIO_TASK_RXEN:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_rxen);
      break;
    case NRF_RADIO_TASK_START:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_start);
      break;
    case NRF_RADIO_TASK_STOP:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_stop);
      break;
    case NRF_RADIO_TASK_DISABLE:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_disable);
      break;
    case NRF_RADIO_TASK_RSSISTART:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_rssistart);
      break;
    case NRF_RADIO_TASK_RSSISTOP:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_rssistop);
      break;
    case NRF_RADIO_TASK_BCSTART:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_bcstart);
      break;
    case NRF_RADIO_TASK_BCSTOP:
      nrf_dppi_subscriber_remove(nrf_radio_tasks_bcstop);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RADIO: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task);
      break;
  }
}

void nrf_radio_subscribe_set(NRF_RADIO_Type * p_reg,
                             nrf_radio_task_t task,
                             uint8_t          channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
          ((uint32_t)channel | RADIO_SUBSCRIBE_TXEN_EN_Msk);
  nrf_radio_subscriber_add(task, channel);
}

void nrf_radio_subscribe_clear(NRF_RADIO_Type * p_reg,
                               nrf_radio_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_radio_subscriber_remove(task);
}

#endif // defined(DPPI_PRESENT)
