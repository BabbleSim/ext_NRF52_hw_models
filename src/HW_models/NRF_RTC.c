/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "NRF_RTC.h"
#include "NRF_PPI.h"
#include "NRF_CLOCK.h"
#include "NRF_HW_model_top.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "bs_tracing.h"
#include "time_machine_if.h"

/*
 * RTC — Real-time counter
 *
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/rtc.html?cp=2_0_0_23#concept_rvn_vkj_sr
 */

/* To simplify, so far it is only modeled what the current BLE controller uses
 *
 * NOT modelled:
 *  * TICK events
 *  * the delay in tasks or in the synchronization of configuration into the LF clock domain
 *  * TRIGOVRFLW task or OVRFLW  event
 *
 * * Note: The COUNTER register is only updated when read with the proper function
 *
 * * Note: In this model all RTCs have 4 working CC registers (for simplicity)
 *
 * * Note: RTC2 cannot be used (it does not have an assigned interrupt)
 */

#define N_RTC 3
#define N_CC 4

#define RTC_COUNTER_MASK 0xFFFFFF /*24 bits*/
NRF_RTC_Type NRF_RTC_regs[N_RTC];

static bool RTC_Running[N_RTC] = {false};
static uint32_t RTC_INTEN[N_RTC] = {0};

bs_time_t Timer_RTC = TIME_NEVER;
static bs_time_t cc_timers[N_RTC][N_CC] = {{TIME_NEVER}}; //when each CC will match (in microseconds)

static bs_time_t RTC_counter_startT[N_RTC] = {TIME_NEVER}; //Time when the counter was "started" (really the time that would correspond to COUNTER = 0)
static uint32_t counter[N_RTC] = {0}; //Internal counter value when the counter was stopped

static bs_time_t first_lf_tick_time = 0;

void nrf_rtc_init() {
  memset(NRF_RTC_regs, 0, sizeof(NRF_RTC_regs));
  for (int i = 0; i < N_RTC ; i++) {
    RTC_Running[i] = false;
    RTC_INTEN[i] = 0;
    counter[i] = 0;
    RTC_counter_startT[i] = TIME_NEVER;
    for (int j  = 0 ; j < N_CC ; j++) {
      cc_timers[i][j] = TIME_NEVER;
    }
  }
  Timer_RTC = TIME_NEVER;
}

void nrf_rtc_clean_up() {

}

void nrf_rtc_notify_first_lf_tick() {
  first_lf_tick_time = tm_get_hw_time();
  bs_trace_raw_time(9, "RTC: First lf tick\n");
}


static bs_time_t get_last_lf_tick_time() {
  bs_time_t now = tm_get_hw_time();

  if (now > TIME_NEVER>>9) {
    bs_trace_error_time_line("Bummer, the RTC model only supports running for 1142 years\n");
    /*If you really need this, generalize the calculation to more than 64 bits*/
  }

  uint64_t lf_ticks = ((now - first_lf_tick_time)<<9) / LF_CLOCK_PERIOD; //floor()
  bs_time_t last_tick_time = lf_ticks * LF_CLOCK_PERIOD >> 9;
  last_tick_time += first_lf_tick_time;

  return last_tick_time;
}

/**
 * Convert a time delta in microseconds to the equivalent count accounting for the PRESCALER
 * Note that the number is rounded down [floor()]
 */
static uint64_t time_to_counter(bs_time_t delta, int rtc) {
  uint64_t ticks;
  //Note: we add 1us, to account for the RTC ticks happening in the floor of the us
  // as 1us is way smaller than the LF_CLOCK_PERIOD it will not cause an issue rounding the counter value up
  ticks = ((delta + 1)<< 9) / ((uint64_t)LF_CLOCK_PERIOD * (NRF_RTC_regs[rtc].PRESCALER + 1));
  return ticks;
}

/**
 * Convert a counter delta to microseconds accounting for the PRESCALER
 * (the fractional number microseconds with 9 bit resolution, is stored in frac)
 */
static bs_time_t counter_to_time(uint64_t counter, int rtc, uint16_t *frac) {
  bs_time_t Elapsed;
  Elapsed = counter  * (uint64_t)LF_CLOCK_PERIOD * (NRF_RTC_regs[rtc].PRESCALER + 1);
  if (frac != NULL){
    *frac = Elapsed & 0x1FF;
  }
  return Elapsed >> 9;
}

/**
 * Return the time it takes for the COUNTER to do 1 wrap
 *  The whole number of microseconds is stored in us, the fractional number of microseconds
 *  with 9 bits resolution is stored in frac
 */
static void time_of_1_counter_wrap(int rtc, bs_time_t *us, uint16_t *frac) {
  *us = counter_to_time((uint64_t)RTC_COUNTER_MASK + 1, rtc, frac);
}

static void update_master_timer() {
  Timer_RTC = TIME_NEVER;
  for (int rtc = 0; rtc < N_RTC ; rtc++) {
    if (RTC_Running[rtc] == false) {
      continue;
    }
    for (int cc = 0 ; cc < N_CC ; cc++) {
      if (cc_timers[rtc][cc] < Timer_RTC) {
        Timer_RTC = cc_timers[rtc][cc];
      }
    }
  }
  nrf_hw_find_next_timer_to_trigger();
}

