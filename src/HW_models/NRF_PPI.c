/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "NRF_AAR.h"
#include "NRF_AES_CCM.h"
#include "NRF_RNG.h"
#include "NRF_PPI.h"
#include "NRF_RTC.h"
#include "NRF_TIMER.h"
#include "NRF_CLOCK.h"
#include "NRF_RADIO.h"
#include "bs_tracing.h"

/*
 * PPI — Programmable peripheral interconnect
 * http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fchips%2Fnrf52840_ps.html&cp=2_0_0
 *
 */

NRF_PPI_Type NRF_PPI_regs; ///< The PPI registers

/**
 * PPI module own TASKs handlers
 */
void nrf_ppi_TASK_CHG_ENDIS( int groupnbr, bool enable /*false=disable task*/ ){
  if ( enable ){
    bs_trace_raw_time(9, "ppi: Channel group %i enabled\n", groupnbr);
    NRF_PPI_regs.CHEN |= NRF_PPI_regs.CHG[groupnbr];
  } else {
    bs_trace_raw_time(9 ,"ppi: Channel group %i disable\n", groupnbr);
    NRF_PPI_regs.CHEN &= ~NRF_PPI_regs.CHG[groupnbr];
  }
  //Note of the author: From the spec I cannot guess if these tasks will affect
  //CHEN or a separate hidden register
}
void nrf_ppi_TASK_CHG0_EN() { nrf_ppi_TASK_CHG_ENDIS(0,true); }
void nrf_ppi_TASK_CHG1_EN() { nrf_ppi_TASK_CHG_ENDIS(1,true); }
void nrf_ppi_TASK_CHG2_EN() { nrf_ppi_TASK_CHG_ENDIS(2,true); }
void nrf_ppi_TASK_CHG3_EN() { nrf_ppi_TASK_CHG_ENDIS(3,true); }
void nrf_ppi_TASK_CHG4_EN() { nrf_ppi_TASK_CHG_ENDIS(4,true); }
void nrf_ppi_TASK_CHG5_EN() { nrf_ppi_TASK_CHG_ENDIS(5,true); }
void nrf_ppi_TASK_CHG0_DIS(){ nrf_ppi_TASK_CHG_ENDIS(0,false); }
void nrf_ppi_TASK_CHG1_DIS(){ nrf_ppi_TASK_CHG_ENDIS(1,false); }
void nrf_ppi_TASK_CHG2_DIS(){ nrf_ppi_TASK_CHG_ENDIS(2,false); }
void nrf_ppi_TASK_CHG3_DIS(){ nrf_ppi_TASK_CHG_ENDIS(3,false); }
void nrf_ppi_TASK_CHG4_DIS(){ nrf_ppi_TASK_CHG_ENDIS(4,false); }
void nrf_ppi_TASK_CHG5_DIS(){ nrf_ppi_TASK_CHG_ENDIS(5,false); }

typedef void (*dest_f_t)(void); ///<Syntactic sugar for task function pointer

typedef struct{
  uint32_t channels_mask; //bitmask indicating which channel the event is mapped to
} ppi_event_to_ch_t;
///Table contain which channels each event is activating (one entry per event)
static ppi_event_to_ch_t ppi_evt_to_ch[NUMBER_PPI_EVENTS];

typedef struct {
  dest_f_t tep_f;
  dest_f_t fork_tep_f;
} ppi_channel_tasks_t;
///Table with TASKs each channel activates
static ppi_channel_tasks_t ppi_ch_tasks[NUMBER_PPI_CHANNELS];

typedef struct {
  void *task_addr;
  dest_f_t dest; //function to be called when task is triggered
} ppi_tasks_table_t;

/**
 * Table of TASKs addresses (as provided by the SW) vs the model function
 * pointer (which handles the task trigger)
 */
