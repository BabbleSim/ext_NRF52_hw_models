/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_GPIOTE_H
#define _NRF_HW_MODEL_GPIOTE_H

#ifdef __cplusplus
extern "C"{
#endif

#define N_GPIOTE_CHANNELS (sizeof(NRF_GPIOTE_regs.CONFIG)/sizeof(NRF_GPIOTE_regs.CONFIG[0]))

void nrf_gpiote_port_event_raise(unsigned int port);

void nrf_gpiote_regw_sideeffects_TASKS_OUT(unsigned int n);
void nrf_gpiote_regw_sideeffects_TASKS_SET(unsigned int n);
void nrf_gpiote_regw_sideeffects_TASKS_CLR(unsigned int n);
void nrf_gpiote_regw_sideeffects_EVENTS_IN(unsigned int n);
void nrf_gpiote_regw_sideeffects_EVENTS_PORT(void);
void nrf_gpiote_regw_sideeffects_INTENSET(void);
void nrf_gpiote_regw_sideeffects_INTENCLR(void);
void nrf_gpiote_regw_sideeffects_CONFIG(unsigned int n);

void nrf_gpiote_TASKS_SET_0(void);
void nrf_gpiote_TASKS_SET_1(void);
void nrf_gpiote_TASKS_SET_2(void);
void nrf_gpiote_TASKS_SET_3(void);
void nrf_gpiote_TASKS_SET_4(void);
void nrf_gpiote_TASKS_SET_5(void);
void nrf_gpiote_TASKS_SET_6(void);
void nrf_gpiote_TASKS_SET_7(void);
void nrf_gpiote_TASKS_CLR_0(void);
void nrf_gpiote_TASKS_CLR_1(void);
void nrf_gpiote_TASKS_CLR_2(void);
void nrf_gpiote_TASKS_CLR_3(void);
void nrf_gpiote_TASKS_CLR_4(void);
void nrf_gpiote_TASKS_CLR_5(void);
void nrf_gpiote_TASKS_CLR_6(void);
void nrf_gpiote_TASKS_CLR_7(void);
void nrf_gpiote_TASKS_OUT_0(void);
void nrf_gpiote_TASKS_OUT_1(void);
void nrf_gpiote_TASKS_OUT_2(void);
void nrf_gpiote_TASKS_OUT_3(void);
void nrf_gpiote_TASKS_OUT_4(void);
void nrf_gpiote_TASKS_OUT_5(void);
void nrf_gpiote_TASKS_OUT_6(void);
void nrf_gpiote_TASKS_OUT_7(void);

#ifdef __cplusplus
}
#endif

#endif
