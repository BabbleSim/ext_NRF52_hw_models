/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "NRF_CLOCK.h"
#include <string.h>
#include <stdint.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "NRF_RTC.h"
#include "irq_ctrl.h"
#include "irq_sources.h"

/*
 * CLOCK — Clock control
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/clock.html?cp=2_0_0_16#frontpage_clock
 */

//Note: We assume the 32.768 KHz clock does not drift relative to the 64MHz one
// which would only happen if they had the same source or there was a tracking
// and adjustment loop of one based on the other

NRF_CLOCK_Type NRF_CLOCK_regs;
static uint32_t CLOCK_INTEN = 0; //interrupt enable

bs_time_t Timer_CLOCK_LF = TIME_NEVER; //LF clock timer
bs_time_t Timer_CLOCK_HF = TIME_NEVER;
static bool LF_Clock_started = false;

static enum { Stopped, Starting, Started, Stopping} HF_Clock_state = Stopped;

uint64_t next_lf_tick = 0; // when will the LF clock tick with 9 bits per us resolution
// note that this will wrap at ((1e-6/2^9)*2^64)/3600/24/365 ~= 1142 years
//=> this model will break after 1142 years of simulated time

void nrf_clock_init(){
  memset(&NRF_CLOCK_regs, 0, sizeof(NRF_CLOCK_regs));
  LF_Clock_started = false;
  HF_Clock_state = Stopped;
  Timer_CLOCK_LF = TIME_NEVER;
  Timer_CLOCK_HF = TIME_NEVER;
}

void nrf_clock_clean_up(){

}

void nrf_clock_TASKS_LFCLKSTART(){
  NRF_CLOCK_regs.LFCLKSRCCOPY = NRF_CLOCK_regs.LFCLKSRC & CLOCK_LFCLKSRC_SRC_Msk;
  NRF_CLOCK_regs.LFCLKRUN = CLOCK_LFCLKRUN_STATUS_Msk;

  Timer_CLOCK_LF = tm_get_hw_time(); //we assume the clock is ready in 1 delta
  //I guess we could assume some delay here.. but there is little need for this in this controller
  nrf_hw_find_next_timer_to_trigger();

}

void nrf_clock_TASKS_HFCLKSTART(){
  if ( ( HF_Clock_state == Stopped ) || ( HF_Clock_state == Stopping ) ) {
    HF_Clock_state = Starting;
    NRF_CLOCK_regs.HFCLKRUN = CLOCK_HFCLKRUN_STATUS_Msk;
    Timer_CLOCK_HF = tm_get_hw_time(); //we assume the clock is ready in 1 delta
    //I guess we could assume some delay here.. but there is little need for this in this controller
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrf_clock_TASKS_HFCLKSTOP(){
  if ( ( HF_Clock_state == Started ) || ( HF_Clock_state == Starting ) ) {
    NRF_CLOCK_regs.HFCLKRUN = 0;
    HF_Clock_state = Stopping;
    Timer_CLOCK_HF = tm_get_hw_time(); //we assume the clock is ready in 1 delta
    //I guess we could assume some delay here.. but there is little need for this in this controller
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrf_clock_reqw_sideeffects_INTENSET(){
  if ( NRF_CLOCK_regs.INTENSET ){
    CLOCK_INTEN |= NRF_CLOCK_regs.INTENSET;
    NRF_CLOCK_regs.INTENSET = CLOCK_INTEN;
  }
}

void nrf_clock_reqw_sideeffects_INTENCLR(){
  if ( NRF_CLOCK_regs.INTENCLR ){
    CLOCK_INTEN  &= ~NRF_CLOCK_regs.INTENCLR;
    NRF_CLOCK_regs.INTENSET = CLOCK_INTEN;
    NRF_CLOCK_regs.INTENCLR = 0;
  }
}

void nrf_clock_reqw_sideeffects_TASKS_LFCLKSTART(){
  if ( NRF_CLOCK_regs.TASKS_LFCLKSTART ){
    NRF_CLOCK_regs.TASKS_LFCLKSTART = 0;
    nrf_clock_TASKS_LFCLKSTART();
  }
}

void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTART(){
  if ( NRF_CLOCK_regs.TASKS_HFCLKSTART ){
    NRF_CLOCK_regs.TASKS_HFCLKSTART = 0;
    nrf_clock_TASKS_HFCLKSTART();
  }
}

void nrf_clock_reqw_sideeffects_TASKS_HFCLKSTOP(){
  if ( NRF_CLOCK_regs.TASKS_HFCLKSTOP ){
    NRF_CLOCK_regs.TASKS_HFCLKSTOP = 0;
    nrf_clock_TASKS_HFCLKSTOP();
  }
}

void nrf_clock_regw_sideeffects(){

  nrf_clock_reqw_sideeffects_INTENSET();

  nrf_clock_reqw_sideeffects_INTENCLR();

  nrf_clock_reqw_sideeffects_TASKS_LFCLKSTART();

  nrf_clock_reqw_sideeffects_TASKS_HFCLKSTART();
}

void nrf_clock_LFTimer_triggered(){
  //We just assume the enable comes with the first tick of the clock

  if ( LF_Clock_started == false ){
    LF_Clock_started = true;

    NRF_CLOCK_regs.LFCLKSTAT = CLOCK_LFCLKSTAT_STATE_Msk
                          | (NRF_CLOCK_regs.LFCLKSRCCOPY << CLOCK_LFCLKSTAT_SRC_Pos);

    NRF_CLOCK_regs.EVENTS_LFCLKSTARTED = 1;
    nrf_ppi_event(CLOCK_EVENTS_LFCLKSTARTED);
    if ( CLOCK_INTEN & CLOCK_INTENSET_LFCLKSTARTED_Msk ){
      hw_irq_ctrl_set_irq(NRF5_IRQ_POWER_CLOCK_IRQn);
    }

    nrf_rtc_notify_first_lf_tick();
  }

  Timer_CLOCK_LF = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_clock_HFTimer_triggered(){
  if ( HF_Clock_state == Starting ){
    HF_Clock_state = Started;

    NRF_CLOCK_regs.HFCLKSTAT = CLOCK_HFCLKSTAT_STATE_Msk
                               | ( CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

    NRF_CLOCK_regs.EVENTS_HFCLKSTARTED = 1;
    nrf_ppi_event(CLOCK_EVENTS_HFCLKSTARTED);
    if ( CLOCK_INTEN & CLOCK_INTENSET_HFCLKSTARTED_Msk ){
      hw_irq_ctrl_set_irq(NRF5_IRQ_POWER_CLOCK_IRQn);
    }

  } else if ( HF_Clock_state == Stopping ){
    HF_Clock_state = Stopped;
    NRF_CLOCK_regs.HFCLKSTAT = 0;
  }

  Timer_CLOCK_HF = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
}