static const ppi_tasks_table_t ppi_tasks_table[]={ //just the ones we may use
    //POWER CLOCK:
    //{ (void*)&NRF_CLOCK_regs.TASKS_LFCLKSTART , nrf_clock_TASKS_LFCLKSTART},
    { (void*)&NRF_CLOCK_regs.TASKS_HFCLKSTART , nrf_clock_TASKS_HFCLKSTART},
    //{ (void*)&NRF_CLOCK_regs.TASKS_HFCLKSTOP , nrf_clock_TASKS_HFCLKSTOP},

    //RADIO:
    { (void*)&NRF_RADIO_regs.TASKS_TXEN,         nrf_radio_tasks_txen},
    { (void*)&NRF_RADIO_regs.TASKS_RXEN,         nrf_radio_tasks_rxen},
    //{ (void*)&NRF_RADIO_regs.TASKS_START,        nrf_radio_tasks_start},
    //{ (void*)&NRF_RADIO_regs.TASKS_STOP,         nrf_radio_tasks_stop},
    { (void*)&NRF_RADIO_regs.TASKS_DISABLE,      nrf_radio_tasks_disable},
    //{ (void*)&NRF_RADIO_regs.TASKS_RSSISTART,    nrf_radio_tasks_rssistart},
    //{ (void*)&NRF_RADIO_regs.TASKS_RSSISTOP,     nrf_radio_tasks_rssistop},
    //{ (void*)&NRF_RADIO_regs.TASKS_BCSTART,      nrf_radio_tasks_bcstart},
    //{ (void*)&NRF_RADIO_regs.TASKS_BCSTOP,       nrf_radio_tasks_bcstop},

    //TIMER0:
    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[0],  nrf_timer0_TASK_CAPTURE_0},
    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[1],  nrf_timer0_TASK_CAPTURE_1},
    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[2],  nrf_timer0_TASK_CAPTURE_2},
    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[3],  nrf_timer0_TASK_CAPTURE_3},
//    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[4],  nrf_timer0_TASK_CAPTURE_4},
//    { (void*)&NRF_TIMER_regs[0].TASKS_CAPTURE[5],  nrf_timer0_TASK_CAPTURE_5},
//    { (void*)&NRF_TIMER_regs[0].TASKS_CLEAR,       nrf_timer0_TASK_CLEAR},
//    { (void*)&NRF_TIMER_regs[0].TASKS_COUNT,       nrf_timer0_TASK_COUNT},
    { (void*)&NRF_TIMER_regs[0].TASKS_START,       nrf_timer0_TASK_START},
//    { (void*)&NRF_TIMER_regs[0].TASKS_STOP,        nrf_timer0_TASK_STOP},
    //TIMER1:
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[0],  nrf_timer1_TASK_CAPTURE_0},
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[1],  nrf_timer1_TASK_CAPTURE_1},
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[2],  nrf_timer1_TASK_CAPTURE_2},
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[3],  nrf_timer1_TASK_CAPTURE_3},
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[4],  nrf_timer1_TASK_CAPTURE_4},
//    { (void*)&NRF_TIMER_regs[1].TASKS_CAPTURE[5],  nrf_timer1_TASK_CAPTURE_5},
    { (void*)&NRF_TIMER_regs[1].TASKS_CLEAR,       nrf_timer1_TASK_CLEAR},