/**
 * Save in cc_timers[t][cc] the next time when this RTC will match the
 * CC[cc] register
 */
static void update_cc_timer(int rtc, int cc) {
  if (RTC_Running[rtc] == true) {
    uint16_t next_match_frac;
    bs_time_t next_match_us = RTC_counter_startT[rtc]
                              + counter_to_time(NRF_RTC_regs[rtc].CC[cc], rtc, &next_match_frac);

    bs_time_t now = tm_get_hw_time();

    if (next_match_us <= now) {
      bs_time_t t_us;
      uint16_t t_frac;

      time_of_1_counter_wrap(rtc, &t_us, &t_frac);

      do {
        next_match_us += t_us;
        next_match_frac += t_frac;
        if (next_match_frac >= 0x200){
          next_match_frac -= 0x200;
          next_match_us +=1;
        }
      } while (next_match_us <= now);

    }
    cc_timers[rtc][cc] = next_match_us;
  } else {
    cc_timers[rtc][cc] = TIME_NEVER;
  }
}

static void update_all_cc_timers(int rtc) {
  for (int cc = 0 ; cc < N_CC; cc++){
    update_cc_timer(rtc, cc);
  }
}

void nrf_rtc_timer_triggered() {
  for ( int rtc = 0; rtc < N_RTC-1/*the 3rd rtc does not have an int*/ ; rtc++ ){
    if ( RTC_Running[rtc] == false ) {
      continue;
    }
    ppi_event_types_t event = RTC0_EVENTS_COMPARE_0;
    unsigned int irq_t         = NRF5_IRQ_RTC0_IRQn;
    switch (rtc){
    case 0:
      event = RTC0_EVENTS_COMPARE_0;
      irq_t = NRF5_IRQ_RTC0_IRQn;
      break;
    case 1:
      event = RTC1_EVENTS_COMPARE_0;
      irq_t = NRF5_IRQ_RTC1_IRQn;
      break;
    case 2:
      event = RTC2_EVENTS_COMPARE_0;
      irq_t = NRF5_IRQ_RTC1_IRQn;
      bs_trace_error_line_time("There is no IRQ mapped for RTC2\n");
      break;
    }

    NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

    uint32_t mask = RTC_EVTEN_COMPARE0_Msk;

    for ( int cc = 0 ; cc < N_CC ; cc++, event++, mask <<=1) {
      if ( cc_timers[rtc][cc] == Timer_RTC ){ //This CC is matching now
        update_cc_timer(rtc, cc); //Next time it will match

        bs_trace_raw_time(8, "RTC%i: CC%i matching now\n", rtc, cc);

        if ( ( RTC_regs->EVTEN | RTC_INTEN[rtc] ) & mask ) {
          RTC_regs->EVENTS_COMPARE[cc] = 1;
          if ( RTC_regs->EVTEN & mask ){
            nrf_ppi_event(event);
          }
          if ( RTC_INTEN[rtc] & mask ){
            hw_irq_ctrl_set_irq(irq_t);
          }
        }
      } //if cc_timers[rtc][cc] == Timer_RTC
    } //for cc
  } //for rtc
  update_master_timer();
}

/**
 * Check if an EVTEN or INTEN has the tick event set
 */
static void check_not_supported_func(uint32_t i) {
  if (i &  RTC_EVTEN_TICK_Msk) {
    bs_trace_warning_line_time("RTC: The TICK functionality is not modelled\n");
  }
  if (i &  RTC_EVTEN_OVRFLW_Msk) {
    bs_trace_warning_line_time("RTC: The OVERFLOW functionality is not modelled\n");
  }
}


void nrf_rtc_update_COUNTER(int rtc) {
  if (RTC_Running[rtc] == true) {
    uint64_t count;
    count = time_to_counter(tm_get_hw_time() - RTC_counter_startT[rtc], rtc);
    NRF_RTC_regs[rtc].COUNTER = count & RTC_COUNTER_MASK;
  } else {
    NRF_RTC_regs[rtc].COUNTER = counter[rtc] & RTC_COUNTER_MASK;
  }
}

/**
 * TASK_START triggered handler
 */
void nrf_rtc_TASKS_START(int rtc) {
  if (RTC_Running[rtc] == true) {
    return;
  }
  bs_trace_raw_time(5, "RTC%i: TASK_START\n", rtc);
  RTC_Running[rtc] = true;
  RTC_counter_startT[rtc] = get_last_lf_tick_time()
                            - counter_to_time(counter[rtc], rtc, NULL); //If the counter is not zero at start, is like if the counter was started earlier
  update_all_cc_timers(rtc);
  update_master_timer();
}

/**
 * TASK_STOP triggered handler
 */
