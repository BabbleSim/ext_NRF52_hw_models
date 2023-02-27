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
#include "NRF_RADIO_priv.h"

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
 * Note11b:The CRC configuration is directly deduced from the modulation, only BLE and 154 CRCs are supported so far
 *
 * Note12: Nothing related to 802.15.4 (including energy detection (ED) and CCA) is implemented
 *
 * Note13: Nothing related to AoA/AoD features (CTE, DFE) is implemented
 *
 * Note14: No 52833 new radio state change events (EVENTS_FRAMESTART, EVENTS_EDEND, EVENTS_EDSTOPPED, EVENTS_CCAIDLE, EVENTS_CCABUSY, EVENTS_CCASTOPPED,
 *         EVENTS_RATEBOOST, EVENTS_MHRMATCH & EVENTS_CTEPRESENT) implemented
 *
 * Note15: PDUSTAT not yet implemented
 *
 * Note16: No antenna switching
 *
 * Note17: Interrupts are modeled as pulses to the NVIC, not level interrupts as they are in reality
 *
 * Note18: EVENTS_SYNC:
 *         a) It is not generated at the exact correct time:
 *         In this model it is generated at the end of the address (or SFD)
 *         whileaccording to the infocenter spec this should come at the end of the preamble,
 *         and only if in coded and 15.4 mode.
 *         In reality it seems to come somewhere during the preamble for 15.4 and coded phy,
 *         and somewhere during the address for 1&2Mbps BLE.
 *         In any case this seems to be a debug signal, and a quite imprecise one,
 *         so the assumption is that nobody uses it for anything timing critical.
 *         b) it is only generated when there is a full address match. While in real HW this is not required
 *         (so false positives happen in real HW)
 *
 * Note19: EVENTS_PHYEND
 *         It is not generated at the exact correct time. In the model it is generated at the
 *         exact same time as END. While according to the spec, it should be generated with the last
 *         bit on *air* (That is the Tx chain delay later for Tx, and RxChainDelay earlier for Rx)
 */

NRF_RADIO_Type NRF_RADIO_regs;
uint32_t NRF_RADIO_INTEN = 0; //interrupt enable (global for RADIO_signals.c)

bs_time_t Timer_RADIO = TIME_NEVER; //main radio timer
bs_time_t Timer_RADIO_abort_reeval = TIME_NEVER; //Abort reevaluation response timer, this timer must have the lowest priority of all events (which may cause an abort)

static TIFS_state_t TIFS_state = TIFS_DISABLE;
static bool TIFS_ToTxNotRx = false; //Are we in a TIFS automatically starting a Tx from a Rx (true), or Rx from Tx (false)
static bs_time_t Timer_TIFS = TIME_NEVER;
static bool from_hw_tifs = false; /* Unfortunate hack due to the SW racing the HW to clear SHORTS*/

static RADIO_Rx_status_t rx_status;
static RADIO_Tx_status_t tx_status;

static double bits_per_us; //Bits per us for the ongoing Tx or Rx

static bs_time_t next_recheck_time; // when we asked the phy to recheck (in our own time) next time
static abort_state_t abort_fsm_state = No_pending_abort_reeval; //This variable shall be set to Tx/Rx_Abort_reeval when the phy is waiting for an abort response (and in no other circumstance)
static int aborting_set = 0; //If set, we will abort the current Tx/Rx at the next abort reevaluation

static nrfra_state_t radio_state;
static nrfra_sub_state_t radio_sub_state;

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
static void nrf_radio_device_address_match();

