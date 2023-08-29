/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TIMER_H
#define _NRF_HW_MODEL_TIMER_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_timer0_TASK_CAPTURE_0(void);
void nrf_timer0_TASK_CAPTURE_1(void);
void nrf_timer0_TASK_CAPTURE_2(void);
void nrf_timer0_TASK_CAPTURE_3(void);

void nrf_timer1_TASK_CAPTURE_0(void);
void nrf_timer1_TASK_CAPTURE_1(void);
void nrf_timer1_TASK_CAPTURE_2(void);
void nrf_timer1_TASK_CAPTURE_3(void);

void nrf_timer2_TASK_CAPTURE_0(void);
void nrf_timer2_TASK_CAPTURE_1(void);
void nrf_timer2_TASK_CAPTURE_2(void);
void nrf_timer2_TASK_CAPTURE_3(void);

void nrf_timer3_TASK_CAPTURE_0(void);
void nrf_timer3_TASK_CAPTURE_1(void);
void nrf_timer3_TASK_CAPTURE_2(void);
void nrf_timer3_TASK_CAPTURE_3(void);
void nrf_timer3_TASK_CAPTURE_4(void);
void nrf_timer3_TASK_CAPTURE_5(void);

void nrf_timer4_TASK_CAPTURE_0(void);
void nrf_timer4_TASK_CAPTURE_1(void);
void nrf_timer4_TASK_CAPTURE_2(void);
void nrf_timer4_TASK_CAPTURE_3(void);
void nrf_timer4_TASK_CAPTURE_4(void);
void nrf_timer4_TASK_CAPTURE_5(void);

void nrf_timer0_TASK_CLEAR(void);
void nrf_timer1_TASK_CLEAR(void);
void nrf_timer2_TASK_CLEAR(void);
void nrf_timer3_TASK_CLEAR(void);
void nrf_timer4_TASK_CLEAR(void);

void nrf_timer0_TASK_COUNT(void);
void nrf_timer1_TASK_COUNT(void);
void nrf_timer2_TASK_COUNT(void);
void nrf_timer3_TASK_COUNT(void);
void nrf_timer4_TASK_COUNT(void);

void nrf_timer0_TASK_START(void);
void nrf_timer1_TASK_START(void);
void nrf_timer2_TASK_START(void);
void nrf_timer3_TASK_START(void);
void nrf_timer4_TASK_START(void);

void nrf_timer0_TASK_STOP(void);
void nrf_timer1_TASK_STOP(void);
void nrf_timer2_TASK_STOP(void);
void nrf_timer3_TASK_STOP(void);
void nrf_timer4_TASK_STOP(void);

void nrf_timer_regw_sideeffects_TASKS_STOP(int t);
void nrf_timer_regw_sideeffects_TASKS_SHUTDOWN(int t);
void nrf_timer_regw_sideeffects_TASKS_START(int t);
void nrf_timer_regw_sideeffects_TASKS_CLEAR(int t);
void nrf_timer_regw_sideeffects_TASKS_CAPTURE(int t, int cc_n);
void nrf_timer_regw_sideeffects_INTENSET(int t);
void nrf_timer_regw_sideeffects_INTENCLR(int t);
void nrf_timer_regw_sideeffects_CC(int t, int cc_n);
/* Side-effecting function when any event register is written: */
void nrf_timer_regw_sideeffects_EVENTS_all(int t);

#ifdef __cplusplus
}
#endif

#endif
