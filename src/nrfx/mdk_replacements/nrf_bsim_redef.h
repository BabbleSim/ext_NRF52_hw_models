/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file redefines macros from nrf that need to be different for simulated devices.
 */

#ifndef NRF_BSIM_REDEF_H
#define NRF_BSIM_REDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(NRF52833_XXAA)
/*
 * Redefine the base addresses.
 */
extern NRF_AAR_Type NRF_AAR_regs;
#undef NRF_AAR_BASE
#define NRF_AAR_BASE                      (&NRF_AAR_regs)
extern NRF_RNG_Type NRF_RNG_regs;
#undef NRF_RNG_BASE
#define NRF_RNG_BASE                      (&NRF_RNG_regs)
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_BASE
#define NRF_TEMP_BASE                     (&NRF_TEMP_regs)
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
#undef NRF_POWER_BASE
extern NRF_POWER_Type NRF_POWER_regs;
#define NRF_POWER_BASE                    (&NRF_POWER_regs)
extern NRF_GPIO_Type NRF_GPIO_regs[];
#undef NRF_P0_BASE
#define NRF_P0_BASE                       (&NRF_GPIO_regs[0])
#undef NRF_P1_BASE
#define NRF_P1_BASE                       (&NRF_GPIO_regs[1])
extern NRF_GPIOTE_Type NRF_GPIOTE_regs;
#undef NRF_GPIOTE_BASE
#define NRF_GPIOTE_BASE                   (&NRF_GPIOTE_regs)
extern NRF_NVMC_Type NRF_NVMC_regs;
#undef NRF_NVMC_BASE
#define NRF_NVMC_BASE                     (&NRF_NVMC_regs)
extern NRF_EGU_Type NRF_EGU_regs[6];
#undef NRF_EGU0_BASE
#define NRF_EGU0_BASE                     (&NRF_EGU_regs[0])
#undef NRF_EGU1_BASE
#define NRF_EGU1_BASE                     (&NRF_EGU_regs[1])
#undef NRF_EGU2_BASE
#define NRF_EGU2_BASE                     (&NRF_EGU_regs[2])
#undef NRF_EGU3_BASE
#define NRF_EGU3_BASE                     (&NRF_EGU_regs[3])
#undef NRF_EGU4_BASE
#define NRF_EGU4_BASE                     (&NRF_EGU_regs[4])
#undef NRF_EGU5_BASE
#define NRF_EGU5_BASE                     (&NRF_EGU_regs[5])
#undef NRF_UICR_BASE
extern NRF_UICR_Type *NRF_UICR_regs_p;
#define NRF_UICR_BASE                     (NRF_UICR_regs_p)

#endif /* defined(NRF52833_XXAA) */

#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
