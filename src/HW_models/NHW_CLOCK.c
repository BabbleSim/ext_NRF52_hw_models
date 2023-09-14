/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CLOCK - Clock control
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/clock.html?cp=5_1_0_4_3
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/clock/doc/clock.html?cp=4_0_0_3_10
 * POWER - POWER control
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/power.html?cp=5_1_0_4_2
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/power/doc/power.html?cp=4_0_0_3_5
 * RESET - RESET control
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/chapters/reset/doc/reset.html?cp=4_0_0_3_9
 *
 * Notes POWER / RESET:
 * 1. The POWER and (nrf5340) RESET peripherals are just stubs with no functionality
 *
 * Note: CLOCK:
 * 1. The clocks are ready in 1 delta cycle (i.e. almost instantaneously),
 *    HFXODEBOUNCE and LFXODEBOUNCE are ignored together with the XTALs
 *    power up times
 *
 * 2. The models do not check the requirement of having the HFXO clock running to be
 *    able to run the radio. The radio models will run just fine without it.
 *
 * 3. The LFRC oscillator calibration TASK_CAL finishes in 1 delta cycle.
 *    The models do not do anything during the LFRC calibration (apart from
 *    generating the done event/interrupt).
 *
 * 4. We assume the 32.768 KHz clock does not drift relative to the 64MHz one.
 *    This would only be the case if they had the same source or there was a tracking
 *    and adjustment loop of one based on the other.
 *
 * 5. LFCLKSRC is ignored (beyond copying LFCLKSRC.SRC to LFCLKSRCCOPY and LFCLKSTAT)
 *
 * 6. TRACECONFIG is ignored
 *
 * 7. The calibration timer timing is exact: it will take exactly CTIV*0.25s to
 *    finish.
 *
 * 8. After TASK_CTSTART EVENTS_CTSTARTED is raised immediately,
 *    After TASK_CTSTOP EVENTS_CTSTOPPED is raised immediately.
 *
 * 9. For the nrf5340 CLOCK. Almost all new functionality is missing
 *     (beyond connection to the DPPI, and having one instance per core)
 *    * The extra app core clocks are not implemented yet (Audio or 192M)
 *    * Automatic HW clock requests are not implemented yet
 */

#include <string.h>
#include <stdint.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_CLOCK.h"
#include "nsi_hw_scheduler.h"
#include "NHW_xPPI.h"
#include "NHW_RTC.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

enum clock_states {Stopped = 0, Starting, Started, Stopping};

struct clkpwr_status {
  NRF_CLOCK_Type *CLOCK_regs;
  NRF_POWER_Type *POWER_regs;
#if (NHW_CLKPWR_HAS_RESET)
  NRF_RESET_Type *RESET_regs;
#endif

  uint inst;

  uint32_t INTEN; //interrupt enable

  bs_time_t Timer_CLOCK_LF;
  bs_time_t Timer_CLOCK_HF;
  bs_time_t Timer_LF_cal;
  bs_time_t Timer_caltimer;

  enum clock_states HF_Clock_state;
  enum clock_states LF_Clock_state;
  enum clock_states LF_cal_state;
  enum clock_states caltimer_state;

#if (NHW_HAS_DPPI)
  uint dppi_map;   //To which DPPI instance are this CLOCK/POWER subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem subscribed_HFCLKSTART;
  struct nhw_subsc_mem subscribed_HFCLKSTOP;
  struct nhw_subsc_mem subscribed_LFCLKSTART;
  struct nhw_subsc_mem subscribed_LFCLKSTOP;
  struct nhw_subsc_mem subscribed_CAL;
  struct nhw_subsc_mem subscribed_HFCLKAUDIOSTART;
  struct nhw_subsc_mem subscribed_HFCLKAUDIOSTOP;
  struct nhw_subsc_mem subscribed_HFCLK192MSTART;
  struct nhw_subsc_mem subscribed_HFCLK192MSTOP;
#endif
};

static bs_time_t Timer_PWRCLK = TIME_NEVER;
static struct clkpwr_status nhw_clkpwr_st[NHW_CLKPWR_TOTAL_INST];
union NRF_CLKPWR_Type NRF_CLKPWR_regs[NHW_CLKPWR_TOTAL_INST];