//    { (void*)&NRF_TIMER_regs[1].TASKS_COUNT,       nrf_timer1_TASK_COUNT},
//    { (void*)&NRF_TIMER_regs[1].TASKS_START,       nrf_timer1_TASK_START},
//    { (void*)&NRF_TIMER_regs[1].TASKS_STOP,        nrf_timer1_TASK_STOP},
    //TIMER2:
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[0],  nrf_timer2_TASK_CAPTURE_0},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[1],  nrf_timer2_TASK_CAPTURE_1},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[2],  nrf_timer2_TASK_CAPTURE_2},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[3],  nrf_timer2_TASK_CAPTURE_3},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[4],  nrf_timer2_TASK_CAPTURE_4},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CAPTURE[5],  nrf_timer2_TASK_CAPTURE_5},
//    { (void*)&NRF_TIMER_regs[2].TASKS_CLEAR,       nrf_timer2_TASK_CLEAR},
//    { (void*)&NRF_TIMER_regs[2].TASKS_COUNT,       nrf_timer2_TASK_COUNT},
//    { (void*)&NRF_TIMER_regs[2].TASKS_START,       nrf_timer2_TASK_START},
//    { (void*)&NRF_TIMER_regs[2].TASKS_STOP,        nrf_timer2_TASK_STOP},
    //TIMER3:
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[0],  nrf_timer3_TASK_CAPTURE_0},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[1],  nrf_timer3_TASK_CAPTURE_1},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[2],  nrf_timer3_TASK_CAPTURE_2},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[3],  nrf_timer3_TASK_CAPTURE_3},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[4],  nrf_timer3_TASK_CAPTURE_4},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CAPTURE[5],  nrf_timer3_TASK_CAPTURE_5},
//    { (void*)&NRF_TIMER_regs[3].TASKS_CLEAR,       nrf_timer3_TASK_CLEAR},
//    { (void*)&NRF_TIMER_regs[3].TASKS_COUNT,       nrf_timer3_TASK_COUNT},
//    { (void*)&NRF_TIMER_regs[3].TASKS_START,       nrf_timer3_TASK_START},
//    { (void*)&NRF_TIMER_regs[3].TASKS_STOP,        nrf_timer3_TASK_STOP},
    //TIMER4:
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[0],  nrf_timer4_TASK_CAPTURE_0},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[1],  nrf_timer4_TASK_CAPTURE_1},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[2],  nrf_timer4_TASK_CAPTURE_2},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[3],  nrf_timer4_TASK_CAPTURE_3},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[4],  nrf_timer4_TASK_CAPTURE_4},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CAPTURE[5],  nrf_timer4_TASK_CAPTURE_5},
//    { (void*)&NRF_TIMER_regs[4].TASKS_CLEAR,       nrf_timer4_TASK_CLEAR},
//    { (void*)&NRF_TIMER_regs[4].TASKS_COUNT,       nrf_timer4_TASK_COUNT},
//    { (void*)&NRF_TIMER_regs[4].TASKS_START,       nrf_timer4_TASK_START},
//    { (void*)&NRF_TIMER_regs[4].TASKS_STOP,        nrf_timer4_TASK_STOP},

    //RTC:
    //{ (void*)&(NRF_RTC_regs[0]).TASKS_CLEAR, nrf_rtc0_TASKS_CLEAR},
    //{ (void*)&(NRF_RTC_regs[0]).TASKS_START, nrf_rtc0_TASKS_START},
    //{ (void*)&(NRF_RTC_regs[0]).TASKS_STOP , nrf_rtc0_TASKS_STOP},
    //{ (void*)&(NRF_RTC_regs[0]).TASKS_TRIGOVRFLW , nrf_rtc0_TASKS_TRIGOVRFLW},
    //{ (void*)&(NRF_RTC_regs[1]).TASKS_CLEAR, nrf_rtc1_TASKS_CLEAR},
    //{ (void*)&(NRF_RTC_regs[1]).TASKS_START, nrf_rtc1_TASKS_START},
    //{ (void*)&(NRF_RTC_regs[1]).TASKS_STOP , nrf_rtc1_TASKS_STOP},
    //{ (void*)&(NRF_RTC_regs[1]).TASKS_TRIGOVRFLW , nrf_rtc1_TASKS_TRIGOVRFLW},
    //{ (void*)&(NRF_RTC_regs[2]).TASKS_CLEAR, nrf_rtc2_TASKS_CLEAR},
    //{ (void*)&(NRF_RTC_regs[2]).TASKS_START, nrf_rtc2_TASKS_START},
    //{ (void*)&(NRF_RTC_regs[2]).TASKS_STOP , nrf_rtc2_TASKS_STOP},
    //{ (void*)&(NRF_RTC_regs[2]).TASKS_TRIGOVRFLW , nrf_rtc2_TASKS_TRIGOVRFLW},

    //RNG:
    { (void*)&NRF_RNG_regs.TASKS_START, nrf_rng_task_start},
    { (void*)&NRF_RNG_regs.TASKS_STOP , nrf_rng_task_stop},


    //ECB

    //AAR
    { (void*)&NRF_AAR_regs.TASKS_START , nrf_aar_TASK_START},

    //CCM
    { (void*)&NRF_CCM_regs.TASKS_CRYPT , nrf_ccm_TASK_CRYPT},

    //PPI:
    { (void*)&NRF_PPI_regs.TASKS_CHG[0].EN,  nrf_ppi_TASK_CHG0_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[1].EN,  nrf_ppi_TASK_CHG1_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[2].EN,  nrf_ppi_TASK_CHG2_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[3].EN,  nrf_ppi_TASK_CHG3_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[4].EN,  nrf_ppi_TASK_CHG4_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[5].EN,  nrf_ppi_TASK_CHG5_EN},
    { (void*)&NRF_PPI_regs.TASKS_CHG[0].DIS, nrf_ppi_TASK_CHG0_DIS},
    { (void*)&NRF_PPI_regs.TASKS_CHG[1].DIS, nrf_ppi_TASK_CHG1_DIS},
    { (void*)&NRF_PPI_regs.TASKS_CHG[2].DIS, nrf_ppi_TASK_CHG2_DIS},
    { (void*)&NRF_PPI_regs.TASKS_CHG[3].DIS, nrf_ppi_TASK_CHG3_DIS},
    { (void*)&NRF_PPI_regs.TASKS_CHG[4].DIS, nrf_ppi_TASK_CHG4_DIS},
    { (void*)&NRF_PPI_regs.TASKS_CHG[5].DIS, nrf_ppi_TASK_CHG5_DIS},

    //End marker
    { NULL, NULL }
};

