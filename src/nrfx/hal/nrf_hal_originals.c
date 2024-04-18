/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* We want to use all real nRFx HAL definitions, while allowing some of them to be overridden.
 * We do this by including here all real nRFx HAL provided functions, but defining them as "weak".
 * This allows the HW models to provide their own version when needed due to HW models differences.
 */
#define NRF_DONT_DECLARE_ONLY
#define NRF_STATIC_INLINE __attribute__((weak))

#include "nrfx.h"

#if defined(NRF54L15_XXAA)
/* These are not yet supported by the HAL (fail to build), even though the MDK claims are present */
#undef AAR_PRESENT
#undef PDM_PRESENT
#endif

#ifdef AAR_PRESENT
#include "hal/nrf_aar.h"
#endif

#ifdef ACL_PRESENT
#include "hal/nrf_acl.h"
#endif

#ifdef ADC_PRESENT
#include "hal/nrf_adc.h"
#endif

#ifdef BPROT_PRESENT
#include "hal/nrf_bprot.h"
#endif

#ifdef CACHE_PRESENT
#include "hal/nrf_cache.h"
#endif

#ifdef CCM_PRESENT
#include "hal/nrf_ccm.h"
#endif

#ifdef CLOCK_PRESENT
#include "hal/nrf_clock.h"
#endif

#include "hal/nrf_common.h"

#ifdef COMP_PRESENT
#include "hal/nrf_comp.h"
#endif

#ifdef CRACEN_PRESENT
#include "hal/nrf_cracen.h"
#endif

#ifdef NRF_DCNF_NS
#include "hal/nrf_dcnf.h"
#endif

#ifdef DPPI_PRESENT
#include "hal/nrf_dppi.h"
#endif

#ifdef ECB_PRESENT
#include "hal/nrf_ecb.h"
#endif

#ifdef EGU_PRESENT
#include "hal/nrf_egu.h"
#endif

#include "hal/nrf_ficr.h"

#ifdef GLITCHDET_PRESENT
#include "hal/nrf_glitchdet.h"
#endif

#ifdef GPIO_PRESENT
#include "hal/nrf_gpio.h"
#endif

#ifdef GPIOTE_PRESENT
#include "hal/nrf_gpiote.h"
#endif

#ifdef GRTC_PRESENT
#include "hal/nrf_grtc.h"
#endif

#ifdef I2S_PRESENT
#include "hal/nrf_i2s.h"
#endif

#ifdef IPC_PRESENT
#include "hal/nrf_ipc.h"
#endif

#ifdef KMU_PRESENT
#include "hal/nrf_kmu.h"
#endif

#ifdef LPCOMP_PRESENT
#include "hal/nrf_lpcomp.h"
#endif

#ifdef MEMCONF_PRESENT
#include "hal/nrf_memconf.h"
#endif

#ifdef MPC_PRESENT
#include "hal/nrf_mpc.h"
#endif

#ifdef MUTEX_PRESENT
#include "hal/nrf_mutex.h"
#endif

#ifdef MWU_PRESENT
#include "hal/nrf_mwu.h"
#endif

#ifdef NFCT_PRESENT
#include "hal/nrf_nfct.h"
#endif

#ifdef NVMC_PRESENT
#include "hal/nrf_nvmc.h"
#endif

#ifdef OSCILLATORS_PRESENT
#include "hal/nrf_oscillators.h"
#endif

#ifdef PDM_PRESENT
#include "hal/nrf_pdm.h"
#endif

#ifdef POWER_PRESENT
#include "hal/nrf_power.h"
#endif

#ifdef PPI_PRESENT
#include "hal/nrf_ppi.h"
#endif

#ifdef PPIB_PRESENT
#include "hal/nrf_ppib.h"
#endif

#ifdef PWM_PRESENT
#include "hal/nrf_pwm.h"
#endif

#ifdef QDEC_PRESENT
#include "hal/nrf_qdec.h"
#endif

#ifdef QSPI_PRESENT
#include "hal/nrf_qspi.h"
#endif

#ifdef RADIO_PRESENT
#include "hal/nrf_radio.h"
#endif

#ifdef REGULATORS_PRESENT
#include "hal/nrf_regulators.h"
#endif

#ifdef NRF_RESET /* The MDK lacks a RESET_PRESENT macro */
#include "hal/nrf_reset.h"
#endif

#ifdef RNG_PRESENT
#include "hal/nrf_rng.h"
#endif

#ifdef RRAMC_PRESENT
#include "hal/nrf_rramc.h"
#endif

#ifdef RTC_PRESENT
#include "hal/nrf_rtc.h"
#endif

#ifdef SAADC_PRESENT
#include "hal/nrf_saadc.h"
#endif

#ifdef SPI_PRESENT
#include "hal/nrf_spi.h"
#endif

#ifdef SPIM_PRESENT
#include "hal/nrf_spim.h"
#endif

#ifdef SPIS_PRESENT
#include "hal/nrf_spis.h"
#endif

#ifdef SPU_PRESENT
#include "hal/nrf_spu.h"
#endif

#ifdef TAMPC_PRESENT
#include "hal/nrf_tampc.h"
#endif

#ifdef TEMP_PRESENT
#include "hal/nrf_temp.h"
#endif

#ifdef TIMER_PRESENT
#include "hal/nrf_timer.h"
#endif

#ifdef TWI_PRESENT
#include "hal/nrf_twi.h"
#endif

#ifdef TWIM_PRESENT
#include "hal/nrf_twim.h"
#endif

#ifdef TWIS_PRESENT
#include "hal/nrf_twis.h"
#endif

#ifdef UART_PRESENT
#include "hal/nrf_uart.h"
#endif

#ifdef UARTE_PRESENT
#include "hal/nrf_uarte.h"
#endif

#ifdef USBD_PRESENT
#include "hal/nrf_usbd.h"
#endif

#ifdef USBREG_PRESENT
#include "hal/nrf_usbreg.h"
#endif

#ifdef VMC_PRESENT
#include "hal/nrf_vmc.h"
#endif

#ifdef VREQCTRL_PRESENT
#include "hal/nrf_vreqctrl.h"
#endif

#ifdef WDT_PRESENT
#include "hal/nrf_wdt.h"
#endif
