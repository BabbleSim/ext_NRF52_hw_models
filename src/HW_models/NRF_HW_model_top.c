/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_utils.h"
#include "NRF_HW_model_top.h"
#include "bstest_ticker.h"
#include "time_machine_if.h"
#include "NRF_hw_args.h"
#include "NRF_AAR.h"
#include "NRF_RNG.h"
#include "NRF_RTC.h"
#include "NRF_AES_ECB.h"
#include "NRF_AES_CCM.h"
#include "NRF_CLOCK.h"
#include "NRF_RADIO.h"
#include "NRF_FICR.h"
#include "NRF_PPI.h"
#include "NRF_TIMER.h"
#include "irq_ctrl.h"
#include "BLECrypt_if.h"
#include "fake_timer.h"

#define IS_TIMER_VALID( _t_id ) do{ \
                                  if( !Timers[_t_id] ){ \
                                    bs_trace_error_line("Pointer to timer: %s is corrupted\n", Timers_string[_t_id]); \
                                    return; \
                                  } \
                                } while(0);

extern bs_time_t Timer_event_fw_test_ticker;
extern bs_time_t Timer_irq_ctrl;
extern bs_time_t Timer_RNG;
extern bs_time_t Timer_CLOCK_LF;
extern bs_time_t Timer_CLOCK_HF;
extern bs_time_t Timer_RTC;
extern bs_time_t Timer_TIMERs;
extern bs_time_t Timer_RADIO;
extern bs_time_t Timer_RADIO_abort_reeval; //This timer should have the lowest priority
extern bs_time_t Timer_ECB;
extern bs_time_t Timer_AAR;
extern bs_time_t Timer_fake_timer;
extern bs_time_t Timer_RADIO_bitcounter;

static void bad_timer_triggered(void);

/**
 * The events are prioritize from top to bottom
 * (priority == which executes if they have the same timing)
 */
#define NRF_HW_TIMERS_LIST \
  /*        Timer name (enum)          pointer to timer              function to trigger                      */ \
  HW_TIMER( fake_timer,                &Timer_fake_timer,            fake_timer_triggered()                     )\
  HW_TIMER( fw_test_ticker,            &Timer_event_fw_test_ticker,  bst_ticker_triggered(timer_nrf_main_timer) )\
  HW_TIMER( irq_ctrl_timer,            &Timer_irq_ctrl,              hw_irq_ctrl_timer_triggered()              )\
  HW_TIMER( RNG_timer,                 &Timer_RNG,                   nrf_rng_timer_triggered()                  )\
  HW_TIMER( ECB_timer,                 &Timer_ECB,                   nrf_ecb_timer_triggered()                  )\
  HW_TIMER( AAR_timer,                 &Timer_AAR,                   nrf_aar_timer_triggered()                  )\
  HW_TIMER( CLOCK_LF_timer,            &Timer_CLOCK_LF,              nrf_clock_LFTimer_triggered()              )\
  HW_TIMER( CLOCK_HF_timer,            &Timer_CLOCK_HF,              nrf_clock_HFTimer_triggered()              )\
  HW_TIMER( RTC_timer,                 &Timer_RTC,                   nrf_rtc_timer_triggered()                  )\
  HW_TIMER( TIMER_timer,               &Timer_TIMERs,                nrf_hw_model_timer_timer_triggered()       )\
  HW_TIMER( RADIO_timer,               &Timer_RADIO,                 nrf_radio_timer_triggered()                )\
  HW_TIMER( RADIO_bitcounter,          &Timer_RADIO_bitcounter,      nrf_radio_bitcounter_timer_triggered()     )\
  /*        This timer should always be the latest in this list (lowest priority)                             */ \
  HW_TIMER( RADIO_abort_reeval_timer,  &Timer_RADIO_abort_reeval,    nrf_radio_timer_abort_reeval_triggered()   )\
  HW_TIMER( NumberOfNRFHWTimers,       NULL,                         bad_timer_triggered()                      )\
/* ------------------------------------------- */

#define HW_TIMER( name, timer, func ) name,
typedef enum {
  NRF_HW_TIMERS_LIST
} NRF_HW_next_timer_to_trigger_t;
#undef HW_TIMER

#define HW_TIMER( name, timer, func ) #name,
static char *Timers_string[] = {
  NRF_HW_TIMERS_LIST
};
#undef HW_TIMER

/* Indexed with NRF_HW_next_timer_to_trigger_t */
#define HW_TIMER( name, timer, func ) timer,
static bs_time_t *Timers[] = {
  NRF_HW_TIMERS_LIST
};
#undef HW_TIMER

bs_time_t timer_nrf_main_timer = TIME_NEVER; //This timer is exposed to the top level time_machine which will call us when it is reached
static NRF_HW_next_timer_to_trigger_t nrf_hw_next_timer_to_trigger = NumberOfNRFHWTimers;

void nrf_hw_models_free_all() {
  nrf_clock_clean_up();
  nrf_rng_clean_up();
  nrf_rtc_clean_up();
  nrf_aes_ecb_clean_up();
  nrf_aes_ccm_clean_up();
  nrf_aar_clean_up();
  nrf_radio_clean_up();
  nrf_ficr_clean_up();
  nrf_ppi_clean_up();
  nrf_hw_model_timer_clean_up();
}

/*
 * Run any pre-initialization tasks which needs to be done very early
 * Like registering command line arguments or dump files
 */
void nrf_hw_pre_init() {

}

/*
 * Initialize all HW models and set registers to defaults
 */
void nrf_hw_initialize(nrf_hw_sub_args_t *args) {

  BLECrypt_if_enable_real_encryption(args->useRealAES);
  hw_irq_ctrl_init();
  nrf_clock_init();
  nrg_rng_init();
  nrf_rtc_init();
  nrf_aes_ecb_init();
  nrf_aes_ccm_init();
  nrf_aar_init();
  nrf_radio_init();
  nrf_ficr_init();
  nrf_ppi_init();
  nrf_hw_model_timer_init();
  nrf_hw_find_next_timer_to_trigger();
}

/**
 * Look into all timers and update  next_timer_to_trigger* accordingly
 * To be called each time a "timed process" updates its timer
 */
void nrf_hw_find_next_timer_to_trigger() {
  bs_time_t new_timer;
  nrf_hw_next_timer_to_trigger = 0;

  IS_TIMER_VALID(0);

  new_timer = *Timers[0];
  for (uint i = 1; i < NumberOfNRFHWTimers; i++) {
    IS_TIMER_VALID(i);
    if ( new_timer > *Timers[i] ) {
      new_timer = *Timers[i];
      nrf_hw_next_timer_to_trigger = i;
    }
  }
  if ( new_timer != timer_nrf_main_timer ) {
    timer_nrf_main_timer = new_timer;
    tm_find_next_timer_to_trigger();
  }
}

void nrf_hw_some_timer_reached() {
  bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: %s\n",
    Timers_string[nrf_hw_next_timer_to_trigger]);

  switch ( nrf_hw_next_timer_to_trigger ) {
    #define HW_TIMER( name, timer, func ) \
      case name: \
        func; \
      break;
      NRF_HW_TIMERS_LIST
    #undef HW_TIMER
  default:
    bad_timer_triggered();
    break;
  }
}

static void bad_timer_triggered(void) {
  bs_trace_error_line("nrf_hw_next_timer_to_trigger corrupted\n");
}
