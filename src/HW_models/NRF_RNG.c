/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RNG — Random number generator
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/rng.html?cp=4_1_0_5_18
 *
 * Very rough model
 *
 * The delay is constant
 */

#include "NRF_RNG.h"
#include <string.h>
#include <stdbool.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_rand_main.h"

NRF_RNG_Type NRF_RNG_regs;
bs_time_t Timer_RNG = TIME_NEVER; //Time when the next random number will be ready

static bool RNG_hw_started = false;
static bool RNG_INTEN = false; //interrupt enable

/**
 * Initialize the RNG model
 */
void nrf_rng_init(){
  memset(&NRF_RNG_regs, 0, sizeof(NRF_RNG_regs));
  RNG_hw_started = false;
  RNG_INTEN = false;
  Timer_RNG = TIME_NEVER;
}

/**
 * Clean up the RNG model before program exit
 */
void nrf_rng_clean_up(){

}

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
  Timer_RNG = tm_get_hw_time() + delay;

  nrf_hw_find_next_timer_to_trigger();
}

/**
 * TASK_START triggered handler
 */
void nrf_rng_task_start(){
  if (RNG_hw_started) {
    return;
  }
  RNG_hw_started = true;

  nrf_rng_schedule_next(true);
}

/**
 * TASK_STOP triggered handler
 */
void nrf_rng_task_stop(){
  RNG_hw_started = false;
  Timer_RNG = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
}


void nrf_rng_regw_sideeffects_TASK_START(){
  if ( NRF_RNG_regs.TASKS_START ) {
    NRF_RNG_regs.TASKS_START = 0;
    nrf_rng_task_start();
  }
}

void nrf_rng_regw_sideeffects_TASK_STOP(){
  if ( NRF_RNG_regs.TASKS_STOP ) {
    NRF_RNG_regs.TASKS_STOP = 0;
    nrf_rng_task_stop();
  }
}

void nrf_rng_regw_sideeffects_INTENSET(){
  if ( NRF_RNG_regs.INTENSET ) {
    RNG_INTEN = true;
  }
}

void nrf_rng_regw_sideeffects_INTENCLEAR(){
  if ( NRF_RNG_regs.INTENCLR ) {
    RNG_INTEN = false;
    NRF_RNG_regs.INTENSET = 0;
    NRF_RNG_regs.INTENCLR = 0;
  }
}

/**
 * Handle any register side effect (by inspecting the registers states)
 * (deprecated)
 */
void nrf_rng_regw_sideeffects(){

  nrf_rng_regw_sideeffects_TASK_START();

  nrf_rng_regw_sideeffects_TASK_STOP();

  nrf_rng_regw_sideeffects_INTENSET();

  nrf_rng_regw_sideeffects_INTENCLEAR();
}

/**
 * Time has come when a new random number is ready
 */
void nrf_rng_timer_triggered(){

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
