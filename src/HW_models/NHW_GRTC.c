/*
 * Copyright (c) 2024, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * GRTC - Global real-time counter
 *
 * This file provides the implementation of the nRF54 GRTC peripherals,
 * (and initialize it at startup and frees it on exit),
 * as described in the configuration (NHW_config.h)
 *
 * Each GRTC instance has a configurable number of compare capture (CC) registers,
 * and interrupts (and domains).
 *
 * Note1: The GRTC (and its SYSCOUNTER) does NOT go to sleep.
 *        And similarly there is no model of the low frequency/power
 *        counter mechanism.
 *        Note that, if everything is configured correctly, SW should
 *        not notice this in practice.
 *        So, KEEPRUNNING, TIMEOUT, WAKETIME & MODE.AUTOEN are ignored/irrelevant;
 *        EVENTS_SYSCOUNTERVALID is set as soon as the GRTC is started and never
 *        cleared to 0 by HW.
 *        SYSCOUNTERH.BUSY always reads as 0.
 *
 *        TASK_STOP and TASK_CLEAR are not really modeled:
 *        In the real counter STOPPING it (stopping the low freq. part), and trying to
 *        interact with the high frequency part will cause trouble.
 *        But there should be no real need or point of doing so. So we don't model
 *        the weird behavior that can follow.
 *        Similarly there is no real point of clearing it beyond reseting the
 *        count before starting it (we assume the counter is started either
 *        only once, or STARTed after CLEARing it).
 *
 * Note2: There is no model of the PWM functionality.
 *        Its registers and tasks are just ignored.
 *        EVENTS_PWMPERIODEND is not generated.
 *
 * Note3: There is no clock output functionality model.
 *        Its related registers are just ignored.
 *
 * Note4: Split security and ownership are not modeled.
 *        Everything is accessible from any MCU/owner and from any security
 *        state without any constrain.
 *
 * Note5: The GRTC does not automatically start the LFCLK, it is just assumed started
 *
 * Note6: Unlike in real HW, TASK_START is immediate
 *
 */


#include <string.h>
#include <stdint.h>
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_DPPI.h"
#include "NHW_templates.h"
#include "irq_ctrl.h"

struct grtc_status {
  NRF_GRTC_Type *NRF_GRTC_regs;

  uint n_cc;  /* Number of CC registers */
  uint n_domains;  /* Number of "domains" (owners) */

  uint n_int;  /* Number of interrupts */
  struct nhw_irq_mapping *int_map; //[n_int]
  bool *int_line_level; //[n_int]

  uint dppi_map;   //To which DPPI instance are this GRTC subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem* subscribed;   //[n_cc]

  bool rt_counter_running;
  bs_time_t GRTC_start_time;

  uint64_t SYSCOUNTER;
  bs_time_t *SYSCOUNTER_read_deadline; /* Last microsecond before the SYSCOUNTERL would have wrapped*/

  bs_time_t *CC_timers; //[n_CCs] When each compare match is expected to happen
};

static bs_time_t Timer_GRTC = TIME_NEVER;
static struct grtc_status nhw_grtc_st;
NRF_GRTC_Type NRF_GRTC_regs;

static const ptrdiff_t grtc_int_pdiff = offsetof(NRF_GRTC_Type, INTENSET1) - offsetof(NRF_GRTC_Type, INTENSET0);

static void nhw_GRTC_update_SYSCOUNTER(uint inst);
static void nhw_GRTC_update_master_timer(void);
static void nhw_GRTC_update_cc_timer(uint inst, int cc);
static void nhw_GRTC_update_all_cc_timers(uint inst);

/**
 * Initialize the GRTC model
 */
