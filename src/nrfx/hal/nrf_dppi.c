/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#if defined(DPPI_PRESENT)

#include "hal/nrf_dppi.h"
#include "NRF_DPPI.h"

void nrf_dppi_subscriber_dppi_add(nrf_dppi_task_t task, uint8_t channel)
{
  switch(task)
  {
    case NRF_DPPI_TASK_CHG0_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG0_EN);
      break;
    case NRF_DPPI_TASK_CHG0_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG0_DIS);
      break;
    case NRF_DPPI_TASK_CHG1_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG1_EN);
      break;
    case NRF_DPPI_TASK_CHG1_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG1_DIS);
      break;
    case NRF_DPPI_TASK_CHG2_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG2_EN);
      break;
    case NRF_DPPI_TASK_CHG2_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG2_DIS);
      break;
    case NRF_DPPI_TASK_CHG3_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG3_EN);
      break;
    case NRF_DPPI_TASK_CHG3_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG3_DIS);
      break;
    case NRF_DPPI_TASK_CHG4_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG4_EN);
      break;
    case NRF_DPPI_TASK_CHG4_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG4_DIS);
      break;
    case NRF_DPPI_TASK_CHG5_EN:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG5_EN);
      break;
    case NRF_DPPI_TASK_CHG5_DIS:
      nrf_dppi_subscriber_add(channel, nrf_dppi_TASK_CHG5_DIS);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_DPPI: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_dppi_subscriber_dppi_remove(nrf_dppi_task_t task)
{
  switch(task)
  {
    case NRF_DPPI_TASK_CHG0_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG0_EN);
      break;
    case NRF_DPPI_TASK_CHG0_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG0_DIS);
      break;
    case NRF_DPPI_TASK_CHG1_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG1_EN);
      break;
    case NRF_DPPI_TASK_CHG1_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG1_DIS);
      break;
    case NRF_DPPI_TASK_CHG2_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG2_EN);
      break;
    case NRF_DPPI_TASK_CHG2_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG2_DIS);
      break;
    case NRF_DPPI_TASK_CHG3_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG3_EN);
      break;
    case NRF_DPPI_TASK_CHG3_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG3_DIS);
      break;
    case NRF_DPPI_TASK_CHG4_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG4_EN);
      break;
    case NRF_DPPI_TASK_CHG4_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG4_DIS);
      break;
    case NRF_DPPI_TASK_CHG5_EN:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG5_EN);
      break;
    case NRF_DPPI_TASK_CHG5_DIS:
      nrf_dppi_subscriber_remove(nrf_dppi_TASK_CHG5_DIS);
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_DPPI: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
        task, channel);
      break;
  }
}

void nrf_dppi_channels_enable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
  p_reg->CHENSET = mask;
  nrf_dppi_regw_sideeffects_CHENSET();
}

void nrf_dppi_channels_disable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
  p_reg->CHENCLR = mask;
  nrf_dppi_regw_sideeffects_CHENCLR();
}

void nrf_dppi_subscribe_set(NRF_DPPIC_Type * p_reg,
                            nrf_dppi_task_t  task,
                            uint8_t          channel)
{
  NRFX_ASSERT(channel < DPPI_CH_NUM);
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | DPPIC_SUBSCRIBE_CHG_EN_EN_Msk);
  nrf_dppi_subscriber_dppi_add(channel, task);
}

void nrf_dppi_subscribe_clear(NRF_DPPIC_Type * p_reg, nrf_dppi_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_dppi_subscriber_dppi_remove(task);
}

#endif //defined(DPPI_PRESENT)
