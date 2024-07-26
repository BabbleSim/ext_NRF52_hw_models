/*
 * Copyright (c) 2024, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_GRTC_H
#define _NRF_HW_MODEL_GRTC_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_GRTC_Type NRF_GRTC_regs;

void nhw_GRTC_regw_sideeffects_TASKS_CAPTURE(uint inst, uint n);
void nhw_GRTC_regw_sideeffects_TASKS_START(uint inst);
void nhw_GRTC_regw_sideeffects_TASKS_STOP(uint inst);
void nhw_GRTC_regw_sideeffects_TASKS_CLEAR(uint inst);
void nhw_GRTC_regw_sideeffects_TASKS_PWMSTART(uint inst);
void nhw_GRTC_regw_sideeffects_TASKS_PWMSTOP(uint inst);

void nhw_GRTC_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint n);

void nhw_GRTC_regw_sideeffects_EVENTS_all(uint inst);

void nhw_GRTC_regw_sideeffects_INTEN(uint inst, uint n);
void nhw_GRTC_regw_sideeffects_INTENSET(uint inst, uint n);
void nhw_GRTC_regw_sideeffects_INTENCLR(uint inst, uint n);

void nhw_GRTC_regw_sideeffects_CC_CCADD(uint inst, uint n);
void nhw_GRTC_regw_sideeffects_CC_CCEN(uint inst, uint cc);
void nhw_GRTC_regw_sideeffects_CC_CCL(uint inst, uint cc);
void nhw_GRTC_regw_sideeffects_CC_CCH(uint inst, uint cc);

uint32_t nhw_GRTC_regr_sideeffects_SYSCOUNTERL(uint inst, uint n);
uint32_t nhw_GRTC_regr_sideeffects_SYSCOUNTERH(uint inst, uint n);


/**
 * Return the time for a given GRTC counter value.
 *
 * @param inst GRTC instance
 * @param value The GRTC value
 * 
 * @returns The timestamp corresponding to a given GRTC value.
 */
bs_time_t nhw_GRTC_counter_to_time(uint inst, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif

