/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RTC - Real-time counter
 *
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/rtc.html?cp=5_1_0_5_19
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/rtc.html?cp=4_0_0_6_27
 */

/*
 * This file provides the implementation of the RTC peripherals,
 * and instantiates N of them, as described in the configuration (NHW_config.h)
 *
 * Notes:
 *
 *  * TICK events are NOT modeled
 *
 *  * The COUNTER register is only updated when read with the proper HAL function
 *
 *  * Unlike the real HW, there is no jitter or (variable) delay in the tasks/events,
 *    operations, or  synchronization of configuration:
 *      * Triggering a task (Thru the PPI or register write) starts the operation
 *        immediately (in HW it takes between 1/2 and 1+1/2 LFCLKs:
 *        "CLEAR and STOP and TRIGOVRFLW [..] will be delayed as long as it
 *        takes for the peripheral to clock a falling edge and a rising edge of the LFCLK.")
 *      * Events and interrupts are raised immediately once produced (in real HW
 *        they can be raised relatively at +-1/2 LFCLK or +-1/2 PCLK16M of each other)
 *        (In real HW this is due to the 32K-16M clock domain crossing synchronization)
 *      * A STOP task stops the counter immediately.
 *
 *  * As the CLEAR task does not have delay, a SHORT of COMPARE<n>_CLEAR will cause the
 *    CLEAR to be instantaneous.
 *    While in the real HW, as per the spec:
 *    "If the COMPARE[i]_CLEAR short is enabled, the COUNTER will be cleared
 *     *one LFClk after* the COMPARE event"
 *
 *  * Unlike in real HW reading the COUNTER register is instantaneous (in real HW
 *    it takes up to 6 PCLK16M cycles, during which the CPU is stalled.
 *
 *  * Writing to the PRESCALER register when the RTC is running is not prevented
 *     (unlike in real HW), but doing so can have unintended consequences.
 *     Though the PRESCALER is shadowed into an internal register on the tasks
 *     START, CLEAR, and TRIGOVRFLW as per the spec.
 *
 *  * Note, in nrf52 devices, the spec seems to confusingly state that the LFCLK
 *    clock must be ready before the RTC can be used, yet that a TRIGOVRFLW task
 *    will get the RTC to request the LFClock.
 *    But in real life (and the model) it seems no task will automatically
 *    request the clock.
 *    For nrf5340 devices, TRIGOVRFLW does not seem to request the CLOCK either,
 *    but START seems to as per the spec.
 *
 *  * Note this model does not yet automatically request the LFCLK (for a nRF5340)
 *    with task START
 *
 *  * This models assumes that once the LFCLK is started it is never stopped.
 *
 *  * Note that, just like for the real RTC, the event generation logic is slightly
 *    different than for other peripherals
 *
 * Implementation notes:
 *   In a naive (but very simple) implementation the RTC model could be called
 *   every LFCLK tick. But this would be really slow (as we'd need to call it 32768
 *   times per second).
 *   Instead this model checks ahead, when the next compare or overflow event would
 *   trigger and sets a timer for its callback.
 *   There is one common timer exposed to the HW scheduler, and a set of internal timers
 *   per RTC instance (one per CC register, and one for the counter overflow)
 *
 *   The RTC keeps track internally of the time with submicrosecond resolution
 *   (9 decimal bits) to have a exact representation of the LF clock ticks,
 *   and avoid accumulating rounding errors.
 *   With this current implementation, the maximum runtime of the RTC is limited to
 *   2**64/2**9 microseconds from boot (~1142 years)
 *
 *
 * Pending to implement:
 *  * TICK events
 *
 *  * Delay of TASKs CLEAR, STOP and TRIGOVRFLW
 *
 *  * For nrf5340: With task START, the RTC should automatically request the LFCLK source with RC oscillator if the LFCLK is not already running.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_xPPI.h"
#include "NHW_CLOCK.h"
#include "irq_ctrl.h"
#include "NHW_RTC.h"

#define RTC_COUNTER_MASK 0xFFFFFF /*24 bits*/
#define RTC_TRIGGER_OVERFLOW_COUNTER_VALUE 0xFFFFF0