static void radio_reset() {
  memset(&NRF_RADIO_regs, 0, sizeof(NRF_RADIO_regs));
  radio_state = RAD_DISABLED;
  NRF_RADIO_INTEN = 0;
  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  rssi_sampling_on = false;

  TIFS_state = TIFS_DISABLE;
  TIFS_ToTxNotRx = false;
  Timer_TIFS = TIME_NEVER;

  nrf_radio_bitcounter_reset();

  //Registers' reset values:
  NRF_RADIO_regs.FREQUENCY = 0x00000002;
  NRF_RADIO_regs.DATAWHITEIV = 0x00000040;
  NRF_RADIO_regs.MODECNF0 = 0x00000200;
  NRF_RADIO_regs.SFD = 0xA7;
  NRF_RADIO_regs.CCACTRL = 0x052D0000;
  NRF_RADIO_regs.CTEINLINECONF = 0x00002800;
  NRF_RADIO_regs.DFECTRL1 = 0x00023282;
  for (int i = 0; i < 8; i++)
    NRF_RADIO_regs.PSEL.DFEGPIO[i] = 0xFFFFFFFF;
  NRF_RADIO_regs.DFEPACKET.MAXCNT = 0x00001000;
  NRF_RADIO_regs.POWER = 1;
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

static inline void nrfra_set_Timer_RADIO(bs_time_t t){
  Timer_RADIO = t;
  nrf_hw_find_next_timer_to_trigger();
}

static inline void nrfra_set_Timer_abort_reeval(bs_time_t t){
  Timer_RADIO_abort_reeval = t;
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_radio_tasks_TXEN() {
  if ( ( radio_state != RAD_DISABLED )
      && ( radio_state != RAD_TXIDLE )
      && ( radio_state != RAD_RXIDLE ) ){
    bs_trace_warning_line_time(
        "NRF_RADIO: TXEN received when the radio was not DISABLED or TX/RXIDLE but in state %i. It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  TIFS_state = TIFS_DISABLE;
  radio_state = RAD_TXRU;
  NRF_RADIO_regs.STATE = RAD_TXRU;

  nrfra_set_Timer_RADIO(tm_get_hw_time() + nrfra_timings_get_rampup_time(1, from_hw_tifs));
}

void nrf_radio_tasks_RXEN() {
  if ( ( radio_state != RAD_DISABLED )
      && ( radio_state != RAD_TXIDLE )
      && ( radio_state != RAD_RXIDLE ) ){
    bs_trace_warning_line_time(
        "NRF_RADIO: RXEN received when the radio was not DISABLED or TX/RXIDLE but in state %i. It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  TIFS_state = TIFS_DISABLE;
  radio_state = RAD_RXRU;
  NRF_RADIO_regs.STATE = RAD_RXRU;
  nrfra_set_Timer_RADIO(tm_get_hw_time() + nrfra_timings_get_rampup_time(0, from_hw_tifs));
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

void nrf_radio_tasks_START () {
  if ( radio_state == RAD_TXIDLE ) {
    start_Tx();
  } else if ( radio_state == RAD_RXIDLE ) {
    start_Rx();
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_START received while the radio was not in either TXIDLE or RXIDLE but in state %i. It will be ignored => expect problems\n",
        radio_state);
  }
}

void nrf_radio_tasks_CCASTART() {
  bs_trace_error_time_line("%s not yet implemented\n", __func__);
}
void nrf_radio_tasks_CCASTOP() {
  bs_trace_error_time_line("%s not yet implemented\n", __func__);
}
void nrf_radio_tasks_EDSTART() {
  bs_trace_error_time_line("%s not yet implemented\n", __func__);
}
void nrf_radio_tasks_EDSTOP() {
  bs_trace_error_time_line("%s not yet implemented\n", __func__);
}

void nrf_radio_tasks_STOP(){
  nrf_radio_stop_bit_counter();

  if ( radio_state == RAD_TX ){
    abort_if_needed();
    radio_state = RAD_TXIDLE;
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
    nrfra_set_Timer_RADIO(TIME_NEVER);
  } else if ( radio_state == RAD_RX ){
    abort_if_needed();
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nrfra_set_Timer_RADIO(TIME_NEVER);
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_STOP received while the radio was not on either TX or RX but in state %i. It will be ignored => ignored\n",
        radio_state);
  }
}

void nrf_radio_tasks_DISABLE() {
  nrf_radio_stop_bit_counter();

  if ( radio_state == RAD_TX ){
    abort_if_needed();
    radio_state = RAD_TXIDLE; //Momentary (will be changedin the if below)
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
  } else if ( radio_state == RAD_RX ){
    abort_if_needed();
    radio_state = RAD_RXIDLE; //Momentary (will be changed in the if below)
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
  }

  if (TIFS_state != TIFS_DISABLE) {
    TIFS_state = TIFS_DISABLE;
    nrfra_set_Timer_RADIO(TIME_NEVER);
    Timer_TIFS = TIME_NEVER;
  }

  if ( ( radio_state == RAD_TXRU ) || ( radio_state == RAD_TXIDLE ) ) {
    radio_state = RAD_TXDISABLE;
    NRF_RADIO_regs.STATE = RAD_TXDISABLE;
    TIFS_state = TIFS_DISABLE;
    nrfra_set_Timer_RADIO(tm_get_hw_time() + nrfra_timings_get_TX_rampdown_time());
  } else if ( ( radio_state == RAD_RXRU ) || ( radio_state == RAD_RXIDLE ) ) {
    radio_state = RAD_RXDISABLE;
    NRF_RADIO_regs.STATE = RAD_RXDISABLE;
    TIFS_state = TIFS_DISABLE;
    nrfra_set_Timer_RADIO(tm_get_hw_time() + nrfra_timings_get_RX_rampdown_time());
  } else if ( radio_state == RAD_DISABLED ) {
    //It seems the radio will also signal a DISABLED event even if it was already disabled
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
  }
}

void nrf_radio_tasks_RSSISTART () {
  rssi_sampling_on = true;
}

void nrf_radio_tasks_RSSISTOP(){
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
/**
 * This is a fake task meant to start a HW timer for the TX->RX or RX->TX TIFS
 */
void nrf_radio_fake_task_TRXEN_TIFS(){
  if ( TIFS_state == TIFS_WAITING_FOR_DISABLE ) {
    TIFS_state = TIFS_TRIGGERING_TRX_EN;
    nrfra_set_Timer_RADIO(Timer_TIFS);
    if ( Timer_RADIO < tm_get_hw_time() ){
      bs_trace_warning_line_time("NRF_RADIO: TIFS Ups: The Ramp down from Rx/Tx into a Tx/Rx takes more than the programmed TIFS time\n");
    }
  }
}

/**
 * If the HW automatic TIFS switch is enabled, prepare the internals for an automatic switch
 * (when a fake_task_TRXEN_TIFS is automatically triggered after a disable due to a shortcut)
 * otherwise do nothing
 *
 * Input Tx_Not_Rx: Are we finishing a Tx (true) or an Rx (false)
 */
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

/**
 * The main radio timer (Timer_RADIO) has just triggered,
 * continue whatever activity we are on
 * (typically do something at the end/start of a state, set the new state
 * and schedule further the next state change)
 */
void nrf_radio_timer_triggered(){
  if ( radio_state == RAD_TXRU ){
    radio_state = RAD_TXIDLE;
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
    nrfra_set_Timer_RADIO(TIME_NEVER);
    nrf_radio_signal_READY();
    nrf_radio_signal_TXREADY();
  } else if ( radio_state == RAD_RXRU ){
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nrfra_set_Timer_RADIO(TIME_NEVER);
    nrf_radio_signal_READY();
    nrf_radio_signal_RXREADY();
  } else if ( radio_state == RAD_TX ){
    if ( radio_sub_state == TX_WAIT_FOR_ADDRESS_END ){
      radio_sub_state = TX_WAIT_FOR_PAYLOAD_END;
      nrfra_set_Timer_RADIO(tx_status.PAYLOAD_end_time);
      nrf_radio_signal_ADDRESS();
    } else if ( radio_sub_state == TX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = TX_WAIT_FOR_CRC_END;
      nrfra_set_Timer_RADIO(tx_status.CRC_end_time);
      nrf_radio_signal_PAYLOAD();
    } else if ( radio_sub_state == TX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RAD_TXIDLE;
      NRF_RADIO_regs.STATE = RAD_TXIDLE;
      nrfra_set_Timer_RADIO(TIME_NEVER);
      nrf_radio_stop_bit_counter();
      nrf_radio_signal_END();
      nrf_radio_signal_PHYEND(); //See note on EVENTS_PHYEND
      maybe_prepare_TIFS(true);
    }  else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == RAD_RX ){
    if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ) {
      nrfra_set_Timer_RADIO(TIME_NEVER);
      nrf_hw_find_next_timer_to_trigger();
      nrf_radio_signal_SYNC(); //See note on EVENTS_SYNC
      nrf_radio_signal_ADDRESS();
      Rx_Addr_received();
      radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
      nrfra_set_Timer_RADIO(rx_status.PAYLOAD_End_Time);
    } else if ( radio_sub_state == RX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = RX_WAIT_FOR_CRC_END;
      nrfra_set_Timer_RADIO(rx_status.CRC_End_Time);
      nrf_radio_signal_PAYLOAD();
    } else if ( radio_sub_state == RX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RAD_RXIDLE;
      NRF_RADIO_regs.STATE = RAD_RXIDLE;
      nrfra_set_Timer_RADIO(TIME_NEVER);
      if ( rx_status.CRC_OK ) {
        nrf_radio_signal_CRCOK();
      } else {
        nrf_radio_signal_CRCERROR();
      }
      nrf_radio_stop_bit_counter();
      nrf_radio_signal_PHYEND(); //See note on EVENTS_PHYEND
      nrf_radio_signal_END();
      maybe_prepare_TIFS(false);
    } else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == RAD_TXDISABLE ){
    radio_state = RAD_DISABLED;
    NRF_RADIO_regs.STATE = RAD_DISABLED;
    nrfra_set_Timer_RADIO(TIME_NEVER);
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
  } else if ( radio_state == RAD_RXDISABLE ){
    radio_state = RAD_DISABLED;
    NRF_RADIO_regs.STATE = RAD_DISABLED;
    nrfra_set_Timer_RADIO(TIME_NEVER);
    nrf_radio_stop_bit_counter();
    nrf_radio_signal_DISABLED();
  } else {
    if ( ( radio_state == RAD_DISABLED ) && ( TIFS_state == TIFS_TRIGGERING_TRX_EN ) ) {
      if ( Timer_RADIO != Timer_TIFS ){
        bs_trace_warning_line_time("NRF_RADIO: TIFS Ups 3\n");
      }
      TIFS_state = TIFS_DISABLE;
      nrfra_set_Timer_RADIO(TIME_NEVER);
      from_hw_tifs = true;
      if ( TIFS_ToTxNotRx ) {
        nrf_radio_tasks_TXEN();
      } else {
        nrf_radio_tasks_RXEN();
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
 * => we can now respond to the Phy with our abort decision
 */
void nrf_radio_timer_abort_reeval_triggered(){
  nrfra_set_Timer_abort_reeval(TIME_NEVER);

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
 * Handle all possible responses to a Tx request from the Phy
 */
static void handle_Tx_response(int ret){
  if (ret == -1){
    bs_trace_raw_manual_time(3,tm_get_abs_time(),"The phy disconnected us during a Tx\n");
    hwll_disconnect_phy_and_exit();
  } else if ( ret == P2G4_MSG_TX_END  ) {
    bs_time_t end_time = hwll_dev_time_from_phy(tx_status.tx_resp.end_time);
    tm_update_last_phy_sync_time( end_time );
    //The main machine was already pre-programmed at the Tx Start, no need to do anything else now
  } else if ( ret == P2G4_MSG_ABORTREEVAL ) {
    tm_update_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = Tx_Abort_reeval;
    nrfra_set_Timer_abort_reeval(next_recheck_time);
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
 * so we answer to the Phy with our decision
 */
static void Tx_abort_eval_respond(){
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &tx_status.tx_req.abort;

  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_tx_abort_nc_b(abort);

  handle_Tx_response(ret);
}

/*
 * Actually start the Tx in this microsecond (+ the Tx chain delay in the Phy)
 */
static void start_Tx(){

  radio_state = RAD_TX;
  NRF_RADIO_regs.STATE = RAD_TX;

  nrfra_check_packet_conf();

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_len;
  uint8_t address_len;
  uint8_t header_len;
  uint payload_len;
  //Note: I assume in Tx the length is always < PCNF1.MAXLEN and that STATLEN is always 0 (otherwise add a check)
  uint8_t crc_len;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    preamble_len = 1; //1 byte
    address_len = 4;
    header_len  = 2;
    crc_len = 3;
    bits_per_us = 1;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    preamble_len = 2; //2 bytes
    address_len = 4;
    header_len  = 2;
    crc_len = 3;
    bits_per_us = 2;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    preamble_len = 4;
    address_len = 1;
    header_len  = 1;
    crc_len = 2;
    bits_per_us = 0.25;
  }

  payload_len = nrfra_tx_copy_payload(tx_buf);

  //TODO: CRC for 15.4, or in general whatever CRC
  uint32_t crc_init = NRF_RADIO_regs.CRCINIT & RADIO_CRCINIT_CRCINIT_Msk;
  append_crc_ble(tx_buf, 2/*header*/ + payload_len, crc_init); //This size needs to be changed for 15.4

  bs_time_t packet_duration; //From preamble to CRC
  packet_duration  = preamble_len*8 + address_len*8;
  packet_duration += header_len*8 + payload_len*8 + crc_len*8;
  packet_duration /= bits_per_us;
  uint packet_size = header_len + payload_len + crc_len;

  nrfra_prep_tx_request(&tx_status.tx_req, packet_size, packet_duration);

  update_abort_struct(&tx_status.tx_req.abort, &next_recheck_time);

  //Request the Tx from the Phy:
  int ret = p2G4_dev_req_txv2_nc_b(&tx_status.tx_req, tx_buf,  &tx_status.tx_resp);
  handle_Tx_response(ret);

  tx_status.ADDRESS_end_time = tm_get_hw_time() + (bs_time_t)((preamble_len*8 + address_len*8)/bits_per_us);
  tx_status.PAYLOAD_end_time = tx_status.ADDRESS_end_time + (bs_time_t)(8*(header_len + payload_len)/bits_per_us);
  tx_status.CRC_end_time = tx_status.PAYLOAD_end_time + (bs_time_t)(crc_len*8/bits_per_us);

  radio_sub_state = TX_WAIT_FOR_ADDRESS_END;
  nrfra_set_Timer_RADIO(tx_status.ADDRESS_end_time);
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
    nrfra_set_Timer_abort_reeval( BS_MAX(next_recheck_time,tm_get_abs_time()) );

  } else if ( ( ret == P2G4_MSG_RXV2_ADDRESSFOUND ) && ( radio_state == RAD_RX /*if we havent aborted*/ ) ) {

    bs_time_t address_time = hwll_dev_time_from_phy(rx_status.rx_resp.rx_time_stamp); //this is the end of the sync word in air time
    tm_update_last_phy_sync_time(address_time);

    rx_status.ADDRESS_End_Time = address_time + nrfra_timings_get_Rx_chain_delay();

    uint length = rx_buf[1];
    uint max_length = (NRF_RADIO_regs.PCNF1 & NFCT_MAXLEN_MAXLEN_Msk) >> NFCT_MAXLEN_MAXLEN_Pos;

    if (length > max_length){
      bs_trace_error_time_line("NRF_RADIO: received a packet longer than the configured max lenght (%i>%i), this is not yet handled in this models. I stop before it gets confusing\n", length, max_length);
      length  = max_length;
      //TODO: check packet length. If too long the packet should be truncated and not accepted from the phy, [we already have it in the buffer and we will have a CRC error anyhow. And we cannot let the phy run for longer than we will]
    } else {
      //NRF_RADIO_regs.PDUSTAT = 0; //TODO, set if greater
    }

    rx_status.packet_rejected = false;

    rx_status.PAYLOAD_End_Time = nrfra_timings_get_Rx_chain_delay() +
        hwll_dev_time_from_phy(rx_status.rx_resp.rx_time_stamp
            + (bs_time_t)((2+length)*8/bits_per_us));

    rx_status.CRC_End_Time = nrfra_timings_get_Rx_chain_delay() +
        hwll_dev_time_from_phy(rx_status.rx_resp.rx_time_stamp
            + (bs_time_t)((2+length)*8/bits_per_us)
            + rx_status.CRC_duration); //Provisional value

    if (rx_status.rx_resp.packet_size >= 5) { /*At least the header and CRC, otherwise better to not try to copy it*/
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0] = rx_buf[0];
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1] = rx_buf[1];
      /* We cheat a bit and copy the whole packet already (The AAR block will look in Adv packets after 64 bits)*/
      memcpy(&((uint8_t*)NRF_RADIO_regs.PACKETPTR)[2 + rx_status.S1Offset],
          &rx_buf[2] , length);
    }

    radio_sub_state = RX_WAIT_FOR_ADDRESS_END;
    nrfra_set_Timer_RADIO(rx_status.ADDRESS_End_Time);
  } else if ( ( ret == P2G4_MSG_RXV2_END ) && ( radio_state == RAD_RX /*if we havent aborted*/ ) ) {

    bs_time_t end_time = hwll_dev_time_from_phy(rx_status.rx_resp.end_time);
    tm_update_last_phy_sync_time(end_time);

    if (rx_status.rx_resp.status != P2G4_RXSTATUS_HEADER_ERROR) {
        rx_status.CRC_End_Time = end_time + nrfra_timings_get_Rx_chain_delay();
    } //Otherwise we do not really now how the Nordic RADIO behaves depending on
    //where the biterrors are and so forth. So let's always behave like if the
    //packet lenght was received correctly, and just report a CRC error at the
    //end of the CRC

    if ( ( rx_status.rx_resp.status == P2G4_RXSTATUS_OK ) &&
        ( rx_status.rx_resp.packet_size > 5 ) ) {
      memcpy((void*)&NRF_RADIO_regs.RXCRC, &rx_buf[2 + rx_buf[1]], 3);
    }

    if ( rx_status.rx_resp.status == P2G4_RXSTATUS_OK ){
      rx_status.CRC_OK = 1;
      NRF_RADIO_regs.CRCSTATUS = 1;
    }

    nrf_ccm_radio_received_packet(!rx_status.CRC_OK);
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the phy with our decision
 */
static void Rx_abort_eval_respond(){
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &rx_status.rx_req.abort;
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

  nrfra_check_packet_conf();

  radio_state = RAD_RX;
  NRF_RADIO_regs.STATE = RAD_RX;
  NRF_RADIO_regs.CRCSTATUS = 0;

  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    rx_status.S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  } else {
    rx_status.S1Offset = 0;
  }

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    bits_per_us = 1;
    rx_status.CRC_duration = 3*8/bits_per_us;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    bits_per_us = 2;
    rx_status.CRC_duration = 3*8/bits_per_us;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    bits_per_us = 0.25;
    rx_status.CRC_duration = 2*8/bits_per_us;
  }

  rx_status.CRC_OK = false;
  rx_status.rx_resp.status = P2G4_RXSTATUS_NOSYNC;

  nrfra_prep_rx_request(&rx_status.rx_req, rx_addresses);

  update_abort_struct(&rx_status.rx_req.abort, &next_recheck_time);

  //attempt to receive
  int ret = p2G4_dev_req_rxv2_nc_b(&rx_status.rx_req,
      rx_addresses,
      &rx_status.rx_resp,
      &rx_pkt_buffer_ptr,
      _NRF_MAX_PACKET_SIZE);

  radio_sub_state = SUB_STATE_INVALID;
  nrfra_set_Timer_RADIO(TIME_NEVER);

  handle_Rx_response(ret);
}

/**
 * This function is called at the time when the Packet address would have been
 * completely received
 * (at the time of the end of the last bit of the packet address)
 * To continue processing the reception (the Phy was left waiting for a response)
 *
 * Note that libPhyCom has already copied the whole packet into the input buffer
 */
static void Rx_Addr_received(){

  bool accept_packet = !rx_status.packet_rejected;

  if ( rssi_sampling_on ){
    NRF_RADIO_regs.RSSISAMPLE = nrfra_RSSI_value_to_modem_format(p2G4_RSSI_value_to_dBm(rx_status.rx_resp.rssi.RSSI));
    nrf_radio_signal_RSSIEND();
  }

  NRF_RADIO_regs.RXMATCH = 0; //The only we support so far

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
