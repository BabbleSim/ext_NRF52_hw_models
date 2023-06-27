/*
 * Copyright (c) 2019 Oticon A/S
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx.h"
#include "bs_tracing.h"

IRQn_Type nrfx_get_irq_number(void const * p_reg){
  /*
   * Peripheral numbers match interrupt numbers
   * See https://infocenter.nordicsemi.com/index.jsp?topic=%2Fstruct_nrf52%2Fstruct%2Fnrf52833.html
   */

#define IS_PERIPHERAL_REG(p, per, nbr) \
    (p >= (void*)NRF_##per##nbr##_BASE) && \
    ((intptr_t)p < (intptr_t)NRF_##per##nbr##_BASE + sizeof(NRF_##per##_Type))

  if (IS_PERIPHERAL_REG(p_reg, POWER,)) {
    return POWER_CLOCK_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CLOCK,)) {
    return POWER_CLOCK_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RADIO,)) {
    return RADIO_IRQn;
  /*2-5*/
  } else if (IS_PERIPHERAL_REG(p_reg, GPIOTE,)) {
    return GPIOTE_IRQn;
  /*7 SAADC_IRQn */
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,0)) {
      return TIMER0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,1)) {
      return TIMER1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,2)) {
      return TIMER2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 0)) {
      return RTC0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TEMP, )) {
      return TEMP_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RNG,)) {
      return RNG_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB,)) {
      return ECB_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, AAR,)) {
      return CCM_AAR_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM,)) {
      return CCM_AAR_IRQn;
  /*16 WDT_IRQn*/
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 1)) {
      return RTC1_IRQn;
  /*18-19*/
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 0)) {
      return SWI0_EGU0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 1)) {
      return SWI1_EGU1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 2)) {
      return SWI2_EGU2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 3)) {
      return SWI3_EGU3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 4)) {
      return SWI4_EGU4_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, EGU, 5)) {
      return SWI5_EGU5_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,3)) {
      return TIMER3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,4)) {
      return TIMER4_IRQn;
  /*28-29*/
  } else if (IS_PERIPHERAL_REG(p_reg, NVMC,)) {
      return 30;
  } else if (IS_PERIPHERAL_REG(p_reg, PPI,)) {
      return 0x1F;
  /*32-..*/
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to this HW models\n");
    return 0; /* unreachable */
  }
#undef IS_PERIPHERAL_REG
}
