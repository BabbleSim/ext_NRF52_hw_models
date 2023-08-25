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

#include "NRF_RNG.h"
#include <string.h>
#include <stdbool.h>
#include "nsi_hw_scheduler.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

NRF_RNG_Type NRF_RNG_regs;
static bs_time_t Timer_RNG = TIME_NEVER; //Time when the next random number will be ready

static bool RNG_hw_started = false;
static bool RNG_INTEN = false; //interrupt enable

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
  if ( NRF_RNG_regs.TASKS_START ) {
    NRF_RNG_regs.TASKS_START = 0;
    nrf_rng_task_start();
  }
}

void nrf_rng_regw_sideeffects_TASK_STOP(void) {
  if ( NRF_RNG_regs.TASKS_STOP ) {
    NRF_RNG_regs.TASKS_STOP = 0;
    nrf_rng_task_stop();
  }
}

void nrf_rng_regw_sideeffects_INTENSET(void) {
  if ( NRF_RNG_regs.INTENSET ) {
    RNG_INTEN = true;
  }
}

void nrf_rng_regw_sideeffects_INTENCLEAR(void) {
  if ( NRF_RNG_regs.INTENCLR ) {
    RNG_INTEN = false;
    NRF_RNG_regs.INTENSET = 0;
    NRF_RNG_regs.INTENCLR = 0;
  }
}

/**
 * Time has come when a new random number is ready
 */
static void nrf_rng_timer_triggered(void){

  NRF_RNG_regs.VALUE = bs_random_uint32();
  //A proper random number even if CONFIG is not set to correct the bias

  if ( NRF_RNG_regs.SHORTS & 1 ) {
    nrf_rng_task_stop();
  } else {
    nrf_rng_schedule_next(false);
  }

  NRF_RNG_regs.EVENTS_VALRDY = 1;
  nrf_ppi_event(RNG_EVENTS_VALRDY);
  if ( RNG_INTEN ){
    hw_irq_ctrl_set_irq(RNG_IRQn);
    //Note: there is no real need to delay the interrupt a delta
  }
}

NSI_HW_EVENT(Timer_RNG, nrf_rng_timer_triggered, 50);
