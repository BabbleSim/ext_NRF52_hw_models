/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_AAR_H
#define _NRF_HW_MODEL_AAR_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_aar_TASK_START(void);
void nrf_aar_TASK_STOP(void);
void nrf_aar_regw_sideeffects_INTENSET(void);
void nrf_aar_regw_sideeffects_INTENCLR(void);
void nrf_aar_regw_sideeffects_TASKS_START(void);
void nrf_aar_regw_sideeffects_TASKS_STOP(void);

#ifdef __cplusplus
}
#endif

#endif
