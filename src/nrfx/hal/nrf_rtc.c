/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_rtc.h"
#include "bs_tracing.h"
#include "NRF_RTC.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

static int rtc_number_from_ptr(NRF_RTC_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_RTC_regs[0] ) / sizeof(NRF_RTC_Type);
  return i;
}

void nrf_rtc_cc_set(NRF_RTC_Type * p_reg, uint32_t ch, uint32_t cc_val)
{
  p_reg->CC[ch] = cc_val;
  int i = rtc_number_from_ptr(p_reg);
  nrf_rtc_regw_sideeffects_CC(i, ch);
}

void nrf_rtc_int_enable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->INTENSET = mask;
  nrf_rtc_regw_sideeffect_INTENSET(i);
}

void nrf_rtc_int_disable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->INTENCLR = mask;
  nrf_rtc_regw_sideeffect_INTENCLR(i);
}

#if defined(DPPI_PRESENT)

void nrf_rtc_subscriber_add(int rtc_number, nrf_rtc_task_t task, uint8_t channel)
{
  switch(rtc_number)
  {
    case 0:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_rtc0_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_rtc0_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_rtc0_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task, channel);
        break;
      }
      break;
    case 1:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_rtc1_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_rtc1_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_rtc1_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task, channel);
        break;
      }
      break;
    case 2:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_rtc2_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_rtc2_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_rtc2_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task, channel);
        break;
      }
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RTC: The rtc number %p does not match any modelled RTC => it will be ignored\n",
        rtc_number);
        break;
  }
}

void nrf_rtc_subscriber_remove(int rtc_number, nrf_rtc_task_t task)
{
  switch(rtc_number)
  {
    case 0:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_remove(nrf_rtc0_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_rtc0_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_rtc0_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p fdoes not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task);
        break;
      }
      break;
    case 1:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_remove(nrf_rtc1_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_rtc1_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_rtc1_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p fdoes not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task);
        break;
      }
      break;
    case 2:
      switch(task)
      {
        case NRF_RTC_TASK_START:
          nrf_dppi_subscriber_remove(nrf_rtc2_TASKS_START);
          break;
        case NRF_RTC_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_rtc2_TASKS_STOP);
          break;
        case NRF_RTC_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_rtc2_TASKS_CLEAR);
          break;
        default:
          bs_trace_warning_line_time(
            "NRF_RTC%i: The task %p fdoes not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            rtc_number, task);
        break;
      }
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_RTC: The rtc number %i does not match any modelled RTC => it will be ignored\n",
        rtc_number);
        break;
  }
}

void nrf_rtc_subscribe_set(NRF_RTC_Type * p_reg,
                           nrf_rtc_task_t task,
                           uint8_t        channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | RTC_SUBSCRIBE_START_EN_Msk);
  nrf_rtc_subscriber_add(rtc_number_from_ptr(p_reg), task, channel);
}

void nrf_rtc_subscribe_clear(NRF_RTC_Type * p_reg,
                             nrf_rtc_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_rtc_subscriber_remove(rtc_number_from_ptr(p_reg), task);
}
#endif // defined(DPPI_PRESENT)

uint32_t nrf_rtc_counter_get(NRF_RTC_Type const * p_reg)
{
  int i = rtc_number_from_ptr(p_reg);
  nrf_rtc_update_COUNTER(i);
  return p_reg->COUNTER;
}

void nrf_rtc_task_trigger(NRF_RTC_Type * p_reg, nrf_rtc_task_t task)
{
  int i = rtc_number_from_ptr(p_reg);

  if ( task == NRF_RTC_TASK_START ) {
    p_reg->TASKS_START = 1;
    nrf_rtc_regw_sideeffect_TASKS_START(i);
  } else if ( task == NRF_RTC_TASK_STOP ) {
    p_reg->TASKS_STOP = 1;
    nrf_rtc_regw_sideeffect_TASKS_STOP(i);
  } else if ( task == NRF_RTC_TASK_CLEAR ) {
    p_reg->TASKS_CLEAR = 1;
    nrf_rtc_regw_sideeffect_TASKS_CLEAR(i);
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_rtc %i\n", i);
  }
}

void nrf_rtc_event_enable(NRF_RTC_Type * p_reg, uint32_t mask)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->EVTENSET = mask;
  nrf_rtc_regw_sideeffect_EVTENSET(i);
}

void nrf_rtc_event_disable(NRF_RTC_Type * p_reg, uint32_t event)
{
  int i = rtc_number_from_ptr(p_reg);
  p_reg->EVTENCLR = event;
  nrf_rtc_regw_sideeffect_EVTENCLR(i);
}
