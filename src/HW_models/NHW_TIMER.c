/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * TIMER - Timer/counter
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/timer.html?cp=5_1_0_5_27
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/timer.html?cp=4_0_0_6_34
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
#include <stdint.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_TIMER.h"
#include "nsi_hw_scheduler.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

#define N_TIMERS NHW_TIMER_TOTAL_INST
#define N_MAX_CC NHW_TIMER_MAX_N_CC

struct timer_status {
  NRF_TIMER_Type *NRF_TIMER_regs;

  int n_CCs;  //Number of compare/capture registers in this timer instance
  int base_freq; //Base frequency (in MHz) of the timer input clock

  bs_time_t *CC_timers; //[n_CCs] In timer mode: When each compare match is expected to happen
  bool *oneshot_flag; //[n_CCs] The CC register has been written, and a compare event has not yet been generated
  bs_time_t start_t; //Time when the timer was started (only for timer mode)
  uint32_t Counter; //Internal count value. Used in count mode, and in Timer mode during stops.
  uint32_t INTEN;
  bool is_running;

#if (NHW_HAS_DPPI)
  uint dppi_map;   //To which DPPI instance are this TIMER subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem* subscribed_CAPTURE;   //[n_CCs]
  struct nhw_subsc_mem subscribed_START;
  struct nhw_subsc_mem subscribed_STOP;
  struct nhw_subsc_mem subscribed_COUNT;
  struct nhw_subsc_mem subscribed_CLEAR;
  struct nhw_subsc_mem subscribed_SHUTDOWN;
#endif
};

static bs_time_t Timer_TIMERs = TIME_NEVER;
static struct timer_status nhw_timer_st[NHW_TIMER_TOTAL_INST];
NRF_TIMER_Type NRF_TIMER_regs[NHW_TIMER_TOTAL_INST];

/**
 * Initialize the TIMER model
 */
static void nhw_timer_init(void) {
#if (NHW_HAS_DPPI)
  /* Mapping of peripheral instance to DPPI instance */
  uint nhw_timer_dppi_map[NHW_TIMER_TOTAL_INST] = NHW_TIMER_DPPI_MAP;
#endif
  int Timer_n_CCs[N_TIMERS] = NHW_TIMER_N_CC;
  int Timer_freqs[N_TIMERS] = NHW_TIMER_FREQ;

  memset(NRF_TIMER_regs, 0, sizeof(NRF_TIMER_regs));

  for (int t = 0; t < NHW_TIMER_TOTAL_INST ; t++) {
    struct timer_status *t_st = &nhw_timer_st[t];

    t_st->NRF_TIMER_regs = &NRF_TIMER_regs[t];

    t_st->INTEN = 0;
    t_st->is_running = false;
    t_st->start_t = TIME_NEVER;
    t_st->Counter = 0;

    t_st->base_freq = Timer_freqs[t];
    t_st->n_CCs = Timer_n_CCs[t];
    t_st->CC_timers = (bs_time_t *)bs_malloc(sizeof(bs_time_t)*Timer_n_CCs[t]);
    t_st->oneshot_flag = (bool *)bs_calloc(Timer_n_CCs[t], sizeof(bool));

    for (int cc = 0; cc < t_st->n_CCs ; cc++) {
      t_st->CC_timers[cc] = TIME_NEVER;
    }

#if (NHW_HAS_DPPI)
    t_st->dppi_map = nhw_timer_dppi_map[t];
    t_st->subscribed_CAPTURE = (struct nhw_subsc_mem*)bs_calloc(Timer_n_CCs[t], sizeof(struct nhw_subsc_mem));
#endif
  }
  Timer_TIMERs = TIME_NEVER;
}

NSI_TASK(nhw_timer_init, HW_INIT, 100);

/*
 * Free all TIMER instances resources before program exit
 */
static void nhw_timer_free(void)
{
  for (int t = 0; t< NHW_TIMER_TOTAL_INST; t++) {
    struct timer_status *t_st = &nhw_timer_st[t];

    free(t_st->CC_timers);
    t_st->CC_timers = NULL;

    free(t_st->oneshot_flag);
    t_st->oneshot_flag = NULL;

#if (NHW_HAS_DPPI)
    free(t_st->subscribed_CAPTURE);
    t_st->subscribed_CAPTURE = NULL;
#endif /* (NHW_HAS_DPPI) */
  }
}

