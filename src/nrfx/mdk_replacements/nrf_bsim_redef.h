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

void *nhw_convert_periph_base_addr(void *hw_addr);

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
#undef NRF_RTC2_BASE
#define NRF_RTC2_BASE                     (&NRF_RTC_regs[2])
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
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_BASE
#define NRF_NVMC_BASE                     (NRF_NVMC_regs_p[0])
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
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#define NRF_UICR_BASE                     (NRF_UICR_regs_p[0])
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UART0_BASE
#define NRF_UART0_BASE                    (&NRF_UARTE_regs[NHW_UART_0])
#undef NRF_UARTE0_BASE
#define NRF_UARTE0_BASE                   (&NRF_UARTE_regs[NHW_UART_0])
#undef NRF_UARTE1_BASE
#define NRF_UARTE1_BASE                   (&NRF_UARTE_regs[NHW_UART_1])

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF5340_XXAA_NETWORK)

extern void *NRF_FICR_regs_p[];
#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE           (NRF_FICR_regs_p[NHW_FICR_NET])
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_NS_BASE
#define NRF_UICR_NS_BASE           (NRF_UICR_regs_p[NHW_UICR_NET0])
#undef NRF_CTI_NS_BASE
#define NRF_CTI_NS_BASE            NULL
#undef NRF_DCNF_NS_BASE
#define NRF_DCNF_NS_BASE           NULL
extern NRF_VREQCTRL_Type NRF_VREQCTRL_regs;
#undef NRF_VREQCTRL_NS_BASE
#define NRF_VREQCTRL_NS_BASE       (&NRF_VREQCTRL_regs)
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
extern NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE            (&NRF_IPC_regs[NHW_IPC_NET0])
#undef NRF_SPIM0_NS_BASE
#define NRF_SPIM0_NS_BASE          NULL
#undef NRF_SPIS0_NS_BASE
#define NRF_SPIS0_NS_BASE          NULL
#undef NRF_TWIM0_NS_BASE
#define NRF_TWIM0_NS_BASE          NULL
#undef NRF_TWIS0_NS_BASE
#define NRF_TWIS0_NS_BASE          NULL
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE         (&NRF_UARTE_regs[NHW_UARTE_NET0])
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
extern NRF_MUTEX_Type NRF_MUTEX_regs;
#undef NRF_APPMUTEX_NS_BASE
#define NRF_APPMUTEX_NS_BASE       (&NRF_MUTEX_regs)
#undef NRF_APPMUTEX_S_BASE
#define NRF_APPMUTEX_S_BASE        (&NRF_MUTEX_regs)
#undef NRF_ACL_NS_BASE
#define NRF_ACL_NS_BASE            NULL
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE           (NRF_NVMC_regs_p[NHW_NVMC_NET0])
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
extern void *NRF_FICR_regs_p[];
#undef NRF_FICR_S_BASE
#define NRF_FICR_S_BASE             (NRF_FICR_regs_p[NHW_FICR_APP])
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE             (NRF_UICR_regs_p[NHW_UICR_APP0])
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
extern NRF_UARTE_Type NRF_UARTE_regs[];
#undef NRF_UARTE0_NS_BASE
#define NRF_UARTE0_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP0])
#undef NRF_SPIM0_S_BASE
#define NRF_SPIM0_S_BASE            NULL
#undef NRF_SPIS0_S_BASE
#define NRF_SPIS0_S_BASE            NULL
#undef NRF_TWIM0_S_BASE
#define NRF_TWIM0_S_BASE            NULL
#undef NRF_TWIS0_S_BASE
#define NRF_TWIS0_S_BASE            NULL
#undef NRF_UARTE0_S_BASE
#define NRF_UARTE0_S_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP0])
#undef NRF_SPIM1_NS_BASE
#define NRF_SPIM1_NS_BASE           NULL
#undef NRF_SPIS1_NS_BASE
#define NRF_SPIS1_NS_BASE           NULL
#undef NRF_TWIM1_NS_BASE
#define NRF_TWIM1_NS_BASE           NULL
#undef NRF_TWIS1_NS_BASE
#define NRF_TWIS1_NS_BASE           NULL
#undef NRF_UARTE1_NS_BASE
#define NRF_UARTE1_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP1])
#undef NRF_SPIM1_S_BASE
#define NRF_SPIM1_S_BASE            NULL
#undef NRF_SPIS1_S_BASE
#define NRF_SPIS1_S_BASE            NULL
#undef NRF_TWIM1_S_BASE
#define NRF_TWIM1_S_BASE            NULL
#undef NRF_TWIS1_S_BASE
#define NRF_TWIS1_S_BASE            NULL
#undef NRF_UARTE1_S_BASE
#define NRF_UARTE1_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP1])
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
#define NRF_UARTE2_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP2])
#undef NRF_SPIM2_S_BASE
#define NRF_SPIM2_S_BASE            NULL
#undef NRF_SPIS2_S_BASE
#define NRF_SPIS2_S_BASE            NULL
#undef NRF_TWIM2_S_BASE
#define NRF_TWIM2_S_BASE            NULL
#undef NRF_TWIS2_S_BASE
#define NRF_TWIS2_S_BASE            NULL
#undef NRF_UARTE2_S_BASE
#define NRF_UARTE2_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP2])
#undef NRF_SPIM3_NS_BASE
#define NRF_SPIM3_NS_BASE           NULL
#undef NRF_SPIS3_NS_BASE
#define NRF_SPIS3_NS_BASE           NULL
#undef NRF_TWIM3_NS_BASE
#define NRF_TWIM3_NS_BASE           NULL
#undef NRF_TWIS3_NS_BASE
#define NRF_TWIS3_NS_BASE           NULL
#undef NRF_UARTE3_NS_BASE
#define NRF_UARTE3_NS_BASE          (&NRF_UARTE_regs[NHW_UARTE_APP3])
#undef NRF_SPIM3_S_BASE
#define NRF_SPIM3_S_BASE            NULL
#undef NRF_SPIS3_S_BASE
#define NRF_SPIS3_S_BASE            NULL
#undef NRF_TWIM3_S_BASE
#define NRF_TWIM3_S_BASE            NULL
#undef NRF_TWIS3_S_BASE
#define NRF_TWIS3_S_BASE            NULL
#undef NRF_UARTE3_S_BASE
#define NRF_UARTE3_S_BASE           (&NRF_UARTE_regs[NHW_UARTE_APP3])
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
extern NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];
#undef NRF_IPC_NS_BASE
#define NRF_IPC_NS_BASE             (&NRF_IPC_regs[NHW_IPC_APP0])
#undef NRF_IPC_S_BASE
#define NRF_IPC_S_BASE              (&NRF_IPC_regs[NHW_IPC_APP0])
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
extern NRF_MUTEX_Type NRF_MUTEX_regs;
#undef NRF_MUTEX_NS_BASE
#define NRF_MUTEX_NS_BASE           (&NRF_MUTEX_regs)
#undef NRF_MUTEX_S_BASE
#define NRF_MUTEX_S_BASE            (&NRF_MUTEX_regs)
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
extern NRF_NVMC_Type *NRF_NVMC_regs_p[];
#undef NRF_NVMC_NS_BASE
#define NRF_NVMC_NS_BASE            (NRF_NVMC_regs_p[NHW_NVMC_APP0])
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE              NULL
#undef NRF_NVMC_S_BASE
#define NRF_NVMC_S_BASE             (NRF_NVMC_regs_p[NHW_NVMC_APP0])
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

