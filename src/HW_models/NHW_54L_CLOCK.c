/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * nRF54L
 * CLOCK - Clock control
 * POWER - POWER control
 * RESET - RESET control
 *
 * Note: CLOCK:
 * 1. This model ignores the system ON/OFF status
 *
 * 2. We assume the 32.768 KHz clock does not drift relative to the 64MHz one.
 *    This would only be the case if they had the same source or there was a tracking
 *    and adjustment loop of one based on the other.
 *
 * 3. All tasks complete in 1 delta cycle
 *
 * 4. LFCLK.SRC is ignored (beyond copying LFCLK.SRC to LFCLK.SRCCOPY and LFCLK.STAT)
 *
 * 5. Turning the clocks on/off has no effect on other peripherals models. Other peripherals
 *    do not request clocks to this model.
 *    So triggering a clock STOP task will always be done right away, without needing
 *    to wait for a peripheral which is using that clock to stop requesting it.
 *
 *    There is no relationship to the OSCILLATORS peripheral yet.
 *
 * 6. From the spec, it is unclear if the *.RUN.STATUS register fields are kept on even
 *    after the operation is completed or they are cleared.
 *    The model just leaves them at 1, unless the STOP task is triggered.
 *
 * 7. XOTUNE does nothing more than generate the XOTUNED event in one delta. It cannot fail.
 *    the events XOTUNEERROR and XOTUNEFAILED are never generated.
 *    XOTUNEABORT does nothing.
 *
 * 8. The models do not check the requirement of having the HFXO clock running to be
 *    able to run the RADIO. The RADIO models will run just fine without it.
 *
 * Notes: POWER & RESET
 * * Only a register stub without functionality is provided
 */

#include <string.h>
#include <stdint.h>
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_54L_CLOCK.h"
#include "NHW_templates.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

union NRF_CLKPWR_Type NRF_CLKPWR_regs[NHW_CLKPWR_TOTAL_INST];

NRF_CLOCK_Type *NRF_CLOCK_regs[NHW_CLKPWR_TOTAL_INST];
NRF_POWER_Type *NRF_POWER_regs[NHW_CLKPWR_TOTAL_INST];
NRF_RESET_Type *NRF_RESET_regs[NHW_CLKPWR_TOTAL_INST];

enum clock_states {Stopped = 0, Starting, Started, Stopping};

struct clkpwr_status {
  bs_time_t Timer_XO;
  bs_time_t Timer_PLL;
  bs_time_t Timer_LFCLK;
  bs_time_t Timer_CAL;
  bs_time_t Timer_XOTUNE;

  enum clock_states XO_state;
  enum clock_states LFCLK_state;
  enum clock_states PLL_state;
  enum clock_states CAL_state;
  enum clock_states XOTUNE_state;
};

static bs_time_t Timer_PWRCLK = TIME_NEVER;
static struct clkpwr_status nhw_clkpwr_st;
static uint nhw_CLOCK_dppi_map[] = NHW_CLKPWR_DPPI_MAP;

static void nhw_CLOCK_update_master_timer(void) {

  Timer_PWRCLK = TIME_NEVER;

  bs_time_t t1 = BS_MIN(nhw_clkpwr_st.Timer_XO, nhw_clkpwr_st.Timer_PLL);
  bs_time_t t2 = BS_MIN(nhw_clkpwr_st.Timer_LFCLK, nhw_clkpwr_st.Timer_CAL);

  bs_time_t el_min = BS_MIN(t1, t2);
  if (el_min < Timer_PWRCLK) {
    Timer_PWRCLK = el_min;
  }
  nsi_hws_find_next_event();
}

