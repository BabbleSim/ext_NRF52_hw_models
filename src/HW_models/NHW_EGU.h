/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_EGU_H
#define _NRF_HW_MODEL_EGU_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_egu_regw_sideeffects_TASK_TRIGGER(uint inst, uint task_nbr);
void nhw_egu_regw_sideeffect_EVENTS_TRIGGERED(uint inst, uint event_nbr);
void nhw_egu_regw_sideeffect_INTENSET(uint inst);
void nhw_egu_regw_sideeffect_INTENCLR(uint inst);
void nhw_egu_regw_sideeffect_INTEN(uint inst);
void nhw_egu_regw_sideeffects_SUBSCRIBE_TRIGGER(uint inst, uint n);

#if (NHW_HAS_PPI)
/*
 * Trampolines to automatically call from the PPI
 */
void nhw_egu_0_TASK_TRIGGER_0(void);
void nhw_egu_0_TASK_TRIGGER_1(void);
void nhw_egu_0_TASK_TRIGGER_2(void);
void nhw_egu_0_TASK_TRIGGER_3(void);
void nhw_egu_0_TASK_TRIGGER_4(void);
void nhw_egu_0_TASK_TRIGGER_5(void);
void nhw_egu_0_TASK_TRIGGER_6(void);
void nhw_egu_0_TASK_TRIGGER_7(void);
void nhw_egu_0_TASK_TRIGGER_8(void);
void nhw_egu_0_TASK_TRIGGER_9(void);
void nhw_egu_0_TASK_TRIGGER_10(void);
void nhw_egu_0_TASK_TRIGGER_11(void);
void nhw_egu_0_TASK_TRIGGER_12(void);
void nhw_egu_0_TASK_TRIGGER_13(void);
void nhw_egu_0_TASK_TRIGGER_14(void);
void nhw_egu_0_TASK_TRIGGER_15(void);
void nhw_egu_1_TASK_TRIGGER_0(void);
void nhw_egu_1_TASK_TRIGGER_1(void);
void nhw_egu_1_TASK_TRIGGER_2(void);
void nhw_egu_1_TASK_TRIGGER_3(void);
void nhw_egu_1_TASK_TRIGGER_4(void);
void nhw_egu_1_TASK_TRIGGER_5(void);
void nhw_egu_1_TASK_TRIGGER_6(void);
void nhw_egu_1_TASK_TRIGGER_7(void);
void nhw_egu_1_TASK_TRIGGER_8(void);
void nhw_egu_1_TASK_TRIGGER_9(void);
void nhw_egu_1_TASK_TRIGGER_10(void);
void nhw_egu_1_TASK_TRIGGER_11(void);
void nhw_egu_1_TASK_TRIGGER_12(void);
void nhw_egu_1_TASK_TRIGGER_13(void);
void nhw_egu_1_TASK_TRIGGER_14(void);
void nhw_egu_1_TASK_TRIGGER_15(void);
void nhw_egu_2_TASK_TRIGGER_0(void);
void nhw_egu_2_TASK_TRIGGER_1(void);
void nhw_egu_2_TASK_TRIGGER_2(void);
void nhw_egu_2_TASK_TRIGGER_3(void);
void nhw_egu_2_TASK_TRIGGER_4(void);
void nhw_egu_2_TASK_TRIGGER_5(void);
void nhw_egu_2_TASK_TRIGGER_6(void);
void nhw_egu_2_TASK_TRIGGER_7(void);
void nhw_egu_2_TASK_TRIGGER_8(void);
void nhw_egu_2_TASK_TRIGGER_9(void);
void nhw_egu_2_TASK_TRIGGER_10(void);
void nhw_egu_2_TASK_TRIGGER_11(void);
void nhw_egu_2_TASK_TRIGGER_12(void);
void nhw_egu_2_TASK_TRIGGER_13(void);
void nhw_egu_2_TASK_TRIGGER_14(void);
void nhw_egu_2_TASK_TRIGGER_15(void);
void nhw_egu_3_TASK_TRIGGER_0(void);
void nhw_egu_3_TASK_TRIGGER_1(void);
void nhw_egu_3_TASK_TRIGGER_2(void);
void nhw_egu_3_TASK_TRIGGER_3(void);
void nhw_egu_3_TASK_TRIGGER_4(void);
void nhw_egu_3_TASK_TRIGGER_5(void);
void nhw_egu_3_TASK_TRIGGER_6(void);
void nhw_egu_3_TASK_TRIGGER_7(void);
void nhw_egu_3_TASK_TRIGGER_8(void);
void nhw_egu_3_TASK_TRIGGER_9(void);
void nhw_egu_3_TASK_TRIGGER_10(void);
void nhw_egu_3_TASK_TRIGGER_11(void);
void nhw_egu_3_TASK_TRIGGER_12(void);
void nhw_egu_3_TASK_TRIGGER_13(void);
void nhw_egu_3_TASK_TRIGGER_14(void);
void nhw_egu_3_TASK_TRIGGER_15(void);
void nhw_egu_4_TASK_TRIGGER_0(void);
void nhw_egu_4_TASK_TRIGGER_1(void);
void nhw_egu_4_TASK_TRIGGER_2(void);
void nhw_egu_4_TASK_TRIGGER_3(void);
void nhw_egu_4_TASK_TRIGGER_4(void);
void nhw_egu_4_TASK_TRIGGER_5(void);
void nhw_egu_4_TASK_TRIGGER_6(void);
void nhw_egu_4_TASK_TRIGGER_7(void);
void nhw_egu_4_TASK_TRIGGER_8(void);
void nhw_egu_4_TASK_TRIGGER_9(void);
void nhw_egu_4_TASK_TRIGGER_10(void);
void nhw_egu_4_TASK_TRIGGER_11(void);
void nhw_egu_4_TASK_TRIGGER_12(void);
void nhw_egu_4_TASK_TRIGGER_13(void);
void nhw_egu_4_TASK_TRIGGER_14(void);
void nhw_egu_4_TASK_TRIGGER_15(void);
void nhw_egu_5_TASK_TRIGGER_0(void);
void nhw_egu_5_TASK_TRIGGER_1(void);
void nhw_egu_5_TASK_TRIGGER_2(void);
void nhw_egu_5_TASK_TRIGGER_3(void);
void nhw_egu_5_TASK_TRIGGER_4(void);
void nhw_egu_5_TASK_TRIGGER_5(void);
void nhw_egu_5_TASK_TRIGGER_6(void);
void nhw_egu_5_TASK_TRIGGER_7(void);
void nhw_egu_5_TASK_TRIGGER_8(void);
void nhw_egu_5_TASK_TRIGGER_9(void);
void nhw_egu_5_TASK_TRIGGER_10(void);
void nhw_egu_5_TASK_TRIGGER_11(void);
void nhw_egu_5_TASK_TRIGGER_12(void);
void nhw_egu_5_TASK_TRIGGER_13(void);
void nhw_egu_5_TASK_TRIGGER_14(void);
void nhw_egu_5_TASK_TRIGGER_15(void);
#endif /* (NHW_HAS_PPI) */

#ifdef __cplusplus
}
#endif

#endif

