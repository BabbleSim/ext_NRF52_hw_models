/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_timer.h"
#include "bs_tracing.h"
#include "NRF_TIMER.h"
#if defined(DPPI_PRESENT)
#include "NRF_DPPI.h"
#endif

static int timer_number_from_ptr(NRF_TIMER_Type * p_reg){
  int i = ( (int)p_reg - (int)&NRF_TIMER_regs[0] ) / sizeof(NRF_TIMER_Type);
  return i;
}

void nrf_timer_cc_set(NRF_TIMER_Type * p_reg,
    nrf_timer_cc_channel_t cc_channel,
    uint32_t               cc_value)
{
  int i = timer_number_from_ptr(p_reg);
  p_reg->CC[cc_channel] = cc_value;
  nrf_timer_regw_sideeffects_CC(i, cc_channel);
}

void nrf_timer_task_trigger(NRF_TIMER_Type * p_reg,
    nrf_timer_task_t task)
{
  int i = timer_number_from_ptr(p_reg);

  if ( task == NRF_TIMER_TASK_CAPTURE0) {
    p_reg->TASKS_CAPTURE[0] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 0);
  } else if ( task == NRF_TIMER_TASK_CAPTURE1) {
    p_reg->TASKS_CAPTURE[1] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 1);
  } else if ( task == NRF_TIMER_TASK_CAPTURE2) {
    p_reg->TASKS_CAPTURE[2] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 2);
  } else if ( task == NRF_TIMER_TASK_CAPTURE3) {
    p_reg->TASKS_CAPTURE[3] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 3);
#if defined(TIMER_INTENSET_COMPARE4_Msk)
  } else if ( task == NRF_TIMER_TASK_CAPTURE4) {
    p_reg->TASKS_CAPTURE[4] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 4);
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
  } else if ( task == NRF_TIMER_TASK_CAPTURE5) {
    p_reg->TASKS_CAPTURE[5] = 1;
    nrf_timer_regw_sideeffects_TASKS_CAPTURE(i, 5);
#endif
  } else if ( task == NRF_TIMER_TASK_CLEAR) {
    p_reg->TASKS_CLEAR = 1;
    nrf_timer_regw_sideeffects_TASKS_CLEAR(i);
  } else if ( task == NRF_TIMER_TASK_START) {
    p_reg->TASKS_START = 1;
    nrf_timer_regw_sideeffects_TASKS_START(i);
  } else if ( task == NRF_TIMER_TASK_STOP) {
    p_reg->TASKS_STOP = 1;
    nrf_timer_regw_sideeffects_TASKS_STOP(i);
  } else if ( task == NRF_TIMER_TASK_SHUTDOWN) {
    /* Shutdown not modelled (deprecated) */
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_timer%i\n",
                             (int) task);
  }
}

void nrf_timer_int_enable(NRF_TIMER_Type * p_reg,
                          uint32_t         mask)
{
    int i = timer_number_from_ptr(p_reg);
    p_reg->INTENSET = mask;
    nrf_timer_regw_sideeffects_INTENSET(i);
}

void nrf_timer_int_disable(NRF_TIMER_Type * p_reg,
                           uint32_t         mask)
{
    int i = timer_number_from_ptr(p_reg);
    p_reg->INTENCLR = mask;
    nrf_timer_regw_sideeffects_INTENCLR(i);
}

#if defined(DPPI_PRESENT)

void nrf_timer_subscriber_add(int timer_number, nrf_timer_task_t task, uint8_t channel)
{
  switch(timer_number)
  {
    case 0:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_STOP);
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_add(channel, nrf_timer0_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task, channel);
          break;
      }
      break;
    case 1:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_STOP);
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_add(channel, nrf_timer1_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task, channel);
          break;
      }
      break;
    case 2:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_STOP);
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_add(channel, nrf_timer2_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p for chnbr %i does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task, channel);
          break;
      }
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_TIMER: The timer number %i does not match any modelled TIMER => it will be ignored\n",
        timer_number);
      break;
  }
}

void nrf_timer_subscriber_remove(int timer_number, nrf_timer_task_t task)
{
  switch(timer_number)
  {
    case 0:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_STOP);
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_remove(nrf_timer0_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task);
          break;
      }
      break;
    case 1:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_STOP); 
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_remove(nrf_timer1_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p does not match any  modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task);
          break;
      }
      break;
    case 2:
      switch(task)
      {
        case NRF_TIMER_TASK_START:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_START);
          break;
        case NRF_TIMER_TASK_STOP:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_STOP);
          break;
        case NRF_TIMER_TASK_COUNT:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_COUNT);
          break;
        case NRF_TIMER_TASK_CLEAR:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CLEAR);
          break;
        case NRF_TIMER_TASK_CAPTURE0:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_0);
          break;
        case NRF_TIMER_TASK_CAPTURE1:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_1);
          break;
        case NRF_TIMER_TASK_CAPTURE2:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_2);
          break;
        case NRF_TIMER_TASK_CAPTURE3:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_3);
          break;
#if defined(TIMER_INTENSET_COMPARE4_Msk)
        case NRF_TIMER_TASK_CAPTURE4:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_4);
          break;
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk)
        case NRF_TIMER_TASK_CAPTURE5:
          nrf_dppi_subscriber_remove(nrf_timer2_TASK_CAPTURE_5);
          break;
#endif
        default:
          bs_trace_warning_line_time(
            "NRF_TIMER%i: The task %p does not match any modelled task in NRF_DPPI.c => it will be ignored\n",
            timer_number, task);
          break;
      }
      break;
    default:
      bs_trace_warning_line_time(
        "NRF_TIMER: The timer number %i does not match any modelled TIMER => it will be ignored\n",
        timer_number);
      break;
  }
}

void nrf_timer_subscribe_set(NRF_TIMER_Type * p_reg,
                             nrf_timer_task_t task,
                             uint8_t          channel)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
          ((uint32_t)channel | TIMER_SUBSCRIBE_START_EN_Msk);
  nrf_timer_subscriber_add(timer_number_from_ptr(p_reg), task, channel);
}

void nrf_timer_subscribe_clear(NRF_TIMER_Type * p_reg,
                               nrf_timer_task_t task)
{
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
  nrf_timer_subscriber_remove(timer_number_from_ptr(p_reg), task);
}

#endif // defined(DPPI_PRESENT)

void nrf_timer_mode_set(NRF_TIMER_Type * p_reg,
                        nrf_timer_mode_t mode)
{
    if (mode == NRF_TIMER_MODE_COUNTER) {
      bs_trace_warning_line_time("Counter mode is not fully supported\n");
    }
    p_reg->MODE = (p_reg->MODE & ~TIMER_MODE_MODE_Msk) |
                  ((mode << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);
}