static void nhw_CLOCK_init(void) {
  NRF_CLOCK_regs[0] = (NRF_CLOCK_Type *)&NRF_CLKPWR_regs[0];
  NRF_POWER_regs[0] = (NRF_POWER_Type *)&NRF_CLKPWR_regs[0];
  NRF_RESET_regs[0] = (NRF_RESET_Type *)&NRF_CLKPWR_regs[0];

  memset(NRF_CLKPWR_regs, 0, sizeof(NRF_CLKPWR_regs));

  nhw_clkpwr_st.Timer_XO    = TIME_NEVER;
  nhw_clkpwr_st.Timer_PLL   = TIME_NEVER;
  nhw_clkpwr_st.Timer_LFCLK = TIME_NEVER;
  nhw_clkpwr_st.Timer_CAL   = TIME_NEVER;
  nhw_clkpwr_st.Timer_XOTUNE= TIME_NEVER;

  nhw_clkpwr_st.XO_state    = Stopped;
  nhw_clkpwr_st.LFCLK_state = Stopped;
  nhw_clkpwr_st.PLL_state   = Stopped;
  nhw_clkpwr_st.CAL_state   = Stopped;
  nhw_clkpwr_st.XOTUNE_state= Stopped;

  nhw_CLOCK_update_master_timer();
}

NSI_TASK(nhw_CLOCK_init, HW_INIT, 100);

static void nhw_CLOCK_eval_interrupt(uint inst) {
  static struct nhw_irq_mapping nhw_CLOCK_irq_map[] = NHW_CLKPWR_INT_MAP;
  static bool clock_int_line; /* Is the CLOCK currently driving its interrupt line high */
  bool new_int_line = false;

  NRF_CLOCK_regs[0]->INTPEND = 0;

  #define check_interrupt(x) \
		if (NRF_CLOCK_regs[0]->EVENTS_ ##x && (NRF_CLOCK_regs[0]->INTEN & CLOCK_INTENSET_## x ##_Msk)){ \
		    new_int_line = 1; \
		    NRF_CLOCK_regs[0]->INTPEND |= CLOCK_INTENSET_## x ##_Msk; \
		}

  check_interrupt(XOSTARTED)
  check_interrupt(PLLSTARTED)
  check_interrupt(LFCLKSTARTED)
  check_interrupt(DONE)
  check_interrupt(XOTUNED)
  check_interrupt(XOTUNEERROR)
  check_interrupt(XOTUNEFAILED)

  hw_irq_ctrl_toggle_level_irq_line_if(&clock_int_line,
                                        new_int_line,
                                        &nhw_CLOCK_irq_map[0]);
}

