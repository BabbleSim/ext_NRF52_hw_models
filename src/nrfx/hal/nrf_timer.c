/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "nrf_timer.h"
#include "bs_tracing.h"
#include "NRF_TIMER.h"

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