#define SUB_US_BITS 9 // Bits representing sub-microsecond units

#define LF_CLOCK_PERIOD_subus  15625 /*in a fixed point format with 9 bits per us, the LF clock period*/
#define LF_CLOCK_PERIOD_us     31 /* the LF clock period in us ceil(1e6/32768) = ceil(30.517578125us) */

struct rtc_status {
  NRF_RTC_Type *NRF_RTC_regs;

  int n_CCs;  //Number of compare/capture registers in this rtc instance

  bs_time_t *cc_timers;     //[n_CCs] When each CC will match (in microseconds)
  bs_time_t overflow_timer; // When the timer will overflow (in microseconds)
  uint64_t overflow_timer_sub_us; // When the timer will overflow (in sub-microsecond units)

  uint64_t counter_startT_sub_us; //Time when the counter was "started" (really the time that would correspond to COUNTER = 0)
  uint64_t counter_startT_negative_sub_us;

  uint32_t counter_at_stop; //Internal counter value when the counter was *stopped*

  uint32_t INTEN;
  uint32_t PRESC;
  bool running;     // Is this RTC running/started

#if (NHW_HAS_DPPI)
  uint dppi_map; //To which DPPI instance are this RTC subscribe&publish ports connected to
  struct nhw_subsc_mem* subscribed_CAPTURE;   //[n_CCs]
  struct nhw_subsc_mem subscribed_START;
  struct nhw_subsc_mem subscribed_STOP;
  struct nhw_subsc_mem subscribed_CLEAR;
  struct nhw_subsc_mem subscribed_TRIGOVRFLW;
#endif
};

static uint64_t first_lf_tick_time_sub_us;

static bs_time_t Timer_RTC = TIME_NEVER;
static struct rtc_status nhw_rtc_st[NHW_RTC_TOTAL_INST];
NRF_RTC_Type NRF_RTC_regs[NHW_RTC_TOTAL_INST];

static bs_time_t sub_us_time_to_us_time(uint64_t sub_us_time);
static uint64_t us_time_to_sub_us_time(bs_time_t us_time);
static void nhw_rtc_TASKS_CLEAR(uint rtc);
static void nhw_rtc_signal_OVERFLOW(uint rtc);
static void nhw_rtc_signal_COMPARE(uint rtc, uint cc);

static void nhw_rtc_init(void) {
#if (NHW_HAS_DPPI)
  /* Mapping of peripheral instance to DPPI instance */
  uint nhw_rtc_dppi_map[NHW_RTC_TOTAL_INST] = NHW_RTC_DPPI_MAP;
#endif
  int RTC_n_CCs[NHW_RTC_TOTAL_INST] = NHW_RTC_N_CC;

  memset(NRF_RTC_regs, 0, sizeof(NRF_RTC_regs));

  for (int i = 0; i < NHW_RTC_TOTAL_INST ; i++) {
    struct rtc_status *rtc_st = &nhw_rtc_st[i];

    rtc_st->NRF_RTC_regs = &NRF_RTC_regs[i];
    rtc_st->n_CCs = RTC_n_CCs[i];

    rtc_st->counter_startT_sub_us = TIME_NEVER;

    rtc_st->cc_timers = (bs_time_t *)bs_malloc(sizeof(bs_time_t)*RTC_n_CCs[i]);

    for (int j = 0 ; j < rtc_st->n_CCs ; j++) {
      rtc_st->cc_timers[j] = TIME_NEVER;
    }
    rtc_st->overflow_timer = TIME_NEVER;
    rtc_st->overflow_timer_sub_us = TIME_NEVER;

#if (NHW_HAS_DPPI)
    rtc_st->dppi_map = nhw_rtc_dppi_map[i];
    rtc_st->subscribed_CAPTURE = (struct nhw_subsc_mem*)bs_calloc(RTC_n_CCs[i], sizeof(struct nhw_subsc_mem));
#endif
  }
  Timer_RTC = TIME_NEVER;
}

NSI_TASK(nhw_rtc_init, HW_INIT, 100);

/*
 * Free all RTC instances resources before program exit
 */
