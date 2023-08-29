/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_RADIO_H
#define _NRF_RADIO_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_radio_tasks_TXEN(void);
void nrf_radio_tasks_RXEN(void);
void nrf_radio_tasks_START(void);
void nrf_radio_tasks_CCASTART(void);
void nrf_radio_tasks_CCASTOP(void);
void nrf_radio_tasks_EDSTART(void);
void nrf_radio_tasks_EDSTOP(void);
void nrf_radio_tasks_STOP() ;
void nrf_radio_tasks_DISABLE(void);
void nrf_radio_tasks_RSSISTART(void);
void nrf_radio_tasks_RSSISTOP(void);
void nrf_radio_tasks_BCSTART(void);
void nrf_radio_tasks_BCSTOP(void);

void nrf_radio_regw_sideeffects_BCC(void);
void nrf_radio_regw_sideeffects_POWER(void);
void nrf_radio_regw_sideeffects_TASKS_TXEN(void);
void nrf_radio_regw_sideeffects_TASKS_RXEN(void);
void nrf_radio_regw_sideeffects_TASKS_START(void);
void nrf_radio_regw_sideeffects_TASKS_STOP(void);
void nrf_radio_regw_sideeffects_TASKS_DISABLE(void);
void nrf_radio_regw_sideeffects_TASKS_RSSISTART(void);
void nrf_radio_regw_sideeffects_TASKS_RSSISTOP(void);
void nrf_radio_regw_sideeffects_TASKS_BCSTART(void);
void nrf_radio_regw_sideeffects_TASKS_BCSTOP(void);
void nrf_radio_regw_sideeffects_TASKS_EDSTART(void);
void nrf_radio_regw_sideeffects_TASKS_EDSTOP(void);
void nrf_radio_regw_sideeffects_TASKS_CCASTART(void);
void nrf_radio_regw_sideeffects_TASKS_CCASTOP(void);
void nrf_radio_regw_sideeffects_INTENSET(void);
void nrf_radio_regw_sideeffects_INTENCLR(void);

/*
 * Internal interface to bitcounter
 */
double nrf_radio_get_bpus(void);

#ifdef __cplusplus
}
#endif

#endif

