/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_RTC_H
#define _NRF_HW_MODEL_RTC_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_RTC_Type NRF_RTC_regs[];
void nrf_rtc_init();
void nrf_rtc_clean_up();
void nrf_rtc_LF_tick();
void nrf_rtc_timer_triggered();
void nrf_rtc_update_COUNTER(int rtc);
void nrf_rtc_regw_sideeffect_TASKS_START(int i);
void nrf_rtc_regw_sideeffect_TASKS_STOP(int i);
void nrf_rtc_regw_sideeffect_TASKS_CLEAR(int i);
void nrf_rtc_regw_sideeffect_INTENSET(int i);
void nrf_rtc_regw_sideeffect_INTENCLR(int i);
void nrf_rtc_regw_sideeffect_EVTENSET(int i);
void nrf_rtc_regw_sideeffect_EVTENCLR(int i);
void nrf_rtc_regw_sideeffects_CC(int rtc, int cc_n);
void nrf_rtc_notify_first_lf_tick(void);

#ifdef __cplusplus
}
#endif

#endif
