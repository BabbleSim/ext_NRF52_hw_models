/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_AES_CCM_H
#define _NRF_HW_MODEL_AES_CCM_H

#include "NRF_regs.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_CCM_Type NRF_CCM_regs;

void nrf_aes_ccm_init();
void nrf_aes_ccm_clean_up();

void nrf_ccm_TASK_KSGEN();
void nrf_ccm_TASK_CRYPT();
void nrf_ccm_TASK_STOP();
void nrf_ccm_TASK_RATEOVERRIDE();
void nrf_ccm_radio_received_packet(bool crc_error);
void nrf_ccm_regw_sideeffects_INTENSET();
void nrf_ccm_regw_sideeffects_INTENCLR();
void nrf_ccm_regw_sideeffects_TASKS_KSGEN();

#ifdef __cplusplus
}
#endif

#endif
