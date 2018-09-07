/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_AAR_H
#define _NRF_HW_MODEL_AAR_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_AAR_Type NRF_AAR_regs;

void nrf_aar_init();
void nrf_aar_clean_up();

void nrf_aar_TASK_START();
void nrf_aar_TASK_STOP() ;

void nrf_aar_timer_triggered();

#ifdef __cplusplus
}
#endif

#endif
