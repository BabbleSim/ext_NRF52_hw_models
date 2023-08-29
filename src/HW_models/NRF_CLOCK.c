/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CLOCK - Clock control
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/clock.html?cp=5_1_0_4_3
 *
 * Notes:
 *
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
 */

#include "NHW_peri_types.h"
#include "NRF_CLOCK.h"
#include <string.h>
#include <stdint.h>
#include "nsi_hw_scheduler.h"
#include "NRF_PPI.h"
#include "NRF_RTC.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

NRF_CLOCK_Type NRF_CLOCK_regs;
static uint32_t CLOCK_INTEN = 0; //interrupt enable

static bs_time_t Timer_CLOCK = TIME_NEVER;

static bs_time_t Timer_CLOCK_LF = TIME_NEVER;
static bs_time_t Timer_CLOCK_HF = TIME_NEVER;
static bs_time_t Timer_LF_cal   = TIME_NEVER;
static bs_time_t Timer_caltimer = TIME_NEVER;

enum clock_states { Stopped, Starting, Started, Stopping};
static enum clock_states HF_Clock_state = Stopped;
static enum clock_states LF_Clock_state = Stopped;
static enum clock_states LF_cal_state = Stopped;
static enum clock_states caltimer_state = Stopped;

static void nrf_clock_update_master_timer(void) {
  bs_time_t t1 = BS_MIN(Timer_CLOCK_HF, Timer_CLOCK_LF);
  bs_time_t t2 = BS_MIN(Timer_LF_cal, Timer_caltimer);
  Timer_CLOCK = BS_MIN(t1, t2);

  nsi_hws_find_next_event();
}

static void nrf_clock_init(void) {
  memset(&NRF_CLOCK_regs, 0, sizeof(NRF_CLOCK_regs));
  NRF_CLOCK_regs.HFXODEBOUNCE = 0x00000010;

  HF_Clock_state = Stopped;
  LF_Clock_state = Stopped;
  LF_cal_state   = Stopped;
  caltimer_state = Stopped;
  Timer_CLOCK_LF = TIME_NEVER;
  Timer_CLOCK_HF = TIME_NEVER;
  Timer_LF_cal   = TIME_NEVER;
  Timer_caltimer = TIME_NEVER;
}

NSI_TASK(nrf_clock_init, HW_INIT, 100);