static void nhw_grtc_init(void) {
  /* Mapping of peripheral instance to DPPI instance */
  const uint nhw_grtc_n_cc = NHW_GRTC_N_CC;
  const uint nhw_GRTC_n_domains = NHW_GRTC_N_DOMAINS;
  const uint nhw_GRTC_n_int = NHW_GRTC_N_INT;
  struct nhw_irq_mapping nhw_GRTC_int_map[] = NHW_GRTC_INT_MAP;
  const uint nhw_GRTC_dppi_map = NHW_GRTC_DPPI_MAP;

  memset(&NRF_GRTC_regs, 0, sizeof(NRF_GRTC_Type));
  NRF_GRTC_regs.WAKETIME = GRTC_WAKETIME_ResetValue;
#if (NHW_GRTC_HAS_CLKOUT)
  NRF_GRTC_regs.CLKCFG = GRTC_CLKCFG_ResetValue;
#endif

  nhw_grtc_st.NRF_GRTC_regs = &NRF_GRTC_regs;
  nhw_grtc_st.n_cc = nhw_grtc_n_cc;
  nhw_grtc_st.n_domains = nhw_GRTC_n_domains;
  nhw_grtc_st.n_int = nhw_GRTC_n_int;
  nhw_grtc_st.int_map = (struct nhw_irq_mapping *)bs_calloc(nhw_grtc_st.n_int, sizeof(struct nhw_irq_mapping));
  memcpy(nhw_grtc_st.int_map, nhw_GRTC_int_map, sizeof(struct nhw_irq_mapping) * nhw_grtc_st.n_int);
  nhw_grtc_st.int_line_level = (bool *)bs_calloc(nhw_grtc_st.n_int, sizeof(bool));
  nhw_grtc_st.dppi_map = nhw_GRTC_dppi_map;
  nhw_grtc_st.subscribed = (struct nhw_subsc_mem*)bs_calloc(nhw_grtc_n_cc, sizeof(struct nhw_subsc_mem));
  nhw_grtc_st.rt_counter_running = false;
  nhw_grtc_st.GRTC_start_time = 0;
  nhw_grtc_st.SYSCOUNTER_read_deadline = (bs_time_t *)bs_calloc(nhw_grtc_st.n_domains, sizeof(bs_time_t));


  nhw_grtc_st.CC_timers = (bs_time_t *)bs_calloc(nhw_grtc_st.n_cc, sizeof(bs_time_t));

  nhw_GRTC_update_all_cc_timers(0);

  nhw_GRTC_update_master_timer();
}

NSI_TASK(nhw_grtc_init, HW_INIT, 100);

/*
 * Free all GRTC instances resources before program exit
 */
static void nhw_grtc_free(void)
{
  free(nhw_grtc_st.int_map);
  nhw_grtc_st.int_map = NULL;
  free(nhw_grtc_st.int_line_level);
  nhw_grtc_st.int_line_level = NULL;
  free(nhw_grtc_st.subscribed);
  nhw_grtc_st.subscribed = NULL;
  free(nhw_grtc_st.SYSCOUNTER_read_deadline);
  nhw_grtc_st.SYSCOUNTER_read_deadline = NULL;
  free(nhw_grtc_st.CC_timers);
  nhw_grtc_st.CC_timers = NULL;
}

NSI_TASK(nhw_grtc_free, ON_EXIT_PRE, 100);

static inline void nhw_GRTC_check_valid_cc_index(uint inst, uint nbr, const char *type)
{
  if ((inst >= NHW_GRTC_TOTAL_INST) || (nbr >= nhw_grtc_st.n_cc) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u (>= %u CC channels) in GRTC instance %u\n",
                             type, nbr, nhw_grtc_st.n_cc, inst);
  }
}

static inline void nhw_GRTC_check_valid_irq_index(uint inst, uint nbr, const char *type)
{
  if ((inst >= NHW_GRTC_TOTAL_INST) || (nbr >= nhw_grtc_st.n_int) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u (>= %u irqs) in GRTC instance %u\n",
                             type, nbr, nhw_grtc_st.n_int, inst);
  }
}

static inline void nhw_GRTC_check_valid_domain_index(uint inst, uint nbr, const char *type)
{
  if ((inst >= NHW_GRTC_TOTAL_INST) || (nbr >= nhw_grtc_st.n_domains) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u (>= %u domains) in GRTC instance %u\n",
                             type, nbr, nhw_grtc_st.n_domains, inst);
  }
}


static bs_time_t nhw_GRTC_counter_to_time(uint inst, uint64_t value) {
  (void) inst;
  return value + nhw_grtc_st.GRTC_start_time;
}

/**
 * Find the CC register timer (CC_timers[][]) which will trigger earliest (if any)
 */
