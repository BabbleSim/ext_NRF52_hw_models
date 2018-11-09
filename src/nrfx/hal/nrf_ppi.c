/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_ppi.h"
#include "NRF_PPI.h"
#include "bs_tracing.h"

void nrf_ppi_channels_disable(uint32_t mask){
  NRF_PPI->CHENCLR = mask;
  nrf_ppi_regw_sideeffects_CHENCLR();
}

void nrf_ppi_channels_enable(uint32_t mask){
  NRF_PPI->CHENSET = mask;
  nrf_ppi_regw_sideeffects_CHENSET();
}

void nrf_ppi_group_disable(nrf_ppi_channel_group_t group)
{
  NRF_PPI->TASKS_CHG[(uint32_t) group].DIS = 1;
  nrf_ppi_regw_sideeffects_TASKS_CHG_DIS(group);
}

void nrf_ppi_channel_endpoint_setup(nrf_ppi_channel_t channel,
    uint32_t eep,
    uint32_t tep)
{
  NRF_PPI->CH[(uint32_t) channel].EEP = eep;
  nrf_ppi_regw_sideeffects_EEP(channel);
  NRF_PPI->CH[(uint32_t) channel].TEP = tep;
  nrf_ppi_regw_sideeffects_TEP(channel);
}

void nrf_ppi_event_endpoint_setup(nrf_ppi_channel_t channel,
    uint32_t eep){
  NRF_PPI->CH[(uint32_t) channel].EEP = eep;
  nrf_ppi_regw_sideeffects_EEP(channel);
}

void nrf_ppi_task_endpoint_setup(nrf_ppi_channel_t channel,
    uint32_t tep){
  NRF_PPI->CH[(uint32_t) channel].TEP = tep;
  nrf_ppi_regw_sideeffects_TEP(channel);
}
