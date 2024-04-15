/*
 * Copyright (c) 2019 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx.h"
#include "bs_tracing.h"

#define PERIPHERAL_REG_BASE(per, nbr, post) \
    (void*)NRF_##per##nbr##post##_BASE

#define IS_PERIPHERAL_REG(p, per, nbr, post) \
    (p >= PERIPHERAL_REG_BASE(per, nbr, post)) && \
    ((intptr_t)p < (intptr_t)PERIPHERAL_REG_BASE(per, nbr, post) + sizeof(NRF_##per##_Type))

IRQn_Type nrfx_get_irq_number(void const * p_reg){

#if defined(NRF52833_XXAA)
  /*
   * Peripheral numbers match interrupt numbers
   * See https://infocenter.nordicsemi.com/index.jsp?topic=%2Fstruct_nrf52%2Fstruct%2Fnrf52833.html
   */

  if (IS_PERIPHERAL_REG(p_reg, POWER,,)) {
    return POWER_CLOCK_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK,,)) {
    return POWER_CLOCK_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RADIO,,)) {
    return RADIO_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 0,)) {
    return UARTE0_UART0_IRQn;
  /*3-5*/
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE,,)) {
    return GPIOTE_IRQn;
  /*7 SAADC_IRQn */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 0,)) {
      return TIMER0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 1,)) {
      return TIMER1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 2,)) {
      return TIMER2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 0,)) {
      return RTC0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TEMP,,)) {
      return TEMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RNG,,)) {
      return RNG_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB,,)) {
      return ECB_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, AAR,,)) {
      return CCM_AAR_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM,,)) {
      return CCM_AAR_IRQn;
  /*16 WDT_IRQn*/
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 1,)) {
      return RTC1_IRQn;
  /*18-19*/
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 0,)) {
      return SWI0_EGU0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 1,)) {
      return SWI1_EGU1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 2,)) {
      return SWI2_EGU2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 3,)) {
      return SWI3_EGU3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 4,)) {
      return SWI4_EGU4_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 5,)) {
      return SWI5_EGU5_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 3,)) {
      return TIMER3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 4,)) {
      return TIMER4_IRQn;
  /*28-29*/
  } else if (IS_PERIPHERAL_REG(p_reg, NVMC,,)) {
      return 30;
  } else if (IS_PERIPHERAL_REG(p_reg, PPI,,)) {
      return 0x1F;
  /*32-35*/
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 2,)) {
    return RTC2_IRQn;
  /*37-39*/
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 1,)) {
    return UARTE1_IRQn;
  /*40-47*/
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to these HW models\n");
    return 0; /* unreachable */
  }


#elif defined(NRF5340_XXAA_NETWORK)

  if (IS_PERIPHERAL_REG(p_reg, POWER,, _NS)) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK,, _NS)) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RADIO,, _NS)) {
    return RADIO_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RNG,, _NS)) {
      return RNG_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE,, _NS)) {
    return GPIOTE_IRQn;
  /* 11 WDT */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 0, _NS)) {
      return TIMER0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB,, _NS)) {
      return ECB_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, AAR,, _NS)) {
      return AAR_CCM_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM,, _NS)) {
      return AAR_CCM_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TEMP,, _NS)) {
      return TEMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 0, _NS)) {
      return RTC0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, IPC,, _NS)) {
      return IPC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 0, _NS)) {
      return SERIAL0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 0, _NS)) {
      return EGU0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 1, _NS)) {
      return RTC1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 1, _NS)) {
      return TIMER1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 2, _NS)) {
      return TIMER2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SWI, 0, _NS)) {
      return SWI0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SWI, 1, _NS)) {
      return SWI1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SWI, 2, _NS)) {
      return SWI2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SWI, 3, _NS)) {
      return SWI3_IRQn;
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to these HW models\n");
    return 0; /* unreachable */
  }
