/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TEMP_H
#define _NRF_HW_MODEL_TEMP_H

#ifdef __cplusplus
extern "C"{
#endif

void nhw_TEMP_regw_sideeffects_TASKS_START(void);
void nhw_TEMP_regw_sideeffects_TASKS_STOP(void);
void nhw_TEMP_regw_sideeffects_INTENSET(void);
void nhw_TEMP_regw_sideeffects_INTENCLR(void);
void nhw_TEMP_TASK_START(void);
void nhw_TEMP_TASK_STOP(void);
void nhw_TEMP_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nhw_TEMP_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_TEMP_regw_sideeffects_EVENTS_all(unsigned int inst);

#ifdef __cplusplus
}
#endif

#endif