static void nhw_GRTC_update_master_timer(void) {
  Timer_GRTC = TIME_NEVER;

  for (int cc = 0 ; cc < nhw_grtc_st.n_cc ; cc++) {
    if (nhw_grtc_st.CC_timers[cc] < Timer_GRTC) {
      Timer_GRTC = nhw_grtc_st.CC_timers[cc];
    }
  }
  nsi_hws_find_next_event();
}

/**
 * Save in CC_timers[cc] when the GRTC SYSCOUNTER will match the CC[cc]
 * register (the event/match condition is CC[cc] <= SYSCOUNTER)
 */
static void nhw_GRTC_update_cc_timer(uint inst, int cc) {
  (void) inst;
  if (NRF_GRTC_regs.CC[cc].CCEN) {
    uint64_t cc_value = ((uint64_t)NRF_GRTC_regs.CC[cc].CCH << 32) | NRF_GRTC_regs.CC[cc].CCL;
    nhw_grtc_st.CC_timers[cc] = nhw_GRTC_counter_to_time(inst, cc_value);
    if (nhw_grtc_st.CC_timers[cc] < nsi_hws_get_time()) {
      nhw_grtc_st.CC_timers[cc] = nsi_hws_get_time();
    }
  } else {
    nhw_grtc_st.CC_timers[cc] = TIME_NEVER;
  }
}

static void nhw_GRTC_update_all_cc_timers(uint inst) {
  for (int cc = 0 ; cc < nhw_grtc_st.n_cc; cc++) {
    nhw_GRTC_update_cc_timer(inst, cc);
  }
}

/**
 * Check if any interrupt line for the GRTC needs to be raised or lowered
 * (as a result of either the interrupt mask or the event register having
 * been modified)
 */
static void nhw_GRTC_eval_interrupt(uint inst)
{
  (void) inst;
  struct grtc_status *this = &nhw_grtc_st;
  uint32_t *INTEN;
  uint32_t *INTPEND;

  for (int irql = 0; irql < this->n_int; irql++) {
    uint32_t event_bitmask = 0;
    bool new_line_level;
    INTEN =  (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTEN0 + irql*grtc_int_pdiff);
    INTPEND= (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTPEND0 + irql*grtc_int_pdiff);

    for (int i = 0; i < this->n_cc; i++) {
      if (NRF_GRTC_regs.EVENTS_COMPARE[i]) {
        event_bitmask |= 1<<i;
      }
    }
    if (NRF_GRTC_regs.EVENTS_SYSCOUNTERVALID) {
      event_bitmask |= GRTC_INTENSET0_SYSCOUNTERVALID_Msk;
    }
#if (NHW_GRTC_HAS_PWM)
    if (NRF_GRTC_regs.EVENTS_PWMPERIODEND) {
      event_bitmask |= GRTC_INTENSET0_PWMPERIODEND_Msk;
    }
#endif
    *INTPEND = event_bitmask & *INTEN;

    new_line_level = (*INTPEND != 0);

    hw_irq_ctrl_toggle_level_irq_line_if(&nhw_grtc_st.int_line_level[irql],
                                         new_line_level,
                                         &this->int_map[irql]);
  }
}

static void nhw_GRTC_signal_EVENTS_COMPARE(uint inst, uint n) {
  NRF_GRTC_regs.EVENTS_COMPARE[n] = 1;
  nhw_GRTC_eval_interrupt(inst);

  nhw_dppi_event_signal_if(nhw_grtc_st.dppi_map,
                           NRF_GRTC_regs.PUBLISH_COMPARE[n]);
}

static void nhw_GRTC_signal_EVENTS_SYSCOUNTERVALID(uint inst) {
  NRF_GRTC_regs.EVENTS_SYSCOUNTERVALID = 1;
  nhw_GRTC_eval_interrupt(inst);
  /* Not connected to DPPI */
}

#if (NHW_GRTC_HAS_PWM)
//(Unused so far)
//static void nhw_GRTC_signal_EVENTS_PWMPERIODEND(uint inst) {
//  NRF_GRTC_regs.EVENTS_PWMPERIODEND = 1;
//  nhw_GRTC_eval_interrupt(inst);
//  /* Not connected to DPPI */
//}
#endif

