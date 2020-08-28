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
extern NRF_GPIO_Type NRF_P0_regs;
#undef NRF_P0_BASE
#define NRF_P0_BASE                       (&NRF_P0_regs)
extern NRF_GPIOTE_Type NRF_GPIOTE_regs;
#undef NRF_GPIOTE_BASE
#define NRF_GPIOTE_BASE                   (&NRF_GPIOTE_regs)
extern NRF_NVMC_Type NRF_NVMC_regs;
#undef NRF_NVMC_BASE
#define NRF_NVMC_BASE                     (&NRF_NVMC_regs)

/*
 * Redefine the peripheral pointers
 */
#define NRF_FICR                    ((NRF_FICR_Type*)          NRF_FICR_BASE)
#define NRF_UICR                    ((NRF_UICR_Type*)          NRF_UICR_BASE)
#define NRF_BPROT                   ((NRF_BPROT_Type*)         NRF_BPROT_BASE)
#define NRF_POWER                   ((NRF_POWER_Type*)         NRF_POWER_BASE)
#define NRF_CLOCK                   ((NRF_CLOCK_Type*)         NRF_CLOCK_BASE)
#define NRF_RADIO                   ((NRF_RADIO_Type*)         NRF_RADIO_BASE)
#define NRF_UARTE0                  ((NRF_UARTE_Type*)         NRF_UARTE0_BASE)
#define NRF_UART0                   ((NRF_UART_Type*)          NRF_UART0_BASE)
#define NRF_SPIM0                   ((NRF_SPIM_Type*)          NRF_SPIM0_BASE)
#define NRF_SPIS0                   ((NRF_SPIS_Type*)          NRF_SPIS0_BASE)
#define NRF_TWIM0                   ((NRF_TWIM_Type*)          NRF_TWIM0_BASE)
#define NRF_TWIS0                   ((NRF_TWIS_Type*)          NRF_TWIS0_BASE)
#define NRF_SPI0                    ((NRF_SPI_Type*)           NRF_SPI0_BASE)
#define NRF_TWI0                    ((NRF_TWI_Type*)           NRF_TWI0_BASE)
#define NRF_SPIM1                   ((NRF_SPIM_Type*)          NRF_SPIM1_BASE)
#define NRF_SPIS1                   ((NRF_SPIS_Type*)          NRF_SPIS1_BASE)
#define NRF_TWIM1                   ((NRF_TWIM_Type*)          NRF_TWIM1_BASE)
#define NRF_TWIS1                   ((NRF_TWIS_Type*)          NRF_TWIS1_BASE)
#define NRF_SPI1                    ((NRF_SPI_Type*)           NRF_SPI1_BASE)
#define NRF_TWI1                    ((NRF_TWI_Type*)           NRF_TWI1_BASE)
#define NRF_NFCT                    ((NRF_NFCT_Type*)          NRF_NFCT_BASE)
#define NRF_GPIOTE                  ((NRF_GPIOTE_Type*)        NRF_GPIOTE_BASE)
#define NRF_SAADC                   ((NRF_SAADC_Type*)         NRF_SAADC_BASE)
#define NRF_TIMER0                  ((NRF_TIMER_Type*)         NRF_TIMER0_BASE)
#define NRF_TIMER1                  ((NRF_TIMER_Type*)         NRF_TIMER1_BASE)
#define NRF_TIMER2                  ((NRF_TIMER_Type*)         NRF_TIMER2_BASE)
#define NRF_RTC0                    ((NRF_RTC_Type*)           NRF_RTC0_BASE)
#define NRF_TEMP                    ((NRF_TEMP_Type*)          NRF_TEMP_BASE)
#define NRF_RNG                     ((NRF_RNG_Type*)           NRF_RNG_BASE)
#define NRF_ECB                     ((NRF_ECB_Type*)           NRF_ECB_BASE)
#define NRF_CCM                     ((NRF_CCM_Type*)           NRF_CCM_BASE)
#define NRF_AAR                     ((NRF_AAR_Type*)           NRF_AAR_BASE)
#define NRF_WDT                     ((NRF_WDT_Type*)           NRF_WDT_BASE)
#define NRF_RTC1                    ((NRF_RTC_Type*)           NRF_RTC1_BASE)
#define NRF_QDEC                    ((NRF_QDEC_Type*)          NRF_QDEC_BASE)
#define NRF_COMP                    ((NRF_COMP_Type*)          NRF_COMP_BASE)
#define NRF_LPCOMP                  ((NRF_LPCOMP_Type*)        NRF_LPCOMP_BASE)
#define NRF_SWI0                    ((NRF_SWI_Type*)           NRF_SWI0_BASE)
#define NRF_EGU0                    ((NRF_EGU_Type*)           NRF_EGU0_BASE)
#define NRF_SWI1                    ((NRF_SWI_Type*)           NRF_SWI1_BASE)
#define NRF_EGU1                    ((NRF_EGU_Type*)           NRF_EGU1_BASE)
#define NRF_SWI2                    ((NRF_SWI_Type*)           NRF_SWI2_BASE)
#define NRF_EGU2                    ((NRF_EGU_Type*)           NRF_EGU2_BASE)
#define NRF_SWI3                    ((NRF_SWI_Type*)           NRF_SWI3_BASE)
#define NRF_EGU3                    ((NRF_EGU_Type*)           NRF_EGU3_BASE)
#define NRF_SWI4                    ((NRF_SWI_Type*)           NRF_SWI4_BASE)
#define NRF_EGU4                    ((NRF_EGU_Type*)           NRF_EGU4_BASE)
#define NRF_SWI5                    ((NRF_SWI_Type*)           NRF_SWI5_BASE)
#define NRF_EGU5                    ((NRF_EGU_Type*)           NRF_EGU5_BASE)
#define NRF_TIMER3                  ((NRF_TIMER_Type*)         NRF_TIMER3_BASE)
#define NRF_TIMER4                  ((NRF_TIMER_Type*)         NRF_TIMER4_BASE)
#define NRF_PWM0                    ((NRF_PWM_Type*)           NRF_PWM0_BASE)
#define NRF_PDM                     ((NRF_PDM_Type*)           NRF_PDM_BASE)
#define NRF_NVMC                    ((NRF_NVMC_Type*)          NRF_NVMC_BASE)
#define NRF_PPI                     ((NRF_PPI_Type*)           NRF_PPI_BASE)
#define NRF_MWU                     ((NRF_MWU_Type*)           NRF_MWU_BASE)
#define NRF_PWM1                    ((NRF_PWM_Type*)           NRF_PWM1_BASE)
#define NRF_PWM2                    ((NRF_PWM_Type*)           NRF_PWM2_BASE)
#define NRF_SPIM2                   ((NRF_SPIM_Type*)          NRF_SPIM2_BASE)
#define NRF_SPIS2                   ((NRF_SPIS_Type*)          NRF_SPIS2_BASE)
#define NRF_SPI2                    ((NRF_SPI_Type*)           NRF_SPI2_BASE)
#define NRF_RTC2                    ((NRF_RTC_Type*)           NRF_RTC2_BASE)
#define NRF_I2S                     ((NRF_I2S_Type*)           NRF_I2S_BASE)
#define NRF_FPU                     ((NRF_FPU_Type*)           NRF_FPU_BASE)
#define NRF_P0                      ((NRF_GPIO_Type*)          NRF_P0_BASE)

#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