/**
 * Table to convert from a event_address as programmed by the SW into an
 * event_type used here to index tables
 */
typedef struct {
  ppi_event_types_t event_type;
  void *event_addr;
} ppi_event_table_t;

static const ppi_event_table_t ppi_events_table[] = { //better keep same order as in ppi_event_types_t
    {RADIO_EVENTS_READY,   &NRF_RADIO_regs.EVENTS_READY},
    {RADIO_EVENTS_ADDRESS, &NRF_RADIO_regs.EVENTS_ADDRESS},
    {RADIO_EVENTS_PAYLOAD, &NRF_RADIO_regs.EVENTS_PAYLOAD},
    {RADIO_EVENTS_END,     &NRF_RADIO_regs.EVENTS_END},
    {RADIO_EVENTS_DISABLED,&NRF_RADIO_regs.EVENTS_DISABLED},
//    {RADIO_EVENTS_DEVMATCH,&NRF_RADIO_regs.EVENTS_DEVMATCH},
//    {RADIO_EVENTS_DEVMISS ,&NRF_RADIO_regs.EVENTS_DEVMISS},
    {RADIO_EVENTS_BCMATCH, &NRF_RADIO_regs.EVENTS_BCMATCH},

    {TIMER0_EVENTS_COMPARE_0, &NRF_TIMER_regs[0].EVENTS_COMPARE[0]},
    {TIMER0_EVENTS_COMPARE_1, &NRF_TIMER_regs[0].EVENTS_COMPARE[1]},
    {TIMER0_EVENTS_COMPARE_2, &NRF_TIMER_regs[0].EVENTS_COMPARE[2]},
    {TIMER0_EVENTS_COMPARE_3, &NRF_TIMER_regs[0].EVENTS_COMPARE[3]},
//    {TIMER0_EVENTS_COMPARE_4, &NRF_TIMER_regs[0].EVENTS_COMPARE[4]},
//    {TIMER0_EVENTS_COMPARE_5, &NRF_TIMER_regs[0].EVENTS_COMPARE[5]},

    {TIMER1_EVENTS_COMPARE_0, &NRF_TIMER_regs[1].EVENTS_COMPARE[0]},
    {TIMER1_EVENTS_COMPARE_1, &NRF_TIMER_regs[1].EVENTS_COMPARE[1]},
    //    {TIMER1_EVENTS_COMPARE_2, &NRF_TIMER_regs[1].EVENTS_COMPARE[2]},
    //    {TIMER1_EVENTS_COMPARE_3, &NRF_TIMER_regs[1].EVENTS_COMPARE[3]},
    //    {TIMER1_EVENTS_COMPARE_4, &NRF_TIMER_regs[1].EVENTS_COMPARE[4]},
    //    {TIMER1_EVENTS_COMPARE_5, &NRF_TIMER_regs[1].EVENTS_COMPARE[5]},

    //    {TIMER2_EVENTS_COMPARE_0, &NRF_TIMER_regs[2].EVENTS_COMPARE[0]},
    //    {TIMER2_EVENTS_COMPARE_1, &NRF_TIMER_regs[2].EVENTS_COMPARE[1]},
    //    {TIMER2_EVENTS_COMPARE_2, &NRF_TIMER_regs[2].EVENTS_COMPARE[2]},
    //    {TIMER2_EVENTS_COMPARE_3, &NRF_TIMER_regs[2].EVENTS_COMPARE[3]},
    //    {TIMER2_EVENTS_COMPARE_4, &NRF_TIMER_regs[2].EVENTS_COMPARE[4]},
    //    {TIMER2_EVENTS_COMPARE_5, &NRF_TIMER_regs[2].EVENTS_COMPARE[5]},

    {RTC0_EVENTS_OVRFLW, &NRF_RTC_regs[0].EVENTS_OVRFLW},
    //{RTC0_EVENTS_COMPARE_0, &NRF_RTC_regs[0].EVENTS_COMPARE[0]},
    {RTC0_EVENTS_COMPARE_1, &NRF_RTC_regs[0].EVENTS_COMPARE[1]},
    {RTC0_EVENTS_COMPARE_2, &NRF_RTC_regs[0].EVENTS_COMPARE[2]},

    //    {TIMER3_EVENTS_COMPARE_0, &NRF_TIMER_regs[3].EVENTS_COMPARE[0]},
    //    {TIMER3_EVENTS_COMPARE_1, &NRF_TIMER_regs[3].EVENTS_COMPARE[1]},
    //    {TIMER3_EVENTS_COMPARE_2, &NRF_TIMER_regs[3].EVENTS_COMPARE[2]},
    //    {TIMER3_EVENTS_COMPARE_3, &NRF_TIMER_regs[3].EVENTS_COMPARE[3]},
    //    {TIMER3_EVENTS_COMPARE_4, &NRF_TIMER_regs[3].EVENTS_COMPARE[4]},
    //    {TIMER3_EVENTS_COMPARE_5, &NRF_TIMER_regs[3].EVENTS_COMPARE[5]},

    //    {TIMER4_EVENTS_COMPARE_0, &NRF_TIMER_regs[4].EVENTS_COMPARE[0]},
    //    {TIMER4_EVENTS_COMPARE_1, &NRF_TIMER_regs[4].EVENTS_COMPARE[1]},
    //    {TIMER4_EVENTS_COMPARE_2, &NRF_TIMER_regs[4].EVENTS_COMPARE[2]},
    //    {TIMER4_EVENTS_COMPARE_3, &NRF_TIMER_regs[4].EVENTS_COMPARE[3]},
    //    {TIMER4_EVENTS_COMPARE_4, &NRF_TIMER_regs[4].EVENTS_COMPARE[4]},
    //    {TIMER4_EVENTS_COMPARE_5, &NRF_TIMER_regs[4].EVENTS_COMPARE[5]},

    //{RNG_EVENTS_VALRDY, &NRF_RNG_regs.EVENTS_VALRDY},

    {NUMBER_PPI_EVENTS, NULL} //End marker
};


