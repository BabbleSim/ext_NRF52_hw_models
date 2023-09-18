/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This header exists only for backwards compatibility
 * please use NHW_AES_ECB.h instead
 */
#ifndef _NRF_HW_MODEL_NRF_AES_ECB_H
#define _NRF_HW_MODEL_NRF_AES_ECB_H

#include "NHW_AES_ECB.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_aes_ecb_cheat_set_t_ecb(unsigned int new_t);
void nrf_aes_ecb_cheat_reset_t_ecb(void);

#ifdef __cplusplus
}
#endif

#endif