NRF_CLOCK_Type *NRF_CLOCK_regs[NHW_CLKPWR_TOTAL_INST];
NRF_POWER_Type *NRF_POWER_regs[NHW_CLKPWR_TOTAL_INST];
#if (NHW_CLKPWR_HAS_RESET)
NRF_RESET_Type *NRF_RESET_regs[NHW_CLKPWR_TOTAL_INST];
#endif /* (NHW_CLKPWR_HAS_RESET) */

static void nhw_clock_update_master_timer(void) {

  Timer_PWRCLK = TIME_NEVER;

  for (int i = 0; i < NHW_CLKPWR_TOTAL_INST; i++) {
    struct clkpwr_status * c_el = &nhw_clkpwr_st[i];

    bs_time_t t1 = BS_MIN(c_el->Timer_CLOCK_HF, c_el->Timer_CLOCK_LF);
    bs_time_t t2 = BS_MIN(c_el->Timer_LF_cal, c_el->Timer_caltimer);

    bs_time_t el_min = BS_MIN(t1, t2);
    if (el_min < Timer_PWRCLK) {
      Timer_PWRCLK = el_min;
    }
  }

  nsi_hws_find_next_event();
}

static void nhw_clock_init(void) {
#if (NHW_HAS_DPPI)
  /* Mapping of peripheral instance to DPPI instance */
  uint nhw_clkpwr_dppi_map[NHW_CLKPWR_TOTAL_INST] = NHW_CLKPWR_DPPI_MAP;
#endif

  memset(NRF_CLKPWR_regs, 0, sizeof(NRF_CLKPWR_regs));

  for (int i = 0; i < NHW_CLKPWR_TOTAL_INST; i++) {
    struct clkpwr_status * c_el = &nhw_clkpwr_st[i];

    c_el->inst = i;

    NRF_CLOCK_regs[i] = (NRF_CLOCK_Type *)&NRF_CLKPWR_regs[i];
    c_el->CLOCK_regs = (NRF_CLOCK_Type *)&NRF_CLKPWR_regs[i];
    NRF_POWER_regs[i] = (NRF_POWER_Type *)&NRF_CLKPWR_regs[i];
    c_el->POWER_regs = (NRF_POWER_Type *)&NRF_CLKPWR_regs[i];
#if (NHW_CLKPWR_HAS_RESET)
    NRF_RESET_regs[i] = (NRF_RESET_Type *)&NRF_CLKPWR_regs[i];
    c_el->RESET_regs = (NRF_RESET_Type *)&NRF_CLKPWR_regs[i];
#endif /* (NHW_CLKPWR_HAS_RESET) */

    c_el->Timer_CLOCK_LF = TIME_NEVER;
    c_el->Timer_CLOCK_HF = TIME_NEVER;
    c_el->Timer_LF_cal   = TIME_NEVER;
    c_el->Timer_caltimer = TIME_NEVER;

    c_el->HF_Clock_state = Stopped;
    c_el->LF_Clock_state = Stopped;
    c_el->LF_cal_state   = Stopped;
    c_el->caltimer_state = Stopped;
#if (NHW_HAS_DPPI)
    c_el->dppi_map = nhw_clkpwr_dppi_map[i];
#endif

#if defined(CLOCK_HFXODEBOUNCE_HFXODEBOUNCE_Pos)
    NRF_CLOCK_regs[i]->HFXODEBOUNCE = 0x00000010;
#endif
  }
}

NSI_TASK(nhw_clock_init, HW_INIT, 100);

