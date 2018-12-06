/*
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Zephyr's drives and the nrfx HAL
 * expect a few symbols/definitions to be available:
 *   The registers addresses
 *   A few SOC related functions.
 * We provide these here.
 *
 * But note that this model does not provide an actual model
 * of these SOC functions. It is the Zephyr board who is meant
 * to implement them
 */

#ifndef NRF_SOC_IF_H
#define NRF_SOC_IF_H

#include <stddef.h>
#include <stdbool.h>
#include "NRF_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Redefine the addresses to the HW we will be using, so we point
 * to the HW models registers instead of hard addresses
 */
extern NRF_AAR_Type NRF_AAR_regs;
#undef NRF_AAR_BASE
#define NRF_AAR_BASE                      (&NRF_AAR_regs)
extern NRF_RNG_Type NRF_RNG_regs;
#undef NRF_RNG_BASE
#define NRF_RNG_BASE                      (&NRF_RNG_regs)
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_BASE
#define NRF_RTC0_BASE                     (&NRF_RTC_regs[0])
#undef NRF_RTC1_BASE
#define NRF_RTC1_BASE                     (&NRF_RTC_regs[1])
extern NRF_ECB_Type NRF_ECB_regs;
#undef NRF_ECB_BASE
#define NRF_ECB_BASE                      (&NRF_ECB_regs)
extern NRF_CCM_Type NRF_CCM_regs;
#undef NRF_CCM_BASE
#define NRF_CCM_BASE                      (&NRF_CCM_regs)
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_BASE
#define NRF_RADIO_BASE                    (&NRF_RADIO_regs)
extern NRF_CLOCK_Type NRF_CLOCK_regs;
#undef NRF_CLOCK_BASE
#define NRF_CLOCK_BASE                    (&NRF_CLOCK_regs)
extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_BASE
#define NRF_FICR_BASE                     (&NRF_FICR_regs)
extern NRF_PPI_Type NRF_PPI_regs;
#undef NRF_PPI_BASE
#define NRF_PPI_BASE                      (&NRF_PPI_regs)
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_BASE
#define NRF_TIMER0_BASE                   (&NRF_TIMER_regs[0])
#undef NRF_TIMER1_BASE
#define NRF_TIMER1_BASE                   (&NRF_TIMER_regs[1])
#undef NRF_TIMER2_BASE
#define NRF_TIMER2_BASE                   (&NRF_TIMER_regs[2])
#undef NRF_TIMER3_BASE
#define NRF_TIMER3_BASE                   (&NRF_TIMER_regs[3])
#undef NRF_TIMER4_BASE
#define NRF_TIMER4_BASE                   (&NRF_TIMER_regs[4])
extern NRF_POWER_Type NRF_POWER_regs;
#define NRF_POWER_BASE                    (&NRF_POWER_regs)

/**
 * Note that the model of the CPU & IRQ controller driver must provide
 * these functions below. These HW models do not provide them
 */
extern void __WFE(void);
extern void __SEV(void);
extern void NVIC_SetPendingIRQ(IRQn_Type IRQn);
extern void NVIC_ClearPendingIRQ(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif

#endif
