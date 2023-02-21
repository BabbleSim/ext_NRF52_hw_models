/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_egu.h"
#include "bs_tracing.h"
#include "NRF_EGU.h"

static int egu_number_from_ptr(NRF_EGU_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_EGU_regs[0] ) / sizeof(NRF_EGU_Type);
  return i;
}

void nrf_egu_task_trigger(NRF_EGU_Type * p_reg, nrf_egu_task_t egu_task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)egu_task)) = 0x1UL;

  int i = egu_number_from_ptr(p_reg);
  int task_nbr = (egu_task - NRF_EGU_TASK_TRIGGER0)/sizeof(uint32_t);
  nrf_egu_regw_sideeffects_TASK_TRIGGER(i, task_nbr);
}


void nrf_egu_event_clear(NRF_EGU_Type * p_reg, nrf_egu_event_t egu_event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)egu_event)) = 0x0UL;

  int i = egu_number_from_ptr(p_reg);
  int event_nbr = (egu_event - NRF_EGU_EVENT_TRIGGERED0)/sizeof(uint32_t);
  nrf_egu_regw_sideeffect_EVENTS_TRIGGERED(i, event_nbr);
}


void nrf_egu_int_enable(NRF_EGU_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;

  int i = egu_number_from_ptr(p_reg);
  nrf_egu_regw_sideeffect_INTENSET(i);
}

void nrf_egu_int_disable(NRF_EGU_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;

  int i = egu_number_from_ptr(p_reg);
  nrf_egu_regw_sideeffect_INTENCLR(i);
}
