/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RTC - Real-time counter
 *
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/rtc.html?cp=4_1_0_5_19
 */

/* To simplify, so far it is only modeled what the current BLE controller uses
 *
 * NOT modelled:
 *  * TICK events
 *  * the delay in tasks or in the synchronization of configuration into the LF clock domain
 *
 * * Note: The COUNTER register is only updated when read with the proper function
 *
 * * Note: In this model all RTCs have 4 working CC registers (for simplicity)
 *
 * * Note: RTC2 cannot be used (it does not have an assigned interrupt)
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "NHW_types.h"
#include "NRF_PPI.h"
#include "NRF_CLOCK.h"
#include "nsi_hw_scheduler.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

#define N_RTC 3
#define N_CC 4

#define RTC_COUNTER_MASK 0xFFFFFF /*24 bits*/
#define RTC_TRIGGER_OVERFLOW_COUNTER_VALUE 0xFFFFF0

#define SUB_US_BITS 9 // Bits representing sub-microsecond units

NRF_RTC_Type NRF_RTC_regs[N_RTC];

static bool RTC_Running[N_RTC] = {false};
static uint32_t RTC_INTEN[N_RTC] = {0};

static bs_time_t Timer_RTC = TIME_NEVER;
static bs_time_t cc_timers[N_RTC][N_CC] = {{TIME_NEVER}}; //when each CC will match (in microseconds)
static bs_time_t overflow_timer[N_RTC] = {TIME_NEVER}; //when the timer will overflow (in microseconds)

static uint64_t overflow_timer_sub_us[N_RTC] = {TIME_NEVER}; //when the timer will overflow (in sub-microsecond units)
static uint64_t RTC_counter_startT_sub_us[N_RTC] = {TIME_NEVER}; //Time when the counter was "started" (really the time that would correspond to COUNTER = 0)
static uint64_t RTC_counter_startT_negative_sub_us[N_RTC] = {0};

static uint32_t counter[N_RTC] = {0}; //Internal counter value when the counter was stopped

static uint64_t first_lf_tick_time_sub_us = 0;


static bs_time_t sub_us_time_to_us_time(uint64_t sub_us_time);
static uint64_t us_time_to_sub_us_time(bs_time_t us_time);

/**
 * Convert a time delta in sub-microseconds units to the equivalent time in microseconds.
 * The value is always rounded UP. This is because otherwise the events would be registered
 * in a time in microseconds before the event actually occured. This would lead to many imprecise
 * event timings for example if the timing of an event would be calculated base on the last LF
 * clock tick (which happens for example when triggering the CLEAR or TRIGGER_OVERFLOW tasks)
 */
static bs_time_t sub_us_time_to_us_time(uint64_t sub_us_time)
{
  bs_time_t us_time = sub_us_time >> SUB_US_BITS;

  if(sub_us_time % (1U << SUB_US_BITS) != 0) //rounding up
  {
    us_time += 1;
  }

  return us_time;
}

/**
 * Convert a time delta in microseconds to the equivalent time in sub-microseconds units
 */
static uint64_t us_time_to_sub_us_time(bs_time_t us_time)
{
  return us_time << SUB_US_BITS;
}

static uint64_t get_hw_time_sub_us(void)
{
  bs_time_t now = nsi_hws_get_time();

  if (now > sub_us_time_to_us_time(TIME_NEVER)) {
    bs_trace_error_time_line("Bummer, the RTC model only supports running for 1142 years\n");
    /*If you really need this, generalize the calculation to more than 64 bits*/
  }

  return us_time_to_sub_us_time(now);
}

static uint64_t get_last_lf_tick_time_sub_us(void) {
  uint64_t now_sub_us = get_hw_time_sub_us();

  uint64_t lf_ticks = (now_sub_us - first_lf_tick_time_sub_us) / LF_CLOCK_PERIOD; //floor()
  uint64_t last_tick_time_sub_us = lf_ticks * LF_CLOCK_PERIOD;
  last_tick_time_sub_us += first_lf_tick_time_sub_us;

  return last_tick_time_sub_us;
}

/**
 * Convert a time delta in sub-microsecond units to the equivalent count accounting for the PRESCALER
 * Note that the number is rounded down [floor()]
 */
static uint64_t time_sub_us_to_counter(uint64_t delta_sub_us, int rtc) {
  uint64_t ticks;

  ticks = delta_sub_us / ((uint64_t)LF_CLOCK_PERIOD * (NRF_RTC_regs[rtc].PRESCALER + 1));
  return ticks;
}

/**
 * Convert a counter delta to sub-microsecond units accounting for the PRESCALER
 */