static void nhw_rtc_free(void)
{
  for (int t = 0; t < NHW_RTC_TOTAL_INST; t++) {
    struct rtc_status *rtc_st = &nhw_rtc_st[t];

    free(rtc_st->cc_timers);
    rtc_st->cc_timers = NULL;

#if (NHW_HAS_DPPI)
    free(rtc_st->subscribed_CAPTURE);
    rtc_st->subscribed_CAPTURE = NULL;
#endif /* (NHW_HAS_DPPI) */
  }
}

NSI_TASK(nhw_rtc_free, ON_EXIT_PRE, 100);

/**
 * Convert a time delta in sub-microseconds units to the equivalent time in microseconds.
 * The value is always rounded UP. This is because otherwise events would be registered
 * in a time in microseconds before the event actually occurred. This would lead to many imprecise
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

static uint64_t get_time_in_sub_us(void)
{
  bs_time_t now = nsi_hws_get_time();

  if (now >= sub_us_time_to_us_time(TIME_NEVER)) {
    bs_trace_error_time_line("Bummer, the RTC model only supports running for 1142 years\n");
    /*If you really need this, generalize the calculation to more than 64 bits*/
  }

  return us_time_to_sub_us_time(now);
}

static uint64_t get_last_lf_tick_time_sub_us(void) {
  uint64_t now_sub_us = get_time_in_sub_us();

  uint64_t n_lf_ticks = (now_sub_us - first_lf_tick_time_sub_us) / LF_CLOCK_PERIOD_subus; //floor()
  uint64_t last_tick_time_sub_us = n_lf_ticks * LF_CLOCK_PERIOD_subus;
  last_tick_time_sub_us += first_lf_tick_time_sub_us;

  return last_tick_time_sub_us;
}

/**
 * Convert a time delta in sub-microsecond units to the equivalent count accounting for the PRESCALER
 * Note that the number is rounded down [floor()]
 */
static uint64_t time_sub_us_to_counter(uint rtc, uint64_t delta_sub_us) {
  uint64_t ticks;

  ticks = delta_sub_us / ((uint64_t)LF_CLOCK_PERIOD_subus * (nhw_rtc_st[rtc].PRESC + 1));
  return ticks;
}

/**
 * Convert a counter delta to sub-microsecond units accounting for the PRESCALER
 */
static uint64_t counter_to_time_sub_us(uint rtc, uint64_t counter) {
  uint64_t Elapsed;

  Elapsed = counter  * (uint64_t)LF_CLOCK_PERIOD_subus * (nhw_rtc_st[rtc].PRESC + 1);

  return Elapsed;
}

/**
 * Return the time in sub-microsecond units it takes for the COUNTER to do 1 wrap
 */
static uint64_t time_of_1_counter_wrap_sub_us(uint rtc) {
  return counter_to_time_sub_us(rtc, (uint64_t)RTC_COUNTER_MASK + 1);
}

/*
 * Return the *next* time (in us) when the RTC counter will reach <counter_match>
 */
static bs_time_t get_counter_match_time(uint rtc, uint64_t counter_match, uint64_t* next_match_sub_us)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];

  bs_time_t next_match_us = TIME_NEVER;
  *next_match_sub_us = TIME_NEVER;

  if (this->running == true) {
    uint64_t now_sub_us = get_time_in_sub_us();
    uint64_t counter_match_sub_us = counter_to_time_sub_us(rtc, counter_match);

    if(this->counter_startT_sub_us > 0)
    {
      *next_match_sub_us = this->counter_startT_sub_us
                              + counter_match_sub_us;
    }
    else if (counter_match_sub_us > this->counter_startT_negative_sub_us)
    {
      *next_match_sub_us =  counter_match_sub_us - this->counter_startT_negative_sub_us;
    }
    else
    {
      *next_match_sub_us = time_of_1_counter_wrap_sub_us(rtc)
                           + counter_match_sub_us - this->counter_startT_negative_sub_us;
    }

    while(*next_match_sub_us <= now_sub_us)
    {
      *next_match_sub_us += time_of_1_counter_wrap_sub_us(rtc);
    }

    next_match_us = sub_us_time_to_us_time(*next_match_sub_us);
  }

  return next_match_us;
}

