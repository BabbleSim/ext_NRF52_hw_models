/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NHW_AES_ECB_H
#define _NRF_HW_MODEL_NHW_AES_ECB_H

#ifdef __cplusplus
extern "C"{
#endif

void nhw_ECB_regw_sideeffects_INTENSET(void);
void nhw_ECB_regw_sideeffects_INTENCLR(void);
void nhw_ECB_regw_sideeffects_TASKS_STARTECB(void);
void nhw_ECB_regw_sideeffects_TASKS_STOPECB(void);
void nhw_ECB_regw_sideeffects_SUBSCRIBE_STARTECB(unsigned int inst);
void nhw_ECB_regw_sideeffects_SUBSCRIBE_STOPECB(unsigned int inst);
void nhw_ECB_regw_sideeffects_EVENTS_all(unsigned int inst);
void nhw_aes_ecb_cheat_set_t_ecb(unsigned int new_t);
void nhw_aes_ecb_cheat_reset_t_ecb(void);

#ifdef __cplusplus
}
#endif

#endif