NSI_TASK(nhw_timer_free, ON_EXIT_PRE, 100);

/**
 * Convert a time delta in us to the equivalent count accounting for the PRESCALER
 * and the timer clock frequency
 */
static uint32_t time_to_counter(bs_time_t delta, int t){
  uint64_t ticks;
  ticks = (delta * nhw_timer_st[t].base_freq) >> NRF_TIMER_regs[t].PRESCALER;
  return ticks;
}

/**
 * Convert a counter delta to us accounting for the PRESCALER
 * and the timer clock frequency
 */
static bs_time_t counter_to_time(uint64_t counter, int t){
  bs_time_t Elapsed;
  Elapsed = (counter << NRF_TIMER_regs[t].PRESCALER) / nhw_timer_st[t].base_freq;
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
  return counter_to_time((uint64_t)mask_from_bitmode(t) + 1, t);
}

/**
 * Find the CC register timer (CC_timers[][]) which will trigger earliest (if any)
 */
static void update_master_timer(void) {
  Timer_TIMERs = TIME_NEVER;

  for ( int t = 0 ; t < N_TIMERS ; t++){
    struct timer_status *t_st = &nhw_timer_st[t];

    if ((t_st->is_running == true) && (NRF_TIMER_regs[t].MODE == 0)) {
      for (int cc = 0 ; cc < t_st->n_CCs ; cc++) {
        if (t_st->CC_timers[cc] < Timer_TIMERs) {
          Timer_TIMERs = t_st->CC_timers[cc];
        }
      }
    }
  }
  nsi_hws_find_next_event();
}

/**
 * Save in CC_timers[t][cc] the next time when this timer will match the CC[cc]
 * register
 */
static void update_cc_timer(int t, int cc) {
  struct timer_status *this = &nhw_timer_st[t];

  if ((this->is_running == true) && (NRF_TIMER_regs[t].MODE == 0)) {
    bs_time_t next_match = this->start_t
                           + counter_to_time(NRF_TIMER_regs[t].CC[cc], t);
    while (next_match <= nsi_hws_get_time()) {
      next_match += time_of_1_counter_wrap(t);
    }
    this->CC_timers[cc] = next_match;
  } else {
    this->CC_timers[cc] = TIME_NEVER;
  }
}

static void update_all_cc_timers(int t) {
  for (int cc = 0 ; cc < nhw_timer_st[t].n_CCs; cc++) {
    update_cc_timer(t, cc);
  }
}

static void nhw_timer_eval_interrupts(int t) {
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_timer_irq_map[NHW_TIMER_TOTAL_INST] = NHW_TIMER_INT_MAP;
  static bool TIMER_int_line[N_TIMERS]; /* Is the TIMER currently driving its interrupt line high */

  struct timer_status *this = &nhw_timer_st[t];
  bool new_int_line = false;

  for (int cc = 0; cc < this->n_CCs; cc++) {
    int mask = this->INTEN & (TIMER_INTENSET_COMPARE0_Msk << cc);
    if (NRF_TIMER_regs[t].EVENTS_COMPARE[cc] && mask) {
      new_int_line = true;
      break; /* No need to check more */
    }
  }

  if (TIMER_int_line[t] == false && new_int_line == true) {
    TIMER_int_line[t] = true;
    hw_irq_ctrl_raise_level_irq_line(nhw_timer_irq_map[t].cntl_inst,
                                     nhw_timer_irq_map[t].int_nbr);
  } else if (TIMER_int_line[t] == true && new_int_line == false) {
    TIMER_int_line[t] = false;

    hw_irq_ctrl_lower_level_irq_line(nhw_timer_irq_map[t].cntl_inst,
                                     nhw_timer_irq_map[t].int_nbr);
  }
}

