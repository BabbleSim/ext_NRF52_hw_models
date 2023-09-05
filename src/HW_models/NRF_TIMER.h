/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TIMER_H
#define _NRF_HW_MODEL_TIMER_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif
void nrf_timer_regw_sideeffects_TASKS_STOP(uint t);
void nrf_timer_regw_sideeffects_TASKS_COUNT(uint t);
void nrf_timer_regw_sideeffects_TASKS_SHUTDOWN(uint t);
void nrf_timer_regw_sideeffects_TASKS_START(uint t);
void nrf_timer_regw_sideeffects_TASKS_CLEAR(uint t);
void nrf_timer_regw_sideeffects_TASKS_CAPTURE(uint t, uint cc_n);
void nrf_timer_regw_sideeffects_INTENSET(uint t);
void nrf_timer_regw_sideeffects_INTENCLR(uint t);
void nrf_timer_regw_sideeffects_CC(uint t, uint cc_n);
/* Side-effecting function when any event register is written: */
void nrf_timer_regw_sideeffects_EVENTS_all(uint t);

void nrf_timer_regw_sideeffects_SUBSCRIBE_START(uint inst);
void nrf_timer_regw_sideeffects_SUBSCRIBE_STOP(uint inst);
void nrf_timer_regw_sideeffects_SUBSCRIBE_COUNT(uint inst);
void nrf_timer_regw_sideeffects_SUBSCRIBE_CLEAR(uint inst);
void nrf_timer_regw_sideeffects_SUBSCRIBE_SHUTDOWN(uint inst);
void nrf_timer_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint cc_n);

#if (NHW_HAS_PPI)
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
#endif /* (NHW_HAS_PPI) */

#ifdef __cplusplus
}
#endif

#endif