static void nhw_rtc_update_master_timer(void) {
  Timer_RTC = TIME_NEVER;
  for (int rtc = 0; rtc < NHW_RTC_TOTAL_INST ; rtc++) {
    struct rtc_status *this = &nhw_rtc_st[rtc];

    if (this->running == false) {
      continue;
    }
    for (int cc = 0 ; cc < this->n_CCs ; cc++) {
      if (this->cc_timers[cc] < Timer_RTC) {
        Timer_RTC = this->cc_timers[cc];
      }
    }

    if (this->overflow_timer < Timer_RTC) {
      Timer_RTC = this->overflow_timer;
    }
  }
  nsi_hws_find_next_event();
}

/**
 * Save in cc_timers[cc] the *next* time when this RTC will match the
 * CC[cc] register
 */
static void update_cc_timer(uint rtc, uint cc) {
  uint64_t match_sub_us; // Only to comply to the interface
  nhw_rtc_st[rtc].cc_timers[cc] = get_counter_match_time(rtc, NRF_RTC_regs[rtc].CC[cc], &match_sub_us);
}

/*
 * Update all cc_timers[*] for a RTC instance
 * to the *next* time when they will match
 */
static void update_all_cc_timers(uint rtc) {
  for (int cc = 0 ; cc < nhw_rtc_st[rtc].n_CCs; cc++) {
    update_cc_timer(rtc, cc);
  }
}

static void update_overflow_timer(uint rtc) {
  struct rtc_status *this = &nhw_rtc_st[rtc];
  this->overflow_timer = get_counter_match_time(rtc, RTC_COUNTER_MASK + 1, &this->overflow_timer_sub_us);
}

static void update_timers(int rtc)
{
  update_all_cc_timers(rtc);
  update_overflow_timer(rtc);
  nhw_rtc_update_master_timer();
}

/**
 * Sets the internal state of the counter like if the counter was just set to the specified value.
 * This is done by setting the "counter start time"
 * (counter_startT*) to an appropriate value, so that the time elapsed from the counter start
 * corresponds to the given counter value. Such virtual "counter start time" can be negative.
 */
static void nhw_rtc_set_counter(uint rtc, uint64_t counter_val)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];

  counter_val &= RTC_COUNTER_MASK;
  uint64_t counter_val_sub_us = counter_to_time_sub_us(rtc, counter_val);

  // All the functions which use this reset the <PRESC>, so it is like the counter was set
  // on the last LF clock tick
  uint64_t last_lf_tick_sub_us = get_last_lf_tick_time_sub_us();

  if(last_lf_tick_sub_us >= counter_val_sub_us)
  {
    this->counter_startT_sub_us = last_lf_tick_sub_us - counter_val_sub_us;
    this->counter_startT_negative_sub_us = 0;
  }
  else
  {
    this->counter_startT_sub_us = 0;
    this->counter_startT_negative_sub_us = counter_val_sub_us - last_lf_tick_sub_us;
  }

  NRF_RTC_regs[rtc].COUNTER = counter_val;

  update_timers(rtc);
}

static void handle_overflow_event(uint rtc)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];

  // The real time (in sub-microsecond units, not in microseconds)
  // in which the current overflow event occurs.
  // update_overflow_timer will overwrite overflow_timer_sub_us[rtc]
  uint64_t current_overflow_event_sub_us = this->overflow_timer_sub_us;

  update_overflow_timer(rtc); //Next time it will overflow

  bs_trace_raw_time(8, "RTC%i: Timer overflow\n", rtc);

  this->counter_startT_sub_us = current_overflow_event_sub_us;
  this->counter_startT_negative_sub_us = 0;

  nhw_rtc_signal_OVERFLOW(rtc);
}

static void nhw_rtc_timer_triggered(void) {
  for (int rtc = 0; rtc < NHW_RTC_TOTAL_INST ; rtc++) {
    struct rtc_status *rtc_el = &nhw_rtc_st[rtc];
    if (rtc_el->running == false) {
      continue;
    }

    for (int cc = 0 ; cc < rtc_el->n_CCs ; cc++) {
      if (rtc_el->cc_timers[cc] == Timer_RTC ){ //This CC is matching now
        update_cc_timer(rtc, cc); //Next time it will match
        nhw_rtc_signal_COMPARE(rtc, cc);
      }
    }

    if (rtc_el->overflow_timer == Timer_RTC) { //Overflow occurred now
      handle_overflow_event(rtc); // this must always be the last event, as it might update counter_startT_sub_us
    }

  }
  nhw_rtc_update_master_timer();
}

