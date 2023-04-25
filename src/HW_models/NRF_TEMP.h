/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TEMP_H
#define _NRF_HW_MODEL_TEMP_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_temp_init();
void nrf_temp_clean_up();
void nrf_temp_regw_sideeffects_TASK_START();
void nrf_temp_regw_sideeffects_TASK_STOP();
void nrf_temp_regw_sideeffects_INTENSET();
void nrf_temp_regw_sideeffects_INTENCLEAR();
void nrf_temp_timer_triggered();
void nrf_temp_task_start();
void nrf_temp_task_stop();

extern NRF_TEMP_Type NRF_TEMP_regs;

#ifdef __cplusplus
}
#endif

#endif
