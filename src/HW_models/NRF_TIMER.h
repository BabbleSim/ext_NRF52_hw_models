/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TIMER_H
#define _NRF_HW_MODEL_TIMER_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_timer_init();
void nrf_timer_clean_up();
void nrf_timer_timer_triggered();

extern NRF_TIMER_Type NRF_TIMER_regs[];

void nrf_timer0_TASK_CAPTURE_0();
void nrf_timer0_TASK_CAPTURE_1();
void nrf_timer0_TASK_CAPTURE_2();
void nrf_timer0_TASK_CAPTURE_3();
void nrf_timer0_TASK_CAPTURE_4();
void nrf_timer0_TASK_CAPTURE_5();

void nrf_timer1_TASK_CAPTURE_0();
void nrf_timer1_TASK_CAPTURE_1();
void nrf_timer1_TASK_CAPTURE_2();
void nrf_timer1_TASK_CAPTURE_3();
void nrf_timer1_TASK_CAPTURE_4();
void nrf_timer1_TASK_CAPTURE_5();

void nrf_timer2_TASK_CAPTURE_0();
void nrf_timer2_TASK_CAPTURE_1();
void nrf_timer2_TASK_CAPTURE_2();
void nrf_timer2_TASK_CAPTURE_3();
void nrf_timer2_TASK_CAPTURE_4();
void nrf_timer2_TASK_CAPTURE_5();

void nrf_timer3_TASK_CAPTURE_0();
void nrf_timer3_TASK_CAPTURE_1();
void nrf_timer3_TASK_CAPTURE_2();
void nrf_timer3_TASK_CAPTURE_3();
void nrf_timer3_TASK_CAPTURE_4();
void nrf_timer3_TASK_CAPTURE_5();

void nrf_timer4_TASK_CAPTURE_0();
void nrf_timer4_TASK_CAPTURE_1();
void nrf_timer4_TASK_CAPTURE_2();
void nrf_timer4_TASK_CAPTURE_3();
void nrf_timer4_TASK_CAPTURE_4();
void nrf_timer4_TASK_CAPTURE_5();

void nrf_timer0_TASK_CLEAR();
void nrf_timer1_TASK_CLEAR();
void nrf_timer2_TASK_CLEAR();
void nrf_timer3_TASK_CLEAR();
void nrf_timer4_TASK_CLEAR();

void nrf_timer0_TASK_COUNT();
void nrf_timer1_TASK_COUNT();
void nrf_timer2_TASK_COUNT();
void nrf_timer3_TASK_COUNT();
void nrf_timer4_TASK_COUNT();

void nrf_timer0_TASK_START();
void nrf_timer1_TASK_START();
void nrf_timer2_TASK_START();
void nrf_timer3_TASK_START();
void nrf_timer4_TASK_START();

void nrf_timer0_TASK_STOP();
void nrf_timer1_TASK_STOP();
void nrf_timer2_TASK_STOP();
void nrf_timer3_TASK_STOP();
void nrf_timer4_TASK_STOP();

void nrf_timer_regw_sideeffects_TASKS_STOP(int t);
void nrf_timer_regw_sideeffects_TASKS_START(int t);
void nrf_timer_regw_sideeffects_TASKS_CLEAR(int t);
void nrf_timer_regw_sideeffects_TASKS_CAPTURE(int t, int cc_n);
void nrf_timer_regw_sideeffects_CC(int t, int cc_n);

#ifdef __cplusplus
}
#endif

#endif
