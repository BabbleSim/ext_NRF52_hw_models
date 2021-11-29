/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "bs_pc_2G4.h"
#include "bs_pc_2G4_utils.h"
#include "time_machine_if.h"
#include "NRF_RADIO.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "NRF_AES_CCM.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "NRF_HWLowL.h"
#include "crc_ble.h"

/**
 * RADIO — 2.4 GHz Radio
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/radio.html?cp=2_0_0_21#concept_lhd_ygj_4r
 *
 * Note: as of now, only 1&2Mbps BLE packet formats are supported, there is quite many notes around in the code
 * where changes would be required to support other formats
 *
 * Note3: Only logical address 0 (in Tx or Rx) is supported
 *
 * Note4: only default freq. map supported
 *
 * Note5: Only little endian supported (x86 is little endian)
 *
 * Note6: RSSI is always sampled at the end of the address (and RSSIEND raised there)
 *
 * Note7: Whitening is always/never "used" (it is the phy who would use or not whitening), the radio model can assume it is always used (even that we ignore the initialization register)
 *
 * Note8: During idle nothing is sent to the air
 *
 * Note9: Only the BLE polynomial is supported
 *
 * Note10: Maxlength is only partially checked (the packet is cut, but the interaction with the phy is not proper)
 *
 * Note11: During reception we assume that CRCPOLY and CRCINIT are correct on both sides, and just rely on the phy bit error reporting to save processing time
 *         On transmission we generate the correct CRC for correctness of the channel dump traces (and Ellisys traces)
 */

NRF_RADIO_Type NRF_RADIO_regs;
static uint32_t RADIO_INTEN = 0; //interrupt enable

static struct {
  /*Ramp up times*/
  bs_time_t TX_RU_time[2][2][2];
  /* The versions are [1,2Mbps] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  bs_time_t RX_RU_time[2][2][2];

  /*Digital processing delay:*/
  bs_time_t TX_chain_delay;
  bs_time_t RX_chain_delay[2]; /*Indexed as 1Mbps, 2Mbps*/

  /*Ramp down times*/
  bs_time_t TX_RD_time;
  bs_time_t RX_RD_time;
} radio_timings;

bs_time_t Timer_RADIO = TIME_NEVER; //main radio timer
bs_time_t Timer_RADIO_abort_reeval = TIME_NEVER; //Abort reevaluation response timer, this timer must have the lowest priority of all events (which may cause an abort)
bs_time_t Timer_RADIO_bitcounter = TIME_NEVER;

static enum {TIFS_DISABLE = 0, TIFS_WAITING_FOR_DISABLE, TIFS_TRIGGERING_TRX_EN } TIFS_state = TIFS_DISABLE;
bool TIFS_ToTxNotRx = false;
bs_time_t Timer_TIFS = TIME_NEVER;

static bs_time_t TX_ADDRESS_end_time, TX_PAYLOAD_end_time, TX_CRC_end_time;

//Ongoing Rx/Tx structures:
static p2G4_rx_t  ongoing_rx;
static struct {
  bs_time_t CRC_duration;
  bool CRC_OK;
  bs_time_t ADDRESS_End_Time;
  bs_time_t PAYLOAD_End_Time;
  bs_time_t CRC_End_Time;
  bool packet_rejected;
  bool S1Offset;
} ongoing_rx_RADIO_status;
static p2G4_tx_t    ongoing_tx;
static p2G4_tx_done_t ongoing_tx_done;
static p2G4_rx_done_t ongoing_rx_done;
static bs_time_t next_recheck_time; // when we asked the phy to recheck (in our own time) next time
double bits_per_us; //Bits per us for the ongoing Tx or Rx (shared with the bit counter)

typedef enum { No_pending_abort_reeval = 0, Tx_Abort_reeval, Rx_Abort_reeval } abort_state_t;
static abort_state_t abort_fsm_state = No_pending_abort_reeval; //This variable shall be set to Tx/Rx_Abort_reeval when the phy is waiting for an abort response (and in no other circumstance)

typedef enum {
  DISABLED = 0, //No operations are going on inside the radio and the power consumption is at a minimum
  RXRU, //The radio is ramping up and preparing for reception
  RXIDLE, //The radio is ready for reception to start
  RX, //Reception has been started and the addresses enabled in the RXADDRESSES register are being monitored
  RXDISABLE, //The radio is disabling the receiver

  TXRU = 9, //The radio is ramping up and preparing for transmission
  TXIDLE, //The radio is ready for transmission to start
  TX, //The radio is transmitting a packet
  TXDISABLE //The radio is disabling the transmitter
} radio_state_t;
static radio_state_t radio_state;

typedef enum {SUB_STATE_INVALID, /*The timer should not trigger in TX or RX state with this substate*/
  TX_WAIT_FOR_ADDRESS_END, TX_WAIT_FOR_PAYLOAD_END, TX_WAIT_FOR_CRC_END,
  RX_WAIT_FOR_ADDRESS_END, RX_WAIT_FOR_PAYLOAD_END, RX_WAIT_FOR_CRC_END
} radio_sub_state_t;

static radio_sub_state_t radio_sub_state;

static int aborting_set = 0;

#define _NRF_MAX_PACKET_SIZE (256+2+4)
static uint8_t tx_buf[_NRF_MAX_PACKET_SIZE]; //starting from the header, and including CRC
static uint8_t rx_buf[_NRF_MAX_PACKET_SIZE]; // "
static uint8_t *rx_pkt_buffer_ptr;

static bool radio_on = false;

static bool rssi_sampling_on = false;

static bs_time_t Time_BitCounterStarted = TIME_NEVER;
static bool bit_counter_running = false;

