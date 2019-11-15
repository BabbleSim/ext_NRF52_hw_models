/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include "time_machine_if.h"
#include "NRF_TIMER.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "bs_tracing.h"

/*
 * TIMER — Timer/counter
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/timer.html?cp=2_0_0_22#concept_xbd_hqp_sr
 */

/**
 * Notes:
 *   Counter mode is not fully supported (no CC match check)
 *   For simplicity all 5 Timers have 6 functional CC registers
 */

#define N_TIMERS 5
#define N_CC 6
NRF_TIMER_Type NRF_TIMER_regs[N_TIMERS];

static uint32_t TIMER_INTEN[N_TIMERS] = {0};
static bool Timer_running[N_TIMERS] = {false};

static bs_time_t Timer_counter_startT[N_TIMERS] = {TIME_NEVER}; //Time when the counter was started
static uint32_t Counter[N_TIMERS] = {0}; //Used in count mode, and in Timer mode during stops

bs_time_t Timer_TIMERs = TIME_NEVER;
static bs_time_t CC_timers[N_TIMERS][N_CC] = {{TIME_NEVER}};

/**
 * Initialize the TIMER model
 */
void nrf_timer_init(){
  memset(NRF_TIMER_regs, 0, sizeof(NRF_TIMER_regs));
  for (int t = 0; t < N_TIMERS ; t++ ){
    TIMER_INTEN[t] = 0;
    Timer_running[t] = false;
    Timer_counter_startT[t] = TIME_NEVER;
    Counter[t] = 0;
    for ( int cc = 0 ; cc < N_CC ; cc++){
      CC_timers[t][cc] = TIME_NEVER;
    }
  }
  Timer_TIMERs = TIME_NEVER;
}

/**
 * Clean up the TIMER model before program exit
 */
void nrf_timer_clean_up(){

}

/**
 * Convert a time delta in us to the equivalent count accounting for the PRESCALER
 */
static uint32_t time_to_counter(bs_time_t delta, int t){
  uint64_t ticks;
  ticks = ( delta << 4 ) >> NRF_TIMER_regs[t].PRESCALER;
  return ticks;
}

