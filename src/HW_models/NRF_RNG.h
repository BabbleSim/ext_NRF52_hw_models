/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_RNG_H
#define _NRF_HW_MODEL_RNG_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_rng_regw_sideeffects();
void nrf_rng_regw_sideeffects_TASK_START(void);
void nrf_rng_regw_sideeffects_TASK_STOP(void);
void nrf_rng_regw_sideeffects_INTENSET(void);
void nrf_rng_regw_sideeffects_INTENCLEAR(void);
void nrf_rng_task_start(void);
void nrf_rng_task_stop(void);

extern NRF_RNG_Type NRF_RNG_regs;

#ifdef __cplusplus
}
#endif

#endif