void nhw_timer_TASK_START(int t){
  struct timer_status *this = &nhw_timer_st[t];

  //Note: STATUS is missing in NRF_TIMER_Type
  if (this->is_running == false) {
    this->is_running = true;
    if (NRF_TIMER_regs[t].MODE == 0) { //Timer mode
      this->start_t = nsi_hws_get_time() - counter_to_time(this->Counter, t); //If the counter is not zero at start, is like if the counter was started earlier
      update_all_cc_timers(t);
      update_master_timer();
    }
  }
}

void nhw_timer_TASK_STOP(int t) {
  struct timer_status *this = &nhw_timer_st[t];

  //Note: STATUS is missing in NRF_TIMER_Type
  if (this->is_running == true) {
    this->is_running = false;
    if (NRF_TIMER_regs[t].MODE == 0) { //Timer mode
      this->Counter = time_to_counter(nsi_hws_get_time() - this->start_t, t); //we save the value when the counter was stoped in case it is started again without clearing it
    }
    for (int cc = 0 ; cc < this->n_CCs ; cc++) {
      this->CC_timers[cc] = TIME_NEVER;
    }
    update_master_timer();
  }
}

void nhw_timer_TASK_SHUTDOWN(int t) {
  struct timer_status *this = &nhw_timer_st[t];

  /*
   * TASK_SHUTDOWN is not documented in newer SOCs
   * In older ones it indicates it STOPS + it reduces power consumption
   * but that it cannot be resumed after from where it was
   * The assumption is that the internal count is lost/reset to 0
   * Effectively making a SHUTDOWN logically equivalent to a STOP + CLEAR
   */
  //Note: STATUS is missing in NRF_TIMER_Type
  this->is_running = false;
  this->Counter = 0;
  this->start_t = TIME_NEVER;
  for (int cc = 0 ; cc < this->n_CCs ; cc++){
    this->CC_timers[cc] = TIME_NEVER;
  }
  update_master_timer();
}

void nhw_timer_TASK_CAPTURE(int t, int cc_n) {
  struct timer_status *this = &nhw_timer_st[t];

  if (cc_n >= this->n_CCs) {
    bs_trace_error_line_time("%s: Attempted to access non existing task"
                             "TIMER%i.TASK_CAPTURE[%i] (>= %i)\n",
                              t, cc_n, this->n_CCs);
  }
  if ((NRF_TIMER_regs[t].MODE != 0 /* Count mode */) || (this->is_running == false)) {
    NRF_TIMER_regs[t].CC[cc_n] = this->Counter & mask_from_bitmode(t);
  } else { //Timer mode (and running):
    bs_time_t Elapsed = nsi_hws_get_time() - this->start_t;
    NRF_TIMER_regs[t].CC[cc_n] = time_to_counter(Elapsed,t) & mask_from_bitmode(t);

    //The new CC causes a new possible CC match time:
    update_cc_timer(t, cc_n);
    update_master_timer();
  }
}

void nhw_timer_TASK_CLEAR(uint t) {
  struct timer_status *this = &nhw_timer_st[t];

  this->Counter = 0;
  if (NRF_TIMER_regs[t].MODE == 0) {
    //Timer mode:
    this->start_t = nsi_hws_get_time();
    update_all_cc_timers(t);
    update_master_timer();
  }
}

static void nhw_timer_signal_COMPARE(uint t, uint cc) {
  struct timer_status *this = &nhw_timer_st[t];
  NRF_TIMER_Type *TIMER_regs = this->NRF_TIMER_regs;

  if (TIMER_regs->SHORTS & (TIMER_SHORTS_COMPARE0_CLEAR_Msk << cc)) {
    nhw_timer_TASK_CLEAR(t);
  }
  if (TIMER_regs->SHORTS & (TIMER_SHORTS_COMPARE0_STOP_Msk << cc)) {
    nhw_timer_TASK_STOP(t);
  }

  TIMER_regs->EVENTS_COMPARE[cc] = 1;

  nhw_timer_eval_interrupts(t);

#if (NHW_HAS_PPI)
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
#elif (NHW_HAS_DPPI)
  nhw_dppi_event_signal_if(this->dppi_map,
                           TIMER_regs->PUBLISH_COMPARE[cc]);
#endif
}

