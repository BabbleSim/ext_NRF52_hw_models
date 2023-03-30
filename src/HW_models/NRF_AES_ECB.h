/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_AES_ECB_H
#define _NRF_HW_MODEL_AES_ECB_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_aes_ecb_init();
void nrf_aes_ecb_clean_up();
void nrf_ecb_timer_triggered();
void nrf_ecb_regw_sideeffects_INTENSET();
void nrf_ecb_regw_sideeffects_INTENCLEAR();
void nrf_ecb_regw_sideeffects_TASKS_STARTECB();
void nrf_ecb_regw_sideeffects_TASKS_STOPECB();
void nrf_aes_ecb_cheat_set_t_ecb(unsigned int new_t);
void nrf_aes_ecb_cheat_reset_t_ecb(void);

#ifdef __cplusplus
}
#endif

#endif
