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

void nrf_temp_regw_sideeffects_TASK_START(void);
void nrf_temp_regw_sideeffects_TASK_STOP(void);
void nrf_temp_regw_sideeffects_INTENSET(void);
void nrf_temp_regw_sideeffects_INTENCLEAR(void);
void nrf_temp_task_start(void);
void nrf_temp_task_stop(void);

#ifdef __cplusplus
}
#endif

#endif
