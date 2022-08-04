/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_EGU_H
#define _NRF_HW_MODEL_EGU_H

#include "nrfx.h"
#include "hal/nrf_egu.h"
#include "NRF_PPI.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_EGU_Type NRF_EGU_regs[EGU_COUNT];
extern const ppi_event_types_t PPI_EGU_EVENTS[EGU_COUNT][EGU0_CH_NUM];

#define X_EGU_CHANNELS\
	X(0)\
	X(1)\
	X(2)\
	X(3)\
	X(4)\
	X(5)\
	X(6)\
	X(7)\
	X(8)\
	X(9)\
	X(10)\
	X(11)\
	X(12)\
	X(13)\
	X(14)\
	X(15)


#define X(channel) void nrf_egu_0_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

#define X(channel) void nrf_egu_1_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

#define X(channel) void nrf_egu_2_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

#define X(channel) void nrf_egu_3_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

#define X(channel) void nrf_egu_4_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

#define X(channel) void nrf_egu_5_TASK_TRIGGER_ ## channel(void);
X_EGU_CHANNELS
#undef X

void nrf_egu_regw_sideeffects_TASKS_TRIGGER(NRF_EGU_Type *p_reg, nrf_egu_task_t egu_task);
void nrf_egu_regw_sideeffects_EVENTS_TRIGGERED(NRF_EGU_Type *p_reg, nrf_egu_event_t egu_channel);
void nrf_egu_regw_sideeffects_INTENSET(NRF_EGU_Type * p_reg);
void nrf_egu_regw_sideeffects_INTENCLR(NRF_EGU_Type * p_reg);

#ifdef __cplusplus
}
#endif

#endif