NSI_HW_EVENT(Timer_RTC, nhw_rtc_timer_triggered, 50);

/**
 * Check if an EVTEN or INTEN has the tick event set
 */
static void check_not_supported_TICK(uint32_t i) {
  if (i &  RTC_EVTEN_TICK_Msk) {
    bs_trace_warning_line_time("RTC: The TICK functionality is not modelled\n");
  }
}

void nhw_rtc_notify_first_lf_tick(void) {
  first_lf_tick_time_sub_us = get_time_in_sub_us();
  bs_trace_raw_time(9, "RTC: First lf tick\n");
}

/*
 * Update the counter register so it can be read by SW
 */
void nhw_rtc_update_COUNTER(uint rtc) {
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if (this->running == true) {
    uint64_t count;
    count = time_sub_us_to_counter(rtc,
                                   get_time_in_sub_us() - this->counter_startT_sub_us
                                        + this->counter_startT_negative_sub_us);
    RTC_regs->COUNTER = count & RTC_COUNTER_MASK;
  } else {
    RTC_regs->COUNTER = this->counter_at_stop & RTC_COUNTER_MASK;
  }
}

/**
 * TASK_START triggered handler
 */
static void nhw_rtc_TASKS_START(uint rtc) {
  struct rtc_status *this = &nhw_rtc_st[rtc];

  if (this->running == true) {
    return;
  }
  bs_trace_raw_time(5, "RTC%i: TASK_START\n", rtc);
  this->running = true;

  /* Pre-scaler value is latched to an internal register on tasks START, CLEAR, and TRIGOVRFLW */
  this->PRESC = NRF_RTC_regs[rtc].PRESCALER;

  //If the counter is not zero at start, is like if the counter was started earlier
  nhw_rtc_set_counter(rtc, this->counter_at_stop);
}

/**
 * TASK_STOP triggered handler
 */
static void nhw_rtc_TASKS_STOP(uint rtc) {
  struct rtc_status *this = &nhw_rtc_st[rtc];

  if (this->running == false) {
    return;
  }
  bs_trace_raw_time(5, "RTC%i: TASK_STOP\n", rtc);
  this->running = false;
  this->counter_at_stop = time_sub_us_to_counter(rtc,
                                        get_time_in_sub_us() - this->counter_startT_sub_us
                                        + this->counter_startT_negative_sub_us); //we save the value when the counter was stoped in case it is started again without clearing it
  this->counter_at_stop &= RTC_COUNTER_MASK;
  NRF_RTC_regs[rtc].COUNTER = this->counter_at_stop;
  for (int cc = 0 ; cc < this->n_CCs ; cc++) {
    this->cc_timers[cc] = TIME_NEVER;
  }
  this->overflow_timer = TIME_NEVER;
  nhw_rtc_update_master_timer();
}

/**
 * TASK_CLEAR triggered handler
 */
static void nhw_rtc_TASKS_CLEAR(uint rtc) {
  bs_trace_raw_time(5, "RTC%i: TASK_CLEAR\n", rtc);

  /* Pre-scaler value is latched to an internal register on tasks START, CLEAR, and TRIGOVRFLW */
  nhw_rtc_st[rtc].PRESC = NRF_RTC_regs[rtc].PRESCALER;
  nhw_rtc_st[rtc].counter_at_stop = 0;
  nhw_rtc_set_counter(rtc, 0);
}

/**
 * TASK_TRIGGER_OVERFLOW triggered handler
 */
