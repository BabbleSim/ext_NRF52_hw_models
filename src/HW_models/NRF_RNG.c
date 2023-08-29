/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RNG - Random number generator
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/rng.html?cp=5_1_0_5_18
 *
 * Very rough model
 *
 * The delay is constant
 */

#include <string.h>
#include <stdbool.h>
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_common_types.h"
#include "NRF_RNG.h"
#include "NHW_xPPI.h"
#include "nsi_hw_scheduler.h"
#include "irq_ctrl.h"
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

NRF_RNG_Type NRF_RNG_regs;
static bs_time_t Timer_RNG = TIME_NEVER; //Time when the next random number will be ready

static bool RNG_hw_started = false;
static bool RNG_INTEN = false; //interrupt enable

/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_rng_irq_map[NHW_RNG_TOTAL_INST] = NHW_RNG_INT_MAP;
#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_rng_dppi_map[NHW_RNG_TOTAL_INST] = NHW_RNG_DPPI_MAP;
#endif

static void nhw_rng_eval_interrupt(uint inst);

/**
 * Initialize the RNG model
 */
static void nrf_rng_init(void) {
  memset(&NRF_RNG_regs, 0, sizeof(NRF_RNG_regs));
  RNG_hw_started = false;
  RNG_INTEN = false;
  Timer_RNG = TIME_NEVER;
}

NSI_TASK(nrf_rng_init, HW_INIT, 100);

static void nrf_rng_schedule_next(bool first_time){
  bs_time_t delay = 0;

  if ( first_time ) {
    delay = 128;
  }

  if ( NRF_RNG_regs.CONFIG ){ //Bias correction enabled
    delay += 120;
    /*
     * The spec says that the delay is unpredictable yet it does not
     * provide any indication of what kind of random distribution to
     * expect => I just assume the value is always the average(?) they
     * provide
     */
  } else {
    delay += 30;
  }
  Timer_RNG = nsi_hws_get_time() + delay;

  nsi_hws_find_next_event();
}

/**
 * TASK_START triggered handler
 */
void nrf_rng_task_start(void) {
  if (RNG_hw_started) {
    return;
  }
  RNG_hw_started = true;

  nrf_rng_schedule_next(true);
}

/**
 * TASK_STOP triggered handler
 */
void nrf_rng_task_stop(void) {
  RNG_hw_started = false;
  Timer_RNG = TIME_NEVER;
  nsi_hws_find_next_event();
}


void nrf_rng_regw_sideeffects_TASK_START(void) {
  if (NRF_RNG_regs.TASKS_START) { /* LCOV_EXCL_BR_LINE */
    NRF_RNG_regs.TASKS_START = 0;
    nrf_rng_task_start();
  }
}

void nrf_rng_regw_sideeffects_TASK_STOP(void) {
  if (NRF_RNG_regs.TASKS_STOP) { /* LCOV_EXCL_BR_LINE */
    NRF_RNG_regs.TASKS_STOP = 0;
    nrf_rng_task_stop();
  }
}

void nrf_rng_regw_sideeffects_INTENSET(void) {
  if (NRF_RNG_regs.INTENSET) { /* LCOV_EXCL_BR_LINE */
    RNG_INTEN |= NRF_RNG_regs.INTENSET;
    NRF_RNG_regs.INTENSET = RNG_INTEN;
    nhw_rng_eval_interrupt(0);
  }
}

void nrf_rng_regw_sideeffects_INTENCLEAR(void) {
  if (NRF_RNG_regs.INTENCLR) { /* LCOV_EXCL_BR_LINE */
    RNG_INTEN &= ~NRF_RNG_regs.INTENCLR;
    NRF_RNG_regs.INTENSET = RNG_INTEN;
    NRF_RNG_regs.INTENCLR = 0;
    nhw_rng_eval_interrupt(0);
  }
}

void nrf_rng_regw_sideeffects_EVENTS_all(void) {
  nhw_rng_eval_interrupt(0);
}

static void nhw_rng_eval_interrupt(uint inst) {
  static bool rng_int_line[NHW_RNG_TOTAL_INST]; /* Is the RNG currently driving its interrupt line high */
  bool new_int_line = false;

  if (NRF_RNG_regs.EVENTS_VALRDY && (RNG_INTEN & RNG_INTENCLR_VALRDY_Msk)){
    new_int_line = true;
  }

  if (rng_int_line[inst] == false && new_int_line == true) {
    rng_int_line[inst] = true;
    nhw_irq_ctrl_raise_level_irq_line(nhw_rng_irq_map[inst].cntl_inst,
                                      nhw_rng_irq_map[inst].int_nbr);
  } else if (rng_int_line[inst] == true && new_int_line == false) {
    rng_int_line[inst] = false;
    nhw_irq_ctrl_lower_level_irq_line(nhw_rng_irq_map[inst].cntl_inst,
                                      nhw_rng_irq_map[inst].int_nbr);
  }
}

static void nhw_rng_signal_VALRDY(uint periph_inst) {
  if (NRF_RNG_regs.SHORTS & 1) {
    nrf_rng_task_stop();
  }

  NRF_RNG_regs.EVENTS_VALRDY = 1;

  nhw_rng_eval_interrupt(periph_inst);

#if (NHW_HAS_PPI)
  nrf_ppi_event(RNG_EVENTS_VALRDY);
#elif (NHW_HAS_DPPI)
  nhw_dppi_event_signal_if(nhw_rng_dppi_map[periph_inst],
                           NRF_RNG_regs.PUBLISH_VALRDY);
#endif
}

/**
 * Time has come when a new random number is ready
 */
static void nrf_rng_timer_triggered(void){

  NRF_RNG_regs.VALUE = bs_random_uint32();
  //A proper random number even if CONFIG is not set to correct the bias

  nrf_rng_schedule_next(false);

  nhw_rng_signal_VALRDY(0);
}

NSI_HW_EVENT(Timer_RNG, nrf_rng_timer_triggered, 50);