void nrf_rtc_TASKS_STOP(int rtc) {
  if (RTC_Running[rtc] == false) {
    return;
  }
  bs_trace_raw_time(5, "RTC%i: TASK_STOP\n", rtc);
  RTC_Running[rtc] = false;
  counter[rtc] = time_to_counter(tm_get_hw_time() - RTC_counter_startT[rtc], rtc); //we save the value when the counter was stoped in case it is started again without clearing it
  counter[rtc] &= RTC_COUNTER_MASK;
  for (int cc = 0 ; cc < N_CC ; cc++){
    cc_timers[rtc][cc] = TIME_NEVER;
  }
  update_master_timer();
}

/**
 * TASK_CLEAR triggered handler
 */
void nrf_rtc_TASKS_CLEAR(int rtc) {
  bs_trace_raw_time(5, "RTC%i: TASK_CLEAR\n", rtc);
  NRF_RTC_regs[rtc].COUNTER = 0;
  counter[rtc] = 0;
  RTC_counter_startT[rtc] = get_last_lf_tick_time();
  update_all_cc_timers(rtc);
  update_master_timer();
}

void nrf_rtc0_TASKS_START() { nrf_rtc_TASKS_START(0); }
void nrf_rtc0_TASKS_STOP()  { nrf_rtc_TASKS_STOP(0);  }
void nrf_rtc0_TASKS_CLEAR() { nrf_rtc_TASKS_CLEAR(0); }
void nrf_rtc1_TASKS_START() { nrf_rtc_TASKS_START(1); }
void nrf_rtc1_TASKS_STOP()  { nrf_rtc_TASKS_STOP(1);  }
void nrf_rtc1_TASKS_CLEAR() { nrf_rtc_TASKS_CLEAR(1); }
void nrf_rtc2_TASKS_START() { nrf_rtc_TASKS_START(2); }
void nrf_rtc2_TASKS_STOP()  { nrf_rtc_TASKS_STOP(2);  }
void nrf_rtc2_TASKS_CLEAR() { nrf_rtc_TASKS_CLEAR(2); }

void nrf_rtc_regw_sideeffect_TASKS_START(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->TASKS_START ){
    RTC_regs->TASKS_START = 0;
    nrf_rtc_TASKS_START(i);
  }
}

void nrf_rtc_regw_sideeffect_TASKS_STOP(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->TASKS_STOP ){
    NRF_RTC_regs[i].TASKS_STOP = 0;
    nrf_rtc_TASKS_STOP(i);
  }
}

void nrf_rtc_regw_sideeffect_TASKS_CLEAR(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->TASKS_CLEAR ){
    NRF_RTC_regs[i].TASKS_CLEAR = 0;
    nrf_rtc_TASKS_CLEAR(i);
  }
}

void nrf_rtc_regw_sideeffect_INTENSET(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->INTENSET ){
    RTC_INTEN[i] |= RTC_regs->INTENSET;
    RTC_regs->INTENSET = RTC_INTEN[i];
    check_not_supported_func(RTC_INTEN[i]);
  }
}

void nrf_rtc_regw_sideeffect_INTENCLR(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->INTENCLR ){
    RTC_INTEN[i]  &= ~RTC_regs->INTENCLR;
    RTC_regs->INTENSET = RTC_INTEN[i];
    RTC_regs->INTENCLR = 0;
  }
}

void nrf_rtc_regw_sideeffect_EVTENSET(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->EVTENSET ){
    RTC_regs->EVTEN |= RTC_regs->EVTENSET;
    RTC_regs->EVTENSET = RTC_regs->EVTEN;
    check_not_supported_func(RTC_regs->EVTEN);
  }
}

void nrf_rtc_regw_sideeffect_EVTENCLR(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->EVTENCLR ){
    RTC_regs->EVTEN  &= ~RTC_regs->EVTENCLR;
    RTC_regs->EVTENSET = RTC_regs->EVTEN;
    RTC_regs->EVTENCLR = 0;
  }
}

void nrf_rtc_regw_sideeffects_CC(int rtc, int cc_n) {
  if (RTC_Running[rtc] == true) {
    update_cc_timer(rtc, cc_n);
    update_master_timer();
  }
}

/**
 * Handle any register side effect (by inspecting the registers values)
 * (deprecated)
 */
void nrf_rtc_regw_sideeffects(int i){
  nrf_rtc_regw_sideeffect_TASKS_START(i);

  nrf_rtc_regw_sideeffect_TASKS_STOP(i);

  nrf_rtc_regw_sideeffect_TASKS_CLEAR(i);

  nrf_rtc_regw_sideeffect_INTENSET(i);

  nrf_rtc_regw_sideeffect_INTENCLR(i);

  nrf_rtc_regw_sideeffect_EVTENSET(i);

  nrf_rtc_regw_sideeffect_EVTENCLR(i);
}

void nrf_rtc0_regw_sideeffects(){ nrf_rtc_regw_sideeffects(0); }
void nrf_rtc1_regw_sideeffects(){ nrf_rtc_regw_sideeffects(1); }
void nrf_rtc2_regw_sideeffects(){ nrf_rtc_regw_sideeffects(2); }