static uint64_t counter_to_time_sub_us(uint64_t counter, int rtc) {
  uint64_t Elapsed;

  Elapsed = counter  * (uint64_t)LF_CLOCK_PERIOD * (NRF_RTC_regs[rtc].PRESCALER + 1);

  return Elapsed;
}

/**
 * Return the time in sub-microsecond units it takes for the COUNTER to do 1 wrap
 */
static uint64_t time_of_1_counter_wrap_sub_us(int rtc) {
  return counter_to_time_sub_us((uint64_t)RTC_COUNTER_MASK + 1, rtc);
}

static bs_time_t get_counter_match_time(uint64_t counter_match, int rtc, uint64_t* next_match_sub_us)
{
  bs_time_t next_match_us = TIME_NEVER;
  *next_match_sub_us = TIME_NEVER;

  if (RTC_Running[rtc] == true) {
    uint64_t now_sub_us = get_hw_time_sub_us();
    uint64_t counter_match_sub_us = counter_to_time_sub_us(counter_match, rtc);

    if(RTC_counter_startT_sub_us[rtc] > 0)
    {
      *next_match_sub_us = RTC_counter_startT_sub_us[rtc]
                              + counter_match_sub_us;
    }
    else if (counter_match_sub_us > RTC_counter_startT_negative_sub_us[rtc])
    {
      *next_match_sub_us =  counter_match_sub_us - RTC_counter_startT_negative_sub_us[rtc];
    }
    else
    {
      *next_match_sub_us = time_of_1_counter_wrap_sub_us(rtc) + counter_match_sub_us - RTC_counter_startT_negative_sub_us[rtc];
    }

    while(*next_match_sub_us <= now_sub_us)
    {
      *next_match_sub_us += time_of_1_counter_wrap_sub_us(rtc);
    }

    next_match_us = sub_us_time_to_us_time(*next_match_sub_us);
  }

  return next_match_us;
}

static void update_master_timer(void) {
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

    if (overflow_timer[rtc] < Timer_RTC) {
      Timer_RTC = overflow_timer[rtc];
    }
  }
  nsi_hws_find_next_event();
}

/**
 * Save in cc_timers[t][cc] the next time when this RTC will match the
 * CC[cc] register
 */
static void update_cc_timer(int rtc, int cc) {
  uint64_t match_sub_us; // Only to comply to the interface
  cc_timers[rtc][cc] = get_counter_match_time(NRF_RTC_regs[rtc].CC[cc], rtc, &match_sub_us);
}

static void update_all_cc_timers(int rtc) {
  for (int cc = 0 ; cc < N_CC; cc++){
    update_cc_timer(rtc, cc);
  }
}

static void update_overflow_timer(int rtc) {
  overflow_timer[rtc] = get_counter_match_time(RTC_COUNTER_MASK + 1, rtc, &overflow_timer_sub_us[rtc]);
}

static void update_timers(int rtc)
{
  update_all_cc_timers(rtc);
  update_overflow_timer(rtc);
  update_master_timer();
}

/**
 * Sets the counter value to the specified value. This is done by setting the "counter start time"
 * to an appropriate value, so that the time ellapsed from the counter start corresponds to the given
 * counter value. Such virtual "counter start time" can be negative.
 */
static void set_counter_to(uint64_t counter_val, int rtc)
{
  counter_val &= RTC_COUNTER_MASK;
  uint64_t counter_val_sub_us = counter_to_time_sub_us(counter_val, rtc);

  // All the functions which use this reset the <PRESC>, so it is like the counter was set
  // on the last LF clock tick
  uint64_t last_lf_tick_sub_us = get_last_lf_tick_time_sub_us();

  if(last_lf_tick_sub_us >= counter_val_sub_us)
  {
    RTC_counter_startT_sub_us[rtc] = last_lf_tick_sub_us - counter_val_sub_us;
    RTC_counter_startT_negative_sub_us[rtc] = 0;
  }
  else
  {
    RTC_counter_startT_sub_us[rtc] = 0;
    RTC_counter_startT_negative_sub_us[rtc] = counter_val_sub_us - last_lf_tick_sub_us;
  }

  counter[rtc] = counter_val;
  NRF_RTC_regs[rtc].COUNTER = counter_val;

  update_timers(rtc);
}

static unsigned int get_irq_t(int rtc)
{
    unsigned int irq_t = RTC0_IRQn;

    switch (rtc){
    case 0:
      irq_t = RTC0_IRQn;
      break;
    case 1:
      irq_t = RTC1_IRQn;
      break;
    case 2:
      irq_t = RTC2_IRQn;
      bs_trace_error_line_time("There is no IRQ mapped for RTC2\n");
      break;
    }

    return irq_t;
}

