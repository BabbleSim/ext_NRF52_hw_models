/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_rtc.h"
#include "bs_tracing.h"
#include "NHW_RTC.h"

static int rtc_number_from_ptr(NRF_RTC_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_RTC_regs[0] ) / sizeof(NRF_RTC_Type);
  return i;
}

void nrf_rtc_cc_set(NRF_RTC_Type * p_reg, uint32_t ch, uint32_t cc_val)
{
  p_reg->CC[ch] = cc_val;
  int i = rtc_number_from_ptr(p_reg);
  nhw_rtc_regw_sideeffects_CC(i, ch);
}

void nrf_rtc_int_enable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->INTENSET = mask;
  nhw_rtc_regw_sideeffect_INTENSET(i);
}

void nrf_rtc_int_disable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->INTENCLR = mask;
  nhw_rtc_regw_sideeffect_INTENCLR(i);
}

uint32_t nrf_rtc_counter_get(NRF_RTC_Type const * p_reg)
{
  int i = rtc_number_from_ptr(p_reg);
  nhw_rtc_update_COUNTER(i);
  return p_reg->COUNTER;
}

void nrf_rtc_task_trigger(NRF_RTC_Type * p_reg, nrf_rtc_task_t task)
{
  *(uint32_t *)((uint32_t)p_reg + task) = 1;

  int i = rtc_number_from_ptr(p_reg);

  if ( task == NRF_RTC_TASK_START ) {
    nhw_rtc_regw_sideeffect_TASKS_START(i);
  } else if ( task == NRF_RTC_TASK_STOP ) {
    nhw_rtc_regw_sideeffect_TASKS_STOP(i);
  } else if ( task == NRF_RTC_TASK_CLEAR ) {
    nhw_rtc_regw_sideeffect_TASKS_CLEAR(i);
  } else if ( task == NRF_RTC_TASK_TRIGGER_OVERFLOW ) {
    nhw_rtc_regw_sideeffect_TASKS_TRIGOVRFLW(i);
#if defined(RTC_TASKS_CAPTURE_TASKS_CAPTURE_Msk)
  } else if ( task >= NRF_RTC_TASK_CAPTURE_0 ) {
    int cc = (task - NRF_RTC_TASK_CAPTURE_0)/sizeof(uint32_t);
    nhw_rtc_regw_sideeffect_TASKS_CAPTURE(i, cc);
#endif
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_rtc %i\n", i);
  }
}

void nrf_rtc_event_clear(NRF_RTC_Type * p_reg, nrf_rtc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;

    int i = rtc_number_from_ptr(p_reg);
    nhw_rtc_regw_sideeffects_EVENTS_all(i);
}

void nrf_rtc_event_enable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->EVTENSET = mask;
  nhw_rtc_regw_sideeffect_EVTENSET(i);
}

void nrf_rtc_event_disable(NRF_RTC_Type * p_reg, uint32_t event)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->EVTENCLR = event;
  nhw_rtc_regw_sideeffect_EVTENCLR(i);
}

#if defined(DPPI_PRESENT)

static void nrf_rtc_subscribe_common(NRF_RTC_Type * p_reg,
                                     nrf_rtc_task_t task)
{
  int i = rtc_number_from_ptr(p_reg);

  if (task == NRF_RTC_TASK_START) {
    nhw_rtc_regw_sideeffects_SUBSCRIBE_START(i);
  } else if (task == NRF_RTC_TASK_STOP) {
    nhw_rtc_regw_sideeffects_SUBSCRIBE_STOP(i);
  } else if (task == NRF_RTC_TASK_CLEAR) {
    nhw_rtc_regw_sideeffects_SUBSCRIBE_CLEAR(i);
  } else if (task == NRF_RTC_TASK_TRIGGER_OVERFLOW) {
    nhw_rtc_regw_sideeffects_SUBSCRIBE_TRIGOVRFLW(i);
#if defined(RTC_TASKS_CAPTURE_TASKS_CAPTURE_Msk)
  } else if (task >= NRF_RTC_TASK_CAPTURE_0) {
    int cc_n = (task - NRF_RTC_TASK_CAPTURE_0)/sizeof(uint32_t);
    nhw_rtc_regw_sideeffects_SUBSCRIBE_CAPTURE(i, cc_n);
#endif
  } else {
    bs_trace_error_line_time("Attempted to subscribe to a not-supported task in the nrf_rtc (%i)\n",
                             task);
  }
}

void nrf_rtc_subscribe_set(NRF_RTC_Type * p_reg,
                           nrf_rtc_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_rtc_subscribe_common(p_reg, task);
}

void nrf_rtc_subscribe_clear(NRF_RTC_Type * p_reg,
                             nrf_rtc_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_rtc_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
