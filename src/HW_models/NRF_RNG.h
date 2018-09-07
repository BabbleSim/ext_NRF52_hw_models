/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_RNG_H
#define _NRF_HW_MODEL_RNG_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrg_rng_init();
void nrf_rng_clean_up();
void nrf_rng_regw_sideeffects();
void nrf_rng_regw_sideeffects_TASK_START();
void nrf_rng_regw_sideeffects_TASK_STOP();
void nrf_rng_regw_sideeffects_INTENSET();
void nrf_rng_regw_sideeffects_INTENCLEAR();
void nrf_rng_timer_triggered();
void nrf_rng_task_start();
void nrf_rng_task_stop();

extern NRF_RNG_Type NRF_RNG_regs;

#ifdef __cplusplus
}
#endif

#endif
