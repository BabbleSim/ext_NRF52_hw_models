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

void nrf_hw_models_free_all(){
  nrf_clock_clean_up();
  nrf_rng_clean_up();
  nrf_rtc_clean_up();
  nrf_aes_ecb_clean_up();
  nrf_aes_ccm_clean_up();
  nrf_aar_clean_up();
  nrf_radio_clean_up();
  nrf_ficr_clean_up();
  nrf_ppi_clean_up();
  nrf_timer_clean_up();
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
void nrf_hw_initialize(nrf_hw_sub_args_t *args){

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
  nrf_timer_init();
  nrf_hw_find_next_timer_to_trigger();
}

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

//The events priorities are as in this enum from top to bottom
// (priority == which executes if they have the same timing)
typedef enum {
  fake_timer,
  fw_test_ticker,
  irq_ctrl_timer,
  RNG_timer,
  ECB_timer,
  AAR_timer,
  CLOCK_LF_timer,
  CLOCK_HF_timer,
  RTC_timer,
  TIMER_timer,
  RADIO_timer,
  RADIO_bitcounter,
  RADIO_abort_reeval_timer,
  NumberOfNRFHWTimers,
  None
} NRF_HW_next_timer_to_trigger_t;
static bs_time_t *Timers[NumberOfNRFHWTimers] = { //Indexed with NRF_HW_next_timer_to_trigger_t
    &Timer_fake_timer,
    &Timer_event_fw_test_ticker,
    &Timer_irq_ctrl,
    &Timer_RNG,
    &Timer_ECB,
    &Timer_AAR,
    &Timer_CLOCK_LF,
    &Timer_CLOCK_HF,
    &Timer_RTC,
    &Timer_TIMERs,
    &Timer_RADIO,
    &Timer_RADIO_bitcounter,
    &Timer_RADIO_abort_reeval //This timer should always be the latest in this list (lowest priority)
};

bs_time_t timer_nrf_main_timer = TIME_NEVER; //This timer is exposed to the top level time_machine which will call us when it is reached
static NRF_HW_next_timer_to_trigger_t nrf_hw_next_timer_to_trigger = None;

/**
 * Look into all timers and update  next_timer_to_trigger* accordingly
 * To be called each time a "timed process" updates its timer
 */
void nrf_hw_find_next_timer_to_trigger(){
  bs_time_t new_timer = *Timers[0];
  nrf_hw_next_timer_to_trigger = 0;

  for (uint i = 1; i < NumberOfNRFHWTimers ; i++){
    if ( new_timer > *Timers[i] ) {
      new_timer = *Timers[i];
      nrf_hw_next_timer_to_trigger = i;
    }
  }
  if ( new_timer != timer_nrf_main_timer ){
    timer_nrf_main_timer = new_timer;
    tm_find_next_timer_to_trigger();
  }
}

void nrf_hw_some_timer_reached() {

  switch ( nrf_hw_next_timer_to_trigger ) {
  case fake_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: fake timer\n");
    fake_timer_triggered();
    break;
  case fw_test_ticker:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: FW test ticker\n");
    bst_ticker_triggered(timer_nrf_main_timer);
    break;
  case irq_ctrl_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: IRQ ctrl timer\n");
    hw_irq_ctrl_timer_triggered();
    break;
  case RNG_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: RNG timer\n");
    nrf_rng_timer_triggered();
    break;
  case ECB_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: ECB timer\n");
    nrf_ecb_timer_triggered();
    break;
  case AAR_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: AAR timer\n");
    nrf_aar_timer_triggered();
    break;
  case CLOCK_LF_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: CLOCK LF timer\n");
    nrf_clock_LFTimer_triggered();
    break;
  case CLOCK_HF_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: CLOCK HF timer\n");
    nrf_clock_HFTimer_triggered();
    break;
  case RTC_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: RTC timer\n");
    nrf_rtc_timer_triggered();
    break;
  case TIMER_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: TIMERx timer\n");
    nrf_timer_timer_triggered();
    break;
  case RADIO_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: RADIO timer\n");
    nrf_radio_timer_triggered();
    break;
  case RADIO_bitcounter:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: RADIO bitcounter timer\n");
    nrf_radio_bitcounter_timer_triggered();
    break;
  case RADIO_abort_reeval_timer:
    bs_trace_raw_manual_time(8, tm_get_abs_time(),"NRF HW: RADIO abort reeval timer\n");
    nrf_radio_timer_abort_reeval_triggered();
    break;
  default:
    bs_trace_error_line("nrf_hw_next_timer_to_trigger corrupted\n");
    break;
  }
}
