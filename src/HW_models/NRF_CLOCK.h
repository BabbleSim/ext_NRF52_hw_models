/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_CLOCK_H
#define _NRF_HW_MODEL_CLOCK_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_CLOCK_Type NRF_CLOCK_regs;
void nrf_clock_init();
void nrf_clock_clean_up();
void nrf_clock_LFTimer_triggered();
void nrf_clock_HFTimer_triggered();
void nrf_clock_TASKS_LFCLKSTART();
void nrf_clock_TASKS_HFCLKSTART();
void nrf_clock_reqw_sideeffects_INTENSET();
void nrf_clock_reqw_sideeffects_INTENCLR();
void nrf_clock_reqw_sideeffects_TASKS_LFCLKSTART();
void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTART();
void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTOP();

#define LF_CLOCK_PERIOD  15625 /*in a fixed point format with 9 bits per us, the LF clock period*/

#ifdef __cplusplus
}
#endif

#endif