static void nhw_pwrclk_eval_interrupt(int inst) {
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_clock_irq_map[NHW_CLKPWR_TOTAL_INST] = NHW_CLKPWR_INT_MAP;
  static bool clock_int_line[NHW_CLKPWR_TOTAL_INST]; /* Is the CLOCK currently driving its interrupt line high */
  bool new_int_line = false;

  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

#define check_interrupt(x) \
	if (NRF_CLOCK_regs[inst]->EVENTS_##x \
	    && (this->INTEN & CLOCK_INTENCLR_##x##_Msk)){ \
	  new_int_line = true; \
	} \

  check_interrupt(HFCLKSTARTED);
  check_interrupt(LFCLKSTARTED);
  check_interrupt(DONE);
#if (NHW_CLKPWR_HAS_CALTIMER)
  check_interrupt(CTTO);
  check_interrupt(CTSTARTED);
  check_interrupt(CTSTOPPED);
#endif /* NHW_CLKPWR_HAS_CALTIMER */
#if (NHW_CLKPWR_HAS_HFCLKAUDIOCLK)
  check_interrupt(HFCLKAUDIOSTARTED);
#endif
#if (NHW_CLKPWR_HAS_HFCLK192MCLK)
  check_interrupt(HFCLK192MSTARTED);
#endif

  if (clock_int_line[inst] == false && new_int_line == true) {
    clock_int_line[inst] = true;
    hw_irq_ctrl_raise_level_irq_line(nhw_clock_irq_map[inst].cntl_inst,
                                      nhw_clock_irq_map[inst].int_nbr);
  } else if (clock_int_line[inst] == true && new_int_line == false) {
    clock_int_line[inst] = false;
    hw_irq_ctrl_lower_level_irq_line(nhw_clock_irq_map[inst].cntl_inst,
                                      nhw_clock_irq_map[inst].int_nbr);
  }
}

#if (NHW_HAS_PPI)
#define nhw_clock_signal_handler(x)         \
  static void nhw_clock_signal_##x(int i) { \
    NRF_CLOCK_regs[i]->EVENTS_##x = 1;     \
    nhw_pwrclk_eval_interrupt(i);           \
    nrf_ppi_event(CLOCK_EVENTS_##x);       \
  }
#else
#define nhw_clock_signal_handler(x)         \
  static void nhw_clock_signal_##x(int i) { \
    NRF_CLOCK_regs[i]->EVENTS_##x = 1;     \
    nhw_pwrclk_eval_interrupt(i);           \
    nhw_dppi_event_signal_if(nhw_clkpwr_st[i].dppi_map,      \
                             NRF_CLOCK_regs[i]->PUBLISH_##x);\
  }
#endif

/*
 * CLOCK POWER & RESET do not have shortcuts, so all we need to do
 * is set the corresponding event and evaluate the interrupt.
 */
nhw_clock_signal_handler(HFCLKSTARTED)
nhw_clock_signal_handler(LFCLKSTARTED)
nhw_clock_signal_handler(DONE)
#if (NHW_CLKPWR_HAS_CALTIMER)
nhw_clock_signal_handler(CTTO)
nhw_clock_signal_handler(CTSTARTED)
nhw_clock_signal_handler(CTSTOPPED)
#endif /* NHW_CLKPWR_HAS_CALTIMER */
#if (NHW_CLKPWR_HAS_HFCLKAUDIOCLK)
//nhw_clock_signal_handler(HFCLKAUDIOSTARTED)
#endif
#if (NHW_CLKPWR_HAS_HFCLK192MCLK)
//nhw_clock_signal_handler(HFCLK192MSTARTED)
#endif

void nhw_clock_TASKS_LFCLKSTART(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];
  NRF_CLOCK_Type *CLOCK_regs = this->CLOCK_regs;

  CLOCK_regs->LFCLKSRCCOPY = CLOCK_regs->LFCLKSRC & CLOCK_LFCLKSRC_SRC_Msk;
  CLOCK_regs->LFCLKRUN = CLOCK_LFCLKRUN_STATUS_Msk;
  this->LF_Clock_state = Starting;

  this->Timer_CLOCK_LF = nsi_hws_get_time(); //we assume the clock is ready in 1 delta
  nhw_clock_update_master_timer();
}

void nhw_clock_TASKS_LFCLKSTOP(uint inst) {
  // There is no effect of turning the clock off that is actually modeled
  if ((NRF_CLOCK_regs[inst]->LFCLKSTAT & CLOCK_LFCLKRUN_STATUS_Msk) == 0) { /* LCOV_EXCL_START */
    bs_trace_info_line(3, "%s(%u) Triggered LF oscillator stop while the clock was not running "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__, inst);
  } /* LCOV_EXCL_STOP */

  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if ((this->LF_Clock_state == Started) || (this->LF_Clock_state == Starting)) {
    NRF_CLOCK_regs[inst]->LFCLKRUN = 0;
    this->LF_Clock_state = Stopping;
    this->Timer_CLOCK_LF = nsi_hws_get_time(); //we assume the clock is stopped in 1 delta
    nhw_clock_update_master_timer();
  }
}