static void nhw_rtc_TASKS_TRIGOVRFLW(uint rtc) {

  bs_trace_raw_time(5, "RTC%i: TASK_TRIGGER_OVERFLOW\n", rtc);

  /* Pre-scaler value is latched to an internal register on tasks START, CLEAR, and TRIGOVRFLW */
  nhw_rtc_st[rtc].PRESC = NRF_RTC_regs[rtc].PRESCALER;
  nhw_rtc_st[rtc].counter_at_stop = RTC_TRIGGER_OVERFLOW_COUNTER_VALUE;
  nhw_rtc_set_counter(rtc, RTC_TRIGGER_OVERFLOW_COUNTER_VALUE);
}

#if (NHW_RTC_HAS_CAPTURE)
static void nhw_rtc_TASKS_CAPTURE(uint rtc, uint cc_n) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  nhw_rtc_update_COUNTER(rtc);
  RTC_regs->CC[cc_n] = RTC_regs->COUNTER;

  nhw_rtc_regw_sideeffects_CC(rtc, cc_n);
}
#endif /* NHW_RTC_HAS_CAPTURE */

static void nhw_rtc_eval_interrupts(uint rtc) {
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_rtc_irq_map[NHW_RTC_TOTAL_INST] = NHW_RTC_INT_MAP;
  static bool RTC_int_line[NHW_RTC_TOTAL_INST]; /* Is the RTC currently driving its interrupt line high */

  struct rtc_status *this = &nhw_rtc_st[rtc];
  bool new_int_line = false;

  for (int cc = 0; cc < this->n_CCs; cc++) {
    int mask = this->INTEN & (RTC_INTENSET_COMPARE0_Msk << cc);
    if (NRF_RTC_regs[rtc].EVENTS_COMPARE[cc] && mask) {
      new_int_line = true;
      break; /* No need to check more */
    }
  }

  if (NRF_RTC_regs[rtc].EVENTS_TICK && (this->INTEN & RTC_INTENSET_TICK_Msk)) {
    new_int_line = true;
  }
  if (NRF_RTC_regs[rtc].EVENTS_OVRFLW && (this->INTEN & RTC_INTENSET_OVRFLW_Msk)) {
    new_int_line = true;
  }

  if (RTC_int_line[rtc] == false && new_int_line == true) {
    RTC_int_line[rtc] = true;
    hw_irq_ctrl_raise_level_irq_line(nhw_rtc_irq_map[rtc].cntl_inst,
                                     nhw_rtc_irq_map[rtc].int_nbr);
  } else if (RTC_int_line[rtc] == true && new_int_line == false) {
    RTC_int_line[rtc] = false;

    hw_irq_ctrl_lower_level_irq_line(nhw_rtc_irq_map[rtc].cntl_inst,
                                     nhw_rtc_irq_map[rtc].int_nbr);
  }
}

static void nhw_rtc_signal_COMPARE(uint rtc, uint cc)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

#if (NHW_RTC_HAS_SHORT_COMP_CLEAR)
  if (RTC_regs->SHORTS & (RTC_SHORTS_COMPARE0_CLEAR_Msk << cc)) {
    nhw_rtc_TASKS_CLEAR(rtc);
    bs_trace_warning_line_time("RTC: COMPARE->CLEAR short used, but CLEAR is instantaneous."
                               "If you are using this to generate a periodic interrupt, the period"
                               "will be 1 count too short\n");
  }
#endif /* NHW_RTC_HAS_SHORT_COMP_CLEAR */

  uint32_t mask = RTC_EVTEN_COMPARE0_Msk << cc;

  if (!((RTC_regs->EVTEN | this->INTEN) & mask)) {
    return;
  }

  RTC_regs->EVENTS_COMPARE[cc] = 1;

  if (RTC_regs->EVTEN & mask) {
#if (NHW_HAS_PPI)
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
    event += cc;
    nrf_ppi_event(event);
#elif (NHW_HAS_DPPI)
    nhw_dppi_event_signal_if(this->dppi_map,
                             RTC_regs->PUBLISH_COMPARE[cc]);
#endif
  }

  nhw_rtc_eval_interrupts(rtc);
}

