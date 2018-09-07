/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_RADIO_H
#define _NRF_RADIO_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_RADIO_Type NRF_RADIO_regs;

void nrf_radio_init();
void nrf_radio_clean_up();
void nrf_radio_timer_triggered();
void nrf_radio_timer_abort_reeval_triggered();

void nrf_radio_tasks_txen ();
void nrf_radio_tasks_rxen ();
void nrf_radio_tasks_start ();
void nrf_radio_tasks_stop () ;
void nrf_radio_tasks_disable ();
void nrf_radio_tasks_rssistart ();
void nrf_radio_tasks_rssistop ();
void nrf_radio_tasks_bcstart ();
void nrf_radio_tasks_bcstop ();

void nrf_radio_bitcounter_timer_triggered();

void nrf_radio_regw_sideeffects_BCC();
void nrf_radio_regw_sideeffects_POWER();
void nrf_radio_regw_sideeffects_TASKS_DISABLE();
void nrf_radio_regw_sideeffects_TASKS_RXEN();
void nrf_radio_regw_sideeffects_TASKS_TXEN();
void nrf_radio_regw_sideeffects_INTENSET();

#ifdef __cplusplus
}
#endif

#endif