void nhw_clock_TASKS_HFCLKSTART(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if ((this->HF_Clock_state == Stopped ) || (this->HF_Clock_state == Stopping)) {
    this->HF_Clock_state = Starting;
    NRF_CLOCK_regs[inst]->HFCLKRUN = CLOCK_HFCLKRUN_STATUS_Msk;
    this->Timer_CLOCK_HF = nsi_hws_get_time(); //we assume the clock is ready in 1 delta
    nhw_clock_update_master_timer();
  }
}

void nhw_clock_TASKS_HFCLKSTOP(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if ((this->HF_Clock_state == Started) || (this->HF_Clock_state == Starting)) {
    NRF_CLOCK_regs[inst]->HFCLKRUN = 0;
    this->HF_Clock_state = Stopping;
    this->Timer_CLOCK_HF = nsi_hws_get_time(); //we assume the clock is stopped in 1 delta
    nhw_clock_update_master_timer();
  }
}

#define HAS_CLOCK_CHECK(CLK_NAME)                                                 \
	  bool has_this_clock[NHW_CLKPWR_TOTAL_INST] = NHW_CLKPWR_HAS_##CLK_NAME##CLK_I;\
	  if (!has_this_clock[inst]) {                                                  \
	    bs_trace_error_time_line("CLOCK#%i does not have "                          \
         "this type of clock (" #CLK_NAME ")", inst);                             \
	  }

void nhw_clock_TASKS_HFCLKAUDIOSTART(uint inst) {
  HAS_CLOCK_CHECK(HFCLKAUDIO);

  bs_trace_warning_time_line("%s not yet implemented\n", __func__);
}

void nhw_clock_TASKS_HFCLKAUDIOSTOP(uint inst) {
  HAS_CLOCK_CHECK(HFCLKAUDIO);

  bs_trace_warning_time_line("%s not yet implemented\n", __func__);
}

void nhw_clock_TASKS_HFCLK192MSTART(uint inst) {
  HAS_CLOCK_CHECK(HFCLK192M);

  bs_trace_warning_time_line("%s not yet implemented\n", __func__);
}

void nhw_clock_TASKS_HFCLK192MSTOP(uint inst) {
  HAS_CLOCK_CHECK(HFCLK192M);

  bs_trace_warning_time_line("%s not yet implemented\n", __func__);
}


void nhw_clock_TASKS_CAL(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if (this->HF_Clock_state != Started) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s(%u): Triggered RC oscillator calibration with the HF CLK stopped "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__, inst);
  } /* LCOV_EXCL_STOP */

  this->LF_cal_state = Started; //We don't check for re-triggers, as we are going to be done right away
  this->Timer_LF_cal = nsi_hws_get_time(); //we assume the calibration is done in 1 delta
  nhw_clock_update_master_timer();
}

#if (NHW_CLKPWR_HAS_CALTIMER)
void nhw_clock_TASKS_CTSTART(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if ( this->caltimer_state == Started ) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s(%u) Calibration timer was already running. "
                          "Raising CTSTARTED event immediately. "
                          "Timeout is not affected.\n", __func__, inst);
  } else {  /* LCOV_EXCL_STOP */
    this->caltimer_state = Started;
    this->Timer_caltimer = nsi_hws_get_time() + (bs_time_t)NRF_CLOCK_regs[inst]->CTIV * 250000;
    nhw_clock_update_master_timer();
  }
  nhw_clock_signal_CTSTARTED(inst);
}

void nhw_clock_TASKS_CTSTOP(uint inst) {
  struct clkpwr_status *this = &nhw_clkpwr_st[inst];

  if ( this->caltimer_state == Stopped ) { /* LCOV_EXCL_START */
    bs_trace_info_line(3, "%s(%u) Calibration timer was already stopped. "
                          "Raising CTSTOPPED event immediately.\n", __func__, inst);
  } /* LCOV_EXCL_STOP */
  this->caltimer_state = Stopped;
  this->Timer_caltimer = TIME_NEVER;
  nhw_clock_update_master_timer();
  nhw_clock_signal_CTSTOPPED(inst);
}
#endif /* NHW_CLKPWR_HAS_CALTIMER */

void nhw_clock_regw_sideeffects_INTENSET(uint i) {
  if (NRF_CLOCK_regs[i]->INTENSET) { /* LCOV_EXCL_BR_LINE */
    struct clkpwr_status *this = &nhw_clkpwr_st[i];

    this->INTEN |= NRF_CLOCK_regs[i]->INTENSET;
    NRF_CLOCK_regs[i]->INTENSET = this->INTEN;
    nhw_pwrclk_eval_interrupt(i);
  }
}