static void nhw_GRTC_TASK_START(uint inst) {
  (void) inst;
  nhw_grtc_st.rt_counter_running = true;
  /* We assume the GRTC is started only once, or started after clearing */
  nhw_grtc_st.GRTC_start_time = nsi_hws_get_time();

  nhw_GRTC_signal_EVENTS_SYSCOUNTERVALID(inst);
}

static void nhw_GRTC_TASK_STOP(uint inst) {
  (void) inst;
  nhw_grtc_st.rt_counter_running = false;
  bs_trace_warning_time_line("GRTC TASK_STOP is not properly supported.\n");
}

static void nhw_GRTC_TASK_CLEAR(uint inst) {
  (void) inst;
  /* In real HW this would clear the RTCOUNTER.
   * Here we do nothing, hoping that if it is called
   * it is done while the counter is stopped */
  if (nhw_grtc_st.rt_counter_running) {
    bs_trace_warning_time_line("GRTC models, TASK_CLEAR not supported while GRTC is running\n");
  }
}

#if (NHW_GRTC_HAS_PWM)
static void nhw_GRTC_TASK_PWMSTART(uint inst) {
  /* Not modeled */
  (void) inst;
}

static void nhw_GRTC_TASK_PWMSTOP(uint inst) {
  /* Not modeled */
  (void) inst;
}
#endif

static inline uint32_t nhw_GRTC_get_SYNCOUNTERL(uint inst) {
  (void) inst;
  return nhw_grtc_st.SYSCOUNTER & UINT32_MAX;
}

static inline uint32_t nhw_GRTC_get_counterhighword(uint64_t value) {
  return (value >> 32) & (UINT32_MAX >> BS_MIN(64 - NHW_GRTC_SYSCOUNTER_BW, 32));
}

static inline uint32_t nhw_GRTC_get_SYNCOUNTERH(uint inst) {
  (void) inst;
  return nhw_GRTC_get_counterhighword(nhw_grtc_st.SYSCOUNTER);
}

static void nhw_GRTC_TASK_CAPTURE(uint inst, uint n) {
  NRF_GRTC_regs.CC[n].CCEN = 0; /* Trigger the capture task disables the compare feature */
  nhw_GRTC_update_cc_timer(inst, n);
  nhw_GRTC_update_master_timer();

  nhw_GRTC_update_SYSCOUNTER(inst);
  NRF_GRTC_regs.CC[n].CCL = nhw_GRTC_get_SYNCOUNTERL(inst);
  NRF_GRTC_regs.CC[n].CCH = nhw_GRTC_get_SYNCOUNTERH(inst);
}

NHW_SIDEEFFECTS_TASKS(GRTC, NRF_GRTC_regs., START)
NHW_SIDEEFFECTS_TASKS(GRTC, NRF_GRTC_regs., STOP)
NHW_SIDEEFFECTS_TASKS(GRTC, NRF_GRTC_regs., CLEAR)
#if (NHW_GRTC_HAS_PWM)
NHW_SIDEEFFECTS_TASKS(GRTC, NRF_GRTC_regs., PWMSTART)
NHW_SIDEEFFECTS_TASKS(GRTC, NRF_GRTC_regs., PWMSTOP)
#endif

void nhw_GRTC_regw_sideeffects_TASKS_CAPTURE(uint inst, uint n) {
  if (NRF_GRTC_regs.TASKS_CAPTURE[n]) {
    NRF_GRTC_regs.TASKS_CAPTURE[n] = 0;
    nhw_GRTC_TASK_CAPTURE(inst, n);
  }
}

static void nhw_GRTC_taskcapture_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint cc_n = (uintptr_t)param & 0xFFFF;
  nhw_GRTC_TASK_CAPTURE(inst, cc_n);
}

void nhw_GRTC_regw_sideeffects_SUBSCRIBE_CAPTURE(uint inst, uint cc_n) {
  struct grtc_status *this = &nhw_grtc_st;

  nhw_GRTC_check_valid_cc_index(inst, cc_n, "SUBSCRIBE_CAPTURE");

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_GRTC_regs->SUBSCRIBE_CAPTURE[cc_n],
                                       &this->subscribed[cc_n],
                                       nhw_GRTC_taskcapture_wrap,
                                       (void*)((inst << 16) + cc_n));
}