static void nhw_rtc_signal_OVERFLOW(uint rtc)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if (!((RTC_regs->EVTEN | this->INTEN) & RTC_EVTEN_OVRFLW_Msk)) {
    return;
  }

  RTC_regs->EVENTS_OVRFLW = 1;

  if (RTC_regs->EVTEN & RTC_EVTEN_OVRFLW_Msk) {
#if (NHW_HAS_PPI)
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
    nrf_ppi_event(event);
#elif (NHW_HAS_DPPI)
    nhw_dppi_event_signal_if(this->dppi_map,
                             RTC_regs->PUBLISH_OVRFLW);
#endif
  }

  nhw_rtc_eval_interrupts(rtc);
}


/*static*/ void nhw_rtc_signal_TICK(uint rtc) /*Not yet used, as all TICK functionality is not yet implemented */
{
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if (!((RTC_regs->EVTEN | this->INTEN) & RTC_EVTEN_TICK_Msk)) {
    return;
  }

  RTC_regs->EVENTS_TICK = 1;

  if (RTC_regs->EVTEN & RTC_EVTEN_TICK_Msk) {
#if (NHW_HAS_PPI)
    ppi_event_types_t event = RTC0_EVENTS_TICK;
    switch (rtc){
    case 0:
      event = RTC0_EVENTS_TICK;
      break;
    case 1:
      event = RTC1_EVENTS_TICK;
      break;
    case 2:
      event = RTC2_EVENTS_TICK;
      break;
    }
    nrf_ppi_event(event);
#elif (NHW_HAS_DPPI)
    nhw_dppi_event_signal_if(this->dppi_map,
                             RTC_regs->PUBLISH_TICK);
#endif
  }

  nhw_rtc_eval_interrupts(rtc);
}

void nhw_rtc_regw_sideeffect_TASKS_START(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if (RTC_regs->TASKS_START) {
    RTC_regs->TASKS_START = 0;
    nhw_rtc_TASKS_START(i);
  }
}

void nhw_rtc_regw_sideeffect_TASKS_STOP(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if (RTC_regs->TASKS_STOP) {
    RTC_regs->TASKS_STOP = 0;
    nhw_rtc_TASKS_STOP(i);
  }
}

void nhw_rtc_regw_sideeffect_TASKS_CLEAR(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if (RTC_regs->TASKS_CLEAR) {
    RTC_regs->TASKS_CLEAR = 0;
    nhw_rtc_TASKS_CLEAR(i);
  }
}

void nhw_rtc_regw_sideeffect_TASKS_TRIGOVRFLW(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if (RTC_regs->TASKS_TRIGOVRFLW) {
    RTC_regs->TASKS_TRIGOVRFLW = 0;
    nhw_rtc_TASKS_TRIGOVRFLW(i);
  }
}

#if (NHW_RTC_HAS_CAPTURE)
void nhw_rtc_regw_sideeffect_TASKS_CAPTURE(uint i, uint cc) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if (RTC_regs->TASKS_CAPTURE[cc]) {
    RTC_regs->TASKS_CAPTURE[cc] = 0;
    nhw_rtc_TASKS_CAPTURE(i, cc);
  }
}
#endif /* NHW_RTC_HAS_CAPTURE */

#if (NHW_HAS_DPPI)

static void nhw_rtc_taskcapture_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint cc_n = (uintptr_t)param & 0xFFFF;
  nhw_rtc_TASKS_CAPTURE(inst, cc_n);
}

void nhw_rtc_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint cc_n) {
  struct rtc_status *this = &nhw_rtc_st[inst];

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_RTC_regs->SUBSCRIBE_CAPTURE[cc_n],
                                       &this->subscribed_CAPTURE[cc_n],
                                       nhw_rtc_taskcapture_wrap,
                                       (void*)((inst << 16) + cc_n));
}

#define NHW_RTC_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                 \
  static void nhw_rtc_task##TASK_N##_wrap(void* param)                            \
  {                                                                               \
    nhw_rtc_TASKS_##TASK_N((int) param);                                           \
  }                                                                               \
                                                                                  \
  void nhw_rtc_regw_sideeffects_SUBSCRIBE_##TASK_N(uint inst)                     \
  {                                                                               \
     struct rtc_status *this = &nhw_rtc_st[inst];                                 \
                                                                                  \
     nhw_dppi_common_subscribe_sideeffect(this->dppi_map,                         \
                                          this->NRF_RTC_regs->SUBSCRIBE_##TASK_N, \
                                          &this->subscribed_##TASK_N,             \
                                          nhw_rtc_task##TASK_N##_wrap,            \
                                          (void*) inst);                          \
  }

