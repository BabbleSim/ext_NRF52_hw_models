/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * TIMER — Timer/counter
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/timer.html?cp=5_1_0_5_27
 */

/**
 * Notes:
 *   * The functionality of TASK_SHUTDOWN is a bit of a guess
 *   * INTENCLR will always read as 0
 *   * Unlike in real HW, tasks cannot occur simultaneously, they always happen in some sequence
 *     so task priority is not accounted for
 *
 * Implementation notes:
 *
 * In Timer mode, the timer is not actually counting all the time, but instead
 * a HW event timer is programmed on the expected matches' times whenever the
 * timer is started, stopped, or the compare values are updated.
 *
 * In Count mode, the timer count value (Counter[t]) is updated each time the
 * corresponding TASK_COUNT is triggered.
 */

#include <string.h>
#include <stdbool.h>
#include "time_machine_if.h"
#include "NRF_TIMER.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"

#define N_TIMERS 5
#define N_MAX_CC 6
#define N_TIMER_CC_REGS {4, 4, 4, 6, 6} /* Number CC registers for each Timer */

NRF_TIMER_Type NRF_TIMER_regs[N_TIMERS];

static uint32_t TIMER_INTEN[N_TIMERS] = {0};

static bool Timer_running[N_TIMERS] = {false};

static int Timer_n_CCs[N_TIMERS] = N_TIMER_CC_REGS;

static bs_time_t Timer_counter_startT[N_TIMERS] = {TIME_NEVER}; //Time when the timer was started (only for timer mode)
static uint32_t Counter[N_TIMERS] = {0}; //Internal count value. Used in count mode, and in Timer mode during stops.

bs_time_t Timer_TIMERs = TIME_NEVER;
/* In timer mode: When each compare match is expected to happen: */
static bs_time_t CC_timers[N_TIMERS][N_MAX_CC] = {{TIME_NEVER}};

/**
 * Initialize the TIMER model
 */
void nrf_hw_model_timer_init(void) {
  memset(NRF_TIMER_regs, 0, sizeof(NRF_TIMER_regs));
  for (int t = 0; t < N_TIMERS ; t++ ){
    TIMER_INTEN[t] = 0;
    Timer_running[t] = false;
    Timer_counter_startT[t] = TIME_NEVER;
    Counter[t] = 0;
    for ( int cc = 0 ; cc < Timer_n_CCs[t] ; cc++){
      CC_timers[t][cc] = TIME_NEVER;
    }
  }
  Timer_TIMERs = TIME_NEVER;
}

/**
 * Clean up the TIMER model before program exit
 */
void nrf_hw_model_timer_clean_up(void) {

}

/**
 * Convert a time delta in us to the equivalent count accounting for the PRESCALER
 * The timer base clock is 16MHz
 */
static uint32_t time_to_counter(bs_time_t delta, int t){
  uint64_t ticks;
  ticks = ( delta << 4 ) >> NRF_TIMER_regs[t].PRESCALER;
  return ticks;
}

/**
 * Convert a counter delta to us accounting for the PRESCALER
 * The timer base clock is 16MHz
 */
static bs_time_t counter_to_time(uint64_t counter, int t){
  bs_time_t Elapsed;
  Elapsed = ( counter << NRF_TIMER_regs[t].PRESCALER ) >> 4;
  return Elapsed;
}

/**
 * Return the counter mask (due to BITMODE) for this TIMER<t>
 */
static uint32_t mask_from_bitmode(int t){
  switch (NRF_TIMER_regs[t].BITMODE){
  case 0:
    return 0xFFFF;
    break;
  case 1:
    return 0xFF;
    break;
  case 2:
    return 0xFFFFFF;
    break;
  default:
    return 0xFFFFFFFF;
    break;
  }
}

/**
 * Return the time in us it takes for the COUNTER to do 1 wrap
 * given BITMODE
 */
static uint64_t time_of_1_counter_wrap(int t){
  return counter_to_time( (uint64_t)mask_from_bitmode(t) + 1,t);
}

/**
 * Find the CC register timer (CC_timers[][]) which will trigger earliest (if any)
 */