/********************************************************************/
/********************************************************************/
/********************************************************************/
#elif defined(NRF54L15_XXAA)

extern NRF_FICR_Type NRF_FICR_regs;
#undef NRF_FICR_NS_BASE
#define NRF_FICR_NS_BASE (&NRF_FICR_regs)
extern NRF_UICR_Type *NRF_UICR_regs_p[];
#undef NRF_UICR_S_BASE
#define NRF_UICR_S_BASE (NRF_UICR_regs_p[0])
#undef NRF_SICR_S_BASE
#define NRF_SICR_S_BASE NULL
#undef NRF_CRACENCORE_S_BASE
#define NRF_CRACENCORE_S_BASE NULL
#undef NRF_SPU00_S_BASE
#define NRF_SPU00_S_BASE NULL
#undef NRF_MPC00_S_BASE
#define NRF_MPC00_S_BASE NULL
extern NRF_DPPIC_Type NRF_DPPIC_regs[];
#undef NRF_DPPIC00_NS_BASE
#define NRF_DPPIC00_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
#undef NRF_DPPIC00_S_BASE
#define NRF_DPPIC00_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_00])
extern NRF_PPIB_Type NRF_PPIB_regs[];
#undef NRF_PPIB00_NS_BASE
#define NRF_PPIB00_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB00_S_BASE
#define NRF_PPIB00_S_BASE (&NRF_PPIB_regs[NHW_PPIB_00])
#undef NRF_PPIB01_NS_BASE
#define NRF_PPIB01_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_PPIB01_S_BASE
#define NRF_PPIB01_S_BASE (&NRF_PPIB_regs[NHW_PPIB_01])
#undef NRF_KMU_S_BASE
#define NRF_KMU_S_BASE NULL
#undef NRF_AAR00_NS_BASE
#define NRF_AAR00_NS_BASE NULL
#undef NRF_CCM00_NS_BASE
#define NRF_CCM00_NS_BASE NULL
#undef NRF_AAR00_S_BASE
#define NRF_AAR00_S_BASE NULL
#undef NRF_CCM00_S_BASE
#define NRF_CCM00_S_BASE NULL
#undef NRF_ECB00_NS_BASE
#define NRF_ECB00_NS_BASE NULL
#undef NRF_ECB00_S_BASE
#define NRF_ECB00_S_BASE NULL
#undef NRF_CRACEN_S_BASE
#define NRF_CRACEN_S_BASE NULL
#undef NRF_SPIM00_NS_BASE
#define NRF_SPIM00_NS_BASE NULL
#undef NRF_SPIS00_NS_BASE
#define NRF_SPIS00_NS_BASE NULL
#undef NRF_UARTE00_NS_BASE
#define NRF_UARTE00_NS_BASE NULL
#undef NRF_SPIM00_S_BASE
#define NRF_SPIM00_S_BASE NULL
#undef NRF_SPIS00_S_BASE
#define NRF_SPIS00_S_BASE NULL
#undef NRF_UARTE00_S_BASE
#define NRF_UARTE00_S_BASE NULL
#undef NRF_GLITCHDET_S_BASE
#define NRF_GLITCHDET_S_BASE NULL
extern void* nhw_RRAMC_get_RRAM_base_address(unsigned int inst);
#define NRF_RRAM_BASE_ADDR ((uintptr_t)nhw_RRAMC_get_RRAM_base_address(0))
extern NRF_RRAMC_Type *NRF_RRAMC_regs_p[];
#undef NRF_RRAMC_S_BASE
#define NRF_RRAMC_S_BASE (NRF_RRAMC_regs_p[0])
#undef NRF_VPR00_NS_BASE
#define NRF_VPR00_NS_BASE NULL
#undef NRF_VPR00_S_BASE
#define NRF_VPR00_S_BASE NULL
#undef NRF_P2_NS_BASE
#define NRF_P2_NS_BASE NULL
#undef NRF_P2_S_BASE
#define NRF_P2_S_BASE NULL
#undef NRF_CTRLAP_NS_BASE
#define NRF_CTRLAP_NS_BASE NULL
#undef NRF_CTRLAP_S_BASE
#define NRF_CTRLAP_S_BASE NULL
#undef NRF_TAD_NS_BASE
#define NRF_TAD_NS_BASE NULL
#undef NRF_TAD_S_BASE
#define NRF_TAD_S_BASE NULL
extern NRF_TIMER_Type NRF_TIMER_regs[];
#undef NRF_TIMER00_NS_BASE
#define NRF_TIMER00_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
#undef NRF_TIMER00_S_BASE
#define NRF_TIMER00_S_BASE (&NRF_TIMER_regs[NHW_TIMER_00])
#undef NRF_SPU10_S_BASE
#define NRF_SPU10_S_BASE NULL
#undef NRF_DPPIC10_NS_BASE
#define NRF_DPPIC10_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_DPPIC10_S_BASE
#define NRF_DPPIC10_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_10])
#undef NRF_PPIB10_NS_BASE
#define NRF_PPIB10_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB10_S_BASE
#define NRF_PPIB10_S_BASE (&NRF_PPIB_regs[NHW_PPIB_10])
#undef NRF_PPIB11_NS_BASE
#define NRF_PPIB11_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_PPIB11_S_BASE
#define NRF_PPIB11_S_BASE (&NRF_PPIB_regs[NHW_PPIB_11])
#undef NRF_TIMER10_NS_BASE
#define NRF_TIMER10_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
#undef NRF_TIMER10_S_BASE
#define NRF_TIMER10_S_BASE (&NRF_TIMER_regs[NHW_TIMER_10])
extern NRF_RTC_Type NRF_RTC_regs[];
#undef NRF_RTC10_NS_BASE
#define NRF_RTC10_NS_BASE (&NRF_RTC_regs[NHW_RTC_10])
#undef NRF_RTC10_S_BASE
#define NRF_RTC10_S_BASE (&NRF_RTC_regs[NHW_RTC_10])
extern NRF_EGU_Type NRF_EGU_regs[];
#undef NRF_EGU10_NS_BASE
#define NRF_EGU10_NS_BASE (&NRF_EGU_regs[NHW_EGU_10])
#undef NRF_EGU10_S_BASE
#define NRF_EGU10_S_BASE (&NRF_EGU_regs[NHW_EGU_10])
#undef NRF_RADIO_NS_BASE
#define NRF_RADIO_NS_BASE NULL
#undef NRF_RADIO_S_BASE
#define NRF_RADIO_S_BASE NULL
#undef NRF_SPU20_S_BASE
#define NRF_SPU20_S_BASE NULL
#undef NRF_DPPIC20_NS_BASE
#define NRF_DPPIC20_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_DPPIC20_S_BASE
#define NRF_DPPIC20_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_20])
#undef NRF_PPIB20_NS_BASE
#define NRF_PPIB20_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB20_S_BASE
#define NRF_PPIB20_S_BASE (&NRF_PPIB_regs[NHW_PPIB_20])
#undef NRF_PPIB21_NS_BASE
#define NRF_PPIB21_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB21_S_BASE
#define NRF_PPIB21_S_BASE (&NRF_PPIB_regs[NHW_PPIB_21])
#undef NRF_PPIB22_NS_BASE
#define NRF_PPIB22_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_PPIB22_S_BASE
#define NRF_PPIB22_S_BASE (&NRF_PPIB_regs[NHW_PPIB_22])
#undef NRF_SPIM20_NS_BASE
#define NRF_SPIM20_NS_BASE NULL
#undef NRF_SPIS20_NS_BASE
#define NRF_SPIS20_NS_BASE NULL
#undef NRF_TWIM20_NS_BASE
#define NRF_TWIM20_NS_BASE NULL
#undef NRF_TWIS20_NS_BASE
#define NRF_TWIS20_NS_BASE NULL
#undef NRF_UARTE20_NS_BASE
#define NRF_UARTE20_NS_BASE NULL
#undef NRF_SPIM20_S_BASE
#define NRF_SPIM20_S_BASE NULL
#undef NRF_SPIS20_S_BASE
#define NRF_SPIS20_S_BASE NULL
#undef NRF_TWIM20_S_BASE
#define NRF_TWIM20_S_BASE NULL
#undef NRF_TWIS20_S_BASE
#define NRF_TWIS20_S_BASE NULL
#undef NRF_UARTE20_S_BASE
#define NRF_UARTE20_S_BASE NULL
#undef NRF_SPIM21_NS_BASE
#define NRF_SPIM21_NS_BASE NULL
#undef NRF_SPIS21_NS_BASE
#define NRF_SPIS21_NS_BASE NULL
#undef NRF_TWIM21_NS_BASE
#define NRF_TWIM21_NS_BASE NULL
#undef NRF_TWIS21_NS_BASE
#define NRF_TWIS21_NS_BASE NULL
#undef NRF_UARTE21_NS_BASE
#define NRF_UARTE21_NS_BASE NULL
#undef NRF_SPIM21_S_BASE
#define NRF_SPIM21_S_BASE NULL
#undef NRF_SPIS21_S_BASE
#define NRF_SPIS21_S_BASE NULL
#undef NRF_TWIM21_S_BASE
#define NRF_TWIM21_S_BASE NULL
#undef NRF_TWIS21_S_BASE
#define NRF_TWIS21_S_BASE NULL
#undef NRF_UARTE21_S_BASE
#define NRF_UARTE21_S_BASE NULL
#undef NRF_SPIM22_NS_BASE
#define NRF_SPIM22_NS_BASE NULL
#undef NRF_SPIS22_NS_BASE
#define NRF_SPIS22_NS_BASE NULL
#undef NRF_TWIM22_NS_BASE
#define NRF_TWIM22_NS_BASE NULL
#undef NRF_TWIS22_NS_BASE
#define NRF_TWIS22_NS_BASE NULL
#undef NRF_UARTE22_NS_BASE
#define NRF_UARTE22_NS_BASE NULL
#undef NRF_SPIM22_S_BASE
#define NRF_SPIM22_S_BASE NULL
#undef NRF_SPIS22_S_BASE
#define NRF_SPIS22_S_BASE NULL
#undef NRF_TWIM22_S_BASE
#define NRF_TWIM22_S_BASE NULL
#undef NRF_TWIS22_S_BASE
#define NRF_TWIS22_S_BASE NULL
#undef NRF_UARTE22_S_BASE
#define NRF_UARTE22_S_BASE NULL
#undef NRF_EGU20_NS_BASE
#define NRF_EGU20_NS_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_EGU20_S_BASE
#define NRF_EGU20_S_BASE (&NRF_EGU_regs[NHW_EGU_20])
#undef NRF_TIMER20_NS_BASE
#define NRF_TIMER20_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER20_S_BASE
#define NRF_TIMER20_S_BASE (&NRF_TIMER_regs[NHW_TIMER_20])
#undef NRF_TIMER21_NS_BASE
#define NRF_TIMER21_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER21_S_BASE
#define NRF_TIMER21_S_BASE (&NRF_TIMER_regs[NHW_TIMER_21])
#undef NRF_TIMER22_NS_BASE
#define NRF_TIMER22_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER22_S_BASE
#define NRF_TIMER22_S_BASE (&NRF_TIMER_regs[NHW_TIMER_22])
#undef NRF_TIMER23_NS_BASE
#define NRF_TIMER23_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER23_S_BASE
#define NRF_TIMER23_S_BASE (&NRF_TIMER_regs[NHW_TIMER_23])
#undef NRF_TIMER24_NS_BASE
#define NRF_TIMER24_NS_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_TIMER24_S_BASE
#define NRF_TIMER24_S_BASE (&NRF_TIMER_regs[NHW_TIMER_24])
#undef NRF_MEMCONF_NS_BASE
#define NRF_MEMCONF_NS_BASE NULL
#undef NRF_MEMCONF_S_BASE
#define NRF_MEMCONF_S_BASE NULL
#undef NRF_PDM20_NS_BASE
#define NRF_PDM20_NS_BASE NULL
#undef NRF_PDM20_S_BASE
#define NRF_PDM20_S_BASE NULL
#undef NRF_PDM21_NS_BASE
#define NRF_PDM21_NS_BASE NULL
#undef NRF_PDM21_S_BASE
#define NRF_PDM21_S_BASE NULL
#undef NRF_PWM20_NS_BASE
#define NRF_PWM20_NS_BASE NULL
#undef NRF_PWM20_S_BASE
#define NRF_PWM20_S_BASE NULL
#undef NRF_PWM21_NS_BASE
#define NRF_PWM21_NS_BASE NULL
#undef NRF_PWM21_S_BASE
#define NRF_PWM21_S_BASE NULL
#undef NRF_PWM22_NS_BASE
#define NRF_PWM22_NS_BASE NULL
#undef NRF_PWM22_S_BASE
#define NRF_PWM22_S_BASE NULL
#undef NRF_SAADC_NS_BASE
#define NRF_SAADC_NS_BASE NULL
#undef NRF_SAADC_S_BASE
#define NRF_SAADC_S_BASE NULL
#undef NRF_NFCT_NS_BASE
#define NRF_NFCT_NS_BASE NULL
#undef NRF_NFCT_S_BASE
#define NRF_NFCT_S_BASE NULL
#undef NRF_TEMP_NS_BASE
#define NRF_TEMP_NS_BASE NULL
#undef NRF_TEMP_S_BASE
#define NRF_TEMP_S_BASE NULL
#undef NRF_P1_NS_BASE
#define NRF_P1_NS_BASE NULL
#undef NRF_P1_S_BASE
#define NRF_P1_S_BASE NULL
#undef NRF_GPIOTE20_NS_BASE
#define NRF_GPIOTE20_NS_BASE NULL
#undef NRF_GPIOTE20_S_BASE
#define NRF_GPIOTE20_S_BASE NULL
#undef NRF_TAMPC_S_BASE
#define NRF_TAMPC_S_BASE NULL
#undef NRF_I2S20_NS_BASE
#define NRF_I2S20_NS_BASE NULL
#undef NRF_I2S20_S_BASE
#define NRF_I2S20_S_BASE NULL
#undef NRF_QDEC20_NS_BASE
#define NRF_QDEC20_NS_BASE NULL
#undef NRF_QDEC20_S_BASE
#define NRF_QDEC20_S_BASE NULL
#undef NRF_QDEC21_NS_BASE
#define NRF_QDEC21_NS_BASE NULL
#undef NRF_QDEC21_S_BASE
#define NRF_QDEC21_S_BASE NULL
extern NRF_GRTC_Type NRF_GRTC_regs;
#undef NRF_GRTC_NS_BASE
#define NRF_GRTC_NS_BASE (&NRF_GRTC_regs)
#undef NRF_GRTC_S_BASE
#define NRF_GRTC_S_BASE (&NRF_GRTC_regs)
#undef NRF_SPU30_S_BASE
#define NRF_SPU30_S_BASE NULL
#undef NRF_DPPIC30_NS_BASE
#define NRF_DPPIC30_NS_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_DPPIC30_S_BASE
#define NRF_DPPIC30_S_BASE (&NRF_DPPIC_regs[NHW_DPPI_30])
#undef NRF_PPIB30_NS_BASE
#define NRF_PPIB30_NS_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_PPIB30_S_BASE
#define NRF_PPIB30_S_BASE (&NRF_PPIB_regs[NHW_PPIB_30])
#undef NRF_SPIM30_NS_BASE
#define NRF_SPIM30_NS_BASE NULL
#undef NRF_SPIS30_NS_BASE
#define NRF_SPIS30_NS_BASE NULL
#undef NRF_TWIM30_NS_BASE
#define NRF_TWIM30_NS_BASE NULL
#undef NRF_TWIS30_NS_BASE
#define NRF_TWIS30_NS_BASE NULL
#undef NRF_UARTE30_NS_BASE
#define NRF_UARTE30_NS_BASE NULL
#undef NRF_SPIM30_S_BASE
#define NRF_SPIM30_S_BASE NULL
#undef NRF_SPIS30_S_BASE
#define NRF_SPIS30_S_BASE NULL
#undef NRF_TWIM30_S_BASE
#define NRF_TWIM30_S_BASE NULL
#undef NRF_TWIS30_S_BASE
#define NRF_TWIS30_S_BASE NULL
#undef NRF_UARTE30_S_BASE
#define NRF_UARTE30_S_BASE NULL
#undef NRF_RTC30_NS_BASE
#define NRF_RTC30_NS_BASE (&NRF_RTC_regs[NHW_RTC_30])
#undef NRF_RTC30_S_BASE
#define NRF_RTC30_S_BASE (&NRF_RTC_regs[NHW_RTC_30])
#undef NRF_COMP_NS_BASE
#define NRF_COMP_NS_BASE NULL
#undef NRF_LPCOMP_NS_BASE
#define NRF_LPCOMP_NS_BASE NULL
#undef NRF_COMP_S_BASE
#define NRF_COMP_S_BASE NULL
#undef NRF_LPCOMP_S_BASE
#define NRF_LPCOMP_S_BASE NULL
#undef NRF_WDT30_S_BASE
#define NRF_WDT30_S_BASE NULL
#undef NRF_WDT31_NS_BASE
#define NRF_WDT31_NS_BASE NULL
#undef NRF_WDT31_S_BASE
#define NRF_WDT31_S_BASE NULL
#undef NRF_P0_NS_BASE
#define NRF_P0_NS_BASE NULL
#undef NRF_P0_S_BASE
#define NRF_P0_S_BASE NULL
#undef NRF_GPIOTE30_NS_BASE
#define NRF_GPIOTE30_NS_BASE NULL
#undef NRF_GPIOTE30_S_BASE
#define NRF_GPIOTE30_S_BASE NULL
#undef NRF_CLOCK_NS_BASE
#define NRF_CLOCK_NS_BASE NULL
#undef NRF_POWER_NS_BASE
#define NRF_POWER_NS_BASE NULL
#undef NRF_RESET_NS_BASE
#define NRF_RESET_NS_BASE NULL
#undef NRF_CLOCK_S_BASE
#define NRF_CLOCK_S_BASE NULL
#undef NRF_POWER_S_BASE
#define NRF_POWER_S_BASE NULL
#undef NRF_RESET_S_BASE
#define NRF_RESET_S_BASE NULL
#undef NRF_OSCILLATORS_NS_BASE
#define NRF_OSCILLATORS_NS_BASE NULL
#undef NRF_REGULATORS_NS_BASE
#define NRF_REGULATORS_NS_BASE NULL
#undef NRF_OSCILLATORS_S_BASE
#define NRF_OSCILLATORS_S_BASE NULL
#undef NRF_REGULATORS_S_BASE
#define NRF_REGULATORS_S_BASE NULL

#else
#error "Platform not supported"
# endif

#ifdef __cplusplus
}
#endif

#endif /* NRF_BSIM_REDEF_H */
