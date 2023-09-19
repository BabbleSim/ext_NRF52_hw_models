/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_RNG_H
#define _NRF_HW_MODEL_RNG_H

#ifdef __cplusplus
extern "C"{
#endif

void nhw_RNG_regw_sideeffects_TASKS_START(void);
void nhw_RNG_regw_sideeffects_TASKS_STOP(void);
void nhw_RNG_regw_sideeffects_INTENSET(void);
void nhw_RNG_regw_sideeffects_INTENCLR(void);
void nhw_RNG_regw_sideeffects_EVENTS_all(unsigned int inst);
void nhw_RNG_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nhw_RNG_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_RNG_TASK_START(void);
void nhw_RNG_TASK_STOP(void);

#ifdef __cplusplus
}
#endif

#endif