static void update_master_timer(void) {
  Timer_TIMERs = TIME_NEVER;
  for ( int t = 0 ; t < N_TIMERS ; t++){
    if ( ( Timer_running[t] == true ) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
      for ( int cc = 0 ; cc < Timer_n_CCs[t] ; cc++){
        if ( CC_timers[t][cc] < Timer_TIMERs ){
          Timer_TIMERs = CC_timers[t][cc];
        }
      }
    }
  }
  nrf_hw_find_next_timer_to_trigger();
}

/**
 * Save in CC_timers[t][cc] the next time when this timer will match the CC[cc]
 * register
 */
static void update_cc_timer(int t, int cc){
  if ( ( Timer_running[t] == true ) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
    bs_time_t next_match = Timer_counter_startT[t]
                           + counter_to_time(NRF_TIMER_regs[t].CC[cc], t);
    while ( next_match <= tm_get_hw_time() ){
      next_match += time_of_1_counter_wrap(t);
    }
    CC_timers[t][cc] = next_match;
  } else {
    CC_timers[t][cc] = TIME_NEVER;
  }
}

static void update_all_cc_timers(int t){
  for (int cc = 0 ; cc < Timer_n_CCs[t]; cc++){
    update_cc_timer(t,cc);
  }
}

static int nrf_timer_get_irq_number(int t){
  int irq = TIMER0_IRQn;

  switch (t){
  case 0:
    irq = TIMER0_IRQn;
    break;
  case 1:
    irq = TIMER1_IRQn;
    break;
  case 2:
    irq = TIMER2_IRQn;
    break;
  case 3:
    irq = TIMER3_IRQn;
    break;
  case 4:
    irq = TIMER4_IRQn;
    break;
  default:
    irq = -1;
    break;
  }
  return irq;
}

static void nrf_timer_eval_interrupts(int t) {
  static bool TIMER_int_line[N_TIMERS]; /* Is the TIMER currently driving its interrupt line high */
  bool new_int_line = false;
  int irq_line = nrf_timer_get_irq_number(t);
  const char *no_int_error = "NRF HW TIMER%i interrupt triggered "
                             "but there is no interrupt mapped for it\n";

  for (int cc = 0; cc < Timer_n_CCs[t]; cc++) {
    int mask = TIMER_INTEN[t] & (TIMER_INTENSET_COMPARE0_Msk << cc);
    if (NRF_TIMER_regs[t].EVENTS_COMPARE[cc] && mask) {
      new_int_line = true;
      break; /* No need to check more */
    }
  }

  if (TIMER_int_line[t] == false && new_int_line == true) {
    TIMER_int_line[t] = true;
    if (irq_line < -1) {
      bs_trace_error_line_time(no_int_error, t);
    }
    hw_irq_ctrl_raise_level_irq_line(irq_line);
  } else if (TIMER_int_line[t] == true && new_int_line == false) {
    TIMER_int_line[t] = false;
    if (irq_line < -1) {
      bs_trace_error_line_time(no_int_error, t);
    }
    hw_irq_ctrl_lower_level_irq_line(irq_line);
  }
}

void nrf_timer_TASK_START(int t){
  //Note: STATUS is missing in NRF_TIMER_Type
  if ( Timer_running[t] == false ) {
    Timer_running[t] = true;
    if ( NRF_TIMER_regs[t].MODE == 0 ){ //Timer mode
      Timer_counter_startT[t] = tm_get_hw_time() - counter_to_time(Counter[t], t); //If the counter is not zero at start, is like if the counter was started earlier
      update_all_cc_timers(t);
      update_master_timer();
    }
  }
}

void nrf_timer_TASK_STOP(int t){
  //Note: STATUS is missing in NRF_TIMER_Type
  if (Timer_running[t] == true) {
    Timer_running[t] = false;
    if ( NRF_TIMER_regs[t].MODE == 0 ){ //Timer mode
      Counter[t] = time_to_counter(tm_get_hw_time() - Timer_counter_startT[t], t); //we save the value when the counter was stoped in case it is started again without clearing it
    }
    for (int cc = 0 ; cc < Timer_n_CCs[t] ; cc++){
      CC_timers[t][cc] = TIME_NEVER;
    }
    update_master_timer();
  }
}