static void set_fixed_channel_routes(){
  //TODO: add handler function pointers as we add those functions while modelling the different parts

  //Set the fixed channels configuration:
  //  20 TIMER0->EVENTS_COMPARE[0] RADIO->TASKS_TXEN
    ppi_evt_to_ch[TIMER0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 20 );
    ppi_ch_tasks[20].tep_f = nrf_radio_tasks_txen; //RADIO->TASKS_TXEN

  //  21 TIMER0->EVENTS_COMPARE[0] RADIO->TASKS_RXEN
    ppi_evt_to_ch[TIMER0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 21 );
    ppi_ch_tasks[21].tep_f = nrf_radio_tasks_rxen; //RADIO->TASKS_RXEN

  //  22 TIMER0->EVENTS_COMPARE[1] RADIO->TASKS_DISABLE
    ppi_evt_to_ch[TIMER0_EVENTS_COMPARE_1].channels_mask |= ( 1 << 22 );
    ppi_ch_tasks[22].tep_f = nrf_radio_tasks_disable; //RADIO->TASKS_DISABLE

  //  23 RADIO->EVENTS_BCMATCH AAR->TASKS_START
    ppi_evt_to_ch[RADIO_EVENTS_BCMATCH].channels_mask |= ( 1 << 23 );
    ppi_ch_tasks[23].tep_f = nrf_aar_TASK_START; //AAR->TASKS_START

  //  24 RADIO->EVENTS_READY CCM->TASKS_KSGEN
    ppi_evt_to_ch[RADIO_EVENTS_READY].channels_mask |= ( 1 << 24 );
    ppi_ch_tasks[24].tep_f = nrf_ccm_TASK_KSGEN; //CCM->TASKS_KSGEN

  //  25 RADIO->EVENTS_ADDRESS CCM->TASKS_CRYPT
    ppi_evt_to_ch[RADIO_EVENTS_ADDRESS].channels_mask |= ( 1 << 25 );
    ppi_ch_tasks[25].tep_f = nrf_ccm_TASK_CRYPT; //CCM->TASKS_CRYPT

  //  26 RADIO->EVENTS_ADDRESS TIMER0->TASKS_CAPTURE[1]
    ppi_evt_to_ch[RADIO_EVENTS_ADDRESS].channels_mask |= ( 1 << 26 );
    ppi_ch_tasks[26].tep_f = nrf_timer0_TASK_CAPTURE_1; //TIMER0->TASKS_CAPTURE[1]

  //  27 RADIO->EVENTS_END TIMER0->TASKS_CAPTURE[2]
    ppi_evt_to_ch[RADIO_EVENTS_END].channels_mask |= ( 1 << 27 );
    ppi_ch_tasks[27].tep_f = nrf_timer0_TASK_CAPTURE_2; //TIMER0->TASKS_CAPTURE[2]

  //  28 RTC0->EVENTS_COMPARE[0] RADIO->TASKS_TXEN
    ppi_evt_to_ch[RTC0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 28 );
    ppi_ch_tasks[28].tep_f = nrf_radio_tasks_txen; //RADIO->TASKS_TXEN

  //  29 RTC0->EVENTS_COMPARE[0] RADIO->TASKS_RXEN
    ppi_evt_to_ch[RTC0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 29 );
    ppi_ch_tasks[29].tep_f = nrf_radio_tasks_rxen; //RADIO->TASKS_RXEN

  //  30 RTC0->EVENTS_COMPARE[0] TIMER0->TASKS_CLEAR
    ppi_evt_to_ch[RTC0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 30 );
    ppi_ch_tasks[30].tep_f = nrf_timer0_TASK_CLEAR; //TIMER0->TASKS_CLEAR

  //  31 RTC0->EVENTS_COMPARE[0] TIMER0->TASKS_START
    ppi_evt_to_ch[RTC0_EVENTS_COMPARE_0].channels_mask |= ( 1 << 31 );
    ppi_ch_tasks[31].tep_f = nrf_timer0_TASK_START; //TIMER0->TASKS_START
}