static void nhw_timer_signal_COMPARE_if(uint t, uint cc_n) {
  struct timer_status *this = &nhw_timer_st[t];
#if (NHW_TIMER_HAS_ONE_SHOT)
  if ((this->oneshot_flag[cc_n] == false) && this->NRF_TIMER_regs->ONESHOTEN[cc_n]) {
    return;
  }
#endif
  this->oneshot_flag[cc_n] = false;
  nhw_timer_signal_COMPARE(t, cc_n);
}

void nhw_timer_TASK_COUNT(uint t) {
  struct timer_status *this = &nhw_timer_st[t];

  if ((NRF_TIMER_regs[t].MODE != 0 /* Count mode */) && (this->is_running == true)) {
    this->Counter = (this->Counter + 1) & mask_from_bitmode(t);

    for (int cc_n = 0; cc_n < this->n_CCs; cc_n++) {
      if (this->Counter == (NRF_TIMER_regs[t].CC[cc_n] & mask_from_bitmode(t))){
        nhw_timer_signal_COMPARE_if(t, cc_n);
      }
    }
  } //Otherwise ignored
}

void nhw_timer_regw_sideeffects_TASKS_START(uint t) {
  if ( NRF_TIMER_regs[t].TASKS_START ){
    NRF_TIMER_regs[t].TASKS_START = 0;
    nhw_timer_TASK_START(t);
  }
}

void nhw_timer_regw_sideeffects_TASKS_STOP(uint t) {
  if (NRF_TIMER_regs[t].TASKS_STOP) {
    NRF_TIMER_regs[t].TASKS_STOP = 0;
    nhw_timer_TASK_STOP(t);
  }
}

void nhw_timer_regw_sideeffects_TASKS_SHUTDOWN(uint t) {
  if (NRF_TIMER_regs[t].TASKS_SHUTDOWN) {
    NRF_TIMER_regs[t].TASKS_SHUTDOWN = 0;
    nhw_timer_TASK_SHUTDOWN(t);
  }
}

void nhw_timer_regw_sideeffects_TASKS_CAPTURE(uint t, uint cc_n){
  if ( NRF_TIMER_regs[t].TASKS_CAPTURE[cc_n] ){
    NRF_TIMER_regs[t].TASKS_CAPTURE[cc_n] = 0;
    nhw_timer_TASK_CAPTURE(t,cc_n);
  }
}

void nhw_timer_regw_sideeffects_TASKS_CLEAR(uint t) {
  if (NRF_TIMER_regs[t].TASKS_CLEAR) {
    NRF_TIMER_regs[t].TASKS_CLEAR = 0;
    nhw_timer_TASK_CLEAR(t);
  }
}

void nhw_timer_regw_sideeffects_TASKS_COUNT(uint t) {
  if ( NRF_TIMER_regs[t].TASKS_COUNT ){
    NRF_TIMER_regs[t].TASKS_COUNT = 0;
    nhw_timer_TASK_COUNT(t);
  }
}

#if (NHW_HAS_DPPI)

static void nhw_timer_taskcapture_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint cc_n = (uintptr_t)param & 0xFFFF;
  nhw_timer_TASK_CAPTURE(inst, cc_n);
}

void nhw_timer_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint cc_n) {
  struct timer_status *this = &nhw_timer_st[inst];

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_TIMER_regs->SUBSCRIBE_CAPTURE[cc_n],
                                       &this->subscribed_CAPTURE[cc_n],
                                       nhw_timer_taskcapture_wrap,
                                       (void*)((inst << 16) + cc_n));
}

#define NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                 \
  static void nhw_timer_task##TASK_N##_wrap(void* param)                            \
  {                                                                                 \
    nhw_timer_TASK_##TASK_N((int) param);                                           \
  }                                                                                 \
                                                                                    \
  void nhw_timer_regw_sideeffects_SUBSCRIBE_##TASK_N(uint inst)                     \
  {                                                                                 \
     struct timer_status *this = &nhw_timer_st[inst];                               \
                                                                                    \
     nhw_dppi_common_subscribe_sideeffect(this->dppi_map,                           \
                                          this->NRF_TIMER_regs->SUBSCRIBE_##TASK_N, \
                                          &this->subscribed_##TASK_N,               \
                                          nhw_timer_task##TASK_N##_wrap,            \
                                          (void*) inst);                            \
  }

NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(START)
NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(STOP)
NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(COUNT)
NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(CLEAR)
NHW_TIMER_REGW_SIDEFFECTS_SUBSCRIBE(SHUTDOWN)

#endif /* NHW_HAS_DPPI */


void nhw_timer_regw_sideeffects_EVENTS_all(uint t) {
  nhw_timer_eval_interrupts(t);
}

void nhw_timer_regw_sideeffects_INTENSET(uint t) {
  struct timer_status *this = &nhw_timer_st[t];

  if ( NRF_TIMER_regs[t].INTENSET ){
    this->INTEN |= NRF_TIMER_regs[t].INTENSET;
    NRF_TIMER_regs[t].INTENSET = this->INTEN;
    nhw_timer_eval_interrupts(t);
  }
}

void nhw_timer_regw_sideeffects_INTENCLR(uint t) {
  struct timer_status *this = &nhw_timer_st[t];

  if ( NRF_TIMER_regs[t].INTENCLR ){
    this->INTEN  &= ~NRF_TIMER_regs[t].INTENCLR;
    NRF_TIMER_regs[t].INTENSET = this->INTEN;
    NRF_TIMER_regs[t].INTENCLR = 0;
    nhw_timer_eval_interrupts(t);
  }
}

void nhw_timer_regw_sideeffects_CC(uint t, uint cc_n) {
  struct timer_status *this = &nhw_timer_st[t];

  if (cc_n >= this->n_CCs) {
    bs_trace_error_line_time("%s: Attempted to access non existing register "
                             "TIMER%i.CC[%i] (>= %i)\n",
                              __func__, t, cc_n, this->n_CCs);
  }

  this->oneshot_flag[cc_n] = true;

  if ( (this->is_running == true) && ( NRF_TIMER_regs[t].MODE == 0 ) ) {
    update_cc_timer(t, cc_n);
    update_master_timer();
  }
}

