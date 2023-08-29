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

void nrf_rng_regw_sideeffects_TASK_START(void);
void nrf_rng_regw_sideeffects_TASK_STOP(void);
void nrf_rng_regw_sideeffects_INTENSET(void);
void nrf_rng_regw_sideeffects_INTENCLEAR(void);
void nrf_rng_regw_sideeffects_EVENTS_all(void);
void nrf_rng_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nrf_rng_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nrf_rng_task_start(void);
void nrf_rng_task_stop(void);

#ifdef __cplusplus
}
#endif

#endif
