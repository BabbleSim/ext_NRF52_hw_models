/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
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
#include "NRF_HWLowL.h"
#include "crc_ble.h"
#include "NRF_RADIO_signals.h"
#include "NRF_RADIO_utils.h"
#include "NRF_RADIO_timings.h"
#include "NRF_RADIO_bitcounter.h"

/**
 * RADIO — 2.4 GHz Radio
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/radio.html?cp=4_1_0_5_17
 *
 * Note: as of now, only 1&2Mbps BLE packet formats are supported, there is quite many notes around in the code
 * where changes would be required to support other formats
 *
 * Note3: Only logical address 0 (in Tx or Rx) is supported
 *
 * Note4: only default freq. map supported
 *
 * Note5: Only little endian hosts supported (x86 is little endian)
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
 *
 * Note12: Nothing related to 802.15.4 (including energy detection (ED) and CCA) is implemented
 *
 * Note13: Nothing related to AoA/AoD features (CTE, DFE) is implemented
 *
 * Note14: No 52833 new radio state change events (EVENTS_FRAMESTART, EVENTS_EDEND, EVENTS_EDSTOPPED, EVENTS_CCAIDLE, EVENTS_CCABUSY, EVENTS_CCASTOPPED,
 *         EVENTS_RATEBOOST, EVENTS_TXREADY, EVENTS_RXREADY, EVENTS_MHRMATCH, EVENTS_SYNC, EVENTS_PHYEND & EVENTS_CTEPRESENT) implemented
 *
 * Note15: PDUSTAT not yet implemented
 *
 * Note16: No antenna switching
 *
 * Note17: Interrupts are modeled as pulses to the NVIC, not level interrupts as they are in reality
 */

NRF_RADIO_Type NRF_RADIO_regs;
uint32_t NRF_RADIO_INTEN = 0; //interrupt enable

bs_time_t Timer_RADIO = TIME_NEVER; //main radio timer
bs_time_t Timer_RADIO_abort_reeval = TIME_NEVER; //Abort reevaluation response timer, this timer must have the lowest priority of all events (which may cause an abort)

static enum {TIFS_DISABLE = 0, TIFS_WAITING_FOR_DISABLE, TIFS_TRIGGERING_TRX_EN } TIFS_state = TIFS_DISABLE;
bool TIFS_ToTxNotRx = false;
bs_time_t Timer_TIFS = TIME_NEVER;
static bool from_hw_tifs = false; /* Unfortunate hack due to the SW racing the HW to clear SHORTS*/

static bs_time_t TX_ADDRESS_end_time, TX_PAYLOAD_end_time, TX_CRC_end_time;

//Ongoing Rx/Tx structures:
static p2G4_rxv2_t  ongoing_rx;
static struct {
  bs_time_t CRC_duration;
  bool CRC_OK;
  bs_time_t ADDRESS_End_Time;
  bs_time_t PAYLOAD_End_Time;
  bs_time_t CRC_End_Time;
  bool packet_rejected;
  bool S1Offset;
} ongoing_rx_RADIO_status;
static p2G4_txv2_t    ongoing_tx;
static p2G4_tx_done_t ongoing_tx_done;
static p2G4_rxv2_done_t ongoing_rx_done;
static bs_time_t next_recheck_time; // when we asked the phy to recheck (in our own time) next time
static double bits_per_us; //Bits per us for the ongoing Tx or Rx (shared with the bit counter)

typedef enum { No_pending_abort_reeval = 0, Tx_Abort_reeval, Rx_Abort_reeval } abort_state_t;
static abort_state_t abort_fsm_state = No_pending_abort_reeval; //This variable shall be set to Tx/Rx_Abort_reeval when the phy is waiting for an abort response (and in no other circumstance)
static int aborting_set = 0; //If set, we will abort the current Tx/Rx at the next abort reevaluation

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

#define _NRF_MAX_PACKET_SIZE (256+2+4)
static uint8_t tx_buf[_NRF_MAX_PACKET_SIZE]; //starting from the header, and including CRC
static uint8_t rx_buf[_NRF_MAX_PACKET_SIZE]; // "
static uint8_t *rx_pkt_buffer_ptr = (uint8_t*)&rx_buf;

static bool radio_on = false;

static bool rssi_sampling_on = false;

static void start_Tx();
static void start_Rx();
static void Rx_Addr_received();
static void Tx_abort_eval_respond();
static void Rx_abort_eval_respond();