void nhw_clock_regw_sideeffects_INTENCLR(uint i) {
  if (NRF_CLOCK_regs[i]->INTENCLR) { /* LCOV_EXCL_BR_LINE */
    struct clkpwr_status *this = &nhw_clkpwr_st[i];

    this->INTEN  &= ~NRF_CLOCK_regs[i]->INTENCLR;
    NRF_CLOCK_regs[i]->INTENSET = this->INTEN;
    NRF_CLOCK_regs[i]->INTENCLR = 0;
    nhw_pwrclk_eval_interrupt(i);
  }
}

#define nhw_clock_regw_sideeffects_TASKS_(x)                   \
  void nhw_clock_regw_sideeffects_TASKS_##x(uint i) {          \
    if (NRF_CLOCK_regs[i]->TASKS_##x) { /* LCOV_EXCL_BR_LINE */\
      NRF_CLOCK_regs[i]->TASKS_##x = 0;                        \
      nhw_clock_TASKS_##x(i);                                  \
    }                                                          \
  }

nhw_clock_regw_sideeffects_TASKS_(LFCLKSTART)
nhw_clock_regw_sideeffects_TASKS_(LFCLKSTOP)
nhw_clock_regw_sideeffects_TASKS_(HFCLKSTART)
nhw_clock_regw_sideeffects_TASKS_(HFCLKSTOP)

#if (NHW_CLKPWR_HAS_HFCLKAUDIOCLK)
nhw_clock_regw_sideeffects_TASKS_(HFCLKAUDIOSTART)
nhw_clock_regw_sideeffects_TASKS_(HFCLKAUDIOSTOP)
#endif
#if (NHW_CLKPWR_HAS_HFCLK192MCLK)
nhw_clock_regw_sideeffects_TASKS_(HFCLK192MSTART)
nhw_clock_regw_sideeffects_TASKS_(HFCLK192MSTOP)
#endif
nhw_clock_regw_sideeffects_TASKS_(CAL)
#if (NHW_CLKPWR_HAS_CALTIMER)
nhw_clock_regw_sideeffects_TASKS_(CTSTART)
nhw_clock_regw_sideeffects_TASKS_(CTSTOP)
#endif /* NHW_CLKPWR_HAS_CALTIMER */

void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint i) {
  nhw_pwrclk_eval_interrupt(i);
}

void nhw_clock_LFTimer_triggered(struct clkpwr_status *this) {
  NRF_CLOCK_Type *CLOCK_regs = this->CLOCK_regs;

  //For simplicity we assume the enable comes at the same instant as the first
  //tick of the clock so we start ticking in this same instant

  if (this->LF_Clock_state == Starting) { /* LCOV_EXCL_BR_LINE */
    CLOCK_regs->LFCLKSTAT = CLOCK_LFCLKSTAT_STATE_Msk
                          | (CLOCK_regs->LFCLKSRCCOPY << CLOCK_LFCLKSTAT_SRC_Pos);

    nhw_clock_signal_LFCLKSTARTED(this->inst);

    nhw_rtc_notify_first_lf_tick();
  } else if (this->LF_Clock_state == Stopping) {
    this->LF_Clock_state = Stopped;
    CLOCK_regs->LFCLKSTAT &= ~CLOCK_LFCLKSTAT_STATE_Msk;
  }

  this->Timer_CLOCK_LF = TIME_NEVER;
  nhw_clock_update_master_timer();
}

#ifndef CLOCK_HFCLKSTAT_SRC_Xtal
#define CLOCK_HFCLKSTAT_SRC_Xtal CLOCK_HFCLKSTAT_SRC_HFXO /* Bit name change from 52 -> 53 series but same meaning*/
#endif