static bool from_hw_tifs = false; /* Unfortunate hack due to the SW racing the HW to clear SHORTS*/

static void nrf_radio_stop_bit_counter();

static void radio_reset() {
  memset(&NRF_RADIO_regs, 0, sizeof(NRF_RADIO_regs));
  radio_state = DISABLED;
  RADIO_INTEN = 0;
  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  rssi_sampling_on = false;

  TIFS_state = TIFS_DISABLE;
  TIFS_ToTxNotRx = false;
  Timer_TIFS = TIME_NEVER;

  Timer_RADIO_bitcounter = TIME_NEVER;
  bit_counter_running = 0;
}

void nrf_radio_init() {
  /* The versions are [1,2Mbps] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  radio_timings.TX_RU_time[0][1][0] =  41; // 41000
  radio_timings.TX_RU_time[0][0][1] = 141; //141000
  radio_timings.TX_RU_time[0][0][0] = 130; //130000
  radio_timings.TX_RU_time[1][1][0] =  40; // 40000
  radio_timings.TX_RU_time[1][0][1] = 140; //140000
  radio_timings.TX_RU_time[1][0][0] = 129; //128900

  radio_timings.RX_RU_time[0][1][0] =  40; // 40000
  radio_timings.RX_RU_time[0][0][1] = 140; //140000
  radio_timings.RX_RU_time[0][0][0] = 129; //129000
  radio_timings.RX_RU_time[1][1][0] =  40; // 40000
  radio_timings.RX_RU_time[1][0][1] = 140; //140000
  radio_timings.RX_RU_time[1][0][0] = 129; //129000

  radio_timings.TX_chain_delay    = 1; //0.6 /*both 1 and 2Mbps*/
  radio_timings.RX_chain_delay[0] = 9; //9.4  /* 1Mbps */
  radio_timings.RX_chain_delay[1] = 5; //5.45 /* 2Mbps */

  radio_timings.TX_RD_time = 6;
  radio_timings.RX_RD_time = 0;
  radio_reset();
  radio_on = false;
  bits_per_us = 1;
}

void nrf_radio_clean_up() {

}

static int HW_TIFS_enabled() {
  if ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_END_DISABLE_Msk )
      && ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_RXEN_Msk )
          || ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ) )
  ){
    return 1;
  }
  return 0;
}

static bs_time_t get_Rx_chain_delay(){
  int Mbps2 = 0;
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    Mbps2 = 1;
  }
  return radio_timings.RX_chain_delay[Mbps2];
}

static bs_time_t get_rampup_time(bool TxNotRx, bool from_hw_TIFS) {
  int fast  = 0;
  int Mbps2 = 0;
  int HWTIFS= 0;

  if ( NRF_RADIO_regs.MODECNF0 & 1 ){ /* MODECNF0.RU */
    fast = 1;
  } else {
    HWTIFS = from_hw_TIFS | HW_TIFS_enabled();
  }
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    Mbps2 = 1;
  }
  if (TxNotRx) {
    return radio_timings.TX_RU_time[Mbps2][fast][HWTIFS];
  } else {
    return radio_timings.RX_RU_time[Mbps2][fast][HWTIFS];
  }
}

