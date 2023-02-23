/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RADIO Bitcounter functionality
 * We treat it as a sub-peripheral
 */
#include "bs_types.h"
#include "bs_tracing.h"
#include "time_machine_if.h"
#include "NRF_RADIO.h"
#include "NRF_RADIO_signals.h"
#include "NRF_HW_model_top.h"

bs_time_t Timer_RADIO_bitcounter = TIME_NEVER;

static bs_time_t Time_BitCounterStarted = TIME_NEVER;
static bool bit_counter_running = false;

void nrf_radio_bitcounter_reset() {
  Timer_RADIO_bitcounter = TIME_NEVER;
  bit_counter_running = 0;
}

void nrf_radio_bitcounter_cleanup() {

}

void nrf_radio_bitcounter_timer_triggered() {
  nrf_radio_signal_BCMATCH();
  Timer_RADIO_bitcounter = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
  //Note that we leave the bit counter running, so a new BCC can be programmed to make it trigger later
}

void nrf_radio_tasks_bcstart() {
  /* Note that we do not validate that the end of the address has been received */

  if (bit_counter_running) {
    bs_trace_warning_line_time("NRF_RADIO: BCSTART received while the bitcounter was already running."
        "New BCSTART is just ignored\n");
    return;
  }
  bit_counter_running = true;
  Time_BitCounterStarted = tm_get_hw_time();
  Timer_RADIO_bitcounter = Time_BitCounterStarted + NRF_RADIO_regs.BCC/nrf_radio_get_bpus();
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_radio_stop_bit_counter() {
  if (!bit_counter_running){
    return;
  }
  bit_counter_running = false;
  if (Timer_RADIO_bitcounter != TIME_NEVER) {
    Timer_RADIO_bitcounter = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrf_radio_tasks_bcstop() {
  nrf_radio_stop_bit_counter();
}

void nrf_radio_regw_sideeffects_BCC() {
  if (!bit_counter_running){
    return;
  }
  Timer_RADIO_bitcounter = Time_BitCounterStarted + NRF_RADIO_regs.BCC/nrf_radio_get_bpus();
  if (Timer_RADIO_bitcounter < tm_get_hw_time()) {
    bs_trace_warning_line_time("NRF_RADIO: Reprogrammed bitcounter with a BCC which has already"
        "passed (%"PRItime") => we ignore it\n",
        Timer_RADIO_bitcounter);
    Timer_RADIO_bitcounter = TIME_NEVER;
  }
  nrf_hw_find_next_timer_to_trigger();
}