static void nhw_hw_model_timer_timer_triggered(void) {
  unsigned int t, cc;
  struct {
    unsigned int t[N_TIMERS*N_MAX_CC];
    unsigned int cc[N_TIMERS*N_MAX_CC];
    unsigned int cnt;
  } match;

  match.cnt = 0;

  for (t = 0 ; t < N_TIMERS ; t++) {
    struct timer_status *t_st = &nhw_timer_st[t];

    if ( !((t_st->is_running == true) && (NRF_TIMER_regs[t].MODE == 0)) ) {
      continue;
    }
    for (cc = 0 ; cc < t_st->n_CCs ; cc++) {
      if (t_st->CC_timers[cc] == Timer_TIMERs) {
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
    nhw_timer_signal_COMPARE_if(t,cc);
  }
  update_master_timer();
}

NSI_HW_EVENT(Timer_TIMERs, nhw_hw_model_timer_timer_triggered, 50);

#if (NHW_HAS_PPI)
void nhw_timer0_TASK_START(void) { nhw_timer_TASK_START(0); }
void nhw_timer1_TASK_START(void) { nhw_timer_TASK_START(1); }
void nhw_timer2_TASK_START(void) { nhw_timer_TASK_START(2); }
void nhw_timer3_TASK_START(void) { nhw_timer_TASK_START(3); }
void nhw_timer4_TASK_START(void) { nhw_timer_TASK_START(4); }

void nhw_timer0_TASK_STOP(void) { nhw_timer_TASK_STOP(0); }
void nhw_timer1_TASK_STOP(void) { nhw_timer_TASK_STOP(1); }
void nhw_timer2_TASK_STOP(void) { nhw_timer_TASK_STOP(2); }
void nhw_timer3_TASK_STOP(void) { nhw_timer_TASK_STOP(3); }
void nhw_timer4_TASK_STOP(void) { nhw_timer_TASK_STOP(4); }

void nhw_timer0_TASK_CAPTURE_0(void) { nhw_timer_TASK_CAPTURE(0,0); }
void nhw_timer0_TASK_CAPTURE_1(void) { nhw_timer_TASK_CAPTURE(0,1); }
void nhw_timer0_TASK_CAPTURE_2(void) { nhw_timer_TASK_CAPTURE(0,2); }
void nhw_timer0_TASK_CAPTURE_3(void) { nhw_timer_TASK_CAPTURE(0,3); }

void nhw_timer1_TASK_CAPTURE_0(void) { nhw_timer_TASK_CAPTURE(1,0); }
void nhw_timer1_TASK_CAPTURE_1(void) { nhw_timer_TASK_CAPTURE(1,1); }
void nhw_timer1_TASK_CAPTURE_2(void) { nhw_timer_TASK_CAPTURE(1,2); }
void nhw_timer1_TASK_CAPTURE_3(void) { nhw_timer_TASK_CAPTURE(1,3); }

void nhw_timer2_TASK_CAPTURE_0(void) { nhw_timer_TASK_CAPTURE(2,0); }
void nhw_timer2_TASK_CAPTURE_1(void) { nhw_timer_TASK_CAPTURE(2,1); }
void nhw_timer2_TASK_CAPTURE_2(void) { nhw_timer_TASK_CAPTURE(2,2); }
void nhw_timer2_TASK_CAPTURE_3(void) { nhw_timer_TASK_CAPTURE(2,3); }

void nhw_timer3_TASK_CAPTURE_0(void) { nhw_timer_TASK_CAPTURE(3,0); }
void nhw_timer3_TASK_CAPTURE_1(void) { nhw_timer_TASK_CAPTURE(3,1); }
void nhw_timer3_TASK_CAPTURE_2(void) { nhw_timer_TASK_CAPTURE(3,2); }
void nhw_timer3_TASK_CAPTURE_3(void) { nhw_timer_TASK_CAPTURE(3,3); }
void nhw_timer3_TASK_CAPTURE_4(void) { nhw_timer_TASK_CAPTURE(3,4); }
void nhw_timer3_TASK_CAPTURE_5(void) { nhw_timer_TASK_CAPTURE(3,5); }

void nhw_timer4_TASK_CAPTURE_0(void) { nhw_timer_TASK_CAPTURE(4,0); }
void nhw_timer4_TASK_CAPTURE_1(void) { nhw_timer_TASK_CAPTURE(4,1); }
void nhw_timer4_TASK_CAPTURE_2(void) { nhw_timer_TASK_CAPTURE(4,2); }
void nhw_timer4_TASK_CAPTURE_3(void) { nhw_timer_TASK_CAPTURE(4,3); }
void nhw_timer4_TASK_CAPTURE_4(void) { nhw_timer_TASK_CAPTURE(4,4); }
void nhw_timer4_TASK_CAPTURE_5(void) { nhw_timer_TASK_CAPTURE(4,5); }

void nhw_timer0_TASK_CLEAR(void) { nhw_timer_TASK_CLEAR(0); }
void nhw_timer1_TASK_CLEAR(void) { nhw_timer_TASK_CLEAR(1); }
void nhw_timer2_TASK_CLEAR(void) { nhw_timer_TASK_CLEAR(2); }
void nhw_timer3_TASK_CLEAR(void) { nhw_timer_TASK_CLEAR(3); }
void nhw_timer4_TASK_CLEAR(void) { nhw_timer_TASK_CLEAR(4); }

void nhw_timer0_TASK_COUNT(void) { nhw_timer_TASK_COUNT(0); }
void nhw_timer1_TASK_COUNT(void) { nhw_timer_TASK_COUNT(1); }
void nhw_timer2_TASK_COUNT(void) { nhw_timer_TASK_COUNT(2); }
void nhw_timer3_TASK_COUNT(void) { nhw_timer_TASK_COUNT(3); }
void nhw_timer4_TASK_COUNT(void) { nhw_timer_TASK_COUNT(4); }
#endif /* (NHW_HAS_PPI) */