/**
 * Convert a counter delta to us accounting for the PRESCALER
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
static void update_master_timer(){
  Timer_TIMERs = TIME_NEVER;
  for ( int t = 0 ; t < N_TIMERS ; t++){
    if ( ( Timer_running[t] == true ) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
      for ( int cc = 0 ; cc < N_CC ; cc++){
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
  for (int cc = 0 ; cc < N_CC; cc++){
    update_cc_timer(t,cc);
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

void nrf_timer0_TASK_START() { nrf_timer_TASK_START(0); }
void nrf_timer1_TASK_START() { nrf_timer_TASK_START(1); }
void nrf_timer2_TASK_START() { nrf_timer_TASK_START(2); }
void nrf_timer3_TASK_START() { nrf_timer_TASK_START(3); }
void nrf_timer4_TASK_START() { nrf_timer_TASK_START(4); }

void nrf_timer_TASK_STOP(int t){
  //Note: STATUS is missing in NRF_TIMER_Type
  if (Timer_running[t] == true) {
    Timer_running[t] = false;
    Counter[t] = time_to_counter(tm_get_hw_time() - Timer_counter_startT[t], t); //we save the value when the counter was stoped in case it is started again without clearing it
    for (int cc = 0 ; cc < N_CC ; cc++){
      CC_timers[t][cc] = TIME_NEVER;
    }
    update_master_timer();
  }
}

void nrf_timer0_TASK_STOP() { nrf_timer_TASK_STOP(0); }
void nrf_timer1_TASK_STOP() { nrf_timer_TASK_STOP(1); }
void nrf_timer2_TASK_STOP() { nrf_timer_TASK_STOP(2); }
void nrf_timer3_TASK_STOP() { nrf_timer_TASK_STOP(3); }
void nrf_timer4_TASK_STOP() { nrf_timer_TASK_STOP(4); }

void nrf_timer_TASK_CAPTURE(int t, int cc_n){
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

void nrf_timer0_TASK_CAPTURE_0() { nrf_timer_TASK_CAPTURE(0,0); }
void nrf_timer0_TASK_CAPTURE_1() { nrf_timer_TASK_CAPTURE(0,1); }
void nrf_timer0_TASK_CAPTURE_2() { nrf_timer_TASK_CAPTURE(0,2); }
void nrf_timer0_TASK_CAPTURE_3() { nrf_timer_TASK_CAPTURE(0,3); }
void nrf_timer0_TASK_CAPTURE_4() { nrf_timer_TASK_CAPTURE(0,4); }
void nrf_timer0_TASK_CAPTURE_5() { nrf_timer_TASK_CAPTURE(0,5); }

void nrf_timer1_TASK_CAPTURE_0() { nrf_timer_TASK_CAPTURE(1,0); }
void nrf_timer1_TASK_CAPTURE_1() { nrf_timer_TASK_CAPTURE(1,1); }
void nrf_timer1_TASK_CAPTURE_2() { nrf_timer_TASK_CAPTURE(1,2); }
void nrf_timer1_TASK_CAPTURE_3() { nrf_timer_TASK_CAPTURE(1,3); }
void nrf_timer1_TASK_CAPTURE_4() { nrf_timer_TASK_CAPTURE(1,4); }
void nrf_timer1_TASK_CAPTURE_5() { nrf_timer_TASK_CAPTURE(1,5); }

void nrf_timer2_TASK_CAPTURE_0() { nrf_timer_TASK_CAPTURE(2,0); }
void nrf_timer2_TASK_CAPTURE_1() { nrf_timer_TASK_CAPTURE(2,1); }
void nrf_timer2_TASK_CAPTURE_2() { nrf_timer_TASK_CAPTURE(2,2); }
void nrf_timer2_TASK_CAPTURE_3() { nrf_timer_TASK_CAPTURE(2,3); }
void nrf_timer2_TASK_CAPTURE_4() { nrf_timer_TASK_CAPTURE(2,4); }
void nrf_timer2_TASK_CAPTURE_5() { nrf_timer_TASK_CAPTURE(2,5); }

void nrf_timer3_TASK_CAPTURE_0() { nrf_timer_TASK_CAPTURE(3,0); }
void nrf_timer3_TASK_CAPTURE_1() { nrf_timer_TASK_CAPTURE(3,1); }
void nrf_timer3_TASK_CAPTURE_2() { nrf_timer_TASK_CAPTURE(3,2); }
void nrf_timer3_TASK_CAPTURE_3() { nrf_timer_TASK_CAPTURE(3,3); }
void nrf_timer3_TASK_CAPTURE_4() { nrf_timer_TASK_CAPTURE(3,4); }
void nrf_timer3_TASK_CAPTURE_5() { nrf_timer_TASK_CAPTURE(3,5); }

void nrf_timer4_TASK_CAPTURE_0() { nrf_timer_TASK_CAPTURE(4,0); }
void nrf_timer4_TASK_CAPTURE_1() { nrf_timer_TASK_CAPTURE(4,1); }
void nrf_timer4_TASK_CAPTURE_2() { nrf_timer_TASK_CAPTURE(4,2); }
void nrf_timer4_TASK_CAPTURE_3() { nrf_timer_TASK_CAPTURE(4,3); }
void nrf_timer4_TASK_CAPTURE_4() { nrf_timer_TASK_CAPTURE(4,4); }
void nrf_timer4_TASK_CAPTURE_5() { nrf_timer_TASK_CAPTURE(4,5); }

void nrf_timer_TASK_CLEAR(int t) {
  Counter[t] = 0;
  if (NRF_TIMER_regs[t].MODE == 0) {
    //Timer mode:
    Timer_counter_startT[t] = tm_get_hw_time();
    update_all_cc_timers(t);
    update_master_timer();
  }
}

void nrf_timer0_TASK_CLEAR() { nrf_timer_TASK_CLEAR(0); }
void nrf_timer1_TASK_CLEAR() { nrf_timer_TASK_CLEAR(1); }
void nrf_timer2_TASK_CLEAR() { nrf_timer_TASK_CLEAR(2); }
void nrf_timer3_TASK_CLEAR() { nrf_timer_TASK_CLEAR(3); }
void nrf_timer4_TASK_CLEAR() { nrf_timer_TASK_CLEAR(4); }

void nrf_timer_TASK_COUNT(int t){
  if ( ( NRF_TIMER_regs[t].MODE != 0 ) && ( Timer_running[t] == true ) ){ //Count mode
    Counter[t]++;
    //TODO: check for possible compare match
  } //Otherwise ignored
}

void nrf_timer0_TASK_COUNT() { nrf_timer_TASK_COUNT(0); }
void nrf_timer1_TASK_COUNT() { nrf_timer_TASK_COUNT(1); }
void nrf_timer2_TASK_COUNT() { nrf_timer_TASK_COUNT(2); }
void nrf_timer3_TASK_COUNT() { nrf_timer_TASK_COUNT(3); }
void nrf_timer4_TASK_COUNT() { nrf_timer_TASK_COUNT(4); }

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

void nrf_timer_regw_sideeffects_INTENSET(int t){
  if ( NRF_TIMER_regs[t].INTENSET ){
    TIMER_INTEN[t] |= NRF_TIMER_regs[t].INTENSET;
    NRF_TIMER_regs[t].INTENSET = TIMER_INTEN[t];
  }
}

void nrf_timer_regw_sideeffects_INTENCLR(int t){
  if ( NRF_TIMER_regs[t].INTENCLR ){
    TIMER_INTEN[t]  &= ~NRF_TIMER_regs[t].INTENCLR;
    NRF_TIMER_regs[t].INTENSET = TIMER_INTEN[t];
    NRF_TIMER_regs[t].INTENCLR = 0;
  }
}

void nrf_timer_regw_sideeffects_CC(int t, int cc_n){
  if ( (Timer_running[t] == true) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
    update_cc_timer(t, cc_n);
    update_master_timer();
  }
}

void nrf_timer_timer_triggered() {
  for ( int t = 0 ; t < N_TIMERS ; t++) {
    if ( !(( Timer_running[t] == true ) && ( NRF_TIMER_regs[t].MODE == 0 )) ) {
      continue;
    }
    for ( int cc = 0 ; cc < N_CC ; cc++) {
      if ( CC_timers[t][cc] != Timer_TIMERs ){ //This CC is not matching now
        continue;
      }

      update_cc_timer(t,cc); //Next time it will match

      if ( NRF_TIMER_regs[t].SHORTS & (TIMER_SHORTS_COMPARE0_CLEAR_Msk << cc) ) {
        nrf_timer_TASK_CLEAR(t);
      }
      if ( NRF_TIMER_regs[t].SHORTS & (TIMER_SHORTS_COMPARE0_STOP_Msk << cc) ) {
        nrf_timer_TASK_STOP(t);
      }

      NRF_TIMER_regs[t].EVENTS_COMPARE[cc] = 1;

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
      default: /* Just to silence a -Werror=maybe-uninitialized warning */
        event_cc = TIMER4_EVENTS_COMPARE_0;
        break;
      }

      nrf_ppi_event(event_cc + cc);

      if ( TIMER_INTEN[t] & (TIMER_INTENSET_COMPARE0_Msk << cc) ){
        int irq = NRF5_IRQ_TIMER0_IRQn;
        switch (t){
        case 0:
          irq = NRF5_IRQ_TIMER0_IRQn;
          break;
        case 1:
          irq = NRF5_IRQ_TIMER1_IRQn;
          break;
        case 2:
          irq = NRF5_IRQ_TIMER2_IRQn;
          break;
        default:
          bs_trace_error_line_time("NRF HW TIMER%i CC[%i] interrupt "
              "trigered but there is no interrupt "
              "mapped for it\n", t, cc);
          break;
        }
        hw_irq_ctrl_set_irq(irq);
      } //if interrupt enabled

    } //for cc
  }//for t(imer)
  update_master_timer();
}

