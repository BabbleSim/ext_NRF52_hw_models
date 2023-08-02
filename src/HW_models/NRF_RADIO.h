/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_RADIO_H
#define _NRF_RADIO_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_RADIO_Type NRF_RADIO_regs;

void nrf_radio_tasks_TXEN();
void nrf_radio_tasks_RXEN();
void nrf_radio_tasks_START();
void nrf_radio_tasks_CCASTART();
void nrf_radio_tasks_CCASTOP();
void nrf_radio_tasks_EDSTART();
void nrf_radio_tasks_EDSTOP();
void nrf_radio_tasks_STOP() ;
void nrf_radio_tasks_DISABLE();
void nrf_radio_tasks_RSSISTART();
void nrf_radio_tasks_RSSISTOP();
void nrf_radio_tasks_BCSTART();
void nrf_radio_tasks_BCSTOP();

void nrf_radio_regw_sideeffects_BCC();
void nrf_radio_regw_sideeffects_POWER();
void nrf_radio_regw_sideeffects_TASKS_TXEN();
void nrf_radio_regw_sideeffects_TASKS_RXEN();
void nrf_radio_regw_sideeffects_TASKS_START();
void nrf_radio_regw_sideeffects_TASKS_STOP();
void nrf_radio_regw_sideeffects_TASKS_DISABLE();
void nrf_radio_regw_sideeffects_TASKS_RSSISTART();
void nrf_radio_regw_sideeffects_TASKS_RSSISTOP();
void nrf_radio_regw_sideeffects_TASKS_BCSTART();
void nrf_radio_regw_sideeffects_TASKS_BCSTOP();
void nrf_radio_regw_sideeffects_TASKS_EDSTART();
void nrf_radio_regw_sideeffects_TASKS_EDSTOP();
void nrf_radio_regw_sideeffects_TASKS_CCASTART();
void nrf_radio_regw_sideeffects_TASKS_CCASTOP();
void nrf_radio_regw_sideeffects_INTENSET();
void nrf_radio_regw_sideeffects_INTENCLR();

/*
 * Internal interface to bitcounter
 */
double nrf_radio_get_bpus();

#ifdef __cplusplus
}
#endif

#endif