static void radio_reset() {
  memset(&NRF_RADIO_regs, 0, sizeof(NRF_RADIO_regs));
  radio_state = DISABLED;
  NRF_RADIO_INTEN = 0;
  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  rssi_sampling_on = false;

  TIFS_state = TIFS_DISABLE;
  TIFS_ToTxNotRx = false;
  Timer_TIFS = TIME_NEVER;

  nrf_radio_bitcounter_reset();
}

void nrf_radio_init() {
  nrfra_timings_init();
  radio_reset();
  radio_on = false;
  bits_per_us = 1;
}

void nrf_radio_clean_up() {
  nrf_radio_bitcounter_cleanup();
}

double nrf_radio_get_bpus(){
  return bits_per_us;
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

  Timer_RADIO = tm_get_hw_time() + nrfra_timings_get_rampup_time(1, from_hw_tifs);
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
  Timer_RADIO = tm_get_hw_time() + nrfra_timings_get_rampup_time(0, from_hw_tifs);
  nrf_hw_find_next_timer_to_trigger();
}

static void abort_if_needed(){
  if ( ( abort_fsm_state == Tx_Abort_reeval ) || ( abort_fsm_state == Rx_Abort_reeval ) ){
    //If the phy is waiting for a response from us, we need to tell it, that we are aborting whatever it was doing
    aborting_set = 1;
  }
  if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ){
    //we answer immediately to the phy rejecting the packet
    p2G4_dev_rxv2_cont_after_addr_nc_b(false);
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
    NRF_RADIO_regs.STATE = TXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RX ){
    abort_if_needed();
    radio_state = RXIDLE;
    NRF_RADIO_regs.STATE = RXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_STOP received while the radio was not on either TX or RX but in state %i. It will be ignored => ignored\n",
        radio_state);
  }
}

void nrf_radio_tasks_disable() {
  nrf_radio_stop_bit_counter();

  if ( radio_state == TX ){
    abort_if_needed();
    radio_state = TXIDLE; //Momentary (will be changedin the if below)
    NRF_RADIO_regs.STATE = TXIDLE;
  } else if ( radio_state == RX ){
    abort_if_needed();
    radio_state = RXIDLE; //Momentary (will be changed in the if below)
    NRF_RADIO_regs.STATE = RXIDLE;
  }

  if (TIFS_state != TIFS_DISABLE) {
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = TIME_NEVER;
    Timer_TIFS = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
  }

  if ( ( radio_state == TXRU ) || ( radio_state == TXIDLE ) ) {
    radio_state = TXDISABLE;
    NRF_RADIO_regs.STATE = TXDISABLE;
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = tm_get_hw_time() + nrfra_timings_get_TX_rampdown_time();
    nrf_hw_find_next_timer_to_trigger();
  } else if ( ( radio_state == RXRU ) || ( radio_state == RXIDLE ) ) {
    radio_state = RXDISABLE;
    NRF_RADIO_regs.STATE = RXDISABLE;
    TIFS_state = TIFS_DISABLE;
    Timer_RADIO = tm_get_hw_time() + nrfra_timings_get_RX_rampdown_time();
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == DISABLED ) {
    //It seems the radio will also signal a DISABLED event even if it was already disabled
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
  }
}

void nrf_radio_tasks_rssistart () {
  rssi_sampling_on = true;
}

void nrf_radio_tasks_rssistop(){
  rssi_sampling_on = false;
}

void nrf_radio_regw_sideeffects_INTENSET(){
  if ( NRF_RADIO_regs.INTENSET ){
    NRF_RADIO_INTEN |= NRF_RADIO_regs.INTENSET;
    NRF_RADIO_regs.INTENSET = NRF_RADIO_INTEN;
  }
}