void nrf_timer_TASK_SHUTDOWN(int t){
  /*
   * TASK_SHUTDOWN is not documented in newer SOCs
   * In older ones it indicates it STOPS + it reduces power consumption
   * but that it cannot be resumed after from where it was
   * The assumption is that the internal count is lost/reset to 0
   * Effectively making a SHUTDOWN logically equivalent to a STOP + CLEAR
   */
  //Note: STATUS is missing in NRF_TIMER_Type
  Timer_running[t] = false;
  Counter[t] = 0;
  Timer_counter_startT[t] = TIME_NEVER;
  for (int cc = 0 ; cc < Timer_n_CCs[t] ; cc++){
    CC_timers[t][cc] = TIME_NEVER;
  }
  update_master_timer();
}

void nrf_timer_TASK_CAPTURE(int t, int cc_n){
  if (cc_n >= Timer_n_CCs[t]) {
    bs_trace_error_line_time("%s: Attempted to access non existing task"
                             "TIMER%i.TASK_CAPTURE[%i] (>= %i)\n",
                              t, cc_n, Timer_n_CCs[t]);
  }
  if ( NRF_TIMER_regs[t].MODE != 0 ){ //Count mode
    NRF_TIMER_regs[t].CC[cc_n] = Counter[t] & mask_from_bitmode(t);
  } else { //Timer mode:
    if ( Timer_counter_startT[t] == TIME_NEVER ){
      bs_trace_warning_line_time("NRF HW TIMER%i TASK_CAPTURE[%i] "
                                     "trigered on a timer which was never "
                                     "started => you get garbage\n", t, cc_n);
    }
    bs_time_t Elapsed = tm_get_hw_time() - Timer_counter_startT[t];
    NRF_TIMER_regs[t].CC[cc_n] = time_to_counter(Elapsed,t) & mask_from_bitmode(t);

    //The new CC causes a new possible CC match time:
    update_cc_timer(t, cc_n);
    update_master_timer();
  }
}

void nrf_timer_TASK_CLEAR(int t) {
  Counter[t] = 0;
  if (NRF_TIMER_regs[t].MODE == 0) {
    //Timer mode:
    Timer_counter_startT[t] = tm_get_hw_time();
    update_all_cc_timers(t);
    update_master_timer();
  }
}

static void nrf_timer_signal_COMPARE(int t, int cc) {

  if ( NRF_TIMER_regs[t].SHORTS & (TIMER_SHORTS_COMPARE0_CLEAR_Msk << cc) ) {
    nrf_timer_TASK_CLEAR(t);
  }
  if ( NRF_TIMER_regs[t].SHORTS & (TIMER_SHORTS_COMPARE0_STOP_Msk << cc) ) {
    nrf_timer_TASK_STOP(t);
  }

  NRF_TIMER_regs[t].EVENTS_COMPARE[cc] = 1;

  nrf_timer_eval_interrupts(t);

  ppi_event_types_t event_cc;
  switch (t) {
  case 0:
    event_cc = TIMER0_EVENTS_COMPARE_0;
    break;
  case 1:
    event_cc = TIMER1_EVENTS_COMPARE_0;
    break;
  case 2:
    event_cc = TIMER2_EVENTS_COMPARE_0;
    break;
  case 3:
    event_cc = TIMER3_EVENTS_COMPARE_0;
    break;
  case 4:
  default: /* This default is just to silence a -Werror=maybe-uninitialized warning */
    event_cc = TIMER4_EVENTS_COMPARE_0;
    break;
  }
  nrf_ppi_event(event_cc + cc);
}

void nrf_timer_TASK_COUNT(int t) {
  if ( (NRF_TIMER_regs[t].MODE != 0 /* Count mode */) && (Timer_running[t] == true) ) {
    Counter[t] = (Counter[t] + 1) & mask_from_bitmode(t);

    for (int cc_n = 0; cc_n < Timer_n_CCs[t]; cc_n++) {
      if (Counter[t] == (NRF_TIMER_regs[t].CC[cc_n] & mask_from_bitmode(t))){
        nrf_timer_signal_COMPARE(t, cc_n);
      }
    }
  } //Otherwise ignored
}

void nrf_timer_regw_sideeffects_TASKS_START(int t){
  if ( NRF_TIMER_regs[t].TASKS_START ){
    NRF_TIMER_regs[t].TASKS_START = 0;
    nrf_timer_TASK_START(t);
  }
}

void nrf_timer_regw_sideeffects_TASKS_STOP(int t) {
  if (NRF_TIMER_regs[t].TASKS_STOP) {
    NRF_TIMER_regs[t].TASKS_STOP = 0;
    nrf_timer_TASK_STOP(t);
  }
}