/**
 * Initialize the PPI model
 */
void nrf_ppi_init(){
  memset(&NRF_PPI_regs, 0, sizeof(NRF_PPI_regs));
  memset(ppi_ch_tasks, 0, sizeof(ppi_ch_tasks));
  memset(ppi_evt_to_ch, 0, sizeof(ppi_evt_to_ch));
  set_fixed_channel_routes();
}

/**
 * Cleanup the PPI model before exiting the program
 */
void nrf_ppi_clean_up(){

}

/**
 * HW models call this function when they want to signal an event which
 * may trigger a task
 */
void nrf_ppi_event(ppi_event_types_t event){

  uint32_t ch_mask = ppi_evt_to_ch[event].channels_mask;
  ch_mask &= NRF_PPI_regs.CHEN;

  if ( ch_mask ){
    for ( int ch_nbr = __builtin_ffs(ch_mask) - 1;
          ( ch_mask != 0 ) && ( ch_nbr < NUMBER_PPI_CHANNELS ) ;
          ch_nbr++ ) {
      if ( ch_mask & ( 1 << ch_nbr ) ){
        ch_mask &= ~( (uint64_t) 1 << ch_nbr );
        if ( ppi_ch_tasks[ch_nbr].tep_f != NULL ){
          ppi_ch_tasks[ch_nbr].tep_f();
        }
        if ( ppi_ch_tasks[ch_nbr].fork_tep_f != NULL ){
          ppi_ch_tasks[ch_nbr].fork_tep_f();
        }
      } //if event is mapped to this channel
    } //for channels
  } //if this event is in any channel
}