void nrf_radio_regw_sideeffects_INTENCLR(){
  if ( NRF_RADIO_regs.INTENCLR ){
    NRF_RADIO_INTEN  &= ~NRF_RADIO_regs.INTENCLR;
    NRF_RADIO_regs.INTENSET = NRF_RADIO_INTEN;
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
/*
 * This is a fake task meant to start a HW timer for the TX->RX or RX->TX TIFS
 */
void nrf_radio_fake_task_TRXEN_TIFS(){
  if ( TIFS_state == TIFS_WAITING_FOR_DISABLE ) {
    TIFS_state = TIFS_TRIGGERING_TRX_EN;
    Timer_RADIO = Timer_TIFS;
    if ( Timer_RADIO < tm_get_hw_time() ){
      bs_trace_warning_line_time("NRF_RADIO: TIFS Ups: The Ramp down from Rx/Tx into a Tx/Rx takes more than the programmed TIFS time\n");
    }
    nrf_hw_find_next_timer_to_trigger();
  }
}

void maybe_prepare_TIFS(bool Tx_Not_Rx){
  bs_time_t delta;
  if ( !nrfra_is_HW_TIFS_enabled() ) {
    return;
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ){
    TIFS_ToTxNotRx = true;
  } else {
    TIFS_ToTxNotRx = false;
  }

  if ( Tx_Not_Rx ){ //End of Tx
    delta = NRF_RADIO_regs.TIFS + nrfra_timings_get_TX_chain_delay() - nrfra_timings_get_rampup_time(0, 1) - 3; /*open slightly earlier to have jitter margin*/
  } else { //End of Rx
    delta = NRF_RADIO_regs.TIFS - nrfra_timings_get_Rx_chain_delay() - nrfra_timings_get_TX_chain_delay() - nrfra_timings_get_rampup_time(1, 1) + 1;
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
    nrf_radio_signal_READY();
    //nrf_radio_signal_TXREADY();
  } else if ( radio_state == RXRU ){
    radio_state = RXIDLE;
    NRF_RADIO_regs.STATE = RXIDLE;
    Timer_RADIO = TIME_NEVER;
    nrf_hw_find_next_timer_to_trigger();
    nrf_radio_signal_READY();
    //nrf_radio_signal_RXREADY();
  } else if ( radio_state == TX ){
    if ( radio_sub_state == TX_WAIT_FOR_ADDRESS_END ){
      radio_sub_state = TX_WAIT_FOR_PAYLOAD_END;
      Timer_RADIO = TX_PAYLOAD_end_time;
      nrf_radio_signal_ADDRESS();
    } else if ( radio_sub_state == TX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = TX_WAIT_FOR_CRC_END;
      Timer_RADIO = TX_CRC_end_time;
      nrf_radio_signal_PAYLOAD();
    } else if ( radio_sub_state == TX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = TXIDLE;
      NRF_RADIO_regs.STATE = TXIDLE;
      Timer_RADIO = TIME_NEVER;
      nrf_radio_stop_bit_counter();
      nrf_radio_signal_END();
      maybe_prepare_TIFS(true);
    }  else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RX ){
    if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ) {
      Timer_RADIO = TIME_NEVER;
      nrf_hw_find_next_timer_to_trigger();
      nrf_radio_signal_ADDRESS();
      Rx_Addr_received();
      radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
      Timer_RADIO = ongoing_rx_RADIO_status.PAYLOAD_End_Time;
      nrf_hw_find_next_timer_to_trigger();
    } else if ( radio_sub_state == RX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = RX_WAIT_FOR_CRC_END;
      Timer_RADIO = ongoing_rx_RADIO_status.CRC_End_Time;
      nrf_hw_find_next_timer_to_trigger();
      nrf_radio_signal_PAYLOAD();
    } else if ( radio_sub_state == RX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RXIDLE;
      NRF_RADIO_regs.STATE = RXIDLE;
      Timer_RADIO = TIME_NEVER;
      nrf_hw_find_next_timer_to_trigger();
      if ( ongoing_rx_RADIO_status.CRC_OK ) {
        nrf_radio_signal_CRCOK();
      } else {
        nrf_radio_signal_CRCERROR();
      }
      nrf_radio_stop_bit_counter();
      nrf_radio_signal_END();
      maybe_prepare_TIFS(false);
    } else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == TXDISABLE ){
    radio_state = DISABLED;
    NRF_RADIO_regs.STATE = DISABLED;
    Timer_RADIO = TIME_NEVER;
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
    nrf_hw_find_next_timer_to_trigger();
  } else if ( radio_state == RXDISABLE ){
    radio_state = DISABLED;
    NRF_RADIO_regs.STATE = DISABLED;
    Timer_RADIO = TIME_NEVER;
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
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
 * Set the Phy abort structure to the next time we will want to either abort or have a recheck
 * And store in next_recheck_time the next recheck time
 */
static void update_abort_struct(p2G4_abort_t *abort, bs_time_t *next_recheck_time){
  //We will want to recheck next time anything may decide to stop the radio, that can be SW or HW
  //The only logical way to do so is to set it to the next timer whatever it may be as many can trigger SW interrupts
  *next_recheck_time = tm_get_next_timer_abstime();
  abort->recheck_time = hwll_phy_time_from_dev(*next_recheck_time);

  //We either have decided already we want to abort so we do it right now
  //or we have not decided yet
  if ( aborting_set == 1 ) {
    aborting_set = 0; //By returning tm_get_abs_time(), we are aborting right now
    abort->abort_time = hwll_phy_time_from_dev(tm_get_abs_time());
  } else {
    abort->abort_time = TIME_NEVER;
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the phy with our decision
 */
static void Tx_abort_eval_respond(){
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &ongoing_tx.abort;

  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_tx_abort_nc_b(abort);

  handle_Tx_response(ret);
}

static void start_Tx(){

  radio_state = TX;
  NRF_RADIO_regs.STATE = TX;

  nrfra_check_packet_conf();

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_len;
  uint8_t address_len = 4;
  uint8_t header_len  = 2;
  uint8_t payload_len = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1];
  //Note: I assume in Tx the length is always < PCNF1.MAXLEN and that STATLEN is always 0 (otherwise add a check)
  uint8_t crc_len     = 3;

  //Note only default freq. map supported

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    preamble_len = 1; //1 byte
    bits_per_us = 1;
  } else { //2Mbps
    preamble_len = 2; //2 bytes
    bits_per_us = 2;
  }

  tx_buf[0] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0];
  tx_buf[1] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1];

  int S1Offset = 0;
  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  }
  memcpy(&tx_buf[2], &((uint8_t*)NRF_RADIO_regs.PACKETPTR)[2 + S1Offset], payload_len);

  uint32_t crc_init = NRF_RADIO_regs.CRCINIT & RADIO_CRCINIT_CRCINIT_Msk;
  append_crc_ble(tx_buf, 2/*header*/ + payload_len, crc_init);

  bs_time_t packet_duration; //From preamble to CRC
  packet_duration  = preamble_len*8 + address_len*8;
  packet_duration += header_len*8 + payload_len*8 + crc_len*8;
  packet_duration /= bits_per_us;
  uint packet_size = header_len + payload_len + crc_len;

  nrfra_prep_tx_request(&ongoing_tx, packet_size, packet_duration);

  update_abort_struct(&ongoing_tx.abort, &next_recheck_time);

  //Request the Tx from the phy:
  int ret = p2G4_dev_req_txv2_nc_b(&ongoing_tx, tx_buf,  &ongoing_tx_done);
  handle_Tx_response(ret);

  TX_ADDRESS_end_time = tm_get_hw_time() + (bs_time_t)((preamble_len*8 + address_len*8)/bits_per_us);
  TX_PAYLOAD_end_time = TX_ADDRESS_end_time + (bs_time_t)(8*(header_len + payload_len)/bits_per_us);
  TX_CRC_end_time = TX_PAYLOAD_end_time + (bs_time_t)(crc_len*8/bits_per_us);

  radio_sub_state = TX_WAIT_FOR_ADDRESS_END;
  Timer_RADIO = TX_ADDRESS_end_time;
  nrf_hw_find_next_timer_to_trigger();
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

  } else if ( ( ret == P2G4_MSG_RXV2_ADDRESSFOUND ) && ( radio_state == RX /*if we havent aborted*/ ) ) {

    bs_time_t address_time = hwll_dev_time_from_phy(ongoing_rx_done.rx_time_stamp); //this is the end of the sync word in air time
    tm_update_last_phy_sync_time(address_time);

    ongoing_rx_RADIO_status.ADDRESS_End_Time = address_time + nrfra_timings_get_Rx_chain_delay();
    uint length = rx_buf[1];
    uint max_length = (NRF_RADIO_regs.PCNF1 & NFCT_MAXLEN_MAXLEN_Msk) >> NFCT_MAXLEN_MAXLEN_Pos;
    if (length > max_length){
      bs_trace_error_time_line("NRF_RADIO: received a packet longer than the configured max lenght (%i>%i), this is not yet handled in this models. I stop before it gets confusing\n", length, max_length);
      length  = max_length;
      //TODO: check packet length. If too long the packet should be truncated and not accepted from the phy, [we already have it in the buffer and we will have a CRC error anyhow. And we cannot let the phy run for longer than we will]
    }
    ongoing_rx_RADIO_status.packet_rejected = false;
    ongoing_rx_RADIO_status.PAYLOAD_End_Time = nrfra_timings_get_Rx_chain_delay() +
        hwll_dev_time_from_phy(ongoing_rx_done.rx_time_stamp
            + (bs_time_t)((2+length)*8/bits_per_us));
    ongoing_rx_RADIO_status.CRC_End_Time = nrfra_timings_get_Rx_chain_delay() +
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

  } else if ( ( ret == P2G4_MSG_RXV2_END ) && ( radio_state == RX /*if we havent aborted*/ ) ) {

    bs_time_t end_time = hwll_dev_time_from_phy(ongoing_rx_done.end_time);
    tm_update_last_phy_sync_time(end_time);

    if (ongoing_rx_done.status != P2G4_RXSTATUS_HEADER_ERROR) {
        ongoing_rx_RADIO_status.CRC_End_Time = end_time + nrfra_timings_get_Rx_chain_delay();
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
  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_rxv2_abort_nc_b(abort);

  handle_Rx_response(ret);
}

/*
 * Actually start the Rx in this microsecond
 */
static void start_Rx(){
  #define RX_N_ADDR 8 /* How many addresses we can search in parallel */
  p2G4_address_t rx_addresses[RX_N_ADDR];

  radio_state = RX;
  NRF_RADIO_regs.STATE = RX;
  NRF_RADIO_regs.CRCSTATUS = 0;

  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    ongoing_rx_RADIO_status.S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  } else {
    ongoing_rx_RADIO_status.S1Offset = 0;
  }

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    bits_per_us = 1;
  } else { //2Mbps
    bits_per_us = 2;
  }

  ongoing_rx_RADIO_status.CRC_duration = 3*8/bits_per_us;
  ongoing_rx_RADIO_status.CRC_OK = false;
  ongoing_rx_done.status = P2G4_RXSTATUS_NOSYNC;

  nrfra_prep_rx_request(&ongoing_rx, rx_addresses);

  update_abort_struct(&ongoing_rx.abort, &next_recheck_time);

  //attempt to receive
  int ret = p2G4_dev_req_rxv2_nc_b(&ongoing_rx,
      rx_addresses,
      &ongoing_rx_done,
      &rx_pkt_buffer_ptr,
      _NRF_MAX_PACKET_SIZE);

  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();

  handle_Rx_response(ret);
}

