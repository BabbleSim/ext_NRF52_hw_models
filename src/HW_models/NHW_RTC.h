/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_RTC_H
#define _NRF_HW_MODEL_RTC_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_rtc_LF_tick(void);
void nhw_rtc_update_COUNTER(uint rtc);
void nhw_rtc_regw_sideeffect_TASKS_START(uint i);
void nhw_rtc_regw_sideeffect_TASKS_STOP(uint i);
void nhw_rtc_regw_sideeffect_TASKS_CLEAR(uint i);
void nhw_rtc_regw_sideeffect_TASKS_TRIGOVRFLW(uint i);
void nhw_rtc_regw_sideeffect_TASKS_CAPTURE(uint i, uint cc);
void nhw_rtc_regw_sideeffect_INTENSET(uint i);
void nhw_rtc_regw_sideeffect_INTENCLR(uint i);
void nhw_rtc_regw_sideeffect_EVTENSET(uint i);
void nhw_rtc_regw_sideeffect_EVTENCLR(uint i);
void nhw_rtc_regw_sideeffects_EVENTS_all(uint rtc);
void nhw_rtc_regw_sideeffects_CC(uint rtc, uint cc_n);
void nrf_rtc_regw_sideeffects_SUBSCRIBE_START(uint inst);
void nrf_rtc_regw_sideeffects_SUBSCRIBE_STOP(uint inst);
void nrf_rtc_regw_sideeffects_SUBSCRIBE_CLEAR(uint inst);
void nrf_rtc_regw_sideeffects_SUBSCRIBE_TRIGOVRFLW(uint inst);
void nhw_rtc_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint cc);
void nhw_rtc_notify_first_lf_tick(void);

#ifdef __cplusplus
}
#endif

#endif
