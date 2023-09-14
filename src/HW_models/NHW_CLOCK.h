/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_CLOCK_H
#define _NRF_HW_MODEL_CLOCK_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_clock_TASKS_LFCLKSTART(uint i);
void nhw_clock_TASKS_LFCLKSTOP(uint i);
void nhw_clock_TASKS_HFCLKSTART(uint i);
void nhw_clock_TASKS_HFCLKSTOP(uint i);
void nhw_clock_TASKS_CAL(uint i);
void nhw_clock_TASKS_CTSTART(uint inst);
void nhw_clock_TASKS_CTSTOP(uint inst);
void nhw_clock_regw_sideeffects_INTENSET(uint i);
void nhw_clock_regw_sideeffects_INTENCLR(uint i);
void nhw_clock_regw_sideeffects_TASKS_LFCLKSTART(uint i);
void nhw_clock_regw_sideeffects_TASKS_LFCLKSTOP(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLKSTART(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLKSTOP(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLKAUDIOSTART(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLKAUDIOSTOP(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLK192MSTART(uint i);
void nhw_clock_regw_sideeffects_TASKS_HFCLK192MSTOP(uint i);
void nhw_clock_regw_sideeffects_TASKS_CAL(uint i);
void nhw_clock_regw_sideeffects_TASKS_CTSTART(uint i);
void nhw_clock_regw_sideeffects_TASKS_CTSTOP(uint i);
/* Side-effecting function when any event register is written: */
void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLKSTART(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLKSTOP(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_LFCLKSTART(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_LFCLKSTOP(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_CAL(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLKAUDIOSTART(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLKAUDIOSTOP(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLK192MSTART(uint i);
void nhw_clock_regw_sideeffects_SUBSCRIBE_HFCLK192MSTOP(uint i);

#if (NHW_HAS_PPI)
void nhw_clock0_TASKS_LFCLKSTART(void);
void nhw_clock0_TASKS_LFCLKSTOP(void);
void nhw_clock0_TASKS_HFCLKSTART(void);
void nhw_clock0_TASKS_HFCLKSTOP(void);
void nhw_clock0_TASKS_CAL(void);
void nhw_clock0_TASKS_CTSTART(void);
void nhw_clock0_TASKS_CTSTOP(void);
#endif /* (NHW_HAS_PPI) */

union NRF_CLKPWR_Type {
  NRF_CLOCK_Type CLK;
  NRF_POWER_Type PWR;
#if (NHW_CLKPWR_HAS_RESET)
  NRF_RESET_Type RST;
#endif /* (NHW_CLKPWR_HAS_RESET) */
};

extern union NRF_CLKPWR_Type NRF_CLKPWR_regs[];

#ifdef __cplusplus
}
#endif

#endif