/**
 * Find the task in ppi_tasks_table whose address
 * matches <TEP> and save the task handling function <in dest>
 *
 * Helper function for the TEP and FORK_TEP functions
 */
static void find_task(void *TEP, dest_f_t *dest, int ch_nbr){
  int tt = 0;
  while ( ppi_tasks_table[tt].task_addr != NULL ){
    if ( ppi_tasks_table[tt].task_addr == TEP ){
       *dest = ppi_tasks_table[tt].dest;
       return;
    }
    tt++;
  }
  bs_trace_warning_line_time(
      "NRF_PPI: The task %p for chnbr %i does not match any modelled task in NRF_PPI.c => it will be ignored\n",
      TEP, ch_nbr);

  *dest = NULL;
}

/**
 * Update PPI internal routing status after a
 * CH[<ch_nbr>].EEP update
 */
void nrf_ppi_regw_sideeffects_EEP(int ch_nbr){
  //To save execution time when an event is raised, we build the
  //ppi_event_config_table & ppi_channel_config_table out of the registers

  //first remove this channel from all events masks
  for (int i = 0 ; i < NUMBER_PPI_EVENTS ; i++){
    ppi_evt_to_ch[i].channels_mask &= ~( 1 << ch_nbr );
  }

  //then lets try to find which event (if any) is feeding this channel
  if ( ( ch_nbr < 20 ) && ( (void*)NRF_PPI_regs.CH[ch_nbr].EEP != NULL ) ){
    int i = 0;
    while ( ppi_events_table[i].event_type != NUMBER_PPI_EVENTS ){
      if ( ppi_events_table[i].event_addr == (void*)NRF_PPI_regs.CH[ch_nbr].EEP ) {
        ppi_evt_to_ch[ppi_events_table[i].event_type].channels_mask |= ( 1 << ch_nbr );
        return;
      }
      i++;
    }
    bs_trace_warning_line_time(
        "NRF_PPI: The event NRF_PPI_regs.CH[%i].EEP(=%p) does not match any modelled event in NRF_PPI.c=> it will be ignored\n",
        ch_nbr, NRF_PPI_regs.CH[ch_nbr].EEP);
  }
}

/**
 * Update PPI internal routing status after a
 * CH[<ch_nbr>].TEP update
 */
