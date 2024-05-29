/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_54L_CLOCK_H
#define _NRF_HW_MODEL_54L_CLOCK_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CLOCK_regw_sideeffects_INTENSET(uint i);
void nhw_CLOCK_regw_sideeffects_INTENCLR(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_PLLSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_PLLSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_LFCLKSTART(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_LFCLKSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_CAL(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOTUNE(uint i);
void nhw_CLOCK_regw_sideeffects_TASKS_XOTUNEABORT(uint i);
/* Side-effecting function when any event register is written: */
void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint i);

void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_PLLSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_PLLSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_LFCLKSTART(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_LFCLKSTOP(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_CAL(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOTUNE(uint i);
void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_XOTUNEABORT(uint i);

union NRF_CLKPWR_Type {
  NRF_CLOCK_Type CLK;
  NRF_POWER_Type PWR;
  NRF_RESET_Type RST;
};

extern union NRF_CLKPWR_Type NRF_CLKPWR_regs[];

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_54L_CLOCK_H */
