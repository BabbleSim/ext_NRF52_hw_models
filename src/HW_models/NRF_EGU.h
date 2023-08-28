/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_EGU_H
#define _NRF_HW_MODEL_EGU_H

#include "nrf.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_EGU_Type NRF_EGU_regs[];

void nrf_egu_regw_sideeffects_TASK_TRIGGER(unsigned int inst, unsigned int task_nbr);
void nrf_egu_regw_sideeffect_EVENTS_TRIGGERED(int inst, unsigned int event_nbr);
void nrf_egu_regw_sideeffect_INTENSET(int inst);
void nrf_egu_regw_sideeffect_INTENCLR(int inst);
void nrf_egu_regw_sideeffect_INTEN(int inst);

/*
 * Trampolines to automatically call from the PPI
 */
void nrf_egu_0_TASK_TRIGGER_0(void);
void nrf_egu_0_TASK_TRIGGER_1(void);
void nrf_egu_0_TASK_TRIGGER_2(void);
void nrf_egu_0_TASK_TRIGGER_3(void);
void nrf_egu_0_TASK_TRIGGER_4(void);
void nrf_egu_0_TASK_TRIGGER_5(void);
void nrf_egu_0_TASK_TRIGGER_6(void);
void nrf_egu_0_TASK_TRIGGER_7(void);
void nrf_egu_0_TASK_TRIGGER_8(void);
void nrf_egu_0_TASK_TRIGGER_9(void);
void nrf_egu_0_TASK_TRIGGER_10(void);
void nrf_egu_0_TASK_TRIGGER_11(void);
void nrf_egu_0_TASK_TRIGGER_12(void);
void nrf_egu_0_TASK_TRIGGER_13(void);
void nrf_egu_0_TASK_TRIGGER_14(void);
void nrf_egu_0_TASK_TRIGGER_15(void);
void nrf_egu_1_TASK_TRIGGER_0(void);
void nrf_egu_1_TASK_TRIGGER_1(void);
void nrf_egu_1_TASK_TRIGGER_2(void);
void nrf_egu_1_TASK_TRIGGER_3(void);
void nrf_egu_1_TASK_TRIGGER_4(void);
void nrf_egu_1_TASK_TRIGGER_5(void);
void nrf_egu_1_TASK_TRIGGER_6(void);
void nrf_egu_1_TASK_TRIGGER_7(void);
void nrf_egu_1_TASK_TRIGGER_8(void);
void nrf_egu_1_TASK_TRIGGER_9(void);
void nrf_egu_1_TASK_TRIGGER_10(void);
void nrf_egu_1_TASK_TRIGGER_11(void);
void nrf_egu_1_TASK_TRIGGER_12(void);
void nrf_egu_1_TASK_TRIGGER_13(void);
void nrf_egu_1_TASK_TRIGGER_14(void);
void nrf_egu_1_TASK_TRIGGER_15(void);
void nrf_egu_2_TASK_TRIGGER_0(void);
void nrf_egu_2_TASK_TRIGGER_1(void);
void nrf_egu_2_TASK_TRIGGER_2(void);
void nrf_egu_2_TASK_TRIGGER_3(void);
void nrf_egu_2_TASK_TRIGGER_4(void);
void nrf_egu_2_TASK_TRIGGER_5(void);
void nrf_egu_2_TASK_TRIGGER_6(void);
void nrf_egu_2_TASK_TRIGGER_7(void);
void nrf_egu_2_TASK_TRIGGER_8(void);
void nrf_egu_2_TASK_TRIGGER_9(void);
void nrf_egu_2_TASK_TRIGGER_10(void);
void nrf_egu_2_TASK_TRIGGER_11(void);
void nrf_egu_2_TASK_TRIGGER_12(void);
void nrf_egu_2_TASK_TRIGGER_13(void);
void nrf_egu_2_TASK_TRIGGER_14(void);
void nrf_egu_2_TASK_TRIGGER_15(void);
void nrf_egu_3_TASK_TRIGGER_0(void);
void nrf_egu_3_TASK_TRIGGER_1(void);
void nrf_egu_3_TASK_TRIGGER_2(void);
void nrf_egu_3_TASK_TRIGGER_3(void);
void nrf_egu_3_TASK_TRIGGER_4(void);
void nrf_egu_3_TASK_TRIGGER_5(void);
void nrf_egu_3_TASK_TRIGGER_6(void);
void nrf_egu_3_TASK_TRIGGER_7(void);
void nrf_egu_3_TASK_TRIGGER_8(void);
void nrf_egu_3_TASK_TRIGGER_9(void);
void nrf_egu_3_TASK_TRIGGER_10(void);
void nrf_egu_3_TASK_TRIGGER_11(void);
void nrf_egu_3_TASK_TRIGGER_12(void);
void nrf_egu_3_TASK_TRIGGER_13(void);
void nrf_egu_3_TASK_TRIGGER_14(void);
void nrf_egu_3_TASK_TRIGGER_15(void);
void nrf_egu_4_TASK_TRIGGER_0(void);
void nrf_egu_4_TASK_TRIGGER_1(void);
void nrf_egu_4_TASK_TRIGGER_2(void);
void nrf_egu_4_TASK_TRIGGER_3(void);
void nrf_egu_4_TASK_TRIGGER_4(void);
void nrf_egu_4_TASK_TRIGGER_5(void);
void nrf_egu_4_TASK_TRIGGER_6(void);
void nrf_egu_4_TASK_TRIGGER_7(void);
void nrf_egu_4_TASK_TRIGGER_8(void);
void nrf_egu_4_TASK_TRIGGER_9(void);
void nrf_egu_4_TASK_TRIGGER_10(void);
void nrf_egu_4_TASK_TRIGGER_11(void);
void nrf_egu_4_TASK_TRIGGER_12(void);
void nrf_egu_4_TASK_TRIGGER_13(void);
void nrf_egu_4_TASK_TRIGGER_14(void);
void nrf_egu_4_TASK_TRIGGER_15(void);
void nrf_egu_5_TASK_TRIGGER_0(void);
void nrf_egu_5_TASK_TRIGGER_1(void);
void nrf_egu_5_TASK_TRIGGER_2(void);
void nrf_egu_5_TASK_TRIGGER_3(void);
void nrf_egu_5_TASK_TRIGGER_4(void);
void nrf_egu_5_TASK_TRIGGER_5(void);
void nrf_egu_5_TASK_TRIGGER_6(void);
void nrf_egu_5_TASK_TRIGGER_7(void);
void nrf_egu_5_TASK_TRIGGER_8(void);
void nrf_egu_5_TASK_TRIGGER_9(void);
void nrf_egu_5_TASK_TRIGGER_10(void);
void nrf_egu_5_TASK_TRIGGER_11(void);
void nrf_egu_5_TASK_TRIGGER_12(void);
void nrf_egu_5_TASK_TRIGGER_13(void);
void nrf_egu_5_TASK_TRIGGER_14(void);
void nrf_egu_5_TASK_TRIGGER_15(void);

#ifdef __cplusplus
}
#endif

#endif