void nrf_timer_regw_sideeffects_TASKS_SHUTDOWN(int t) {
  if (NRF_TIMER_regs[t].TASKS_SHUTDOWN) {
    NRF_TIMER_regs[t].TASKS_SHUTDOWN = 0;
    nrf_timer_TASK_SHUTDOWN(t);
  }
}

void nrf_timer_regw_sideeffects_TASKS_CAPTURE(int t, int cc_n){
  if ( NRF_TIMER_regs[t].TASKS_CAPTURE[cc_n] ){
    NRF_TIMER_regs[t].TASKS_CAPTURE[cc_n] = 0;
    nrf_timer_TASK_CAPTURE(t,cc_n);
  }
}

void nrf_timer_regw_sideeffects_TASKS_CLEAR(int t) {
  if (NRF_TIMER_regs[t].TASKS_CLEAR) {
    NRF_TIMER_regs[t].TASKS_CLEAR = 0;
    nrf_timer_TASK_CLEAR(t);
  }
}

void nrf_timer_regw_sideeffects_TASKS_COUNT(int t){
  if ( NRF_TIMER_regs[t].TASKS_COUNT ){
    NRF_TIMER_regs[t].TASKS_COUNT = 0;
    nrf_timer_TASK_COUNT(t);
  }
}

void nrf_timer_regw_sideeffects_EVENTS_all(int t){
  nrf_timer_eval_interrupts(t);
}

void nrf_timer_regw_sideeffects_INTENSET(int t){
  if ( NRF_TIMER_regs[t].INTENSET ){
    TIMER_INTEN[t] |= NRF_TIMER_regs[t].INTENSET;
    NRF_TIMER_regs[t].INTENSET = TIMER_INTEN[t];
    nrf_timer_eval_interrupts(t);
  }
}

void nrf_timer_regw_sideeffects_INTENCLR(int t){
  if ( NRF_TIMER_regs[t].INTENCLR ){
    TIMER_INTEN[t]  &= ~NRF_TIMER_regs[t].INTENCLR;
    NRF_TIMER_regs[t].INTENSET = TIMER_INTEN[t];
    NRF_TIMER_regs[t].INTENCLR = 0;
    nrf_timer_eval_interrupts(t);
  }
}

void nrf_timer_regw_sideeffects_CC(int t, int cc_n){
  if (cc_n >= Timer_n_CCs[t]) {
    bs_trace_error_line_time("%s: Attempted to access non existing register "
                             "TIMER%i.CC[%i] (>= %i)\n",
                              __func__, t, cc_n, Timer_n_CCs[t]);
  }

  if ( (Timer_running[t] == true) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
    update_cc_timer(t, cc_n);
    update_master_timer();
  }
}

void nrf_hw_model_timer_timer_triggered(void) {
  unsigned int t, cc;
  struct {
    unsigned int t[N_TIMERS*N_MAX_CC];
    unsigned int cc[N_TIMERS*N_MAX_CC];
    unsigned int cnt;
  } match;

  match.cnt = 0;

  for (t = 0 ; t < N_TIMERS ; t++) {
    if ( !(( Timer_running[t] == true ) && ( NRF_TIMER_regs[t].MODE == 0 )) ) {
      continue;
    }
    for (cc = 0 ; cc < Timer_n_CCs[t] ; cc++) {
      if ( CC_timers[t][cc] == Timer_TIMERs ){
        match.t[match.cnt] = t;
        match.cc[match.cnt] = cc;
        match.cnt++;
      }
    }
  }
  while (match.cnt > 0) {
    match.cnt--;
    t = match.t[match.cnt];
    cc = match.cc[match.cnt];
    update_cc_timer(t,cc); //Next time it will match
    nrf_timer_signal_COMPARE(t,cc);
  }
  update_master_timer();
}

void nrf_timer0_TASK_START() { nrf_timer_TASK_START(0); }
void nrf_timer1_TASK_START() { nrf_timer_TASK_START(1); }
void nrf_timer2_TASK_START() { nrf_timer_TASK_START(2); }
void nrf_timer3_TASK_START() { nrf_timer_TASK_START(3); }
void nrf_timer4_TASK_START() { nrf_timer_TASK_START(4); }