void nrf_radio_tasks_txen() {
  if ( ( radio_state != DISABLED )
      && ( radio_state != TXIDLE )
      && ( radio_state != RXIDLE ) ){
    bs_trace_warning_line_time(
        "NRF_RADIO: TXEN received when the radio was not DISABLED or TX/RXIDLE but in state %i. It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  TIFS_state = TIFS_DISABLE;
  radio_state = TXRU;
  NRF_RADIO_regs.STATE = TXRU;

  Timer_RADIO = tm_get_hw_time() + get_rampup_time(1, from_hw_tifs);
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_radio_tasks_rxen() {
  if ( ( radio_state != DISABLED )
      && ( radio_state != TXIDLE )
      && ( radio_state != RXIDLE ) ){
    bs_trace_warning_line_time(
        "NRF_RADIO: RXEN received when the radio was not DISABLED or TX/RXIDLE but in state %i. It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  TIFS_state = TIFS_DISABLE;
  radio_state = RXRU;
  NRF_RADIO_regs.STATE = RXRU;
  Timer_RADIO = tm_get_hw_time() + get_rampup_time(0, from_hw_tifs);
  nrf_hw_find_next_timer_to_trigger();
}

static void start_Tx();
static void start_Rx();
static void Rx_Addr_received();

static void abort_if_needed(){
  if ( ( abort_fsm_state == Tx_Abort_reeval ) || ( abort_fsm_state == Rx_Abort_reeval ) ){
    //If the phy is waiting for a response from us, we need to tell it, that we are aborting whatever it was doing
    aborting_set = 1;
  }
  if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ){
    //we answer immediately to the phy rejecting the packet
    p2G4_dev_rx_cont_after_addr_nc_b(false);
    radio_sub_state = SUB_STATE_INVALID;
  }
}

void nrf_radio_tasks_start () {
  if ( radio_state == TXIDLE ) {
    start_Tx();
  } else if ( radio_state == RXIDLE ) {
    start_Rx();
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_START received while the radio was not in either TXIDLE or RXIDLE but in state %i. It will be ignored => expect problems\n",
        radio_state);
  }
}

void nrf_radio_tasks_stop (){
  nrf_radio_stop_bit_counter();

  if ( radio_state == TX ){
    abort_if_needed();
    radio_state = TXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RX ){
    abort_if_needed();
    radio_state = RXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_STOP received while the radio was not on either TX or RX but in state %i. It will be ignored => ignored\n",
        radio_state);
  }
}

static void signal_DISABLED();

void nrf_radio_tasks_disable() {
  nrf_radio_stop_bit_counter();

  if ( radio_state == TX ){
    abort_if_needed();
    radio_state = TXIDLE; //Momentary (will be changedin the if below)
  } else if ( radio_state == RX ){
    abort_if_needed();
    radio_state = RXIDLE; //Momentary (will be changed in the if below)
  }

  if (TIFS_state != TIFS_DISABLE) {
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = TIME_NEVER;
    Timer_TIFS = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  }

  if ( ( radio_state == TXRU ) || ( radio_state == TXIDLE ) ) {
    radio_state = TXDISABLE;
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = tm_get_hw_time() + radio_timings.TX_RD_time;
    nrf_hw_find_next_timer_to_trigger();
  } else if ( ( radio_state == RXRU ) || ( radio_state == RXIDLE ) ) {
    radio_state = RXDISABLE;
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = tm_get_hw_time() + radio_timings.RX_RD_time;
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == DISABLED ) {
    //It seems the radio will also signal a DISABLED event even if it was already disabled
    signal_DISABLED();
  }
}

void nrf_radio_tasks_rssistart () {
  rssi_sampling_on = true;
}

void nrf_radio_tasks_rssistop(){
  rssi_sampling_on = false;
}

void nrf_radio_regw_sideeffects_TASKS_RXEN(){
  if ( NRF_RADIO_regs.TASKS_RXEN ){
    NRF_RADIO_regs.TASKS_RXEN = 0;
    nrf_radio_tasks_rxen();
  }
}

void nrf_radio_regw_sideeffects_TASKS_TXEN(){
  if ( NRF_RADIO_regs.TASKS_TXEN ){
    NRF_RADIO_regs.TASKS_TXEN = 0;
    nrf_radio_tasks_txen();
  }
}

void nrf_radio_regw_sideeffects_TASKS_DISABLE(){
  if ( NRF_RADIO_regs.TASKS_DISABLE ){
    NRF_RADIO_regs.TASKS_DISABLE = 0;
    nrf_radio_tasks_disable();
  }
}

void nrf_radio_regw_sideeffects_TASKS_RSSISTART() {
  //We don't need to model this as per now
  if ( NRF_RADIO_regs.TASKS_RSSISTART ){
    NRF_RADIO_regs.TASKS_RSSISTART = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTART register is not supported by the model\n");
  }
}

void nrf_radio_regw_sideeffects_TASKS_RSSISTOP() {
  //We don't need to model this as per now
  if ( NRF_RADIO_regs.TASKS_RSSISTOP ){
    NRF_RADIO_regs.TASKS_RSSISTOP = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTOP register is not supported by the model\n");
  }
}

void nrf_radio_regw_sideeffects_INTENSET(){
  if ( NRF_RADIO_regs.INTENSET ){
    RADIO_INTEN |= NRF_RADIO_regs.INTENSET;
    NRF_RADIO_regs.INTENSET = RADIO_INTEN;
  }
}

void nrf_radio_regw_sideeffects_INTENCLR(){
  if ( NRF_RADIO_regs.INTENCLR ){
    RADIO_INTEN  &= ~NRF_RADIO_regs.INTENCLR;
    NRF_RADIO_regs.INTENSET = RADIO_INTEN;
    NRF_RADIO_regs.INTENCLR = 0;
  }
}

void nrf_radio_regw_sideeffects_POWER(){
  if ( NRF_RADIO_regs.POWER == 0 ){
    radio_on = false;
  } else {
    if ( radio_on == false ){
      radio_on = true;
      abort_if_needed();
      radio_reset();
      nrf_hw_find_next_timer_to_trigger();
    }
  }
}

static void signal_READY(){
  NRF_RADIO_regs.EVENTS_READY = 1;
  nrf_ppi_event(RADIO_EVENTS_READY);

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_READY_START_Msk ) {
    nrf_radio_tasks_start();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_READY_Msk ){
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}
/*
 * The TX and RX READY missing in the registers header..
static void signal_TXREADY(){
  NRF_RADIO_regs.EVENTS_TXREADY = 1;
  NRF_PPI_Event(RADIO_EVENTS_TXREADY);

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_TXREADY_START_Msk ) {
    NRF_RADIO_TASK_START();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_TXREADY_Msk ){
    hw_irq_controller_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_RXREADY(){
  NRF_RADIO_regs.EVENTS_RXREADY = 1;
  NRF_PPI_Event(RADIO_EVENTS_RXREADY);

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_RXREADY_START_Msk ) {
    NRF_RADIO_TASK_START();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_RXREADY_Msk ){
    hw_irq_controller_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}
*/

static void signal_RSSIEND(){
  NRF_RADIO_regs.EVENTS_RSSIEND = 1;
  nrf_ppi_event(RADIO_EVENTS_RSSIEND);

  if ( RADIO_INTEN & RADIO_INTENSET_RSSIEND_Msk ){
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_ADDRESS(){
  NRF_RADIO_regs.EVENTS_ADDRESS = 1;
  nrf_ppi_event(RADIO_EVENTS_ADDRESS);

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_ADDRESS_RSSISTART_Msk ) {
    nrf_radio_tasks_rssistart();
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_ADDRESS_BCSTART_Msk ) {
    nrf_radio_tasks_bcstart();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_ADDRESS_Msk ){
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_PAYLOAD(){
  NRF_RADIO_regs.EVENTS_PAYLOAD = 1;
  nrf_ppi_event(RADIO_EVENTS_PAYLOAD);

  if ( RADIO_INTEN & RADIO_INTENSET_PAYLOAD_Msk ){
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_CRCOK(){
  NRF_RADIO_regs.EVENTS_CRCOK = 1;
  nrf_ppi_event(RADIO_EVENTS_CRCOK);

  if ( RADIO_INTEN & RADIO_INTENSET_CRCOK_Msk ) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_CRCERROR(){
  NRF_RADIO_regs.EVENTS_CRCERROR = 1;
  nrf_ppi_event(RADIO_EVENTS_CRCERROR);

  if ( RADIO_INTEN & RADIO_INTENSET_CRCERROR_Msk ) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_END(){
  nrf_radio_stop_bit_counter();

  NRF_RADIO_regs.EVENTS_END = 1;
  nrf_ppi_event(RADIO_EVENTS_END);

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_END_DISABLE_Msk ) {
    nrf_radio_tasks_disable();
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_END_START_Msk ) {
    nrf_radio_tasks_start();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_END_Msk ) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_DISABLED(){
  nrf_radio_stop_bit_counter();

  NRF_RADIO_regs.EVENTS_DISABLED = 1;
  nrf_ppi_event(RADIO_EVENTS_DISABLED);

  //These 2 are fake shortcuts meant to start a HW timer for the TIFS
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ) {
    if ( TIFS_state == TIFS_WAITING_FOR_DISABLE ) {
      TIFS_state = TIFS_TRIGGERING_TRX_EN;
      Timer_RADIO = Timer_TIFS;
      if ( Timer_RADIO < tm_get_hw_time() ){
        bs_trace_warning_line_time("NRF_RADIO: TIFS Ups: The Ramp down from Rx into a Tx takes more than the programmed TIFS time\n");
      }
      nrf_hw_find_next_timer_to_trigger();
    }
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_RXEN_Msk ) {
    if ( TIFS_state == TIFS_WAITING_FOR_DISABLE ) {
      TIFS_state = TIFS_TRIGGERING_TRX_EN;
      Timer_RADIO = Timer_TIFS;
      if ( Timer_RADIO < tm_get_hw_time() ){
        bs_trace_warning_line_time("NRF_RADIO: TIFS Ups 2\n");
      }
      nrf_hw_find_next_timer_to_trigger();
    }
  }

  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_RSSISTOP_Msk ) {
    nrf_radio_tasks_rssistop();
  }

  if ( RADIO_INTEN & RADIO_INTENSET_DISABLED_Msk ) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

void maybe_prepare_TIFS(bool Tx_Not_Rx){
  bs_time_t delta;
  if ( !HW_TIFS_enabled() ) {
    return;
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ){
    TIFS_ToTxNotRx = true;
  } else {
    TIFS_ToTxNotRx = false;
  }

  if ( Tx_Not_Rx ){ //End of Tx
    delta = NRF_RADIO_regs.TIFS + radio_timings.TX_chain_delay - get_rampup_time(0, 1) - 3; /*open slightly earlier to have jitter margin*/
  } else { //End of Rx
    delta = NRF_RADIO_regs.TIFS - get_Rx_chain_delay() - radio_timings.TX_chain_delay - get_rampup_time(1, 1) + 1;
  }
  Timer_TIFS = tm_get_hw_time() + delta;
  TIFS_state = TIFS_WAITING_FOR_DISABLE;
}

void nrf_radio_timer_triggered(){

  if ( radio_state == TXRU ){
    radio_state = TXIDLE;
    NRF_RADIO_regs.STATE = TXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
    signal_READY();
    //signal_TXREADY();
  } else if ( radio_state == RXRU ){
    radio_state = RXIDLE;
    NRF_RADIO_regs.STATE = RXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
    signal_READY();
    //signal_RXREADY();
  } else if ( radio_state == TX ){
    if ( radio_sub_state == TX_WAIT_FOR_ADDRESS_END ){
      radio_sub_state = TX_WAIT_FOR_PAYLOAD_END;
      Timer_RADIO = TX_PAYLOAD_end_time;
      signal_ADDRESS();
    } else if ( radio_sub_state == TX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = TX_WAIT_FOR_CRC_END;
      Timer_RADIO = TX_CRC_end_time;
      signal_PAYLOAD();
    } else if ( radio_sub_state == TX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = TXIDLE;
      NRF_RADIO_regs.STATE = TXIDLE;
      Timer_RADIO = TIME_NEVER;
      signal_END();
      maybe_prepare_TIFS(true);
    }  else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RX ){
    if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ) {
      Timer_RADIO = TIME_NEVER;
      nrf_hw_find_next_timer_to_trigger();
      signal_ADDRESS();
      Rx_Addr_received();
      radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
      Timer_RADIO = ongoing_rx_RADIO_status.PAYLOAD_End_Time;
      nrf_hw_find_next_timer_to_trigger();
    } else if ( radio_sub_state == RX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = RX_WAIT_FOR_CRC_END;
      Timer_RADIO = ongoing_rx_RADIO_status.CRC_End_Time;
      nrf_hw_find_next_timer_to_trigger();
      signal_PAYLOAD();
    } else if ( radio_sub_state == RX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RXIDLE;
      NRF_RADIO_regs.STATE = RXIDLE;
      Timer_RADIO = TIME_NEVER;
      nrf_hw_find_next_timer_to_trigger();
      if ( ongoing_rx_RADIO_status.CRC_OK ) {
        signal_CRCOK();
      } else {
        signal_CRCERROR();
      }
      signal_END();
      maybe_prepare_TIFS(false);
    } else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == TXDISABLE ){
    radio_state = DISABLED;
    NRF_RADIO_regs.STATE = DISABLED;
    Timer_RADIO = TIME_NEVER;
    signal_DISABLED();
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RXDISABLE ){
    radio_state = DISABLED;
    NRF_RADIO_regs.STATE = DISABLED;
    Timer_RADIO = TIME_NEVER;
    signal_DISABLED();
    nrf_hw_find_next_timer_to_trigger();
  } else {
    if ( ( radio_state == DISABLED ) && ( TIFS_state == TIFS_TRIGGERING_TRX_EN ) ) {
      if ( Timer_RADIO != Timer_TIFS ){
        bs_trace_warning_line_time("NRF_RADIO: TIFS Ups 3\n");
      }
      TIFS_state = TIFS_DISABLE;
      Timer_RADIO = TIME_NEVER;
      nrf_hw_find_next_timer_to_trigger();
      from_hw_tifs = true;
      if ( TIFS_ToTxNotRx ) {
        nrf_radio_tasks_txen();
      } else {
        nrf_radio_tasks_rxen();
      }
      from_hw_tifs = false;
    } else {
      bs_trace_error_line_time(
          "NRF_RADIO: this should not have happened (radio_state =%i)\n",
          radio_state);
    }
  }
}

static void Tx_abort_eval_respond();
static void Rx_abort_eval_respond();

/**
 * The abort reevaluation timer has just triggered,
 * => we can now respond to the phy with our abort decision
 */
void nrf_radio_timer_abort_reeval_triggered(){
  Timer_RADIO_abort_reeval = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();

  if ( abort_fsm_state == Tx_Abort_reeval ){
    abort_fsm_state = No_pending_abort_reeval;
    Tx_abort_eval_respond();
  } else if ( abort_fsm_state == Rx_Abort_reeval ) {
    abort_fsm_state = No_pending_abort_reeval;
    Rx_abort_eval_respond();
  } else {
    bs_trace_error_line("The abort timer was left running.. somebody forgot to cleanup..\n");
  }
}

/**
 * Return the next time in which we should reevaluate the abort (in abs time)
 */
static bs_time_t abort_ctrl_next_reevaluate_abort_time(){
  return tm_get_next_timer_abstime();
  //This is set to the minimum of any timer which can create a new abort == effectively any timer
}

/**
 * Return the next abort time (in abs time)
 */
static bs_time_t abort_ctrl_next_abort_time(){
  if ( aborting_set == 1 ) {
    aborting_set = 0; //By returning tm_get_abs_time(), we are aborting ASAP
    return tm_get_abs_time();
  } else {
    return TIME_NEVER;
  }
}

/**
 * Handle all possible responses from the phy to a Tx request
 */
static void handle_Tx_response(int ret){
  if (ret == -1){
    bs_trace_raw_manual_time(3,tm_get_abs_time(),"The phy disconnected us during a Tx\n");
    hwll_disconnect_phy_and_exit();
  } else if ( ret == P2G4_MSG_TX_END  ) {
    bs_time_t end_time = hwll_dev_time_from_phy(ongoing_tx_done.end_time);
    tm_update_last_phy_sync_time( end_time );
  } else if ( ret == P2G4_MSG_ABORTREEVAL ) {
    tm_update_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = Tx_Abort_reeval;
    Timer_RADIO_abort_reeval = next_recheck_time;
    nrf_hw_find_next_timer_to_trigger();
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the phy with our decision
 */
static void Tx_abort_eval_respond(){
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &ongoing_tx.abort;
  next_recheck_time = abort_ctrl_next_reevaluate_abort_time();
  abort->abort_time  = hwll_phy_time_from_dev(abort_ctrl_next_abort_time());
  abort->recheck_time = hwll_phy_time_from_dev(next_recheck_time);

  int ret = p2G4_dev_provide_new_tx_abort_nc_b(abort);

  handle_Tx_response(ret);
}

static void start_Tx(){
  int S1Offset = 0;
  radio_state = TX;
  NRF_RADIO_regs.STATE = TX;

  {// a few checks to ensure the model is only used with the currently supported packet format
    int checked =NRF_RADIO_regs.PCNF0 &
        (RADIO_PCNF0_PLEN_Msk
            | RADIO_PCNF0_S1LEN_Msk
            | RADIO_PCNF0_S0LEN_Msk
            | RADIO_PCNF0_LFLEN_Msk);

    if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
      int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
          | ( 1 << RADIO_PCNF0_S0LEN_Pos )
          | ( 0 << RADIO_PCNF0_S1LEN_Pos )
          | ( RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos ) );

      if (checked != check) {
        bs_trace_error_line_time(
            "NRF_RADIO: For 1 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
            NRF_RADIO_regs.PCNF0);
      }

    } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {

      int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
          | ( 1 << RADIO_PCNF0_S0LEN_Pos )
          | ( 0 << RADIO_PCNF0_S1LEN_Pos )
          | ( RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos ) );

      if (checked != check) {
        bs_trace_error_line_time(
            "NRF_RADIO: For 2 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
            NRF_RADIO_regs.PCNF0);
      }

    } else {
      bs_trace_error_line_time(
          "NRF_RADIO: Only 1&2 Mbps BLE packet format supported so far (MODE=%u)\n",
          NRF_RADIO_regs.MODE);
    }

    if ( (NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk)
        != (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) ) {
      bs_trace_error_line_time(
          "NRF_RADIO: CRCCNF Only 3 bytes CRC is supported (CRCCNF=%u)\n",
          NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk);
    }
  }

  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  }

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_len;
  uint8_t address_len = 4;
  uint8_t header_len  = 2;
  uint8_t payload_len = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1];
  //Note: I assume in Tx the length is always < PCNF1.MAXLEN and that STATLEN is always 0 (otherwise add a check)
  uint8_t crc_len     = 3;

  uint32_t crc_init = NRF_RADIO_regs.CRCINIT & RADIO_CRCINIT_CRCINIT_Msk;

  //Note: we only support BALEN = 3 (== BLE 4 byte addresses)
  //Note: We only support address 0 being used
  uint32_t address = ( ( NRF_RADIO_regs.PREFIX0 & RADIO_PREFIX0_AP0_Msk ) << 24 )
                       | (NRF_RADIO_regs.BASE0 >> 8);

  uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;
  //Note only default freq. map supported
  double TxPower = (int8_t)( NRF_RADIO_regs.TXPOWER & RADIO_TXPOWER_TXPOWER_Msk); //the cast is to sign extend it

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    preamble_len = 1; //1 byte
    ongoing_tx.radio_params.modulation = P2G4_MOD_BLE;
    bits_per_us = 1;
  } else { //2Mbps
    preamble_len = 2; //2 bytes
    ongoing_tx.radio_params.modulation = P2G4_MOD_BLE2M;
    bits_per_us = 2;
  }

  tx_buf[0] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0];
  tx_buf[1] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1];
  memcpy(&tx_buf[2], &((uint8_t*)NRF_RADIO_regs.PACKETPTR)[2 + S1Offset], payload_len);
  append_crc_ble(tx_buf, 2/*header*/ + payload_len, crc_init);

  uint packet_bitlen = 0;
  packet_bitlen += preamble_len*8 + address_len*8;
  packet_bitlen += header_len*8;
  packet_bitlen += payload_len*8;
  packet_bitlen += crc_len*8;

  ongoing_tx.phy_address = address;
  ongoing_tx.power_level = p2G4_power_from_d(TxPower); //Note that any possible Tx antenna or PA gain would need to be included here

  p2G4_freq_t center_freq;
  p2G4_freq_from_d(freq_off, 1, &center_freq);
  ongoing_tx.radio_params.center_freq = center_freq;
  ongoing_tx.packet_size  = header_len + payload_len + crc_len; //Not including preamble or address

  bs_time_t tx_start_time = tm_get_abs_time() + radio_timings.TX_chain_delay;
  ongoing_tx.start_time = hwll_phy_time_from_dev(tx_start_time);
  ongoing_tx.end_time = ongoing_tx.start_time + (bs_time_t)(packet_bitlen / bits_per_us);

  //Prepare abort times:
  next_recheck_time = abort_ctrl_next_reevaluate_abort_time();
  ongoing_tx.abort.abort_time = hwll_phy_time_from_dev(abort_ctrl_next_abort_time());
  ongoing_tx.abort.recheck_time = hwll_phy_time_from_dev(next_recheck_time);

  //Request the Tx from the phy:
  int ret = p2G4_dev_req_tx_nc_b(&ongoing_tx, tx_buf,  &ongoing_tx_done);
  handle_Tx_response(ret);

  TX_ADDRESS_end_time = tm_get_hw_time() + (bs_time_t)((preamble_len*8 + address_len*8)/bits_per_us);
  TX_PAYLOAD_end_time = TX_ADDRESS_end_time + (bs_time_t)(8*(header_len + payload_len)/bits_per_us);
  TX_CRC_end_time = TX_PAYLOAD_end_time + (bs_time_t)(crc_len*8/bits_per_us);

  radio_sub_state = TX_WAIT_FOR_ADDRESS_END;
  Timer_RADIO = TX_ADDRESS_end_time;
  nrf_hw_find_next_timer_to_trigger();
}

uint32_t RSSI_value_to_modem_format(double rssi_value){
  rssi_value = -BS_MAX(rssi_value,-127);
  rssi_value = BS_MAX(rssi_value,0);
  return (uint32_t)rssi_value;
}

/**
 * Handle all possible responses from the phy to a Rx request
 */
static void handle_Rx_response(int ret){
  if ( ret == -1 ){
    bs_trace_raw_manual_time(3,tm_get_abs_time(),"Communication with the phy closed during Rx\n");
    hwll_disconnect_phy_and_exit();

  } else if ( ret == P2G4_MSG_ABORTREEVAL ) {
    tm_update_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = Rx_Abort_reeval;
    Timer_RADIO_abort_reeval = BS_MAX(next_recheck_time,tm_get_abs_time());
    nrf_hw_find_next_timer_to_trigger();

  } else if ( ( ret == P2G4_MSG_RX_ADDRESSFOUND ) && ( radio_state == RX /*if we havent aborted*/ ) ) {
    bs_time_t address_time = hwll_dev_time_from_phy(ongoing_rx_done.rx_time_stamp); //this is the end of the sync word in air time
    tm_update_last_phy_sync_time(address_time);

    ongoing_rx_RADIO_status.ADDRESS_End_Time = address_time + get_Rx_chain_delay();
    uint length = rx_buf[1];
    uint max_length = (NRF_RADIO_regs.PCNF1 & NFCT_MAXLEN_MAXLEN_Msk) >> NFCT_MAXLEN_MAXLEN_Pos;
    if (length > max_length){
      length  = max_length;
      //TODO: check packet length. If too long the packet should be truncated and not accepted from the phy, [we already have it in the buffer and we will have a CRC error anyhow. And we cannot let the phy run for longer than we will]
    }
    ongoing_rx_RADIO_status.packet_rejected = false;
    ongoing_rx_RADIO_status.PAYLOAD_End_Time = get_Rx_chain_delay() +
        hwll_dev_time_from_phy(ongoing_rx_done.rx_time_stamp
            + (bs_time_t)((2+length)*8/bits_per_us));
    ongoing_rx_RADIO_status.CRC_End_Time = get_Rx_chain_delay() +
        hwll_dev_time_from_phy(ongoing_rx_done.rx_time_stamp
            + (bs_time_t)((2+length)*8/bits_per_us)
            + ongoing_rx_RADIO_status.CRC_duration); //Provisional value

    if (ongoing_rx_done.packet_size >= 5) { /*At least the header and CRC, otherwise better to not try to copy it*/
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0] = rx_buf[0];
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1] = rx_buf[1];
      /* We cheat a bit and copy the whole packet already (The AAR block will look in Adv packets after 64 bits)*/
      memcpy(&((uint8_t*)NRF_RADIO_regs.PACKETPTR)[2 + ongoing_rx_RADIO_status.S1Offset],
          &rx_buf[2] , length);
    }

    radio_sub_state = RX_WAIT_FOR_ADDRESS_END;
    Timer_RADIO = ongoing_rx_RADIO_status.ADDRESS_End_Time ;
    nrf_hw_find_next_timer_to_trigger();
  } else if ( ( ret == P2G4_MSG_RX_END ) && ( radio_state == RX /*if we havent aborted*/ ) ) {
    bs_time_t end_time = hwll_dev_time_from_phy(ongoing_rx_done.end_time);
    tm_update_last_phy_sync_time(end_time);

    if (ongoing_rx_done.status != P2G4_RXSTATUS_HEADER_ERROR) {
        ongoing_rx_RADIO_status.CRC_End_Time = end_time + get_Rx_chain_delay();
    } //Otherwise we do not really now how the Nordic RADIO behaves depending on
    //where the biterrors are and so forth. So let's always behave like if the
    //packet lenght was received correctly, and just report a CRC error at the
    //end of the CRC

    if ( ( ongoing_rx_done.status == P2G4_RXSTATUS_OK ) &&
        ( ongoing_rx_done.packet_size > 5 ) ) {
      memcpy((void*)&NRF_RADIO_regs.RXCRC, &rx_buf[2 + rx_buf[1]], 3);
    }
    if ( ongoing_rx_done.status == P2G4_RXSTATUS_OK ){
      ongoing_rx_RADIO_status.CRC_OK = 1;
      NRF_RADIO_regs.CRCSTATUS = 1;
    }

    nrf_ccm_radio_received_packet(!ongoing_rx_RADIO_status.CRC_OK);

  }

}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the phy with our decision
 */
