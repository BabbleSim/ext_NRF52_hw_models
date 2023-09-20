/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RNG - Random number generator
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/rng.html?cp=5_1_0_5_18
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/rng.html?cp=4_0_0_6_26
 *
 * Compatible with 52833 & 5340
 *
 * Note:
 *   1. The delay to produce a value is constant (though bias correction increases time)
 *
 *   2. The spec says that the delay is unpredictable but it does not provide any indication
 *      of what kind of random distribution to expect => The model just assumes the value is
 *      always the average(?) the spec provides
 *
 *   3. The produced random value is always "good enough".
 *      The bias correction has no effect on the random value quality
 */

#include <string.h>
#include <stdbool.h>
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_common_types.h"
#include "NHW_templates.h"
#include "NHW_RNG.h"
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

#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_RNG_dppi_map[NHW_RNG_TOTAL_INST] = NHW_RNG_DPPI_MAP;
#endif

/**
 * Initialize the RNG model
 */
static void nhw_rng_init(void) {
  memset(&NRF_RNG_regs, 0, sizeof(NRF_RNG_regs));
  RNG_hw_started = false;
  RNG_INTEN = false;
  Timer_RNG = TIME_NEVER;
}

NSI_TASK(nhw_rng_init, HW_INIT, 100);

static void nhw_rng_schedule_next(bool first_time){
  bs_time_t delay = 0;

  if (first_time) {
    delay = NHW_RNG_tRNG_START;
  }

  //See Note 1.
  if (NRF_RNG_regs.CONFIG & RNG_CONFIG_DERCEN_Msk){ //Bias correction enabled
    delay += NHW_RNG_tRNG_BC;
  } else {
    delay += NHW_RNG_tRNG_RAW;
  }
  Timer_RNG = nsi_hws_get_time() + delay;

  nsi_hws_find_next_event();
}

static void nhw_RNG_eval_interrupt(uint inst) {
  static bool rng_int_line[NHW_RNG_TOTAL_INST]; /* Is the RNG currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_rng_irq_map[NHW_RNG_TOTAL_INST] = NHW_RNG_INT_MAP;
  bool new_int_line = false;

  if (NRF_RNG_regs.EVENTS_VALRDY && (RNG_INTEN & RNG_INTENCLR_VALRDY_Msk)){
    new_int_line = true;
  }

  hw_irq_ctrl_toggle_level_irq_line_if(&rng_int_line[inst],
                                       new_int_line,
                                       &nhw_rng_irq_map[inst]);
}

/**
 * TASK_START triggered handler
 */
void nhw_RNG_TASK_START(void) {
  if (RNG_hw_started) {
    return;
  }
  RNG_hw_started = true;
  nhw_rng_schedule_next(true);
}

/**
 * TASK_STOP triggered handler
 */
void nhw_RNG_TASK_STOP(void) {
  RNG_hw_started = false;
  Timer_RNG = TIME_NEVER;
  nsi_hws_find_next_event();
}

NHW_SIDEEFFECTS_TASKS_si(RNG, START)
NHW_SIDEEFFECTS_TASKS_si(RNG, STOP)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RNG, START)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RNG, STOP)
#endif /* NHW_HAS_DPPI */

NHW_SIDEEFFECTS_INTSET_si(RNG, NRF_RNG_regs., RNG_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(RNG, NRF_RNG_regs., RNG_INTEN)

NHW_SIDEEFFECTS_EVENTS(RNG)

static NHW_SIGNAL_EVENT_si(RNG, VALRDY)

static void nhw_RNG_signal_VALRDY(uint periph_inst) {
  if (NRF_RNG_regs.SHORTS & RNG_SHORTS_VALRDY_STOP_Msk) {
    nhw_RNG_TASK_STOP();
  }

  nhw_RNG_signal_EVENTS_VALRDY(periph_inst);
}

/**
 * Time has come when a new random number is ready
 */
static void nhw_rng_timer_triggered(void) {
  //We generate a proper random number even if CONFIG is not set to correct the bias:
  NRF_RNG_regs.VALUE = bs_random_uint32();

  nhw_rng_schedule_next(false);

  nhw_RNG_signal_VALRDY(0);
}

NSI_HW_EVENT(Timer_RNG, nhw_rng_timer_triggered, 50);