#elif defined(NRF5340_XXAA_APPLICATION)

  /* 0 FPU */
  /* 1 CACHE */
  /* 3 SPU */
  if (IS_PERIPHERAL_REG(p_reg, POWER,, _S)) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK,, _S)) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 0, _S)) {
    return SERIAL0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 1, _S)) {
    return SERIAL1_IRQn;
  /* 10 SPIM4 */
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 2, _S)) {
    return SERIAL2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 3, _S)) {
    return SERIAL3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 0, _S)) {
    return GPIOTE0_IRQn;
  /* 14 SAADC */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 0, _S)) {
      return TIMER0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 1, _S)) {
      return TIMER1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 2, _S)) {
      return TIMER2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 0, _S)) {
      return RTC0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 1, _S)) {
      return RTC1_IRQn;
  /* 24, 25 WDT0,1 */
  /* 26 COMP */
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 0, _S)) {
      return EGU0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 1, _S)) {
        return EGU1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 2, _S)) {
        return EGU2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 3, _S)) {
        return EGU3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 4, _S)) {
        return EGU4_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 5, _S)) {
        return EGU5_IRQn;
  /* 33-36 PWM0-3*/
  /* 38 PDM0 */
  /* 40 I2S */
  } else if (IS_PERIPHERAL_REG(p_reg, IPC,, _S)) {
    return IPC_IRQn;
  /* 43 QPSI */
  /* 45 NFCT */
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 1, _NS)) {
    return GPIOTE1_IRQn;
  /* 51,52 QDEC0,1*/
  /* 54 USBD*/
  /* 55 USBREGULATOR*/
  /* 57 KMU */
  /* 68 CRYPTOCELL */
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to these HW models\n");
    return 0; /* unreachable */
  }


#elif defined(NRF54L15_XXAA) && defined(NRF_APPLICATION)

  /* 28..63 :not real */
  if (IS_PERIPHERAL_REG(p_reg, SPU, 00, _S)) {
    return SPU00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, MPC, 00, _S)) {
    return MPC00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, AAR, 00, _S)) {
    return AAR00_CCM00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM, 00, _S)) {
    return AAR00_CCM00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB, 00, _S)) {
    return ECB00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CRACEN, , _S)) {
    return CRACEN_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 00, _S)) {
    return SERIAL00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RRAMC, , _S)) {
    return RRAMC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, VPR, 00, _S)) {
    return VPR00_IRQn;
  /* CTRLAP */
  /* CM33S */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 00, _S)) {
    return TIMER00_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SPU, 10, _S)) {
    return SPU10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 10, _S)) {
    return TIMER10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 10, _S)) {
    return RTC10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 10, _S)) {
    return EGU10_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RADIO, , _S)) {
    return RADIO_0_IRQn;
    /* Note RADIO has 2 interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, SPU, 20, _S)) {
    return SPU20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 20, _S)) {
    return SERIAL20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 21, _S)) {
    return SERIAL21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 22, _S)) {
    return SERIAL22_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 20, _S)) {
    return EGU20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 20, _S)) {
    return TIMER20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 21, _S)) {
    return TIMER21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 22, _S)) {
    return TIMER22_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 23, _S)) {
    return TIMER23_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER, 24, _S)) {
    return TIMER24_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PDM, 20, _S)) {
    return PDM20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PDM, 21, _S)) {
    return PDM21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PWM, 20, _S)) {
    return PWM20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PWM, 21, _S)) {
    return PWM21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, PWM, 22, _S)) {
    return PWM22_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, SAADC, , _S)) {
    return SAADC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, NFCT, , _S)) {
    return NFCT_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TEMP, , _S)) {
    return TEMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 20, _S)) {
    return GPIOTE20_0_IRQn;
    /* Note GPIO20 has two interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, TAMPC, , _S)) {
    return TAMPC_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, I2S, 20, _S)) {
    return I2S20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, QDEC, 20, _S)) {
    return QDEC20_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, QDEC, 21, _S)) {
    return QDEC21_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GRTC, , _S)) {
    return GRTC_0_IRQn;
    /* Note GRTC has 4 interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, SPU, 30, _S)) {
    return SPU30_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, UARTE, 30, _S)) {
    return SERIAL30_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 30, _S)) {
    return RTC30_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, COMP, , _S)) {
    return COMP_LPCOMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, WDT, 30, _S)) {
    return WDT30_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, WDT, 31, _S)) {
    return WDT31_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE, 30, _S)) {
    return GPIOTE30_0_IRQn;
    /* Note GPIO20 has two interrupts */
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK, , _S)) {
    return CLOCK_POWER_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, POWER, , _S)) {
    return CLOCK_POWER_IRQn;
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to these HW models\n");
    return 0; /* unreachable */
  }

#else
#error "Unsuported SOC/cpu"
#endif

}
