/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020-2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file redefines macros from nrf that need to be different for simulated devices.
 */

#ifndef NRF_BSIM_REDEF_H
#define NRF_BSIM_REDEF_H

#include "NHW_config.h"

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
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_BASE
#define NRF_CLOCK_BASE                    (NRF_CLOCK_regs[NHW_CLKPWR_0])
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
extern NRF_POWER_Type *NRF_POWER_regs[];
#define NRF_POWER_BASE                    (NRF_POWER_regs[NHW_CLKPWR_0])
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

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF5340_XXAA_NETWORK)

#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE           NULL
#undef NRF_UICR_NS_BASE
#define NRF_UICR_NS_BASE           NULL
#undef NRF_CTI_NS_BASE
#define NRF_CTI_NS_BASE            NULL
#undef NRF_DCNF_NS_BASE
#define NRF_DCNF_NS_BASE           NULL
#undef NRF_VREQCTRL_NS_BASE
#define NRF_VREQCTRL_NS_BASE       NULL
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE          (NRF_CLOCK_regs[NHW_CLKPWR_NET0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE          (NRF_POWER_regs[NHW_CLKPWR_NET0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE          (NRF_RESET_regs[NHW_CLKPWR_NET0])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE         NULL
extern NRF_RADIO_Type NRF_RADIO_regs;
#undef NRF_RADIO_NS_BASE
#define NRF_RADIO_NS_BASE          (&NRF_RADIO_regs)
extern NRF_RNG_Type NRF_RNG_regs;
#undef NRF_RNG_NS_BASE
#define NRF_RNG_NS_BASE            (&NRF_RNG_regs)
#undef NRF_GPIOTE_NS_BASE
#define NRF_GPIOTE_NS_BASE         NULL
#undef NRF_WDT_NS_BASE
#define NRF_WDT_NS_BASE            NULL
#undef NRF_ECB_NS_BASE
extern NRF_ECB_Type NRF_ECB_regs;
#define NRF_ECB_NS_BASE            (&NRF_ECB_regs)
extern NRF_AAR_Type NRF_AAR_regs;
#undef NRF_AAR_NS_BASE
#define NRF_AAR_NS_BASE            (&NRF_AAR_regs)
extern NRF_CCM_Type NRF_CCM_regs;
#undef NRF_CCM_NS_BASE
#define NRF_CCM_NS_BASE            (&NRF_CCM_regs)
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC_NS_BASE
#define NRF_DPPIC_NS_BASE          (&NRF_DPPIC_regs[NHW_DPPI_NET_0])
extern NRF_TEMP_Type NRF_TEMP_regs;
#undef NRF_TEMP_NS_BASE
#define NRF_TEMP_NS_BASE           (&NRF_TEMP_regs)
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_NS_BASE
#define NRF_RTC0_NS_BASE           (&NRF_RTC_regs[NHW_RTC_NET0])
#undef NRF_RTC1_NS_BASE
#define NRF_RTC1_NS_BASE           (&NRF_RTC_regs[NHW_RTC_NET1])
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE            NULL
#undef NRF_SPIM0_NS_BASE
#define NRF_SPIM0_NS_BASE          NULL
#undef NRF_SPIS0_NS_BASE
#define NRF_SPIS0_NS_BASE          NULL
#undef NRF_TWIM0_NS_BASE
#define NRF_TWIM0_NS_BASE          NULL
#undef NRF_TWIS0_NS_BASE
#define NRF_TWIS0_NS_BASE          NULL
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE         NULL
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU0_NS_BASE
#define NRF_EGU0_NS_BASE           (&NRF_EGU_regs[NHW_EGU_NET0])
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_NS_BASE
#define NRF_TIMER0_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET0])
#undef NRF_TIMER1_NS_BASE
#define NRF_TIMER1_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET1])
#undef NRF_TIMER2_NS_BASE
#define NRF_TIMER2_NS_BASE         (&NRF_TIMER_regs[NHW_TIMER_NET2])
extern int NRF_SWI_regs[];
#undef NRF_SWI0_NS_BASE
#define NRF_SWI0_NS_BASE           (&NRF_SWI_regs[NHW_SWI_NET0])
#undef NRF_SWI1_NS_BASE
#define NRF_SWI1_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET1])
#undef NRF_SWI2_NS_BASE
#define NRF_SWI2_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET2])
#undef NRF_SWI3_NS_BASE
#define NRF_SWI3_NS_BASE            (&NRF_SWI_regs[NHW_SWI_NET3])
#undef NRF_APPMUTEX_NS_BASE
#define NRF_APPMUTEX_NS_BASE       NULL
#undef NRF_APPMUTEX_S_BASE
#define NRF_APPMUTEX_S_BASE        NULL
#undef NRF_ACL_NS_BASE
#define NRF_ACL_NS_BASE            NULL
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE           NULL
#undef NRF_VMC_NS_BASE
#define NRF_VMC_NS_BASE            NULL
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE             NULL
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE             NULL

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF5340_XXAA_APPLICATION)

#undef NRF_CACHEDATA_S_BASE
#define NRF_CACHEDATA_S_BASE        NULL
#undef NRF_CACHEINFO_S_BASE
#define NRF_CACHEINFO_S_BASE        NULL
#undef NRF_FICR_S_BASE
#define NRF_FICR_S_BASE             NULL
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE             NULL
#undef NRF_CTI_S_BASE
#define NRF_CTI_S_BASE              NULL
#undef NRF_TAD_S_BASE
#define NRF_TAD_S_BASE              NULL
#undef NRF_DCNF_NS_BASE
#define NRF_DCNF_NS_BASE            NULL
#undef NRF_FPU_NS_BASE
#define NRF_FPU_NS_BASE             NULL
#undef NRF_DCNF_S_BASE
#define NRF_DCNF_S_BASE             NULL
#undef NRF_FPU_S_BASE
#define NRF_FPU_S_BASE              NULL
#undef NRF_CACHE_S_BASE
#define NRF_CACHE_S_BASE            NULL
#undef NRF_SPU_S_BASE
#define NRF_SPU_S_BASE              NULL
#undef NRF_OSCILLATORS_NS_BASE
#define NRF_OSCILLATORS_NS_BASE     NULL
#undef NRF_REGULATORS_NS_BASE
#define NRF_REGULATORS_NS_BASE      NULL
#undef NRF_OSCILLATORS_S_BASE
#define NRF_OSCILLATORS_S_BASE      NULL
#undef NRF_REGULATORS_S_BASE
#define NRF_REGULATORS_S_BASE       NULL
extern NRF_CLOCK_Type *NRF_CLOCK_regs[];
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE          (NRF_CLOCK_regs[NHW_CLKPWR_APP0])
extern NRF_POWER_Type *NRF_POWER_regs[];
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE          (NRF_POWER_regs[NHW_CLKPWR_APP0])
extern NRF_RESET_Type *NRF_RESET_regs[];
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE          (NRF_RESET_regs[NHW_CLKPWR_APP0])
#undef NRF_CLOCK_S_BASE
#define NRF_CLOCK_S_BASE           (NRF_CLOCK_regs[NHW_CLKPWR_APP0])
#undef NRF_POWER_S_BASE
#define NRF_POWER_S_BASE           (NRF_POWER_regs[NHW_CLKPWR_APP0])
#undef NRF_RESET_S_BASE
#define NRF_RESET_S_BASE           (NRF_RESET_regs[NHW_CLKPWR_APP0])
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE          NULL
#undef NRF_CTRLAP_S_BASE
#define NRF_CTRLAP_S_BASE           NULL
#undef NRF_SPIM0_NS_BASE
#define NRF_SPIM0_NS_BASE           NULL
#undef NRF_SPIS0_NS_BASE
#define NRF_SPIS0_NS_BASE           NULL
#undef NRF_TWIM0_NS_BASE
#define NRF_TWIM0_NS_BASE           NULL
#undef NRF_TWIS0_NS_BASE
#define NRF_TWIS0_NS_BASE           NULL
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE          NULL
#undef NRF_SPIM0_S_BASE
#define NRF_SPIM0_S_BASE            NULL
#undef NRF_SPIS0_S_BASE
#define NRF_SPIS0_S_BASE            NULL
#undef NRF_TWIM0_S_BASE
#define NRF_TWIM0_S_BASE            NULL
#undef NRF_TWIS0_S_BASE
#define NRF_TWIS0_S_BASE            NULL
#undef NRF_UARTE0_S_BASE
#define NRF_UARTE0_S_BASE           NULL
#undef NRF_SPIM1_NS_BASE
#define NRF_SPIM1_NS_BASE           NULL
#undef NRF_SPIS1_NS_BASE
#define NRF_SPIS1_NS_BASE           NULL
#undef NRF_TWIM1_NS_BASE
#define NRF_TWIM1_NS_BASE           NULL
#undef NRF_TWIS1_NS_BASE
#define NRF_TWIS1_NS_BASE           NULL
#undef NRF_UARTE1_NS_BASE
#define NRF_UARTE1_NS_BASE          NULL
#undef NRF_SPIM1_S_BASE
#define NRF_SPIM1_S_BASE            NULL
#undef NRF_SPIS1_S_BASE
#define NRF_SPIS1_S_BASE            NULL
#undef NRF_TWIM1_S_BASE
#define NRF_TWIM1_S_BASE            NULL
#undef NRF_TWIS1_S_BASE
#define NRF_TWIS1_S_BASE            NULL
#undef NRF_UARTE1_S_BASE
#define NRF_UARTE1_S_BASE           NULL
#undef NRF_SPIM4_NS_BASE
#define NRF_SPIM4_NS_BASE           NULL
#undef NRF_SPIM4_S_BASE
#define NRF_SPIM4_S_BASE            NULL
#undef NRF_SPIM2_NS_BASE
#define NRF_SPIM2_NS_BASE           NULL
#undef NRF_SPIS2_NS_BASE
#define NRF_SPIS2_NS_BASE           NULL
#undef NRF_TWIM2_NS_BASE
#define NRF_TWIM2_NS_BASE           NULL
#undef NRF_TWIS2_NS_BASE
#define NRF_TWIS2_NS_BASE           NULL
#undef NRF_UARTE2_NS_BASE
#define NRF_UARTE2_NS_BASE          NULL
#undef NRF_SPIM2_S_BASE
#define NRF_SPIM2_S_BASE            NULL
#undef NRF_SPIS2_S_BASE
#define NRF_SPIS2_S_BASE            NULL
#undef NRF_TWIM2_S_BASE
#define NRF_TWIM2_S_BASE            NULL
#undef NRF_TWIS2_S_BASE
#define NRF_TWIS2_S_BASE            NULL
#undef NRF_UARTE2_S_BASE
#define NRF_UARTE2_S_BASE           NULL
#undef NRF_SPIM3_NS_BASE
#define NRF_SPIM3_NS_BASE           NULL
#undef NRF_SPIS3_NS_BASE
#define NRF_SPIS3_NS_BASE           NULL
#undef NRF_TWIM3_NS_BASE
#define NRF_TWIM3_NS_BASE           NULL
#undef NRF_TWIS3_NS_BASE
#define NRF_TWIS3_NS_BASE           NULL
#undef NRF_UARTE3_NS_BASE
#define NRF_UARTE3_NS_BASE          NULL
#undef NRF_SPIM3_S_BASE
#define NRF_SPIM3_S_BASE            NULL
#undef NRF_SPIS3_S_BASE
#define NRF_SPIS3_S_BASE            NULL
#undef NRF_TWIM3_S_BASE
#define NRF_TWIM3_S_BASE            NULL
#undef NRF_TWIS3_S_BASE
#define NRF_TWIS3_S_BASE            NULL
#undef NRF_UARTE3_S_BASE
#define NRF_UARTE3_S_BASE           NULL
#undef NRF_GPIOTE0_S_BASE
#define NRF_GPIOTE0_S_BASE          NULL
#undef NRF_SAADC_NS_BASE
#define NRF_SAADC_NS_BASE           NULL
#undef NRF_SAADC_S_BASE
#define NRF_SAADC_S_BASE            NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER0_NS_BASE
#define NRF_TIMER0_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP0])
#undef NRF_TIMER0_S_BASE
#define NRF_TIMER0_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP0])
#undef NRF_TIMER1_NS_BASE
#define NRF_TIMER1_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP1])
#undef NRF_TIMER1_S_BASE
#define NRF_TIMER1_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP1])
#undef NRF_TIMER2_NS_BASE
#define NRF_TIMER2_NS_BASE          (&NRF_TIMER_regs[NHW_TIMER_APP2])
#undef NRF_TIMER2_S_BASE
#define NRF_TIMER2_S_BASE           (&NRF_TIMER_regs[NHW_TIMER_APP2])
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC0_NS_BASE
#define NRF_RTC0_NS_BASE            (&NRF_RTC_regs[NHW_RTC_APP0])
#undef NRF_RTC0_S_BASE
#define NRF_RTC0_S_BASE             (&NRF_RTC_regs[NHW_RTC_APP0])
#undef NRF_RTC1_NS_BASE
#define NRF_RTC1_NS_BASE            (&NRF_RTC_regs[NHW_RTC_APP1])
#undef NRF_RTC1_S_BASE
#define NRF_RTC1_S_BASE             (&NRF_RTC_regs[NHW_RTC_APP1])
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC_NS_BASE
#define NRF_DPPIC_NS_BASE           (&NRF_DPPIC_regs[NHW_DPPI_APP_0])
#undef NRF_DPPIC_S_BASE
#define NRF_DPPIC_S_BASE            (&NRF_DPPIC_regs[NHW_DPPI_APP_0])
#undef NRF_WDT0_NS_BASE
#define NRF_WDT0_NS_BASE            NULL
#undef NRF_WDT0_S_BASE
#define NRF_WDT0_S_BASE             NULL
#undef NRF_WDT1_NS_BASE
#define NRF_WDT1_NS_BASE            NULL
#undef NRF_WDT1_S_BASE
#define NRF_WDT1_S_BASE             NULL
#undef NRF_COMP_NS_BASE
#define NRF_COMP_NS_BASE            NULL
#undef NRF_LPCOMP_NS_BASE
#define NRF_LPCOMP_NS_BASE          NULL
#undef NRF_COMP_S_BASE
#define NRF_COMP_S_BASE             NULL
#undef NRF_LPCOMP_S_BASE
#define NRF_LPCOMP_S_BASE           NULL
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU0_NS_BASE
#define NRF_EGU0_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP0])
#undef NRF_EGU0_S_BASE
#define NRF_EGU0_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP0])
#undef NRF_EGU1_NS_BASE
#define NRF_EGU1_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP1])
#undef NRF_EGU1_S_BASE
#define NRF_EGU1_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP1])
#undef NRF_EGU2_NS_BASE
#define NRF_EGU2_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP2])
#undef NRF_EGU2_S_BASE
#define NRF_EGU2_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP2])
#undef NRF_EGU3_NS_BASE
#define NRF_EGU3_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP3])
#undef NRF_EGU3_S_BASE
#define NRF_EGU3_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP3])
#undef NRF_EGU4_NS_BASE
#define NRF_EGU4_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP4])
#undef NRF_EGU4_S_BASE
#define NRF_EGU4_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP4])
#undef NRF_EGU5_NS_BASE
#define NRF_EGU5_NS_BASE           (&NRF_EGU_regs[NHW_EGU_APP5])
#undef NRF_EGU5_S_BASE
#define NRF_EGU5_S_BASE            (&NRF_EGU_regs[NHW_EGU_APP5])
#undef NRF_PWM0_NS_BASE
#define NRF_PWM0_NS_BASE            NULL
#undef NRF_PWM0_S_BASE
#define NRF_PWM0_S_BASE             NULL
#undef NRF_PWM1_NS_BASE
#define NRF_PWM1_NS_BASE            NULL
#undef NRF_PWM1_S_BASE
#define NRF_PWM1_S_BASE             NULL
#undef NRF_PWM2_NS_BASE
#define NRF_PWM2_NS_BASE            NULL
#undef NRF_PWM2_S_BASE
#define NRF_PWM2_S_BASE             NULL
#undef NRF_PWM3_NS_BASE
#define NRF_PWM3_NS_BASE            NULL
#undef NRF_PWM3_S_BASE
#define NRF_PWM3_S_BASE             NULL
#undef NRF_PDM0_NS_BASE
#define NRF_PDM0_NS_BASE            NULL
#undef NRF_PDM0_S_BASE
#define NRF_PDM0_S_BASE             NULL
#undef NRF_I2S0_NS_BASE
#define NRF_I2S0_NS_BASE            NULL
#undef NRF_I2S0_S_BASE
#define NRF_I2S0_S_BASE             NULL
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE             NULL
#undef NRF_IPC_S_BASE
#define NRF_IPC_S_BASE              NULL
#undef NRF_QSPI_NS_BASE
#define NRF_QSPI_NS_BASE            NULL
#undef NRF_QSPI_S_BASE
#define NRF_QSPI_S_BASE             NULL
#undef NRF_NFCT_NS_BASE
#define NRF_NFCT_NS_BASE            NULL
#undef NRF_NFCT_S_BASE
#define NRF_NFCT_S_BASE             NULL
#undef NRF_GPIOTE1_NS_BASE
#define NRF_GPIOTE1_NS_BASE         NULL
#undef NRF_MUTEX_NS_BASE
#define NRF_MUTEX_NS_BASE           NULL
#undef NRF_MUTEX_S_BASE
#define NRF_MUTEX_S_BASE            NULL
#undef NRF_QDEC0_NS_BASE
#define NRF_QDEC0_NS_BASE           NULL
#undef NRF_QDEC0_S_BASE
#define NRF_QDEC0_S_BASE            NULL
#undef NRF_QDEC1_NS_BASE
#define NRF_QDEC1_NS_BASE           NULL
#undef NRF_QDEC1_S_BASE
#define NRF_QDEC1_S_BASE            NULL
#undef NRF_USBD_NS_BASE
#define NRF_USBD_NS_BASE            NULL
#undef NRF_USBD_S_BASE
#define NRF_USBD_S_BASE             NULL
#undef NRF_USBREGULATOR_NS_BASE
#define NRF_USBREGULATOR_NS_BASE    NULL
#undef NRF_USBREGULATOR_S_BASE
#define NRF_USBREGULATOR_S_BASE     NULL
#undef NRF_KMU_NS_BASE
#define NRF_KMU_NS_BASE             NULL
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE            NULL
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE              NULL
#undef NRF_NVMC_S_BASE
#define NRF_NVMC_S_BASE             NULL
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE              NULL
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE              NULL
#undef NRF_P0_S_BASE
#define NRF_P0_S_BASE               NULL
#undef NRF_P1_S_BASE
#define NRF_P1_S_BASE               NULL
#undef NRF_CRYPTOCELL_S_BASE
#define NRF_CRYPTOCELL_S_BASE       NULL
#undef NRF_VMC_NS_BASE
#define NRF_VMC_NS_BASE             NULL
#undef NRF_VMC_S_BASE
#define NRF_VMC_S_BASE              NULL

# endif


#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
