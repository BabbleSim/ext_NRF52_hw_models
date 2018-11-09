/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_radio.h"
#include "bs_tracing.h"
#include "NRF_RADIO.h"

void nrf_radio_int_enable(uint32_t radio_int_mask)
{
  NRF_RADIO->INTENSET = radio_int_mask;
  nrf_radio_regw_sideeffects_INTENSET();
}

/**
 * @brief Function for setting a specific task.
 *
 * @param[in]  radio_task              Task.
 */
void nrf_radio_task_trigger(nrf_radio_task_t radio_task)
{
  if ( radio_task == NRF_RADIO_TASK_TXEN ) {
    NRF_RADIO->TASKS_TXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_TXEN();
  } else if ( radio_task == NRF_RADIO_TASK_RXEN ) {
    NRF_RADIO->TASKS_RXEN = 1;
    nrf_radio_regw_sideeffects_TASKS_RXEN();
  } else if ( radio_task == NRF_RADIO_TASK_DISABLE ) {
    NRF_RADIO->TASKS_DISABLE = 1;
    nrf_radio_regw_sideeffects_TASKS_DISABLE();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_radio\n");
  }
}

/**
 * @brief Function for setting power mode of the radio peripheral.
 *
 * @param[in]  radio_power                  If radio should powered on.
 */
void nrf_radio_power_set(bool radio_power)
{
  NRF_RADIO->POWER = (uint32_t) radio_power;
  nrf_radio_regw_sideeffects_POWER();
}

/**
 * @brief Function for setting Bit counter compare.
 *
 * @param[in]  radio_bcc                    Bit counter compare [bits].
 */
void nrf_radio_bcc_set(uint32_t radio_bcc)
{
  NRF_RADIO->BCC = radio_bcc;
  nrf_radio_regw_sideeffects_BCC();
}