NHW_RTC_REGW_SIDEFFECTS_SUBSCRIBE(START)
NHW_RTC_REGW_SIDEFFECTS_SUBSCRIBE(STOP)
NHW_RTC_REGW_SIDEFFECTS_SUBSCRIBE(CLEAR)
NHW_RTC_REGW_SIDEFFECTS_SUBSCRIBE(TRIGOVRFLW)

#endif /* NHW_HAS_DPPI */

void nhw_rtc_regw_sideeffect_INTENSET(uint rtc)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if ( RTC_regs->INTENSET ){
    this->INTEN |= RTC_regs->INTENSET;
    RTC_regs->INTENSET = this->INTEN;

    check_not_supported_TICK(this->INTEN);
    nhw_rtc_eval_interrupts(rtc);
  }
}

void nhw_rtc_regw_sideeffect_INTENCLR(uint rtc)
{
  struct rtc_status *this = &nhw_rtc_st[rtc];
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[rtc];

  if ( RTC_regs->INTENCLR ){
    this->INTEN  &= ~RTC_regs->INTENCLR;
    RTC_regs->INTENSET = this->INTEN;
    RTC_regs->INTENCLR = 0;

    nhw_rtc_eval_interrupts(rtc);
  }
}

void nhw_rtc_regw_sideeffect_EVTENSET(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if ( RTC_regs->EVTENSET ){
    RTC_regs->EVTEN |= RTC_regs->EVTENSET;
    RTC_regs->EVTENSET = RTC_regs->EVTEN;
    check_not_supported_TICK(RTC_regs->EVTEN);
  }
}

void nhw_rtc_regw_sideeffect_EVTENCLR(uint i) {
  NRF_RTC_Type *RTC_regs = &NRF_RTC_regs[i];

  if ( RTC_regs->EVTENCLR ){
    RTC_regs->EVTEN  &= ~RTC_regs->EVTENCLR;
    RTC_regs->EVTENSET = RTC_regs->EVTEN;
    RTC_regs->EVTENCLR = 0;
  }
}

void nhw_rtc_regw_sideeffects_EVENTS_all(uint rtc) {
  nhw_rtc_eval_interrupts(rtc);
}

void nhw_rtc_regw_sideeffects_CC(uint rtc, uint cc_n) {
  struct rtc_status *this = &nhw_rtc_st[rtc];

  if (this->running == true) {
    update_cc_timer(rtc, cc_n);
    nhw_rtc_update_master_timer();
  }
}

#if (NHW_HAS_PPI)
void nhw_rtc0_TASKS_START(void)      { nhw_rtc_TASKS_START(0); }
void nhw_rtc0_TASKS_STOP(void)       { nhw_rtc_TASKS_STOP(0);  }
void nhw_rtc0_TASKS_CLEAR(void)      { nhw_rtc_TASKS_CLEAR(0); }
void nhw_rtc0_TASKS_TRIGOVRFLW(void) { nhw_rtc_TASKS_TRIGOVRFLW(0); }
void nhw_rtc1_TASKS_START(void)      { nhw_rtc_TASKS_START(1); }
void nhw_rtc1_TASKS_STOP(void)       { nhw_rtc_TASKS_STOP(1);  }
void nhw_rtc1_TASKS_CLEAR(void)      { nhw_rtc_TASKS_CLEAR(1); }
void nhw_rtc1_TASKS_TRIGOVRFLW(void) { nhw_rtc_TASKS_TRIGOVRFLW(1); }
void nhw_rtc2_TASKS_START(void)      { nhw_rtc_TASKS_START(2); }
void nhw_rtc2_TASKS_STOP(void)       { nhw_rtc_TASKS_STOP(2);  }
void nhw_rtc2_TASKS_CLEAR(void)      { nhw_rtc_TASKS_CLEAR(2); }
void nhw_rtc2_TASKS_TRIGOVRFLW(void) { nhw_rtc_TASKS_TRIGOVRFLW(2); }
#endif /* NHW_HAS_PPI */