static void Rx_abort_eval_respond(){
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &ongoing_rx.abort;
  next_recheck_time = abort_ctrl_next_reevaluate_abort_time();
  abort->abort_time  = hwll_phy_time_from_dev(abort_ctrl_next_abort_time());
  abort->recheck_time = hwll_phy_time_from_dev(next_recheck_time);

  int ret = p2G4_dev_provide_new_rx_abort_nc_b(abort);

  handle_Rx_response(ret);
}

static void start_Rx(){
  radio_state = RX;
  NRF_RADIO_regs.STATE = RX;

  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    ongoing_rx_RADIO_status.S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  } else {
    ongoing_rx_RADIO_status.S1Offset = 0;
  }

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_length;
  uint8_t address_length  = 4;
  uint8_t header_length   = 2;
  //Note: we only support BALEN = 3 (== BLE 4 byte addresses)
  //Note: We only support address 0 being used
  uint32_t address = ( ( NRF_RADIO_regs.PREFIX0 & RADIO_PREFIX0_AP0_Msk ) << 24 )
                                          | (NRF_RADIO_regs.BASE0 >> 8);

  uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    //Note that we only support BLE packet formats by now (so we ignore the configuration of the preamble and just assume it is what it needs to be)
    //we rely on the Tx side error/warning being enough to warn users that we do not support other formats
    preamble_length = 1; //1 byte
    ongoing_rx.radio_params.modulation = P2G4_MOD_BLE;
    bits_per_us = 1;
  } else { //2Mbps
    preamble_length = 2; //2 bytes
    ongoing_rx.radio_params.modulation = P2G4_MOD_BLE2M;
    bits_per_us = 2;
  }

  ongoing_rx_RADIO_status.CRC_duration = 3*8/bits_per_us;
  ongoing_rx_RADIO_status.CRC_OK = false;
  NRF_RADIO_regs.CRCSTATUS = 0;

  p2G4_freq_t center_freq;
  p2G4_freq_from_d(freq_off, 1, &center_freq);
  ongoing_rx.radio_params.center_freq = center_freq;

  ongoing_rx.bps = bits_per_us*1000000;
  ongoing_rx.antenna_gain = 0;

  ongoing_rx.header_duration  = header_length*8/bits_per_us;
  ongoing_rx.header_threshold = 0; //(<=) we tolerate 0 bit errors in the header which will be found in the crc (we may want to tune this)
  ongoing_rx.sync_threshold   = 2; //(<) we tolerate less than 2 errors in the preamble and sync word together

  ongoing_rx.phy_address  = address;

  ongoing_rx.pream_and_addr_duration = (preamble_length + address_length)*8/bits_per_us;

  ongoing_rx.scan_duration = 0xFFFFFFFF; //the phy does not support infinite scans.. but this is 1 hour..

  ongoing_rx_done.status = P2G4_RXSTATUS_NOSYNC;

  //attempt to receive
  next_recheck_time = abort_ctrl_next_reevaluate_abort_time();

  ongoing_rx.start_time  = hwll_phy_time_from_dev(tm_get_abs_time());

  ongoing_rx.abort.abort_time = hwll_phy_time_from_dev(abort_ctrl_next_abort_time());
  ongoing_rx.abort.recheck_time = hwll_phy_time_from_dev(next_recheck_time);

  rx_pkt_buffer_ptr = (uint8_t*)&rx_buf;
  int ret = p2G4_dev_req_rx_nc_b(&ongoing_rx,
      &ongoing_rx_done,
      &rx_pkt_buffer_ptr,
      _NRF_MAX_PACKET_SIZE);

  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();

  handle_Rx_response(ret);
}

