/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_RADIO_H
#define _NRF_RADIO_H

#ifdef __cplusplus
extern "C"{
#endif

void nhw_RADIO_TASK_TXEN(void);
void nhw_RADIO_TASK_RXEN(void);
void nhw_RADIO_TASK_START(void);
void nhw_RADIO_TASK_CCASTART(void);
void nhw_RADIO_TASK_CCASTOP(void);
void nhw_RADIO_TASK_EDSTART(void);
void nhw_RADIO_TASK_EDSTOP(void);
void nhw_RADIO_TASK_STOP(void) ;
void nhw_RADIO_TASK_DISABLE(void);
void nhw_RADIO_TASK_RSSISTART(void);
void nhw_RADIO_TASK_RSSISTOP(void);
void nhw_RADIO_TASK_BCSTART(void);
void nhw_RADIO_TASK_BCSTOP(void);

void nhw_RADIO_regw_sideeffects_BCC(void);
void nhw_RADIO_regw_sideeffects_POWER(void);
void nhw_RADIO_regw_sideeffects_TASKS_TXEN(void);
void nhw_RADIO_regw_sideeffects_TASKS_RXEN(void);
void nhw_RADIO_regw_sideeffects_TASKS_START(void);
void nhw_RADIO_regw_sideeffects_TASKS_STOP(void);
void nhw_RADIO_regw_sideeffects_TASKS_DISABLE(void);
void nhw_RADIO_regw_sideeffects_TASKS_RSSISTART(void);
void nhw_RADIO_regw_sideeffects_TASKS_RSSISTOP(void);
void nhw_RADIO_regw_sideeffects_TASKS_BCSTART(void);
void nhw_RADIO_regw_sideeffects_TASKS_BCSTOP(void);
void nhw_RADIO_regw_sideeffects_TASKS_EDSTART(void);
void nhw_RADIO_regw_sideeffects_TASKS_EDSTOP(void);
void nhw_RADIO_regw_sideeffects_TASKS_CCASTART(void);
void nhw_RADIO_regw_sideeffects_TASKS_CCASTOP(void);
void nhw_RADIO_regw_sideeffects_INTENSET(void);
void nhw_RADIO_regw_sideeffects_INTENCLR(void);
void nhw_RADIO_regw_sideeffects_EVENTS_all(unsigned int inst);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_TXEN(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_RXEN(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_START(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_STOP(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_DISABLE(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_RSSISTART(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_RSSISTOP(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_BCSTART(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_BCSTOP(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_EDSTART(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_EDSTOP(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_CCASTART(unsigned int);
void nhw_RADIO_regw_sideeffects_SUBSCRIBE_CCASTOP(unsigned int);

/*
 * Note
 * hw_radio_testcheat_set_tx_power_gain() &
 * hw_radio_testcheat_set_rx_power_gain()
 * are defined in hw_testcheat_if.h
 */

/*
 * Internal interface to bitcounter
 */
double nhw_radio_get_bpus(void);

#ifdef __cplusplus
}
#endif

#endif

