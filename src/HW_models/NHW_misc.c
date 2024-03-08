/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include "bs_tracing.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "nrf_bsim_redef.h"

/*
 * Get the name of a core/domain
 *
 * Only for debugging/logging/tracing purposes.
 */
const char *nhw_get_core_name(unsigned int core_n)
{
  static const char *corenames[NHW_INTCTRL_TOTAL_INST] = NHW_CORE_NAMES;

  if (core_n < sizeof(corenames)/sizeof(corenames[0])) {
    return corenames[core_n];
  } else {
    return NULL;
  }
}

/**
 * Convert a real peripheral base address (like 0x4000F000UL)
 * into the corresponding simulation peripheral base address
 *
 * Note: In general it is recommended to refer to peripherals
 * using the MDK macros (like NRF_TIMER0), this function
 * exists to cover the cases in which this is not possible.
 */
void *nhw_convert_periph_base_addr(void *hw_addr) {

#if defined(NRF5340)
  /* The nrf_bsim_redef.h header which is meant to hack the
   * nrf definitions only provides declarations for the
   * registers structures for a given core. So we need to declare
   * the ones that are exclusive for either core here.
   *
   * If at some point we have more uses for a bulk peripheral definitions
   * of all peripherals in a SOC consider moving it to a separate header
   */
  extern NRF_AAR_Type NRF_AAR_regs;
  extern NRF_CCM_Type NRF_CCM_regs;
  extern NRF_ECB_Type NRF_ECB_regs;
  extern NRF_RADIO_Type NRF_RADIO_regs;
  extern NRF_RNG_Type NRF_RNG_regs;
  extern int NRF_SWI_regs[];
  extern NRF_TEMP_Type NRF_TEMP_regs;
  extern NRF_VREQCTRL_Type NRF_VREQCTRL_regs;
#endif

  struct {
    void* simu_addr;
    uint32_t real_add;
  } conv_table[] = {
#if defined(NRF52833)
    {(void *)NRF_FICR_BASE       ,  0x10000000UL},
    {(void *)NRF_UICR_BASE       ,  0x10001000UL},
//    {(void *)NRF_APPROTECT_BASE  ,  0x40000000UL},
    {(void *)NRF_CLOCK_BASE      ,  0x40000000UL},
    {(void *)NRF_POWER_BASE      ,  0x40000000UL},
    {(void *)NRF_P0_BASE         ,  0x50000000UL},
    {(void *)NRF_P1_BASE         ,  0x50000300UL},
    {(void *)NRF_RADIO_BASE      ,  0x40001000UL},
    {(void *)NRF_UART0_BASE      ,  0x40002000UL},
    {(void *)NRF_UARTE0_BASE     ,  0x40002000UL},
//    {(void *)NRF_SPI0_BASE       ,  0x40003000UL},
//    {(void *)NRF_SPIM0_BASE      ,  0x40003000UL},
//    {(void *)NRF_SPIS0_BASE      ,  0x40003000UL},
//    {(void *)NRF_TWI0_BASE       ,  0x40003000UL},
//    {(void *)NRF_TWIM0_BASE      ,  0x40003000UL},
//    {(void *)NRF_TWIS0_BASE      ,  0x40003000UL},
//    {(void *)NRF_SPI1_BASE       ,  0x40004000UL},
//    {(void *)NRF_SPIM1_BASE      ,  0x40004000UL},
//    {(void *)NRF_SPIS1_BASE      ,  0x40004000UL},
//    {(void *)NRF_TWI1_BASE       ,  0x40004000UL},
//    {(void *)NRF_TWIM1_BASE      ,  0x40004000UL},
//    {(void *)NRF_TWIS1_BASE      ,  0x40004000UL},
//    {(void *)NRF_NFCT_BASE       ,  0x40005000UL},
    {(void *)NRF_GPIOTE_BASE     ,  0x40006000UL},
//    {(void *)NRF_SAADC_BASE      ,  0x40007000UL},
    {(void *)NRF_TIMER0_BASE     ,  0x40008000UL},
    {(void *)NRF_TIMER1_BASE     ,  0x40009000UL},
    {(void *)NRF_TIMER2_BASE     ,  0x4000A000UL},
    {(void *)NRF_RTC0_BASE       ,  0x4000B000UL},
    {(void *)NRF_TEMP_BASE       ,  0x4000C000UL},
    {(void *)NRF_RNG_BASE        ,  0x4000D000UL},
    {(void *)NRF_ECB_BASE        ,  0x4000E000UL},
    {(void *)NRF_AAR_BASE        ,  0x4000F000UL},
    {(void *)NRF_CCM_BASE        ,  0x4000F000UL},
//    {(void *)NRF_WDT_BASE        ,  0x40010000UL},
    {(void *)NRF_RTC1_BASE       ,  0x40011000UL},
//    {(void *)NRF_QDEC_BASE       ,  0x40012000UL},
//    {(void *)NRF_COMP_BASE       ,  0x40013000UL},
//    {(void *)NRF_LPCOMP_BASE     ,  0x40013000UL},
    {(void *)NRF_EGU0_BASE       ,  0x40014000UL},
//    {(void *)NRF_SWI0_BASE       ,  0x40014000UL},
    {(void *)NRF_EGU1_BASE       ,  0x40015000UL},
//    {(void *)NRF_SWI1_BASE       ,  0x40015000UL},
    {(void *)NRF_EGU2_BASE       ,  0x40016000UL},
//    {(void *)NRF_SWI2_BASE       ,  0x40016000UL},
    {(void *)NRF_EGU3_BASE       ,  0x40017000UL},
//    {(void *)NRF_SWI3_BASE       ,  0x40017000UL},
    {(void *)NRF_EGU4_BASE       ,  0x40018000UL},
//    {(void *)NRF_SWI4_BASE       ,  0x40018000UL},
    {(void *)NRF_EGU5_BASE       ,  0x40019000UL},
//    {(void *)NRF_SWI5_BASE       ,  0x40019000UL},
    {(void *)NRF_TIMER3_BASE     ,  0x4001A000UL},
    {(void *)NRF_TIMER4_BASE     ,  0x4001B000UL},
//    {(void *)NRF_PWM0_BASE       ,  0x4001C000UL},
//    {(void *)NRF_PDM_BASE        ,  0x4001D000UL},
//    {(void *)NRF_ACL_BASE        ,  0x4001E000UL},
    {(void *)NRF_NVMC_BASE       ,  0x4001E000UL},
    {(void *)NRF_PPI_BASE        ,  0x4001F000UL},
//    {(void *)NRF_MWU_BASE        ,  0x40020000UL},
//    {(void *)NRF_PWM1_BASE       ,  0x40021000UL},
//    {(void *)NRF_PWM2_BASE       ,  0x40022000UL},
//    {(void *)NRF_SPI2_BASE       ,  0x40023000UL},
//    {(void *)NRF_SPIM2_BASE      ,  0x40023000UL},
//    {(void *)NRF_SPIS2_BASE      ,  0x40023000UL},
//    {(void *)NRF_RTC2_BASE       ,  0x40024000UL},
//    {(void *)NRF_I2S_BASE        ,  0x40025000UL},
//    {(void *)NRF_FPU_BASE        ,  0x40026000UL},
//    {(void *)NRF_USBD_BASE       ,  0x40027000UL},
    {(void *)NRF_UARTE1_BASE     ,  0x40028000UL},
//    {(void *)NRF_PWM3_BASE       ,  0x4002D000UL},
//    {(void *)NRF_SPIM3_BASE      ,  0x4002F000UL},
#elif defined(NRF5340)
        /*Application core */
//{(void *)NRF_CACHEDATA_S_BASE    ,0x00F00000UL},
//{(void *)NRF_CACHEINFO_S_BASE    ,0x00F08000UL},
{(void *)(NRF_FICR_regs_p[NHW_FICR_APP])  ,0x00FF0000UL},
{(void *)(NRF_UICR_regs_p[NHW_UICR_APP0]) ,0x00FF8000UL},
//{(void *)NRF_CTI_S_BASE          ,0xE0042000UL},
//{(void *)NRF_TAD_S_BASE          ,0xE0080000UL},
//{(void *)NRF_DCNF_NS_BASE        ,0x40000000UL},
//{(void *)NRF_FPU_NS_BASE         ,0x40000000UL},
//{(void *)NRF_DCNF_S_BASE         ,0x50000000UL},
//{(void *)NRF_FPU_S_BASE          ,0x50000000UL},
//{(void *)NRF_CACHE_S_BASE        ,0x50001000UL},
//{(void *)NRF_SPU_S_BASE          ,0x50003000UL},
//{(void *)NRF_OSCILLATORS_NS_BASE ,0x40004000UL},
//{(void *)NRF_REGULATORS_NS_BASE  ,0x40004000UL},
//{(void *)NRF_OSCILLATORS_S_BASE  ,0x50004000UL},
//{(void *)NRF_REGULATORS_S_BASE   ,0x50004000UL},
{(void *)(NRF_CLOCK_regs[NHW_CLKPWR_APP0])       ,0x40005000UL},
{(void *)(NRF_POWER_regs[NHW_CLKPWR_APP0])       ,0x40005000UL},
{(void *)(NRF_RESET_regs[NHW_CLKPWR_APP0])       ,0x40005000UL},
{(void *)(NRF_CLOCK_regs[NHW_CLKPWR_APP0])       ,0x50005000UL},
{(void *)(NRF_POWER_regs[NHW_CLKPWR_APP0])       ,0x50005000UL},
{(void *)(NRF_RESET_regs[NHW_CLKPWR_APP0])       ,0x50005000UL},
//{(void *)NRF_CTRLAP_NS_BASE      ,0x40006000UL},
//{(void *)NRF_CTRLAP_S_BASE       ,0x50006000UL},
//{(void *)NRF_SPIM0_NS_BASE       ,0x40008000UL},
//{(void *)NRF_SPIS0_NS_BASE       ,0x40008000UL},
//{(void *)NRF_TWIM0_NS_BASE       ,0x40008000UL},
//{(void *)NRF_TWIS0_NS_BASE       ,0x40008000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP0])      ,0x40008000UL},
//{(void *)NRF_SPIM0_S_BASE        ,0x50008000UL},
//{(void *)NRF_SPIS0_S_BASE        ,0x50008000UL},
//{(void *)NRF_TWIM0_S_BASE        ,0x50008000UL},
//{(void *)NRF_TWIS0_S_BASE        ,0x50008000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP0])       ,0x50008000UL},
//{(void *)NRF_SPIM1_NS_BASE       ,0x40009000UL},
//{(void *)NRF_SPIS1_NS_BASE       ,0x40009000UL},
//{(void *)NRF_TWIM1_NS_BASE       ,0x40009000UL},
//{(void *)NRF_TWIS1_NS_BASE       ,0x40009000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP1])      ,0x40009000UL},
//{(void *)NRF_SPIM1_S_BASE        ,0x50009000UL},
//{(void *)NRF_SPIS1_S_BASE        ,0x50009000UL},
//{(void *)NRF_TWIM1_S_BASE        ,0x50009000UL},
//{(void *)NRF_TWIS1_S_BASE        ,0x50009000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP1])       ,0x50009000UL},
//{(void *)NRF_SPIM4_NS_BASE       ,0x4000A000UL},
//{(void *)NRF_SPIM4_S_BASE        ,0x5000A000UL},
//{(void *)NRF_SPIM2_NS_BASE       ,0x4000B000UL},
//{(void *)NRF_SPIS2_NS_BASE       ,0x4000B000UL},
//{(void *)NRF_TWIM2_NS_BASE       ,0x4000B000UL},
//{(void *)NRF_TWIS2_NS_BASE       ,0x4000B000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP2])      ,0x4000B000UL},
//{(void *)NRF_SPIM2_S_BASE        ,0x5000B000UL},
//{(void *)NRF_SPIS2_S_BASE        ,0x5000B000UL},
//{(void *)NRF_TWIM2_S_BASE        ,0x5000B000UL},
//{(void *)NRF_TWIS2_S_BASE        ,0x5000B000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP2])       ,0x5000B000UL},
//{(void *)NRF_SPIM3_NS_BASE       ,0x4000C000UL},
//{(void *)NRF_SPIS3_NS_BASE       ,0x4000C000UL},
//{(void *)NRF_TWIM3_NS_BASE       ,0x4000C000UL},
//{(void *)NRF_TWIS3_NS_BASE       ,0x4000C000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP3])      ,0x4000C000UL},
//{(void *)NRF_SPIM3_S_BASE        ,0x5000C000UL},
//{(void *)NRF_SPIS3_S_BASE        ,0x5000C000UL},
//{(void *)NRF_TWIM3_S_BASE        ,0x5000C000UL},
//{(void *)NRF_TWIS3_S_BASE        ,0x5000C000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_APP3])       ,0x5000C000UL},
//{(void *)NRF_GPIOTE0_S_BASE      ,0x5000D000UL},
//{(void *)NRF_SAADC_NS_BASE       ,0x4000E000UL},
//{(void *)NRF_SAADC_S_BASE        ,0x5000E000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP0])      ,0x4000F000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP0])      ,0x5000F000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP1])      ,0x40010000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP1])      ,0x50010000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP2])      ,0x40011000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_APP2])      ,0x50011000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_APP0])        ,0x40014000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_APP0])        ,0x50014000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_APP1])        ,0x40015000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_APP1])        ,0x50015000UL},
{(void *)(&NRF_DPPIC_regs[NHW_DPPI_APP_0])    ,0x40017000UL},
{(void *)(&NRF_DPPIC_regs[NHW_DPPI_APP_0])    ,0x50017000UL},
//{(void *)NRF_WDT0_NS_BASE        ,0x40018000UL},
//{(void *)NRF_WDT0_S_BASE         ,0x50018000UL},
//{(void *)NRF_WDT1_NS_BASE        ,0x40019000UL},
//{(void *)NRF_WDT1_S_BASE         ,0x50019000UL},
//{(void *)NRF_COMP_NS_BASE        ,0x4001A000UL},
//{(void *)NRF_LPCOMP_NS_BASE      ,0x4001A000UL},
//{(void *)NRF_COMP_S_BASE         ,0x5001A000UL},
//{(void *)NRF_LPCOMP_S_BASE       ,0x5001A000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP0])        ,0x4001B000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP0])        ,0x5001B000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP1])        ,0x4001C000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP1])        ,0x5001C000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP2])        ,0x4001D000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP2])        ,0x5001D000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP3])        ,0x4001E000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP3])        ,0x5001E000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP4])        ,0x4001F000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP4])        ,0x5001F000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP5])        ,0x40020000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_APP5])        ,0x50020000UL},
//{(void *)NRF_PWM0_NS_BASE        ,0x40021000UL},
//{(void *)NRF_PWM0_S_BASE         ,0x50021000UL},
//{(void *)NRF_PWM1_NS_BASE        ,0x40022000UL},
//{(void *)NRF_PWM1_S_BASE         ,0x50022000UL},
//{(void *)NRF_PWM2_NS_BASE        ,0x40023000UL},
//{(void *)NRF_PWM2_S_BASE         ,0x50023000UL},
//{(void *)NRF_PWM3_NS_BASE        ,0x40024000UL},
//{(void *)NRF_PWM3_S_BASE         ,0x50024000UL},
//{(void *)NRF_PDM0_NS_BASE        ,0x40026000UL},
//{(void *)NRF_PDM0_S_BASE         ,0x50026000UL},
//{(void *)NRF_I2S0_NS_BASE        ,0x40028000UL},
//{(void *)NRF_I2S0_S_BASE         ,0x50028000UL},
{(void *)(&NRF_IPC_regs[NHW_IPC_APP0])         ,0x4002A000UL},
{(void *)(&NRF_IPC_regs[NHW_IPC_APP0])         ,0x5002A000UL},
//{(void *)NRF_QSPI_NS_BASE        ,0x4002B000UL},
//{(void *)NRF_QSPI_S_BASE         ,0x5002B000UL},
//{(void *)NRF_NFCT_NS_BASE        ,0x4002D000UL},
//{(void *)NRF_NFCT_S_BASE         ,0x5002D000UL},
//{(void *)NRF_GPIOTE1_NS_BASE     ,0x4002F000UL},
{(void *)(&NRF_MUTEX_regs)        ,0x40030000UL},
{(void *)(&NRF_MUTEX_regs)        ,0x50030000UL},
//{(void *)NRF_QDEC0_NS_BASE       ,0x40033000UL},
//{(void *)NRF_QDEC0_S_BASE        ,0x50033000UL},
//{(void *)NRF_QDEC1_NS_BASE       ,0x40034000UL},
//{(void *)NRF_QDEC1_S_BASE        ,0x50034000UL},
//{(void *)NRF_USBD_NS_BASE        ,0x40036000UL},
//{(void *)NRF_USBD_S_BASE         ,0x50036000UL},
//{(void *)NRF_USBREGULATOR_NS_BASE,0x40037000UL},
//{(void *)NRF_USBREGULATOR_S_BASE ,0x50037000UL},
//{(void *)NRF_KMU_NS_BASE         ,0x40039000UL},
{(void *)(NRF_NVMC_regs_p[NHW_NVMC_APP0])        ,0x40039000UL},
//{(void *)NRF_KMU_S_BASE          ,0x50039000UL},
{(void *)(NRF_NVMC_regs_p[NHW_NVMC_APP0])        ,0x50039000UL},
//{(void *)NRF_P0_NS_BASE          ,0x40842500UL},
//{(void *)NRF_P1_NS_BASE          ,0x40842800UL},
//{(void *)NRF_P0_S_BASE           ,0x50842500UL},
//{(void *)NRF_P1_S_BASE           ,0x50842800UL},
//{(void *)NRF_CRYPTOCELL_S_BASE   ,0x50844000UL},
//{(void *)NRF_VMC_NS_BASE         ,0x40081000UL},
//{(void *)NRF_VMC_S_BASE          ,0x50081000UL},
        /* Network core: */
{(void *)(NRF_FICR_regs_p[NHW_FICR_NET])        ,0x01FF0000UL},
{(void *)(NRF_UICR_regs_p[NHW_UICR_NET0])       ,0x01FF8000UL},
//{(void *)NRF_CTI_NS_BASE         ,0xE0042000UL},
//{(void *)NRF_DCNF_NS_BASE        ,0x41000000UL},
{(void *)(&NRF_VREQCTRL_regs)    ,0x41004000UL},
{(void *)(NRF_CLOCK_regs[NHW_CLKPWR_NET0])       ,0x41005000UL},
{(void *)(NRF_POWER_regs[NHW_CLKPWR_NET0])       ,0x41005000UL},
{(void *)(NRF_RESET_regs[NHW_CLKPWR_NET0])       ,0x41005000UL},
//{(void *)NRF_CTRLAP_NS_BASE      ,0x41006000UL},
{(void *)(&NRF_RADIO_regs)       ,0x41008000UL},
{(void *)(&NRF_RNG_regs)         ,0x41009000UL},
//{(void *)NRF_GPIOTE_NS_BASE      ,0x4100A000UL},
//{(void *)NRF_WDT_NS_BASE         ,0x4100B000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_NET0])      ,0x4100C000UL},
{(void *)(&NRF_ECB_regs)         ,0x4100D000UL},
{(void *)(&NRF_AAR_regs)         ,0x4100E000UL},
{(void *)(&NRF_CCM_regs)         ,0x4100E000UL},
{(void *)(&NRF_DPPIC_regs[NHW_DPPI_NET_0])      ,0x4100F000UL},
{(void *)(&NRF_TEMP_regs)        ,0x41010000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_NET0])        ,0x41011000UL},
{(void *)(&NRF_IPC_regs[NHW_IPC_NET0])        ,0x41012000UL},
//{(void *)NRF_SPIM0_NS_BASE       ,0x41013000UL},
//{(void *)NRF_SPIS0_NS_BASE       ,0x41013000UL},
//{(void *)NRF_TWIM0_NS_BASE       ,0x41013000UL},
//{(void *)NRF_TWIS0_NS_BASE       ,0x41013000UL},
{(void *)(&NRF_UARTE_regs[NHW_UARTE_NET0])    ,0x41013000UL},
{(void *)(&NRF_EGU_regs[NHW_EGU_NET0])        ,0x41014000UL},
{(void *)(&NRF_RTC_regs[NHW_RTC_NET1])        ,0x41016000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_NET1])    ,0x41018000UL},
{(void *)(&NRF_TIMER_regs[NHW_TIMER_NET2])    ,0x41019000UL},
{(void *)(&NRF_SWI_regs[NHW_SWI_NET0])        ,0x4101A000UL},
{(void *)(&NRF_SWI_regs[NHW_SWI_NET1])        ,0x4101B000UL},
{(void *)(&NRF_SWI_regs[NHW_SWI_NET2])        ,0x4101C000UL},
{(void *)(&NRF_SWI_regs[NHW_SWI_NET3])        ,0x4101D000UL},
{(void *)(&NRF_MUTEX_regs)     ,0x40030000UL},
{(void *)(&NRF_MUTEX_regs)     ,0x50030000UL},
//{(void *)NRF_ACL_NS_BASE         ,0x41080000UL},
{(void *)(NRF_NVMC_regs_p[NHW_NVMC_NET0])        ,0x41080000UL},
//{(void *)NRF_VMC_NS_BASE         ,0x41081000UL},
//{(void *)NRF_P0_NS_BASE          ,0x418C0500UL},
//{(void *)NRF_P1_NS_BASE          ,0x418C0800UL},
#else
#error "No valid platform was selected"
#endif
  };

  for (int i = 0; i < sizeof(conv_table)/sizeof(conv_table[0]); i++) {
    if (conv_table[i].real_add == (uint32_t)hw_addr) {
      return conv_table[i].simu_addr;
    }
  }
  bs_trace_error_time_line("Could not find real peripheral addr %p\n", hw_addr);
  return NULL;
}