void nhw_clock_HFTimer_triggered(struct clkpwr_status *this) {
  NRF_CLOCK_Type *CLOCK_regs = this->CLOCK_regs;

  if ( this->HF_Clock_state == Starting ){
    this->HF_Clock_state = Started;

    CLOCK_regs->HFCLKSTAT = CLOCK_HFCLKSTAT_STATE_Msk
                               | ( CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

    nhw_clock_signal_HFCLKSTARTED(this->inst);

  } else if ( this->HF_Clock_state == Stopping ){
    this->HF_Clock_state = Stopped;
    CLOCK_regs->HFCLKSTAT = 0;
  }

  this->Timer_CLOCK_HF = TIME_NEVER;
  nhw_clock_update_master_timer();
}

void nhw_clock_LF_cal_triggered(struct clkpwr_status *this) {
  this->LF_cal_state = Stopped;
  this->Timer_LF_cal = TIME_NEVER;
  nhw_clock_update_master_timer();

  nhw_clock_signal_DONE(this->inst);
}

#if (NHW_CLKPWR_HAS_CALTIMER)
void nhw_clock_caltimer_triggered(struct clkpwr_status *this) {

  if (this->caltimer_state != Started) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
  this->caltimer_state = Stopped;
  this->Timer_caltimer = TIME_NEVER;
  nhw_clock_update_master_timer();
  nhw_clock_signal_CTTO(this->inst);
}
#endif /* NHW_CLKPWR_HAS_CALTIMER */

static void nhw_pwrclk_timer_triggered(void) {
  bool any = false;
  for (int i = 0; i < NHW_CLKPWR_TOTAL_INST; i++) {
    struct clkpwr_status * c_el = &nhw_clkpwr_st[i];
    if (Timer_PWRCLK == c_el->Timer_CLOCK_HF) {
      nhw_clock_HFTimer_triggered(c_el);
      any = true;
    } else if (Timer_PWRCLK == c_el->Timer_CLOCK_LF) {
      nhw_clock_LFTimer_triggered(c_el);
      any = true;
    } else if (Timer_PWRCLK == c_el->Timer_LF_cal) {
      nhw_clock_LF_cal_triggered(c_el);
      any = true;
  #if (NHW_CLKPWR_HAS_CALTIMER)
    } else if (Timer_PWRCLK == c_el->Timer_caltimer) {
      nhw_clock_caltimer_triggered(c_el);
      any = true;
  #endif
    }
  }
  if (!any) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
}

NSI_HW_EVENT(Timer_PWRCLK, nhw_pwrclk_timer_triggered, 50);

#if (NHW_HAS_DPPI)

#define NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                 \
  static void nhw_clock_task##TASK_N##_wrap(void* param)                            \
  {                                                                                 \
    nhw_clock_TASKS_##TASK_N((int) param);                                          \
  }                                                                                 \
                                                                                    \
  void nhw_clock_regw_sideeffects_SUBSCRIBE_##TASK_N(uint inst)                     \
  {                                                                                 \
     struct clkpwr_status *this = &nhw_clkpwr_st[inst];                             \
                                                                                    \
     nhw_dppi_common_subscribe_sideeffect(this->dppi_map,                           \
                                          this->CLOCK_regs->SUBSCRIBE_##TASK_N,     \
                                          &this->subscribed_##TASK_N,               \
                                          nhw_clock_task##TASK_N##_wrap,            \
                                          (void*) inst);                            \
  }

NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLKSTART)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLKSTOP)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(LFCLKSTART)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(LFCLKSTOP)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(CAL)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLKAUDIOSTART)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLKAUDIOSTOP)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLK192MSTART)
NRF_CLOCK_REGW_SIDEFFECTS_SUBSCRIBE(HFCLK192MSTOP)

#endif /* NHW_HAS_DPPI */

#if (NHW_HAS_PPI)
void nhw_clock0_TASKS_LFCLKSTART(void) { nhw_clock_TASKS_LFCLKSTART(0); }
void nhw_clock0_TASKS_LFCLKSTOP(void) { nhw_clock_TASKS_LFCLKSTOP(0); }
void nhw_clock0_TASKS_HFCLKSTART(void) { nhw_clock_TASKS_HFCLKSTART(0); }
void nhw_clock0_TASKS_HFCLKSTOP(void) { nhw_clock_TASKS_HFCLKSTOP(0); }
void nhw_clock0_TASKS_CAL(void) { nhw_clock_TASKS_CAL(0); }
void nhw_clock0_TASKS_CTSTART(void) { nhw_clock_TASKS_CTSTART(0); }
void nhw_clock0_TASKS_CTSTOP(void) { nhw_clock_TASKS_CTSTOP(0); }
#endif /* (NHW_HAS_PPI) */