static void do_device_address_match();

/**
 * This function is called when the packet phy address has been received
 * (at the time of the end of the last bit of the packet address)
 */
static void Rx_Addr_received(){
  //libPhyCom has already copied the whole packet into the input buffer

  bool accept_packet = !ongoing_rx_RADIO_status.packet_rejected;

  if ( rssi_sampling_on ){
    NRF_RADIO_regs.RSSISAMPLE = RSSI_value_to_modem_format(p2G4_RSSI_value_to_dBm(ongoing_rx_done.rssi.RSSI));
    signal_RSSIEND();
  }

  NRF_RADIO_regs.RXMATCH = 0;//The only we support so far
  //NRF_RADIO_regs.PDUSTAT = 0; PDUSTAT is missing in registers...

  if (NRF_RADIO_regs.DACNF & 0xFF) { /*If any of the addresses for device address match is enabled*/
    /*
     * NOTE: we cheat and we already check the advertisement addresses and
     * raise the event, even though we should wait for 16 + 48 bits more
     *
     * If this is a problem, add a new timer and delay raising the event
     * until then
     */
    do_device_address_match();
  }

  int ret = p2G4_dev_rx_cont_after_addr_nc_b(accept_packet);

  if ( accept_packet ){
    handle_Rx_response(ret);
  } else {
    //else We said we dont want to continue => there will be no response (ret==0 always). We just close the reception like if the phy finished on its own even though we finished it
  }
}