static void nhw_CLOCK_TASK_XOSTART(uint inst) {
  if ((nhw_clkpwr_st.XO_state == Stopped ) || (nhw_clkpwr_st.XO_state == Stopping)) {
    nhw_clkpwr_st.XO_state = Starting;
    NRF_CLOCK_regs[0]->XO.RUN = CLOCK_XO_RUN_STATUS_Msk;
    nhw_clkpwr_st.Timer_XO = nsi_hws_get_time(); //we assume the clock is ready in 1 delta
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOSTOP(uint inst) {
  if ((nhw_clkpwr_st.XO_state == Started) || (nhw_clkpwr_st.XO_state == Starting)) {
    nhw_clkpwr_st.XO_state = Stopping;
    NRF_CLOCK_regs[0]->XO.RUN = 0;
    nhw_clkpwr_st.Timer_XO = nsi_hws_get_time(); //we assume the clock is stopped in 1 delta
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_PLLSTART(uint inst) {
  if ((nhw_clkpwr_st.PLL_state == Stopped ) || (nhw_clkpwr_st.PLL_state == Stopping)) {
    nhw_clkpwr_st.PLL_state = Starting;
    NRF_CLOCK_regs[0]->PLL.RUN = CLOCK_PLL_RUN_STATUS_Msk;
    nhw_clkpwr_st.Timer_PLL = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_PLLSTOP(uint inst) {
  if ((nhw_clkpwr_st.PLL_state == Started) || (nhw_clkpwr_st.PLL_state == Starting)) {
    nhw_clkpwr_st.PLL_state = Stopping;
    NRF_CLOCK_regs[0]->PLL.RUN = 0;
    nhw_clkpwr_st.Timer_PLL = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_LFCLKSTART(uint inst) {
  if ((nhw_clkpwr_st.LFCLK_state == Stopped ) || (nhw_clkpwr_st.LFCLK_state == Stopping)) {
    nhw_clkpwr_st.LFCLK_state = Starting;
    NRF_CLOCK_regs[0]->LFCLK.RUN = CLOCK_LFCLK_RUN_STATUS_Msk;
    NRF_CLOCK_regs[0]->LFCLK.SRCCOPY = NRF_CLOCK_regs[0]->LFCLK.SRC;
    nhw_clkpwr_st.Timer_LFCLK = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_LFCLKSTOP(uint inst) {
  if ((nhw_clkpwr_st.LFCLK_state == Started) || (nhw_clkpwr_st.LFCLK_state == Starting)) {
    nhw_clkpwr_st.LFCLK_state = Stopping;
    NRF_CLOCK_regs[0]->LFCLK.RUN = 0;
    nhw_clkpwr_st.Timer_LFCLK = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_CAL(uint inst) {
  if (nhw_clkpwr_st.XO_state != Started) { /* LCOV_EXCL_START */
    bs_trace_warning_line("%s: Triggered RC oscillator calibration with the HFXO CLK stopped "
                          "(the model does not have a problem with this, but this is against "
                          "the spec)\n", __func__);
  } /* LCOV_EXCL_STOP */

  if ((nhw_clkpwr_st.CAL_state == Stopped ) || (nhw_clkpwr_st.CAL_state == Stopping)) {
    nhw_clkpwr_st.CAL_state = Starting;
    nhw_clkpwr_st.Timer_CAL = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOTUNE(uint inst) {
  if ((nhw_clkpwr_st.XOTUNE_state == Stopped ) || (nhw_clkpwr_st.XOTUNE_state == Stopping)) {
    nhw_clkpwr_st.XOTUNE_state = Starting;
    nhw_clkpwr_st.Timer_XOTUNE = nsi_hws_get_time();
    nhw_CLOCK_update_master_timer();
  }
}

static void nhw_CLOCK_TASK_XOTUNEABORT(uint inst) {
  /* Deliberately empty by now */
}

NHW_SIDEEFFECTS_INTSET(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)
NHW_SIDEEFFECTS_INTCLR(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)
NHW_SIDEEFFECTS_INTEN(CLOCK, NRF_CLOCK_regs[0]->, NRF_CLOCK_regs[0]->INTEN)

NHW_SIDEEFFECTS_EVENTS(CLOCK)

void nhw_pwrclk_regw_sideeffects_EVENTS_all(uint inst) {
  nhw_CLOCK_eval_interrupt(inst);
}

NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOSTARTED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, PLLSTARTED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTARTED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, DONE)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNED)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEERROR)
NHW_SIGNAL_EVENT(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEFAILED)

NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOSTOP)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, PLLSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, PLLSTOP)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTART)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, LFCLKSTOP)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, CAL)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOTUNE)
NHW_SIDEEFFECTS_TASKS(CLOCK, NRF_CLOCK_regs[0]->, XOTUNEABORT)

#define NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(task) \
  void nhw_CLOCK_regw_sideeffects_SUBSCRIBE_##task(unsigned int inst) { \
    static struct nhw_subsc_mem task##_subscribed[NHW_CLKPWR_TOTAL_INST]; \
    nhw_dppi_common_subscribe_sideeffect(nhw_CLOCK_dppi_map[inst], \
        NRF_CLOCK_regs[0]->SUBSCRIBE_##task, \
        &task##_subscribed[inst], \
        (dppi_callback_t)nhw_CLOCK_TASK_##task, \
        DPPI_CB_NO_PARAM); \
  }

NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOSTOP)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(PLLSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(PLLSTOP)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(LFCLKSTART)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(LFCLKSTOP)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(CAL)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOTUNE)
NHW_CLOCK_SIDEEFFECTS_SUBSCRIBE(XOTUNEABORT)

static void nhw_CLOCK_XOTimer_triggered(void) {
  nhw_clkpwr_st.Timer_XO = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.XO_state == Starting ){
    nhw_clkpwr_st.XO_state = Started;

    NRF_CLOCK_regs[0]->XO.STAT = CLOCK_XO_STAT_STATE_Msk;

    //nhw_CLOCK_signal_EVENTS_XOSTARTED(0);
    nhw_CLOCK_signal_EVENTS_XOTUNED(0);

  } else if ( nhw_clkpwr_st.XO_state == Stopping ){
    nhw_clkpwr_st.XO_state = Stopped;
    NRF_CLOCK_regs[0]->XO.STAT = 0;
  }
}

static void nhw_CLOCK_PLLTimer_triggered(void) {
  nhw_clkpwr_st.Timer_PLL = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.PLL_state == Starting ){
    nhw_clkpwr_st.PLL_state = Started;

    NRF_CLOCK_regs[0]->PLL.STAT = CLOCK_PLL_STAT_STATE_Msk;

    nhw_CLOCK_signal_EVENTS_PLLSTARTED(0);

  } else if ( nhw_clkpwr_st.PLL_state == Stopping ){
    nhw_clkpwr_st.PLL_state = Stopped;
    NRF_CLOCK_regs[0]->PLL.STAT = 0;
  }
}

static void nhw_CLOCK_LFCLK_triggered(void) {
  nhw_clkpwr_st.Timer_LFCLK = TIME_NEVER;
  nhw_CLOCK_update_master_timer();

  if ( nhw_clkpwr_st.LFCLK_state == Starting ){
    nhw_clkpwr_st.LFCLK_state = Started;

    NRF_CLOCK_regs[0]->LFCLK.STAT = CLOCK_LFCLK_STAT_STATE_Msk
                               | (NRF_CLOCK_regs[0]->LFCLK.SRCCOPY << CLOCK_LFCLK_STAT_SRC_Pos);

    nhw_CLOCK_signal_EVENTS_LFCLKSTARTED(0);

  } else if ( nhw_clkpwr_st.LFCLK_state == Stopping ){
    nhw_clkpwr_st.LFCLK_state = Stopped;
    NRF_CLOCK_regs[0]->LFCLK.STAT &= ~CLOCK_LFCLK_STAT_STATE_Msk;
  }
}

static void nhw_CLOCK_CALtimer_triggered(void) {
  nhw_clkpwr_st.CAL_state = Stopped;
  nhw_clkpwr_st.Timer_CAL = TIME_NEVER;
  nhw_CLOCK_update_master_timer();
  nhw_CLOCK_signal_EVENTS_DONE(0);
}

static void nhw_CLOCK_XOTUNEtimer_triggered(void) {
  nhw_clkpwr_st.XOTUNE_state = Stopped;
  nhw_clkpwr_st.Timer_XOTUNE = TIME_NEVER;
  nhw_CLOCK_update_master_timer();
  nhw_CLOCK_signal_EVENTS_XOTUNED(0);
}

static void nhw_pwrclk_timer_triggered(void) {
  if (Timer_PWRCLK == nhw_clkpwr_st.Timer_XO) {
    nhw_CLOCK_XOTimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_PLL) {
    nhw_CLOCK_PLLTimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_LFCLK) {
    nhw_CLOCK_LFCLK_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_CAL) {
    nhw_CLOCK_CALtimer_triggered();
  } else if (Timer_PWRCLK == nhw_clkpwr_st.Timer_XOTUNE) {
    nhw_CLOCK_XOTUNEtimer_triggered();
  } else { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s programming error\n", __func__);
  } /* LCOV_EXCL_STOP */
}

NSI_HW_EVENT(Timer_PWRCLK, nhw_pwrclk_timer_triggered, 50);

//TODO: HAL