static void nrf_clock_eval_interrupt(void) {
  static bool clock_int_line; /* Is the CLOCK currently driving its interrupt line high */
  bool new_int_line = false;

#define check_interrupt(x) \
	if (NRF_CLOCK_regs.EVENTS_##x \
	    && (CLOCK_INTEN & CLOCK_INTENCLR_##x##_Msk)){ \
	  new_int_line = true; \
	} \

  check_interrupt(HFCLKSTARTED);
  check_interrupt(LFCLKSTARTED);
  check_interrupt(DONE);
  check_interrupt(CTTO);
  check_interrupt(CTSTARTED);
  check_interrupt(CTSTOPPED);

  if (clock_int_line == false && new_int_line == true) {
    clock_int_line = true;
    hw_irq_ctrl_raise_level_irq_line(POWER_CLOCK_IRQn);
  } else if (clock_int_line == true && new_int_line == false) {
    clock_int_line = false;
    hw_irq_ctrl_lower_level_irq_line(POWER_CLOCK_IRQn);
  }
}

#define nrf_clock_event_handler(x) \
  static void nrf_clock_event_##x(void) { \
    NRF_CLOCK_regs.EVENTS_##x = 1; \
    nrf_clock_eval_interrupt(); \
    nrf_ppi_event(CLOCK_EVENTS_##x);\
  }
/*
 * CLOCK does not have shortcuts, so all we need to do is set the corresponding even
 * and evaluate the interrupt
 */
nrf_clock_event_handler(HFCLKSTARTED)
nrf_clock_event_handler(LFCLKSTARTED)
nrf_clock_event_handler(DONE)
nrf_clock_event_handler(CTTO)
nrf_clock_event_handler(CTSTARTED)
nrf_clock_event_handler(CTSTOPPED)

void nrf_clock_TASKS_LFCLKSTART(void) {
  NRF_CLOCK_regs.LFCLKSRCCOPY = NRF_CLOCK_regs.LFCLKSRC & CLOCK_LFCLKSRC_SRC_Msk;
  NRF_CLOCK_regs.LFCLKRUN = CLOCK_LFCLKRUN_STATUS_Msk;
  LF_Clock_state = Starting;

  Timer_CLOCK_LF = nsi_hws_get_time(); //we assume the clock is ready in 1 delta
  nrf_clock_update_master_timer();
}

void nrf_clock_TASKS_LFCLKSTOP(void) {
  // There is no effect of turning the clock off that is actually modeled
  if ((NRF_CLOCK_regs.LFCLKSTAT & CLOCK_LFCLKRUN_STATUS_Msk) == 0) { /* LCOV_EXCL_START */
    bs_trace_info_line(3, "%s: Triggered LF oscillator stop while the clock was not running "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__);
  } /* LCOV_EXCL_STOP */
  if ((LF_Clock_state == Started) || (LF_Clock_state == Starting)) {
    NRF_CLOCK_regs.LFCLKRUN = 0;
    LF_Clock_state = Stopping;
    Timer_CLOCK_LF = nsi_hws_get_time(); //we assume the clock is stopped in 1 delta
    nrf_clock_update_master_timer();
  }
}

void nrf_clock_TASKS_HFCLKSTART(void) {
  if ( ( HF_Clock_state == Stopped ) || ( HF_Clock_state == Stopping ) ) {
    HF_Clock_state = Starting;
    NRF_CLOCK_regs.HFCLKRUN = CLOCK_HFCLKRUN_STATUS_Msk;
    Timer_CLOCK_HF = nsi_hws_get_time(); //we assume the clock is ready in 1 delta
    nrf_clock_update_master_timer();
  }
}

void nrf_clock_TASKS_HFCLKSTOP(void) {
  if ( ( HF_Clock_state == Started ) || ( HF_Clock_state == Starting ) ) {
    NRF_CLOCK_regs.HFCLKRUN = 0;
    HF_Clock_state = Stopping;
    Timer_CLOCK_HF = nsi_hws_get_time(); //we assume the clock is stopped in 1 delta
    nrf_clock_update_master_timer();
  }
}

void nrf_clock_TASKS_CAL(void) {
  if (HF_Clock_state != Started) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s: Triggered RC oscillator calibration with the HF CLK stopped "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__);
  } /* LCOV_EXCL_STOP */

  LF_cal_state = Started; //We don't check for re-triggers, as we are going to be done right away
  Timer_LF_cal = nsi_hws_get_time(); //we assume the calibration is done in 1 delta
  nrf_clock_update_master_timer();
}

void nrf_clock_TASKS_CTSTART(void) {
  if ( caltimer_state == Started ) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s Calibration timer was already running. "
                          "Raising CTSTARTED event immediately. "
                          "Timeout is not affected.\n", __func__);
  } else {  /* LCOV_EXCL_STOP */
    caltimer_state = Started;
    Timer_caltimer = nsi_hws_get_time() + (bs_time_t)NRF_CLOCK_regs.CTIV * 250000;
    nrf_clock_update_master_timer();
  }
  nrf_clock_event_CTSTARTED();
}

void nrf_clock_TASKS_CTSTOP(void) {
  if ( caltimer_state == Stopped ) { /* LCOV_EXCL_START */
    bs_trace_info_line(3, "%s Calibration timer was already stopped. "
                          "Raising CTSTOPPED event immediately.\n", __func__);
  } /* LCOV_EXCL_STOP */
  caltimer_state = Stopped;
  Timer_caltimer = TIME_NEVER;
  nrf_clock_update_master_timer();
  nrf_clock_event_CTSTOPPED();
}

void nrf_clock_reqw_sideeffects_INTENSET(void) {
  if (NRF_CLOCK_regs.INTENSET) { /* LCOV_EXCL_BR_LINE */
    CLOCK_INTEN |= NRF_CLOCK_regs.INTENSET;
    NRF_CLOCK_regs.INTENSET = CLOCK_INTEN;
    nrf_clock_eval_interrupt();
  }
}