/**************************************************************
 * Device address match functionality (advertisement packets) *
 **************************************************************/

static void signal_DEVMATCH() {
  NRF_RADIO_regs.EVENTS_DEVMATCH = 1;
  nrf_ppi_event(RADIO_EVENTS_DEVMATCH);

  if (RADIO_INTEN & RADIO_INTENSET_DEVMATCH_Msk) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void signal_DEVMISS() {
  NRF_RADIO_regs.EVENTS_DEVMISS = 1;
  nrf_ppi_event(RADIO_EVENTS_DEVMISS);

  if (RADIO_INTEN & RADIO_INTENSET_DEVMISS_Msk) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

static void do_device_address_match() {
  int i;
  bool match_found = false;
  bool nomatch;
  int TxAdd;

  for (i = 0 ; i < 8; i++) {
    if (((NRF_RADIO_regs.DACNF >> i) & 1) == 0 ) {
      continue;
    }

    TxAdd = (NRF_RADIO_regs.DACNF >> (i + 8)) & 1;

    if (TxAdd != ((rx_buf[0] >> 6) & 1) ) {
      continue;
    }

    nomatch = (*(uint32_t *)(rx_buf + 2) != NRF_RADIO_regs.DAB[i]);
    uint32_t DAP = NRF_RADIO_regs.DAP[i] & UINT16_MAX;
    nomatch |= (*(uint16_t *)(rx_buf + 6) != DAP);

    if (nomatch) {
      continue;
    }

    match_found = true;
    NRF_RADIO_regs.DAI = i;
    break;
  }

  if (match_found) {
    signal_DEVMATCH();
  } else {
    signal_DEVMISS();
  }
}

/******************************
 * Bit counter functionality: *
 ******************************/

static void signal_BCMATCH() {
  NRF_RADIO_regs.EVENTS_BCMATCH = 1;
  nrf_ppi_event(RADIO_EVENTS_BCMATCH);

  if (RADIO_INTEN & RADIO_INTENSET_BCMATCH_Msk) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_RADIO_IRQn);
  }
}