void nhw_GRTC_regw_sideeffects_INTEN(uint inst, uint n) {
  nhw_GRTC_check_valid_irq_index(inst, n, "INTEN");

  uint32_t *INTENSET = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTENSET0 + n*grtc_int_pdiff);
  uint32_t *INTEN    = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTEN0    + n*grtc_int_pdiff);

  *INTENSET = *INTEN;
  nhw_GRTC_eval_interrupt(inst);
}

void nhw_GRTC_regw_sideeffects_INTENSET(uint inst, uint n) {
  nhw_GRTC_check_valid_irq_index(inst, n, "INTENSET");

  uint32_t *INTENSET = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTENSET0 + n*grtc_int_pdiff);
  uint32_t *INTEN    = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTEN0    + n*grtc_int_pdiff);

  if (*INTENSET) {
    *INTEN   |= *INTENSET;
    *INTENSET = *INTEN;
    nhw_GRTC_eval_interrupt(inst);
  }
}

void nhw_GRTC_regw_sideeffects_INTENCLR(uint inst, uint n) {
  nhw_GRTC_check_valid_irq_index(inst, n, "INTENCLR");

  uint32_t *INTENSET = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTENSET0 + n*grtc_int_pdiff);
  uint32_t *INTENCLR = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTENCLR0 + n*grtc_int_pdiff);
  uint32_t *INTEN    = (uint32_t *)((uintptr_t)&NRF_GRTC_regs.INTEN0    + n*grtc_int_pdiff);

  if (*INTENCLR) {
    *INTEN   &= ~*INTENCLR;
    *INTENSET = *INTEN;
    *INTENCLR = 0; //We do not model reading the INTEN register thru the INTENCLR one
    nhw_GRTC_eval_interrupt(inst);
  }
}

NHW_SIDEEFFECTS_EVENTS(GRTC)

static void nhw_GRTC_update_SYSCOUNTER(uint inst) {
  nhw_grtc_st.SYSCOUNTER = nsi_hws_get_time() - nhw_grtc_st.GRTC_start_time;
}

static void nhw_GRTC_check_syscounter_en(uint inst, const char *msg) {
  if (!nhw_grtc_st.rt_counter_running) {
    bs_trace_warning_time_line("The RT counter was not started while trying to %s\n", msg);
    return;
  }
  if (!(NRF_GRTC_regs.MODE & GRTC_MODE_SYSCOUNTEREN_Msk)) {
    bs_trace_warning_time_line("MODE.SYSCOUNTER was not enabled while trying to %s\n", msg);
  }
  return;
}

uint32_t nhw_GRTC_regr_sideeffects_SYSCOUNTERL(uint inst, uint n)
{
  nhw_GRTC_check_syscounter_en(inst, "read SYSCOUNTERL");
  nhw_GRTC_check_valid_domain_index(inst, n, "SYSCOUNTERL");
  nhw_GRTC_update_SYSCOUNTER(inst);
  uint32_t value = nhw_GRTC_get_SYNCOUNTERL(inst);
  uint64_t remain = (uint64_t)UINT32_MAX - value;

  nhw_grtc_st.SYSCOUNTER_read_deadline[n] = nsi_hws_get_time() + remain;

  NRF_GRTC_regs.SYSCOUNTER[n].SYSCOUNTERL = value;

  return value;
}

uint32_t nhw_GRTC_regr_sideeffects_SYSCOUNTERH(uint inst, uint n)
{
  nhw_GRTC_check_syscounter_en(inst, "read SYSCOUNTERH");
  nhw_GRTC_check_valid_domain_index(inst, n, "SYSCOUNTERH");
  nhw_GRTC_update_SYSCOUNTER(inst);
  uint32_t value = nhw_GRTC_get_SYNCOUNTERH(inst);

  //Check for SYSCOUNTERL "wrap":
  bs_time_t now = nsi_hws_get_time();
  if (now > nhw_grtc_st.SYSCOUNTER_read_deadline[n]) {
    value |= GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk;
  }

  NRF_GRTC_regs.SYSCOUNTER[n].SYSCOUNTERH = value;

  return value;
}

