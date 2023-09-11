/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include "hal/nrf_timer.h"
#include "bs_tracing.h"
#include "NHW_TIMER.h"

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
  nhw_timer_regw_sideeffects_CC(i, cc_channel);
}

void nrf_timer_task_trigger(NRF_TIMER_Type * p_reg,
                            nrf_timer_task_t task)
{
  int i = timer_number_from_ptr(p_reg);

  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  if (task == NRF_TIMER_TASK_START) {
    nhw_timer_regw_sideeffects_TASKS_START(i);
  } else if (task == NRF_TIMER_TASK_STOP) {
    nhw_timer_regw_sideeffects_TASKS_STOP(i);
  } else if (task == NRF_TIMER_TASK_COUNT) {
    nhw_timer_regw_sideeffects_TASKS_COUNT(i);
  } else if (task == NRF_TIMER_TASK_CLEAR) {
    nhw_timer_regw_sideeffects_TASKS_CLEAR(i);
  } else if (task == NRF_TIMER_TASK_SHUTDOWN) {
    nhw_timer_regw_sideeffects_TASKS_SHUTDOWN(i);
  } else if (task >= NRF_TIMER_TASK_CAPTURE0) {
    int task_nbr = (task - NRF_TIMER_TASK_CAPTURE0)/sizeof(uint32_t);
    nhw_timer_regw_sideeffects_TASKS_CAPTURE(i, task_nbr);
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_timer%i\n",
                             (int) task);
  }
}

void nrf_timer_event_clear(NRF_TIMER_Type *  p_reg,
                           nrf_timer_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    int t = timer_number_from_ptr(p_reg);
    nhw_timer_regw_sideeffects_EVENTS_all(t);
}

void nrf_timer_int_enable(NRF_TIMER_Type * p_reg,
                          uint32_t         mask)
{
    int i = timer_number_from_ptr(p_reg);
    p_reg->INTENSET = mask;
    nhw_timer_regw_sideeffects_INTENSET(i);
}

void nrf_timer_int_disable(NRF_TIMER_Type * p_reg,
                           uint32_t         mask)
{
    int i = timer_number_from_ptr(p_reg);
    p_reg->INTENCLR = mask;
    nhw_timer_regw_sideeffects_INTENCLR(i);
}

#if defined(DPPI_PRESENT)

static void nrf_timer_subscribe_common(NRF_TIMER_Type * p_reg,
                                       nrf_timer_task_t task)
{
  int i = timer_number_from_ptr(p_reg);

  if (task == NRF_TIMER_TASK_START) {
    nhw_timer_regw_sideeffects_SUBSCRIBE_START(i);
  } else if (task == NRF_TIMER_TASK_STOP) {
    nhw_timer_regw_sideeffects_SUBSCRIBE_STOP(i);
  } else if (task == NRF_TIMER_TASK_COUNT) {
    nhw_timer_regw_sideeffects_SUBSCRIBE_COUNT(i);
  } else if (task == NRF_TIMER_TASK_CLEAR) {
    nhw_timer_regw_sideeffects_SUBSCRIBE_CLEAR(i);
  } else if (task == NRF_TIMER_TASK_SHUTDOWN) {
    nhw_timer_regw_sideeffects_SUBSCRIBE_SHUTDOWN(i);
  } else if (task >= NRF_TIMER_TASK_CAPTURE0) {
    int task_nbr = (task - NRF_TIMER_TASK_CAPTURE0)/sizeof(uint32_t);
    nhw_timer_regw_sideeffects_SUBSCRIBE_CAPTURE(i, task_nbr);
  } else {
    bs_trace_error_line_time("Attempted to subscribe to a not-supported task in the nrf_timer (%i)\n",
                             task);
  }
}

void nrf_timer_subscribe_set(NRF_TIMER_Type * p_reg,
                             nrf_timer_task_t task,
                             uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_timer_subscribe_common(p_reg, task);
}

void nrf_timer_subscribe_clear(NRF_TIMER_Type * p_reg,
                               nrf_timer_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_timer_subscribe_common(p_reg, task);
}

#endif /* defined(DPPI_PRESENT) */