void nrf_radio_bitcounter_timer_triggered() {
  signal_BCMATCH();
  Timer_RADIO_bitcounter = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
  //Note that we leave the bit counter running, so a new BCC can be programmed to make it trigger later
}

void nrf_radio_tasks_bcstart() {
  /* Note that we do not validate that the end of the address has been received */

  if (bit_counter_running) {
    bs_trace_warning_line_time("NRF_RADIO: BCSTART received while the bitcounter was already running."
        "New BCSTART is just ignored\n");
    return;
  }
  bit_counter_running = true;
  Time_BitCounterStarted = tm_get_hw_time();
  Timer_RADIO_bitcounter = Time_BitCounterStarted + NRF_RADIO_regs.BCC/bits_per_us;
  nrf_hw_find_next_timer_to_trigger();
}

static void nrf_radio_stop_bit_counter() {
  if (!bit_counter_running){
    return;
  }
  bit_counter_running = false;
  if (Timer_RADIO_bitcounter != TIME_NEVER) {
    Timer_RADIO_bitcounter = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  }
}

void nrf_radio_tasks_bcstop() {
  nrf_radio_stop_bit_counter();
}

void nrf_radio_regw_sideeffects_BCC() {
  if (!bit_counter_running){
    return;
  }
  Timer_RADIO_bitcounter = Time_BitCounterStarted + NRF_RADIO_regs.BCC/bits_per_us;
  if (Timer_RADIO_bitcounter < tm_get_hw_time()) {
    bs_trace_warning_line_time("NRF_RADIO: Reprogrammed bitcounter with a BCC which has already"
        "passed (%"PRItime") => we ignore it\n",
        Timer_RADIO_bitcounter);
    Timer_RADIO_bitcounter = TIME_NEVER;
  }
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_radio_regw_sideeffects_TASKS_BCSTART() {
  if (NRF_RADIO_regs.TASKS_BCSTART) {
    NRF_RADIO_regs.TASKS_BCSTART = 0;
    nrf_radio_tasks_bcstart();
  }
}

void nrf_radio_regw_sideeffects_TASKS_BCSTOP() {
  if (NRF_RADIO_regs.TASKS_BCSTOP) {
    NRF_RADIO_regs.TASKS_BCSTOP = 0;
    nrf_radio_tasks_bcstop();
  }
}