void nhw_GRTC_regw_sideeffects_CC_CCADD(uint inst, uint cc) {

  nhw_GRTC_check_valid_cc_index(inst, cc, "CC.CCADD");

  uint32_t offset = NRF_GRTC_regs.CC[cc].CCADD & GRTC_CC_CCADD_VALUE_Msk;
  uint64_t value;
  if (NRF_GRTC_regs.CC[cc].CCADD & GRTC_CC_CCADD_REFERENCE_Msk) { //Added to CC
    value = ((uint64_t)NRF_GRTC_regs.CC[cc].CCH << 32) | NRF_GRTC_regs.CC[cc].CCL;
  } else { //Added to SYSCOUNTER
    nhw_GRTC_update_SYSCOUNTER(inst);
    value = nhw_grtc_st.SYSCOUNTER;
  }
  value += offset;

  NRF_GRTC_regs.CC[cc].CCL = value & UINT32_MAX;
  NRF_GRTC_regs.CC[cc].CCH = nhw_GRTC_get_counterhighword(value);

  NRF_GRTC_regs.CC[cc].CCEN = GRTC_CC_CCEN_ACTIVE_Msk; /* Writing to CCADD enables that compare channel */

  nhw_GRTC_update_cc_timer(inst, cc);
  nhw_GRTC_update_master_timer();
}

void nhw_GRTC_regw_sideeffects_CC_CCEN(uint inst, uint cc) {
  nhw_GRTC_check_valid_cc_index(inst, cc, "CC.CCEN");

  nhw_GRTC_update_cc_timer(inst, cc);
  nhw_GRTC_update_master_timer();
}

void nhw_GRTC_regw_sideeffects_CC_CCL(uint inst, uint cc) {
  nhw_GRTC_check_valid_cc_index(inst, cc, "CC.CCL");

  NRF_GRTC_regs.CC[cc].CCEN = 0; /* Writing to CCL disables that compare channel */
  nhw_GRTC_update_cc_timer(inst, cc);
  nhw_GRTC_update_master_timer();
}

void nhw_GRTC_regw_sideeffects_CC_CCH(uint inst, uint cc) {
  nhw_GRTC_check_valid_cc_index(inst, cc, "CC.CCH");

  NRF_GRTC_regs.CC[cc].CCEN = GRTC_CC_CCEN_ACTIVE_Msk; /* Writing to CCH enables that compare channel */

  nhw_GRTC_update_cc_timer(inst, cc);
  nhw_GRTC_update_master_timer();
}

static void nhw_GRTC_compare_reached(uint inst, uint cc) {
  if (NRF_GRTC_regs.CC[cc].CCEN == 0) {
    bs_trace_warning_time_line("Programming error: CCEN was cleared without using the HAL\n");
    return;
  }
  if ((cc==0) && NRF_GRTC_regs.INTERVAL) {
    uint64_t value = ((uint64_t)NRF_GRTC_regs.CC[cc].CCH << 32) | NRF_GRTC_regs.CC[cc].CCL;
    value += NRF_GRTC_regs.INTERVAL;
    NRF_GRTC_regs.CC[cc].CCL = value & UINT32_MAX;
    NRF_GRTC_regs.CC[cc].CCH = nhw_GRTC_get_counterhighword(value);
  } else {
    NRF_GRTC_regs.CC[cc].CCEN = 0;
  }
  nhw_GRTC_update_cc_timer(inst, cc);

  nhw_GRTC_signal_EVENTS_COMPARE(inst, cc);
}

static void nhw_GRTC_timer_triggered(void) {
  uint inst = 0, cc;
  struct {
    uint inst[NHW_GRTC_TOTAL_INST*NHW_GRTC_N_CC];
    uint cc[NHW_GRTC_TOTAL_INST*NHW_GRTC_N_CC];
    uint cnt;
  } match;

  match.cnt = 0;

  for (cc = 0; cc < NHW_GRTC_N_CC; cc++) {
    if (nhw_grtc_st.CC_timers[cc] == Timer_GRTC) {
      match.inst[match.cnt] = inst;
      match.cc[match.cnt] = cc;
      match.cnt++;
    }
  }
  while (match.cnt > 0) {
    match.cnt--;
    inst = match.inst[match.cnt];
    cc = match.cc[match.cnt];
    nhw_GRTC_compare_reached(inst ,cc);
  }
  nhw_GRTC_update_master_timer();
}

NSI_HW_EVENT(Timer_GRTC, nhw_GRTC_timer_triggered, 50);
