/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * This is a fake HW device model which executes a test applicatio
 * timed "task" each time a fake timer is triggered
 */

#include "bs_types.h"
#include "irq_ctrl.h"
#include "nsi_tasks.h"
#include "nsi_hw_scheduler.h"
#include "nsi_hws_models_if.h"

static bs_time_t Timer_event_fw_test_ticker = TIME_NEVER;

static uint8_t awake_cpu_asap = 0;
static bs_time_t Timer_event_fw_test_ticker_internal = TIME_NEVER;
static bs_time_t fw_test_ticker_tick_period = TIME_NEVER;

static void bst_ticker_find_next_time(){
  if ( awake_cpu_asap == 1){
    Timer_event_fw_test_ticker = nsi_hws_get_time(); //We will awake it in this same microsecond
  } else {
    Timer_event_fw_test_ticker = Timer_event_fw_test_ticker_internal;
  }

  nsi_hws_find_next_event();
}

/**
 * Set the FW_test ticker to trigger periodically, with a period of <tick_period>
 * Next trigger at <delta_time> us + current Hw time
 */
void bst_ticker_set_period(bs_time_t tick_period){
  fw_test_ticker_tick_period = tick_period;
  Timer_event_fw_test_ticker_internal = tick_period + nsi_hws_get_time();
  bst_ticker_find_next_time();
}

/**
 * Set the next time the FW test ticker will trigger at <absolute_time> us
 */
void bst_ticker_set_next_tick_absolute(bs_time_t absolute_time){
  Timer_event_fw_test_ticker_internal = absolute_time;
  bst_ticker_find_next_time();
}

/**
 * Set the next time the FW test ticker will trigger
 * at <delta_time> us + current Hw time
 */
void bst_ticker_set_next_tick_delta(bs_time_t delta_time){
  Timer_event_fw_test_ticker_internal = delta_time + nsi_hws_get_time();
  bst_ticker_find_next_time();
}

static void bst_ticker_triggered(void) {
  bs_time_t HW_time = nsi_hws_get_time();

  if ( awake_cpu_asap == 1 ) {
    awake_cpu_asap = 0;
    hw_irq_ctrl_raise_im(PHONY_HARD_IRQ);
  } else {
    if ( fw_test_ticker_tick_period != TIME_NEVER ){
      Timer_event_fw_test_ticker_internal = fw_test_ticker_tick_period + HW_time;
    } else {
      Timer_event_fw_test_ticker_internal = TIME_NEVER;
    }
    extern void nsif_cpu0_bst_tick(uint64_t time);
    nsif_cpu0_bst_tick(HW_time);
  }
  bst_ticker_find_next_time();
}

NSI_HW_EVENT(Timer_event_fw_test_ticker, bst_ticker_triggered, 1 /* Purposedly the second */);

/**
 * Awake the MCU as soon as possible (in this same microsecond, in a following delta)
 */
void bst_awake_cpu_asap(){
  awake_cpu_asap = 1;
  bst_ticker_find_next_time();
}