void nrf_clock_reqw_sideeffects_INTENCLR(void) {
  if (NRF_CLOCK_regs.INTENCLR) { /* LCOV_EXCL_BR_LINE */
    CLOCK_INTEN  &= ~NRF_CLOCK_regs.INTENCLR;
    NRF_CLOCK_regs.INTENSET = CLOCK_INTEN;
    NRF_CLOCK_regs.INTENCLR = 0;
    nrf_clock_eval_interrupt();
  }
}

#define nrf_clock_reqw_sideeffects_TASKS_(x)                \
  void nrf_clock_reqw_sideeffects_TASKS_##x(void) {         \
    if (NRF_CLOCK_regs.TASKS_##x) { /* LCOV_EXCL_BR_LINE */ \
      NRF_CLOCK_regs.TASKS_##x = 0;                         \
      nrf_clock_TASKS_##x();                                \
    }                                                       \
  }


nrf_clock_reqw_sideeffects_TASKS_(LFCLKSTART)
nrf_clock_reqw_sideeffects_TASKS_(LFCLKSTOP)
nrf_clock_reqw_sideeffects_TASKS_(HFCLKSTART)
nrf_clock_reqw_sideeffects_TASKS_(HFCLKSTOP)
nrf_clock_reqw_sideeffects_TASKS_(CAL)
nrf_clock_reqw_sideeffects_TASKS_(CTSTART)
nrf_clock_reqw_sideeffects_TASKS_(CTSTOP)

void nrf_clock_regw_sideeffects_EVENTS_all(void) {
  nrf_clock_eval_interrupt();
}

void nrf_clock_LFTimer_triggered(void) {
  //For simplicity we assume the enable comes at the same instant as the first
  //tick of the clock so we start ticking in this same instant

  if (LF_Clock_state == Starting) { /* LCOV_EXCL_BR_LINE */
    NRF_CLOCK_regs.LFCLKSTAT = CLOCK_LFCLKSTAT_STATE_Msk
                          | (NRF_CLOCK_regs.LFCLKSRCCOPY << CLOCK_LFCLKSTAT_SRC_Pos);

    nrf_clock_event_LFCLKSTARTED();

    nrf_rtc_notify_first_lf_tick();
  } else if (LF_Clock_state == Stopping) {
    LF_Clock_state = Stopped;
    NRF_CLOCK_regs.LFCLKSTAT &= ~CLOCK_LFCLKSTAT_STATE_Msk;
  }

  Timer_CLOCK_LF = TIME_NEVER;
  nrf_clock_update_master_timer();
}

void nrf_clock_HFTimer_triggered(void) {
  if ( HF_Clock_state == Starting ){
    HF_Clock_state = Started;

    NRF_CLOCK_regs.HFCLKSTAT = CLOCK_HFCLKSTAT_STATE_Msk
                               | ( CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

    nrf_clock_event_HFCLKSTARTED();

  } else if ( HF_Clock_state == Stopping ){
    HF_Clock_state = Stopped;
    NRF_CLOCK_regs.HFCLKSTAT = 0;
  }

  Timer_CLOCK_HF = TIME_NEVER;
  nrf_clock_update_master_timer();
}

void nrf_clock_LF_cal_triggered(void) {
  LF_cal_state = Stopped;
  Timer_LF_cal = TIME_NEVER;
  nrf_clock_update_master_timer();

  nrf_clock_event_DONE();
}

void nrf_clock_caltimer_triggered(void) {
  if (caltimer_state != Started) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
  caltimer_state = Stopped;
  Timer_caltimer = TIME_NEVER;
  nrf_clock_update_master_timer();
  nrf_clock_event_CTTO();
}

static void nrf_clock_timer_triggered(void) {
  if (Timer_CLOCK == Timer_CLOCK_HF) {
    nrf_clock_HFTimer_triggered();
  } else if (Timer_CLOCK == Timer_CLOCK_LF) {
    nrf_clock_LFTimer_triggered();
  } else if (Timer_CLOCK == Timer_LF_cal) {
    nrf_clock_LF_cal_triggered();
  } else if (Timer_CLOCK == Timer_caltimer) {
    nrf_clock_caltimer_triggered();
  } else { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
}

NSI_HW_EVENT(Timer_CLOCK, nrf_clock_timer_triggered, 50);