static ppi_event_types_t get_cc_event(int rtc)
{
    ppi_event_types_t event = RTC0_EVENTS_COMPARE_0;
    switch (rtc){
    case 0:
      event = RTC0_EVENTS_COMPARE_0;
      break;
    case 1:
      event = RTC1_EVENTS_COMPARE_0;
      break;
    case 2:
      event = RTC2_EVENTS_COMPARE_0;
      break;
    }

    return event;
}
static ppi_event_types_t get_overflow_event(int rtc)
{
    ppi_event_types_t event = RTC0_EVENTS_OVRFLW;
    switch (rtc){
    case 0:
      event = RTC0_EVENTS_OVRFLW;
      break;
    case 1:
      event = RTC1_EVENTS_OVRFLW;
      break;
    case 2:
      event = RTC2_EVENTS_OVRFLW;
      break;
    }

    return event;
}

static void handle_event(int rtc, ppi_event_types_t event, uint32_t mask)
{
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if ( ( RTC_regs->EVTEN | RTC_INTEN[rtc] ) & mask ) {
    if ( RTC_regs->EVTEN & mask ){
      nrf_ppi_event(event);
    }
    if ( RTC_INTEN[rtc] & mask ){
      hw_irq_ctrl_set_irq(get_irq_t(rtc));
    }
  }
}

static void handle_cc_event(int rtc, int cc)
{
  ppi_event_types_t event = get_cc_event(rtc) + cc;
  uint32_t mask = RTC_EVTEN_COMPARE0_Msk << cc;
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if ( cc_timers[rtc][cc] == Timer_RTC ){ //This CC is matching now
    update_cc_timer(rtc, cc); //Next time it will match

    bs_trace_raw_time(8, "RTC%i: CC%i matching now\n", rtc, cc);

    RTC_regs->EVENTS_COMPARE[cc] = 1;
    handle_event(rtc, event, mask);
  }
}

static void handle_overflow_event(int rtc)
{
  ppi_event_types_t event = get_overflow_event(rtc);
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if ( overflow_timer[rtc] ==  Timer_RTC ) //Overflow occured now
  {
    // The real time (in sub-microsecond units, not in microseconds)
    // in which the current overflow event occurs.
    // update_overflow_timer will overwrite overflow_timer_sub_us[rtc]
    uint64_t current_overflow_event_sub_us = overflow_timer_sub_us[rtc];

    update_overflow_timer(rtc); //Next time it will overflow

    bs_trace_raw_time(8, "RTC%i: Timer overflow\n", rtc);

    RTC_regs->EVENTS_OVRFLW = 1;
    handle_event(rtc, event, RTC_EVTEN_OVRFLW_Msk);

    RTC_counter_startT_sub_us[rtc] = current_overflow_event_sub_us;
    RTC_counter_startT_negative_sub_us[rtc] = 0;
  }
}

static void nrf_rtc_timer_triggered(void) {
  for ( int rtc = 0; rtc < N_RTC-1/*the 3rd rtc does not have an int*/ ; rtc++ ){
    if ( RTC_Running[rtc] == false ) {
      continue;
    }

    for ( int cc = 0 ; cc < N_CC ; cc++) {
      handle_cc_event(rtc, cc);
    }

    handle_overflow_event(rtc); // this must always be the last event, as it might update RTC_counter_startT_sub_us

  } //for rtc
  update_master_timer();
}

NSI_HW_EVENT(Timer_RTC, nrf_rtc_timer_triggered, 50);

/**
 * Check if an EVTEN or INTEN has the tick event set
 */
static void check_not_supported_func(uint32_t i) {
  if (i &  RTC_EVTEN_TICK_Msk) {
    bs_trace_warning_line_time("RTC: The TICK functionality is not modelled\n");
  }
}

static void nrf_rtc_init(void) {
  memset(NRF_RTC_regs, 0, sizeof(NRF_RTC_regs));
  for (int i = 0; i < N_RTC ; i++) {
    RTC_Running[i] = false;
    RTC_INTEN[i] = 0;
    counter[i] = 0;
    RTC_counter_startT_sub_us[i] = TIME_NEVER;
    RTC_counter_startT_negative_sub_us[i] = 0;
    for (int j  = 0 ; j < N_CC ; j++) {
      cc_timers[i][j] = TIME_NEVER;
    }
    overflow_timer[i] = TIME_NEVER;
    overflow_timer_sub_us[i] = TIME_NEVER;
  }
  Timer_RTC = TIME_NEVER;
}