void nrf_ppi_regw_sideeffects_TEP(int ch_nbr){
  //To save execution time when an event is raised, we build the
  //ppi_event_config_table & ppi_channel_config_table out of the registers
  if ( ch_nbr < 20 ){
    if ( (void*)NRF_PPI_regs.CH[ch_nbr].TEP != NULL ){
      find_task((void*)NRF_PPI_regs.CH[ch_nbr].TEP,
                &ppi_ch_tasks[ch_nbr].tep_f,
                ch_nbr);
    } else {
      ppi_ch_tasks[ch_nbr].tep_f = NULL;
    }
  }
}
/**
 * Update PPI internal routing status after a
 * FORK[<ch_nbr>].TEP update
 */
void nrf_ppi_regw_sideeffects_FORK_TEP(int ch_nbr){
  if ( (void*)NRF_PPI_regs.FORK[ch_nbr].TEP != NULL ){
    find_task((void*)NRF_PPI_regs.FORK[ch_nbr].TEP,
              &ppi_ch_tasks[ch_nbr].fork_tep_f,
              ch_nbr);
  } else {
    ppi_ch_tasks[ch_nbr].fork_tep_f = NULL;
  }
}

/**
 * Update PPI CHEN mask after a write to CHENSET
 * (writes to CHEN do not need sideeffects)
 */
void nrf_ppi_regw_sideeffects_CHENSET(){
	if ( NRF_PPI_regs.CHENSET != 0 ){
		NRF_PPI_regs.CHEN |= NRF_PPI_regs.CHENSET;
		NRF_PPI_regs.CHENSET = 0;
	}
}

/**
 * Update PPI CHEN mask after a write to CHENCLR
 * (writes to CHEN do not need sideeffects)
 */
void nrf_ppi_regw_sideeffects_CHENCLR(){
	if ( NRF_PPI_regs.CHENCLR != 0 ){
		NRF_PPI_regs.CHEN &= ~NRF_PPI_regs.CHENCLR;
		NRF_PPI_regs.CHENCLR = 0;
	}
}

/**
 * Update PPI CHEN mask after a write to CHENSET or CHENCLR
 * (writes to CHEN do not need sideeffects)
 */
void nrf_ppi_regw_sideeffects_CHEN(){
	//Note that we do not let people read CHEN from the SET and CLR
	//registers
	nrf_ppi_regw_sideeffects_CHENSET();
	nrf_ppi_regw_sideeffects_CHENCLR();
}

/**
 * If you dont want to bother about individual calls,
 * when any PPI register is written which changes the configuration of the PPI
 * call this to update the routing table or cause other sideeffects
 * This call will inspect all input registers
 *
 * (otherwise call the correspondent sideeffect function above for each register
 * write)
 */
void nrf_ppi_regw_sideeffects(){

  for (int ch_nbr = 0; ch_nbr < 32 ; ch_nbr++){
    if ( ch_nbr < 20 ){
      nrf_ppi_regw_sideeffects_EEP(ch_nbr);
      nrf_ppi_regw_sideeffects_TEP(ch_nbr);
    }
    nrf_ppi_regw_sideeffects_FORK_TEP(ch_nbr);
  }

  nrf_ppi_regw_sideeffects_CHEN();
}

void nrf_ppi_regw_sideeffects_TASKS_CHG_DIS(int i){
	if ( NRF_PPI_regs.TASKS_CHG[i].DIS ) {
		NRF_PPI_regs.TASKS_CHG[i].DIS = 0;
		nrf_ppi_TASK_CHG_ENDIS( i, false );
	}
}

void nrf_ppi_regw_sideeffects_TASKS_CHG_EN(int i){
	if ( NRF_PPI_regs.TASKS_CHG[i].EN ) {
		NRF_PPI_regs.TASKS_CHG[i].EN = 0;
		nrf_ppi_TASK_CHG_ENDIS( i, true );
	}
}

void nrf_ppi_taskw_sideeffects(){
	int i;
	for ( i = 0 ; i < 6 ; i++){
		nrf_ppi_regw_sideeffects_TASKS_CHG_DIS(i);
	}
	for ( i = 0 ; i < 6 ; i++){
		nrf_ppi_regw_sideeffects_TASKS_CHG_EN(i);
	}
}
