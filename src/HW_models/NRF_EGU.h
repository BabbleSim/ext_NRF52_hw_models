/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_EGU_H
#define _NRF_HW_MODEL_EGU_H

#include "nrfx.h"
#include "hal/nrf_egu.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_EGU_Type NRF_EGU_regs[EGU_COUNT];

void nrf_egu_regw_sideeffects_TASKS_TRIGGERED(NRF_EGU_Type * p_reg);
void nrf_egu_regw_sideeffects_EVENTS_TRIGGERED_INTEN(NRF_EGU_Type * p_reg);
void nrf_egu_regw_sideeffects_INTENSET(NRF_EGU_Type * p_reg);
void nrf_egu_regw_sideeffects_INTENCLR(NRF_EGU_Type * p_reg);


#ifdef __cplusplus
}
#endif

#endif