NSI_TASK(nrf_rtc_init, HW_INIT, 100);

void nrf_rtc_notify_first_lf_tick(void) {
  first_lf_tick_time_sub_us = get_hw_time_sub_us();
  bs_trace_raw_time(9, "RTC: First lf tick\n");
}

void nrf_rtc_update_COUNTER(int rtc) {
  if (RTC_Running[rtc] == true) {
    uint64_t count;
    count = time_sub_us_to_counter(get_hw_time_sub_us() - RTC_counter_startT_sub_us[rtc]
                                                          + RTC_counter_startT_negative_sub_us[rtc], rtc);
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

  //If the counter is not zero at start, is like if the counter was started earlier
  set_counter_to(counter[rtc], rtc);
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
  counter[rtc] = time_sub_us_to_counter(get_hw_time_sub_us() - RTC_counter_startT_sub_us[rtc]
                                        + RTC_counter_startT_negative_sub_us[rtc], rtc); //we save the value when the counter was stoped in case it is started again without clearing it
  counter[rtc] &= RTC_COUNTER_MASK;
  for (int cc = 0 ; cc < N_CC ; cc++){
    cc_timers[rtc][cc] = TIME_NEVER;
  }
  overflow_timer[rtc] = TIME_NEVER;
  update_master_timer();
}

/**
 * TASK_CLEAR triggered handler
 */
void nrf_rtc_TASKS_CLEAR(int rtc) {
  bs_trace_raw_time(5, "RTC%i: TASK_CLEAR\n", rtc);

  set_counter_to(0, rtc);
}

/**
 * TASK_TRIGGER_OVERFLOW triggered handler
 */
void nrf_rtc_TASKS_TRIGOVRFLW(int rtc) {

  bs_trace_raw_time(5, "RTC%i: TASK_TRIGGER_OVERFLOW\n", rtc);

  set_counter_to(RTC_TRIGGER_OVERFLOW_COUNTER_VALUE, rtc);
}

void nrf_rtc0_TASKS_START(void)      { nrf_rtc_TASKS_START(0); }
void nrf_rtc0_TASKS_STOP(void)       { nrf_rtc_TASKS_STOP(0);  }
void nrf_rtc0_TASKS_CLEAR(void)      { nrf_rtc_TASKS_CLEAR(0); }
void nrf_rtc0_TASKS_TRIGOVRFLW(void) { nrf_rtc_TASKS_TRIGOVRFLW(0); }
void nrf_rtc1_TASKS_START(void)      { nrf_rtc_TASKS_START(1); }
void nrf_rtc1_TASKS_STOP(void)       { nrf_rtc_TASKS_STOP(1);  }
void nrf_rtc1_TASKS_CLEAR(void)      { nrf_rtc_TASKS_CLEAR(1); }
void nrf_rtc1_TASKS_TRIGOVRFLW(void) { nrf_rtc_TASKS_TRIGOVRFLW(1); }
void nrf_rtc2_TASKS_START(void)      { nrf_rtc_TASKS_START(2); }
void nrf_rtc2_TASKS_STOP(void)       { nrf_rtc_TASKS_STOP(2);  }
void nrf_rtc2_TASKS_CLEAR(void)      { nrf_rtc_TASKS_CLEAR(2); }
void nrf_rtc2_TASKS_TRIGOVRFLW(void) { nrf_rtc_TASKS_TRIGOVRFLW(2); }

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

void nrf_rtc_regw_sideeffect_TASKS_TRIGOVRFLW(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->TASKS_TRIGOVRFLW ){
    NRF_RTC_regs[i].TASKS_TRIGOVRFLW = 0;
    nrf_rtc_TASKS_TRIGOVRFLW(i);
  }
}

void nrf_rtc_regw_sideeffect_INTENSET(int i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];
  if ( RTC_regs->INTENSET ){
    uint32_t new_interrupts = RTC_regs->INTENSET & ~RTC_INTEN[i];
    unsigned int irq_t = get_irq_t(i);
    uint32_t mask = RTC_EVTEN_COMPARE0_Msk;

    RTC_INTEN[i] |= RTC_regs->INTENSET;
    RTC_regs->INTENSET = RTC_INTEN[i];
    for ( int cc = 0 ; cc < N_CC ; cc++, mask <<=1) {
      if (RTC_regs->EVENTS_COMPARE[cc] && (new_interrupts & mask)) {
        hw_irq_ctrl_set_irq(irq_t);
      }
    }

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
