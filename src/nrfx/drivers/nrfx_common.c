/*
 * Copyright (c) 2019 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "bs_tracing.h"
#include "nrfx_common.h"

unsigned int nrfx_peripheral_from_base_address(void const * p_reg){
  /*
   * Peripheral numbers match interrupt numbers
   * See https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52832.ps.v1.1/memory.html?cp=3_1_0_7_2#memorymap
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
  /*2-7*/
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,0)) {
      return TIMER0_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,1)) {
      return TIMER1_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,2)) {
      return TIMER2_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 0)) {
      return RTC0_IRQn;
  /*12*/
  } else if (IS_PERIPHERAL_REG(p_reg, RNG,)) {
      return RNG_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, ECB,)) {
      return ECB_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, AAR,)) {
      return CCM_AAR_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, CCM,)) {
      return CCM_AAR_IRQn;
  /*16*/
  } else if (IS_PERIPHERAL_REG(p_reg, RTC, 1)) {
      return RTC1_IRQn;
  /*18-25*/
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,3)) {
      return TIMER3_IRQn;
  } else if (IS_PERIPHERAL_REG(p_reg, TIMER,4)) {
      return TIMER4_IRQn;
  /*28-30*/
  } else if (IS_PERIPHERAL_REG(p_reg, PPI,)) {
      return 0x1F;
  /*32-..*/
  } else {
    bs_trace_error_time_line("Tried to get the peripheral number of an address unknown to this HW models\n");
    return 0; /* unreachable */
  }
#undef IS_PERIPHERAL_REG
}