void nrf_timer0_TASK_STOP(void) { nrf_timer_TASK_STOP(0); }
void nrf_timer1_TASK_STOP(void) { nrf_timer_TASK_STOP(1); }
void nrf_timer2_TASK_STOP(void) { nrf_timer_TASK_STOP(2); }
void nrf_timer3_TASK_STOP(void) { nrf_timer_TASK_STOP(3); }
void nrf_timer4_TASK_STOP(void) { nrf_timer_TASK_STOP(4); }

void nrf_timer0_TASK_CAPTURE_0(void) { nrf_timer_TASK_CAPTURE(0,0); }
void nrf_timer0_TASK_CAPTURE_1(void) { nrf_timer_TASK_CAPTURE(0,1); }
void nrf_timer0_TASK_CAPTURE_2(void) { nrf_timer_TASK_CAPTURE(0,2); }
void nrf_timer0_TASK_CAPTURE_3(void) { nrf_timer_TASK_CAPTURE(0,3); }

void nrf_timer1_TASK_CAPTURE_0(void) { nrf_timer_TASK_CAPTURE(1,0); }
void nrf_timer1_TASK_CAPTURE_1(void) { nrf_timer_TASK_CAPTURE(1,1); }
void nrf_timer1_TASK_CAPTURE_2(void) { nrf_timer_TASK_CAPTURE(1,2); }
void nrf_timer1_TASK_CAPTURE_3(void) { nrf_timer_TASK_CAPTURE(1,3); }

void nrf_timer2_TASK_CAPTURE_0(void) { nrf_timer_TASK_CAPTURE(2,0); }
void nrf_timer2_TASK_CAPTURE_1(void) { nrf_timer_TASK_CAPTURE(2,1); }
void nrf_timer2_TASK_CAPTURE_2(void) { nrf_timer_TASK_CAPTURE(2,2); }
void nrf_timer2_TASK_CAPTURE_3(void) { nrf_timer_TASK_CAPTURE(2,3); }

void nrf_timer3_TASK_CAPTURE_0(void) { nrf_timer_TASK_CAPTURE(3,0); }
void nrf_timer3_TASK_CAPTURE_1(void) { nrf_timer_TASK_CAPTURE(3,1); }
void nrf_timer3_TASK_CAPTURE_2(void) { nrf_timer_TASK_CAPTURE(3,2); }
void nrf_timer3_TASK_CAPTURE_3(void) { nrf_timer_TASK_CAPTURE(3,3); }
void nrf_timer3_TASK_CAPTURE_4(void) { nrf_timer_TASK_CAPTURE(3,4); }
void nrf_timer3_TASK_CAPTURE_5(void) { nrf_timer_TASK_CAPTURE(3,5); }

void nrf_timer4_TASK_CAPTURE_0(void) { nrf_timer_TASK_CAPTURE(4,0); }
void nrf_timer4_TASK_CAPTURE_1(void) { nrf_timer_TASK_CAPTURE(4,1); }
void nrf_timer4_TASK_CAPTURE_2(void) { nrf_timer_TASK_CAPTURE(4,2); }
void nrf_timer4_TASK_CAPTURE_3(void) { nrf_timer_TASK_CAPTURE(4,3); }
void nrf_timer4_TASK_CAPTURE_4(void) { nrf_timer_TASK_CAPTURE(4,4); }
void nrf_timer4_TASK_CAPTURE_5(void) { nrf_timer_TASK_CAPTURE(4,5); }

void nrf_timer0_TASK_CLEAR(void) { nrf_timer_TASK_CLEAR(0); }
void nrf_timer1_TASK_CLEAR(void) { nrf_timer_TASK_CLEAR(1); }
void nrf_timer2_TASK_CLEAR(void) { nrf_timer_TASK_CLEAR(2); }
void nrf_timer3_TASK_CLEAR(void) { nrf_timer_TASK_CLEAR(3); }
void nrf_timer4_TASK_CLEAR(void) { nrf_timer_TASK_CLEAR(4); }

void nrf_timer0_TASK_COUNT(void) { nrf_timer_TASK_COUNT(0); }
void nrf_timer1_TASK_COUNT(void) { nrf_timer_TASK_COUNT(1); }
void nrf_timer2_TASK_COUNT(void) { nrf_timer_TASK_COUNT(2); }
void nrf_timer3_TASK_COUNT(void) { nrf_timer_TASK_COUNT(3); }
void nrf_timer4_TASK_COUNT(void) { nrf_timer_TASK_COUNT(4); }