static void nrf_radio_device_address_match();

/**
 * This function is called at the time when the Packet address would have been
 * completely received
 * (at the time of the end of the last bit of the packet address)
 * To continue processing the reception (the Phy was left waiting for a response)
 *
 * Note that libPhyCom has already copied the whole packet into the input buffer
 */
static void Rx_Addr_received(){

  bool accept_packet = !ongoing_rx_RADIO_status.packet_rejected;

  if ( rssi_sampling_on ){
    NRF_RADIO_regs.RSSISAMPLE = nrfra_RSSI_value_to_modem_format(p2G4_RSSI_value_to_dBm(ongoing_rx_done.rssi.RSSI));
    nrf_radio_signal_RSSIEND();
  }

  NRF_RADIO_regs.RXMATCH = 0;//The only we support so far
  //NRF_RADIO_regs.PDUSTAT = 0; PDUSTAT is missing in registers...

  if (NRF_RADIO_regs.DACNF & 0xFF) { /*If any of the addresses for device address match is enabled*/
    /*
     * NOTE: we cheat and we already check the advertisement addresses and
     * raise the event, even though we should wait for 16 + 48 bits more
     *
     * If this is a problem, add a new timer and Rx state and delay raising the event
     * until then
     */
    nrf_radio_device_address_match(rx_buf);
  }

  int ret = p2G4_dev_rxv2_cont_after_addr_nc_b(accept_packet);

  if ( accept_packet ){
    handle_Rx_response(ret);
  } else {
    //else We said we dont want to continue => there will be no response (ret==0 always). We just close the reception like if the phy finished on its own even though we finished it
  }
}

/**
 * Check if the address in the received (advertisement) packet
 * matches one configured in the DAP/DAB registers as set by DACNF
 *
 * If it does, it sets appropriately the DAI register,
 * in any case, it generates the DEVMATCH and DEVMISS signals accordingly
 *
 * Note that, as specified in the infocenter documentation,
 * the address is assumed to be the first 48 bits after the 2 byte header
 * and the TxAddr bit to be 7th bit in 1st header byte as per the BT Core spec.
 */
static void nrf_radio_device_address_match(uint8_t rx_buf[]) {
  bool match_found = false;
  bool nomatch;
  int TxAdd;

  for (int i = 0 ; i < 8; i++) {
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
    nrf_radio_signal_DEVMATCH();
  } else {
    nrf_radio_signal_DEVMISS();
  }
}
