/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * RADIO - 2.4 GHz Radio
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/radio.html?cp=5_1_0_5_17
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/radio.html?cp=5_1_0_5_17
 *
 * Note: as of now, only 1&2Mbps BLE & 15.4 packet formats are supported, there is quite many notes around in the code
 * where changes would be required to support other formats. PCNF1.STATLEN is always assumed 0
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
 * Note9: Double buffering of registers is not implemented. Changing the register during the packet Tx/Rx will cause trouble
 *        It should be (at least): PACKETPTR @ START
 *                                 MODE @ TXEN | RXEN
 *                                 CRC config @ START
 *
 * Note10: Regarding MAXLEN:
 *           if CRCINC==1, the CRC LEN is deducted from the length field, before MAXLEN is checked.
 *           This seems to be also the real HW behavior
 *         Bit errors in the length field reception, or even bit errors in the CI field reception (for Coded phy)
 *         are not accounted for when handling the length field in this model. The model will always
 *         act on the transmitted length field.
 *
 * Note11: Only the BLE & 15.4 CRC polynomials are supported
 *         During reception we assume that CRCPOLY and CRCINIT are correct on both sides, and just rely on the phy bit error reporting to save processing time
 *         On transmission we generate the correct CRC for correctness of the channel dump traces (and Ellisys traces)
 * Note11b:The CRC configuration is directly deduced from the modulation, only BLE and 154 CRCs are supported so far
 *
 * Note12: * CCA or ED procedures cannot be performed while the RADIO is performing an actual packet reception (they are exclusive)
 *         * In CCA Mode2 & 3, this model (due to the Phy) does not search for a SFD, or for a correlation peak
 *           instead it searches for a compatible modulation of sufficient power (which is in line with what the 802.15.4
 *           standard specifies)
 *
 * Note13: Nothing related to AoA/AoD features (CTE, DFE) is implemented
 *
 * Note14: Several 52833 radio state change events are not yet implemented
 *         (EVENTS_MHRMATCH & EVENTS_CTEPRESENT)
 *
 * Note16: No antenna switching
 *
 * Note17: Interrupts are modeled as pulses to the NVIC, not level interrupts as they are in reality
 *
 * Note18: EVENTS_SYNC:
 *         a) It is not generated at the exact correct time:
 *         In this model it is generated at the end of the address (or SFD)
 *         while according to the infocenter spec this should come at the end of the preamble,
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
 *
 * Note20: The LQI value is based on a single measurement at the end of the SFD.
 *         While the real HW samples it in 3 places during the payload, and the middle one selected.
 *
 * Note21: Timings:
 *          * Radio ramp down time for 2Mbps BLE is 2 microseconds too long.
 *          * Many timings are simplified, and some events which take slightly different amounts of time to occur
 *            are produced at the same time as others, or so. Check NRF_RADIO_timings.c for some more notes.
 *
 * Note21.b: CodedPhy timings:
 *          * For CodedPhy the spec lacks radio timings, so the values used are mostly rounded
 *            versions of the ones the Zephyr controller used
 *          * At this point (for simplicity) The Rx chain delay for S=2 and S=8 are modeled as being equal.
 *          * It is quite unclear if the CRCOK/ERROR and PAYLOAD events are delayed by the equivalent
 *            of the conv. decoder taking 2 extra input bits or not, and if the ADDRESS event has an
 *            equivalent delay or not.
 *            At this point the model does not add a different delay to these, so this events
 *            timings should be considered a rough guess. CRCOK/ERROR is generated at the same time
 *            and the END event based on the end of TERM2.
 *
 * Note22: EVENTS_FRAMESTART
 *          * It is generated for all modulation types, this seems to be how the HW behaves even if the spec
 *            seems to mildly imply it is only for 15.4
 *          * In Tx: The spec seems unclear about the FRAMESTART being generated or not (after SHR).
 *            Drawings imply it is, the text that it does not. The HW does. The model does generate it after SHR.
 *          * In Rx: In the model it is generated at the SHR/SFD end (not PHR), meaning, at the same time as the ADDRESS EVENT
 *            The spec seems to contradict itself here. But seems in real HW it is generated at the end of the PHR.
 *
 * Note23: Powering off/on is not properly modeled (it is mostly ignored)
 *
 * Note24: Only PCNF1.ENDIAN == Little is supported (What BT and 802.15.4 use)
 *
 * Note25: The RATEBOOST event in real HW seems to be generated: only for Rx, only if the FEC2 block has S=2,
 *         and roughly at the end of TERM1. The models generates it at that point and only in that case.
 *         (It's timing is probably a bit off compared to real HW)
 *
 *
 * Implementation Specification:
 *   A diagram of the main state machine can be found in docs/RADIO_states.svg
 *   That main state machine is driven by a timer (Timer_RADIO) which results in calls to nhw_radio_timer_triggered()
 *   and the tasks which cause transitions and/or the timer to be set to a new value.
 *
 *   Apart from this main state machine there is a small state machine for handling the automatic TIFS re-enabling.
 *   See TIFS_state, Timer_TIFS, nhw_RADIO_fake_task_TRXEN_TIFS, and maybe_prepare_TIFS()
 *   This TIFS machine piggybacks on the main machine and its timer.
 *
 *   And apart from this, there is an "abort" state machine, which is used to handle SW or another peripheral
 *   triggering a TASK which requires us to stop a transaction with the Phy midway.
 *   The idea here, is that when we start a transaction with the Phy (say a Tx), we do not know at the start if something
 *   will want to stop it midway. So we tell the Phy when we start, when we expect to end, but also, when we
 *   want the Phy to recheck with us if the transaction needs to be aborted midway.
 *   This recheck time is set to the time anything may decide to stop. Which for simplicity is whenever *anything* may run.
 *   That is, whenever any timer is scheduled. As this includes other peripherals which may trigger tasks thru the PPI,
 *   or SW doing so after an interrupt.
 *   If at any point, a TASK that stops a transaction comes while that transaction is ongoing, the abort state machine will flag it,
 *   and the next time we need to respond to the Phy we will tell that we are stopping.
 *
 *   Apart from these, there is the interaction with the Phy (check ext_2G4_libPhyComv1/docs):
 *   There is 3 different procedures for this (Tx, Rx & CCA) which fundamentally work in the same way.
 *   At start_Rx/Tx/CCA_ED() (which is called at the micros when the actual Tx/Rx/CCA/ED measurement starts),
 *   the Phy is told we want to start, and immediately we block until we get a response from the Phy.
 *   (1) Here the response may be that:
 *     * The Phy finished the procedure, in which case we just pre-program the main state machine timer,
 *       and set registers and other state accordingly. (as we are done interacting with the Phy for this operation)
 *       OR
 *     * The Phy asks us to reevaluate if we want to abort. In this case, we hold responding to the Phy
 *       and instead set the Timer_RADIO_abort_reeval to the time in which we need to respond to the Phy
 *       and let time pass until that microsecond is ended.
 *       At that point in time:
 *          * If SW (or whatever else may trigger a TASK) has caused the procedure to end, we tell the Phy
 *            we are aborting right now
 *          * If nothing stopped it yet, we respond with a new abort reevaluation time in the future to the Phy,
 *            and continue from (1).
 *       The idea is that Timer_RADIO_abort_reeval is a separate timer that runs in parallel to the main Timer_RADIO and any other
 *       HW event timer. And as Timer_RADIO_abort_reeval is the last timer scheduled by the HW_model_top in a given time, we will know if
 *       anything else has affected the RADIO state in a way that requires us to stop the interaction with the Phy or not.
 *   When we receive the CCA end from the Phy, we will also check the result, set registers accordingly, and pre-set the cca_status so
 *   as to raise or not the CCABUSY/IDLE signals.
 *   For an Rx it is marginally more complex, as we not only receive the end (either no sync, or crcok/failed), but also an intermediate
 *   notification when the address has been received. At this point (address end) we pre-check some of the packet content (address for BLE adv,
 *   length, etc) and already set some status registers and make some decisions about if we should proceed with the packet or not.
 *
 *   The CCA and ED procedures are so similar that they are handled with the same CCA_ED state in the main state machine,
 *   most of the same CCA_ED code, and the same CCA procedure to the Phy.
 *
 *   For BLE Coded Phy, transmissions and receptions are actually handled as 2 piggybacking ones:
 *   One for the FEC1 block and another for the FEC2.
 *   * In transmission, during start_Tx() both FEC1 and FEC2 tx_req and tx_req_fec1 structures are
 *   filled in, and the transmission for the FEC1 is started. When the FEC2 transmission start time
 *   has been reached (the micros after the FEC1 has ended), the main state state machine will call
 *   start_Tx_FEC2() which will update the FEC2 tx_req, and start it. From there it will continue
 *   like a normal transmission until the CRC end.
 *   * In receptions similarly start_Rx() will prefill both rx_req structures. But: the 2nd rx_req
 *   will be updated after the FEC1 CI is received at start_Rx_FEC2(), and when the FEC1 ends.
 *   Unlike a Tx, a reception can take several code paths depending on the possibility of sync'ing,
 *   having an erroneous FEC1 or FEC2 packet. See docs/Rx_Phy_paths.svg for more info.
 *
 *   The main state machine has a few conditions to transition differently for Coded Phy and normal
 *   packets.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "bs_pc_2G4.h"
#include "bs_pc_2G4_utils.h"
#include "bs_rand.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_RADIO.h"
#include "NHW_RADIO_signals.h"
#include "NHW_RADIO_utils.h"
#include "NHW_RADIO_timings.h"
#include "NHW_RADIO_bitcounter.h"
#include "NHW_RADIO_priv.h"
#include "nsi_hw_scheduler.h"
#include "NHW_AES_CCM.h"
#include "irq_ctrl.h"
#include "NRF_HWLowL.h"
#include "crc.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "weak_stubs.h"

#if NHW_RADIO_TOTAL_INST > 1
#error "This model only supports 1 instance so far"
#endif

NRF_RADIO_Type NRF_RADIO_regs;

static bs_time_t Timer_RADIO = TIME_NEVER; //main radio timer
static bs_time_t Timer_RADIO_abort_reeval = TIME_NEVER; //Abort reevaluation response timer, this timer must have the lowest priority of all events (which may cause an abort)

static TIFS_state_t TIFS_state = TIFS_DISABLE;
static bool TIFS_ToTxNotRx = false; //Are we in a TIFS automatically starting a Tx from a Rx (true), or Rx from Tx (false)
static bs_time_t Timer_TIFS = TIME_NEVER;
static bool from_hw_tifs = false; /* Unfortunate hack due to the SW racing the HW to clear SHORTS*/

static RADIO_Rx_status_t rx_status;
static RADIO_Tx_status_t tx_status;
static RADIO_CCA_status_t cca_status;

static double bits_per_us; //Bits per us for the ongoing Tx or Rx

static bs_time_t next_recheck_time; // when we asked the phy to recheck (in our own time) next time
static abort_state_t abort_fsm_state = No_pending_abort_reeval; //This variable shall be set to Tx/Rx_Abort_reeval when the phy is waiting for an abort response (and in no other circumstance)
static int aborting_set = 0; //If set, we will abort the current Tx/Rx/CCA at the next abort reevaluation

static nrfra_state_t radio_state;
static nrfra_sub_state_t radio_sub_state;

static uint8_t tx_buf[_NRF_MAX_PACKET_SIZE]; //starting from the header, and including CRC
static uint8_t rx_buf[_NRF_MAX_PACKET_SIZE]; // "
static uint8_t *rx_pkt_buffer_ptr = (uint8_t*)&rx_buf;

static bool radio_on = false;

static bool rssi_sampling_on = false;

static double cheat_rx_power_offset;

static void start_Tx(void);
static void start_Tx_FEC2(void);
static void start_Rx(void);
static void start_Rx_FEC2(void);
static void start_CCA_ED(bool CCA_not_ED);
static void Rx_Addr_received(void);
static void Tx_abort_eval_respond(void);
static void Rx_abort_eval_respond(void);
static void CCA_abort_eval_respond(void);
static void nhw_radio_device_address_match(uint8_t rx_buf[]);

static void radio_reset(void) {
  memset(&NRF_RADIO_regs, 0, sizeof(NRF_RADIO_regs));
  radio_state = RAD_DISABLED;
  radio_sub_state = SUB_STATE_INVALID;
  Timer_RADIO = TIME_NEVER;
  rssi_sampling_on = false;

  TIFS_state = TIFS_DISABLE;
  TIFS_ToTxNotRx = false;
  Timer_TIFS = TIME_NEVER;

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

  nhwra_signalif_reset();
}

static void nhw_radio_init(void) {
  nrfra_timings_init();
  radio_reset();
  radio_on = false;
  bits_per_us = 1;
}

NSI_TASK(nhw_radio_init, HW_INIT, 100);

double nhw_radio_get_bpus(void) {
  return bits_per_us;
}

static inline void nhwra_set_Timer_RADIO(bs_time_t t){
  Timer_RADIO = t;
  nsi_hws_find_next_event();
}

static inline void nhwra_set_Timer_abort_reeval(bs_time_t t){
  Timer_RADIO_abort_reeval = t;
  nsi_hws_find_next_event();
}

void nhw_RADIO_TASK_TXEN(void) {
  if ( ( radio_state != RAD_DISABLED )
      && ( radio_state != RAD_TXIDLE )
      && ( radio_state != RAD_RXIDLE ) ){
    bs_trace_warning_line_time(
        "NRF_RADIO: TXEN received when the radio was not DISABLED or TX/RXIDLE but in state %i. It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  radio_state = RAD_TXRU;
  NRF_RADIO_regs.STATE = RAD_TXRU;

  nhwra_set_Timer_RADIO(nsi_hws_get_time() + nhwra_timings_get_rampup_time(1, from_hw_tifs));
}

void nhw_RADIO_TASK_RXEN(void) {
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
  nhwra_set_Timer_RADIO(nsi_hws_get_time() + nhwra_timings_get_rampup_time(0, from_hw_tifs));
}

static void abort_if_needed(void) {
  if ( ( abort_fsm_state == Tx_Abort_reeval )
      || ( abort_fsm_state == Rx_Abort_reeval )
      || ( abort_fsm_state == CCA_Abort_reeval ) ){
    //If the phy is waiting for a response from us, we need to tell it, that we are aborting whatever it was doing
    aborting_set = 1;
  }
  /* Note: In Rx, we may be
   *   waiting to respond to the Phy to an abort reevaluation request abort_fsm_state == Rx_Abort_reeval
   *   or waiting to reach the address end time to respond to the Phy if we accepted the packet or not
   * but not both
   */
  if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ){
    //we answer immediately to the phy rejecting the packet
    p2G4_dev_rxv2_cont_after_addr_nc_b(false, NULL);
    radio_sub_state = SUB_STATE_INVALID;
  }
}

void nhw_RADIO_TASK_START(void) {
  if ( radio_state == RAD_TXIDLE ) {
    bs_time_t Tx_start_time = nsi_hws_get_time() + nhwra_timings_get_TX_chain_delay();
    radio_state = RAD_TXSTARTING;
    NRF_RADIO_regs.STATE = RAD_TX;
    nhwra_set_Timer_RADIO(Tx_start_time);
  } else if ( radio_state == RAD_RXIDLE ) {
    start_Rx();
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_START received while the radio was not in either TXIDLE or RXIDLE but in state %i. It will be ignored => expect problems\n",
        radio_state);
  }
}

void nhw_RADIO_TASK_CCASTART(void) {
  if ((radio_state != RAD_RXIDLE)){
    bs_trace_warning_line_time(
        "NRF_RADIO: CCASTART received when the radio was not RXIDLE but in state %i. "
        "It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  start_CCA_ED(1);
}

void nhw_RADIO_TASK_CCASTOP(void) {
  if (( radio_state == RAD_CCA_ED ) && ( cca_status.CCA_notED )) {
    abort_if_needed();
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_RADIO_signal_EVENTS_CCASTOPPED(0);
  } else {
    bs_trace_info_line_time(3,
        "NRF_RADIO: TASK_CCASTOP received while the radio was not on a CCA procedure (was %i, %i). "
        "It will be ignored\n",
        radio_state, cca_status.CCA_notED);
  }
}

void nhw_RADIO_TASK_EDSTART(void) {
  if ((radio_state != RAD_RXIDLE)){
    bs_trace_warning_line_time(
        "NRF_RADIO: EDSTART received when the radio was not RXIDLE but in state %i. "
        "It will be ignored. Expect problems\n",
        radio_state);
    return;
  }
  start_CCA_ED(0);
}

void nhw_RADIO_TASK_EDSTOP(void) {
  if (( radio_state == RAD_CCA_ED ) && ( cca_status.CCA_notED == 0)) {
    abort_if_needed();
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_RADIO_signal_EVENTS_EDSTOPPED(0);
  } else {
    bs_trace_info_line_time(3,
        "NRF_RADIO: TASK_EDSTOP received while the radio was not on a ED procedure (was %i, %i). "
        "It will be ignored\n",
        radio_state, cca_status.CCA_notED);
  }
}

void nhw_RADIO_TASK_STOP(void) {
  nhw_radio_stop_bit_counter();

  if ((radio_state == RAD_TX) || (radio_state == RAD_TXSTARTING)) {
    if (radio_state == RAD_TX) {
      abort_if_needed();
    }
    radio_state = RAD_TXIDLE;
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
  } else if ( radio_state == RAD_RX ){
    abort_if_needed();
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
  } else if ( radio_state == RAD_CCA_ED ){
    //The documentation is not clear about what happens if we get a STOP during a CCA or ED procedure,
    //but it seems for CCA it can cause a bit of a mess depending on CCA mode.
    //the behavior here is that we stop just like if it was an active Rx, and do *not* trigger a CCASTOPPED or EDSTOPPED event
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_STOP received while the radio was performing a CCA or ED procedure. "
        "In this models we stop the procedure, but this can cause a mess in real HW\n");
    abort_if_needed();
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
  } else {
    bs_trace_warning_line_time(
        "NRF_RADIO: TASK_STOP received while the radio was not on either TX or RX but in state %i. "
        "It will be ignored\n",
        radio_state);
  }
}

void nhw_RADIO_TASK_DISABLE(void) {
  nhw_radio_stop_bit_counter();

  if ((radio_state == RAD_TX) || (radio_state == RAD_TXSTARTING)) {
    if (radio_state == RAD_TX) {
      abort_if_needed();
    }
    radio_state = RAD_TXIDLE; //Momentary (will be changed in the if below)
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
  } else if ( radio_state == RAD_RX ){
    abort_if_needed();
    radio_state = RAD_RXIDLE; //Momentary (will be changed in the if below)
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
  } else if ( radio_state == RAD_CCA_ED ){
    //The documentation is not clear about what happens if we get a disable during a CCA  or ED procedure,
    //the assumption here is that we stop just like if it was an active Rx, but do not trigger a CCASTOPPED or EDSTOPPED event
    abort_if_needed();
    radio_state = RAD_RXIDLE; //Momentary (will be changed in the if below)
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
  }

  if (TIFS_state != TIFS_DISABLE) {
    TIFS_state = TIFS_DISABLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    Timer_TIFS = TIME_NEVER;
  }

  if ( ( radio_state == RAD_TXRU ) || ( radio_state == RAD_TXIDLE ) ) {
    radio_state = RAD_TXDISABLE;
    NRF_RADIO_regs.STATE = RAD_TXDISABLE;
    TIFS_state = TIFS_DISABLE;
    nhwra_set_Timer_RADIO(nsi_hws_get_time() + nhwra_timings_get_TX_rampdown_time());
  } else if ( ( radio_state == RAD_RXRU ) || ( radio_state == RAD_RXIDLE ) ) {
    radio_state = RAD_RXDISABLE;
    NRF_RADIO_regs.STATE = RAD_RXDISABLE;
    TIFS_state = TIFS_DISABLE;
    nhwra_set_Timer_RADIO(nsi_hws_get_time() + nhwra_timings_get_RX_rampdown_time());
  } else if ( radio_state == RAD_DISABLED ) {
    //It seems the radio will also signal a DISABLED event even if it was already disabled
    nhw_radio_stop_bit_counter();
    nhw_RADIO_signal_EVENTS_DISABLED(0);
  }
}

void nhw_RADIO_TASK_RSSISTART(void) {
  rssi_sampling_on = true;
}

void nhw_RADIO_TASK_RSSISTOP(void) {
  rssi_sampling_on = false;
}

void nhw_RADIO_regw_sideeffects_POWER(void) {
  if ( NRF_RADIO_regs.POWER == 0 ){
    radio_on = false;
  } else {
    if ( radio_on == false ){
      radio_on = true;
      abort_if_needed();
      radio_reset();
      nsi_hws_find_next_event();
    }
  }
}

/**
 * This is a fake task meant to start a HW timer for the TX->RX or RX->TX TIFS
 */
void nhw_RADIO_fake_task_TRXEN_TIFS(void) {
  if ( TIFS_state == TIFS_WAITING_FOR_DISABLE ) {
    TIFS_state = TIFS_TRIGGERING_TRX_EN;
    nhwra_set_Timer_RADIO(Timer_TIFS);
    if ( Timer_RADIO < nsi_hws_get_time() ){
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
  if ( !nhwra_is_HW_TIFS_enabled() ) {
    TIFS_state = TIFS_DISABLE;
    return;
  }
  if ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ){
    TIFS_ToTxNotRx = true;
  } else {
    TIFS_ToTxNotRx = false;
  }

  if ( Tx_Not_Rx ){ //End of Tx
    delta = NRF_RADIO_regs.TIFS + nhwra_timings_get_TX_chain_delay() - nhwra_timings_get_rampup_time(0, 1) - 3; /*open slightly earlier to have jitter margin*/
  } else { //End of Rx
    delta = NRF_RADIO_regs.TIFS - nhwra_timings_get_Rx_chain_delay() - nhwra_timings_get_TX_chain_delay() - nhwra_timings_get_rampup_time(1, 1) + 1;
  }
  Timer_TIFS = nsi_hws_get_time() + delta;
  TIFS_state = TIFS_WAITING_FOR_DISABLE; /* In Timer_TIFS we will trigger a TxEN or RxEN */
}

static void maybe_signal_event_RATEBOOST(void) {
  if (rx_status.CI == 1) {
    nhw_RADIO_signal_EVENTS_RATEBOOST(0);
  }
}

/**
 * The main radio timer (Timer_RADIO) has just triggered,
 * continue whatever activity we are on
 * (typically do something at the end/start of a state, set the new state
 * and schedule further the next state change)
 */
static void nhw_radio_timer_triggered(void) {
  if ( radio_state == RAD_TXRU ){
    radio_state = RAD_TXIDLE;
    NRF_RADIO_regs.STATE = RAD_TXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_RADIO_signal_EVENTS_READY(0);
    nhw_RADIO_signal_EVENTS_TXREADY(0);
  } else if ( radio_state == RAD_RXRU ){
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_RADIO_signal_EVENTS_READY(0);
    nhw_RADIO_signal_EVENTS_RXREADY(0);
  } else if ( radio_state == RAD_TXSTARTING ){
    nhwra_set_Timer_RADIO(TIME_NEVER);
    start_Tx();
  } else if ( radio_state == RAD_TX ){
    if ( radio_sub_state == TX_WAIT_FOR_ADDRESS_END ){
      if (tx_status.codedphy) {
        radio_sub_state = TX_WAIT_FOR_FEC1_END;
        nhwra_set_Timer_RADIO(tx_status.FEC2_start_time);
      } else {
        radio_sub_state = TX_WAIT_FOR_PAYLOAD_END;
        nhwra_set_Timer_RADIO(tx_status.PAYLOAD_end_time);
      }
      nhw_RADIO_signal_EVENTS_ADDRESS(0);
      nhw_RADIO_signal_EVENTS_FRAMESTART(0); //See note on FRAMESTART
    } else if ( radio_sub_state == TX_WAIT_FOR_FEC1_END ) {
      start_Tx_FEC2();
      radio_sub_state = TX_WAIT_FOR_PAYLOAD_END;
      nhwra_set_Timer_RADIO(tx_status.PAYLOAD_end_time);
    } else if ( radio_sub_state == TX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = TX_WAIT_FOR_CRC_END;
      nhwra_set_Timer_RADIO(tx_status.CRC_end_time);
      nhw_RADIO_signal_EVENTS_PAYLOAD(0);
    } else if ( radio_sub_state == TX_WAIT_FOR_CRC_END ) {
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RAD_TXIDLE;
      NRF_RADIO_regs.STATE = RAD_TXIDLE;
      nhwra_set_Timer_RADIO(TIME_NEVER);
      nhw_radio_stop_bit_counter();
      nhw_RADIO_signal_EVENTS_END(0);
      nhw_RADIO_signal_EVENTS_PHYEND(0); //See note on EVENTS_PHYEND
      maybe_prepare_TIFS(true);
    }  else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == RAD_RX ){
    if ( radio_sub_state == RX_WAIT_FOR_ADDRESS_END ) {
      nhw_RADIO_signal_EVENTS_SYNC(0); //See note on EVENTS_SYNC
      nhw_RADIO_signal_EVENTS_ADDRESS(0);
      nhw_RADIO_signal_EVENTS_FRAMESTART(0); //See note on FRAMESTART
      nhwra_set_Timer_RADIO(TIME_NEVER); //Provisionally clear the RADIO timer for the Rx cont.
      Rx_Addr_received();
      if (rx_status.codedphy) {
        radio_sub_state = RX_WAIT_FOR_FEC1_END;
        // The timer will be set once we get the Phy FEC1 end response
      } else {
        radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
        nhwra_set_Timer_RADIO(rx_status.PAYLOAD_End_Time);
      }
    } else if ( radio_sub_state == RX_WAIT_FOR_FEC1_END ) {
      maybe_signal_event_RATEBOOST();
      /* The next state transition will be programmed when we get the Phy response for the FEC2 */
      nhwra_set_Timer_RADIO(TIME_NEVER);
      start_Rx_FEC2();
    } else if ( radio_sub_state == RX_WAIT_FOR_PAYLOAD_END ) {
      radio_sub_state = RX_WAIT_FOR_CRC_END;
      nhwra_set_Timer_RADIO(rx_status.CRC_End_Time);
      nhw_RADIO_signal_EVENTS_PAYLOAD(0);
    } else if ( radio_sub_state == RX_WAIT_FOR_CRC_END ) {
      nhw_ccm_radio_received_packet(!rx_status.CRC_OK);
      radio_sub_state = SUB_STATE_INVALID;
      radio_state = RAD_RXIDLE;
      NRF_RADIO_regs.STATE = RAD_RXIDLE;
      nhwra_set_Timer_RADIO(TIME_NEVER);
      if ( rx_status.CRC_OK ) {
        nhw_RADIO_signal_EVENTS_CRCOK(0);
      } else {
        nhw_RADIO_signal_EVENTS_CRCERROR(0);
      }
      nhw_radio_stop_bit_counter();
      nhw_RADIO_signal_EVENTS_PHYEND(0); //See note on EVENTS_PHYEND
      nhw_RADIO_signal_EVENTS_END(0);
      maybe_prepare_TIFS(false);
    } else { //SUB_STATE_INVALID
      bs_trace_error_time_line("programming error\n");
    }
  } else if ( radio_state == RAD_CCA_ED ){
    radio_state = RAD_RXIDLE;
    NRF_RADIO_regs.STATE = RAD_RXIDLE;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    if (cca_status.CCA_notED) { //CCA procedure ended
      if (cca_status.is_busy) {
        nhw_RADIO_signal_EVENTS_CCABUSY(0);
      } else {
        nhw_RADIO_signal_EVENTS_CCAIDLE(0);
      }
    } else { //ED procedure ended
      nhw_RADIO_signal_EVENTS_EDEND(0);
    }
  } else if ( radio_state == RAD_TXDISABLE ){
    radio_state = RAD_DISABLED;
    NRF_RADIO_regs.STATE = RAD_DISABLED;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_radio_stop_bit_counter();
    nhw_RADIO_signal_EVENTS_DISABLED(0);
  } else if ( radio_state == RAD_RXDISABLE ){
    radio_state = RAD_DISABLED;
    NRF_RADIO_regs.STATE = RAD_DISABLED;
    nhwra_set_Timer_RADIO(TIME_NEVER);
    nhw_radio_stop_bit_counter();
    nhw_RADIO_signal_EVENTS_DISABLED(0);
  } else {
    if ( ( radio_state == RAD_DISABLED ) && ( TIFS_state == TIFS_TRIGGERING_TRX_EN ) ) {
      if ( Timer_RADIO != Timer_TIFS ){
        bs_trace_warning_line_time("NRF_RADIO: TIFS Ups 3\n");
      }
      TIFS_state = TIFS_DISABLE;
      nhwra_set_Timer_RADIO(TIME_NEVER);
      from_hw_tifs = true;
      if ( TIFS_ToTxNotRx ) {
        nhw_RADIO_TASK_TXEN();
      } else {
        nhw_RADIO_TASK_RXEN();
      }
      from_hw_tifs = false;
    } else {
      bs_trace_error_line_time(
          "NRF_RADIO: this should not have happened (radio_state =%i)\n",
          radio_state);
    }
  }
}

NSI_HW_EVENT(Timer_RADIO, nhw_radio_timer_triggered, 990 /*We want the radio to be one of the very last to avoid unnecessary abort re-evaluations to the Phy*/);

/**
 * The abort reevaluation timer has just triggered,
 * => we can now respond to the Phy with our abort decision
 */
static void nhw_radio_timer_abort_reeval_triggered(void) {
  nhwra_set_Timer_abort_reeval(TIME_NEVER);

  if ( abort_fsm_state == Tx_Abort_reeval ){
    abort_fsm_state = No_pending_abort_reeval;
    Tx_abort_eval_respond();
  } else if ( abort_fsm_state == Rx_Abort_reeval ) {
    abort_fsm_state = No_pending_abort_reeval;
    Rx_abort_eval_respond();
  } else if ( abort_fsm_state == CCA_Abort_reeval ) {
    abort_fsm_state = No_pending_abort_reeval;
    CCA_abort_eval_respond();
  } else {
    bs_trace_error_line("The abort timer was left running.. somebody forgot to cleanup..\n");
  }
}

NSI_HW_EVENT(Timer_RADIO_abort_reeval, nhw_radio_timer_abort_reeval_triggered, 999 /* Purposely the last (all other events must have been evaluated before) */);

/**
 * Handle all possible responses to a Tx request from the Phy
 */
static void handle_Tx_response(int ret){
  if (ret == -1){
    bs_trace_raw_manual_time(3, nsi_hws_get_time(),"The phy disconnected us during a Tx\n");
    hwll_disconnect_phy_and_exit();
  } else if (ret == P2G4_MSG_TX_END) {
    bs_time_t end_time = hwll_dev_time_from_phy(tx_status.tx_resp.end_time);
    phy_sync_ctrl_set_last_phy_sync_time(end_time);
    //The main machine was already pre-programmed at the Tx Start, no need to do anything else now
  } else if ( ret == P2G4_MSG_ABORTREEVAL ) {
    phy_sync_ctrl_set_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = Tx_Abort_reeval;
    nhwra_set_Timer_abort_reeval(next_recheck_time);
  }
}

/**
 * Set the Phy abort structure to the next time we will want to either abort or have a recheck
 * And store in next_recheck_time the next recheck time
 */
static void update_abort_struct(p2G4_abort_t *abort, bs_time_t *next_recheck_time){
  //We will want to recheck next time anything may decide to stop the radio, that can be SW or HW
  //The only logical way to do so is to set it to the next timer whatever it may be as many can trigger SW interrupts
  *next_recheck_time = nsi_hws_get_next_event_time();
  abort->recheck_time = hwll_phy_time_from_dev(*next_recheck_time);

  //We either have decided already we want to abort so we do it right now
  //or we have not decided yet
  if ( aborting_set == 1 ) {
    aborting_set = 0; //By returning nsi_hws_get_time(), we are aborting right now
    abort->abort_time = hwll_phy_time_from_dev(nsi_hws_get_time());
  } else {
    abort->abort_time = TIME_NEVER;
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the Phy with our decision
 */
static void Tx_abort_eval_respond(void) {
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &tx_status.tx_req.abort;

  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_tx_abort_nc_b(abort);

  handle_Tx_response(ret);
}

/*
 * Actually start the Tx in this microsecond (+ the Tx chain delay in the Phy)
 * (For coded phy, starts the FEC1 Tx itself, and prepares the FEC2 to be started later by start_Tx_FEC2() )
 */
static void start_Tx(void) {

  radio_state = RAD_TX;
  NRF_RADIO_regs.STATE = RAD_TX;

  nhwra_check_packet_conf();

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_len = 0;
  uint8_t address_len = 0;
  uint8_t header_len = 0;
  uint payload_len = 0;
  uint8_t crc_len = nhwra_get_crc_length();
  uint8_t CI = 0;
  uint8_t main_packet_coding_rate = 0;

  tx_status.codedphy = false;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    preamble_len = 1; //1 byte
    address_len = 4;
    header_len  = 2;
    bits_per_us = 1;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    preamble_len = 2; //2 bytes
    address_len = 4;
    header_len  = 2;
    bits_per_us = 2;
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
            || (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit)) {
    tx_status.codedphy = true;
    address_len = 4;
    header_len  = 2;
    if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit) {
      bits_per_us = 0.125;
      CI = 0; //0b00
      main_packet_coding_rate = 8;
    } else { /* RADIO_MODE_MODE_Ble_LR500Kbit */
      bits_per_us = 0.5;
      CI = 1; //0b01
      main_packet_coding_rate = 2;
    }
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    preamble_len = 4;
    address_len = 1;
    header_len  = 1;
    bits_per_us = 0.25;
  }

  payload_len = nhwra_tx_copy_payload(tx_buf);

  /* This code should be generalized to support any CRC configuration (CRCCNF, CRCINIT AND CRCPOLY)
   * When doing so, we should still calculate the ble and 154 crc's with their optimized table implementations
   * Here we just assume the CRC is configured as it should given the modulation */
  uint32_t crc_init = NRF_RADIO_regs.CRCINIT & RADIO_CRCINIT_CRCINIT_Msk;
  if (nhwra_is_ble_mode(NRF_RADIO_regs.MODE)) {
    append_crc_ble(tx_buf, header_len + payload_len, crc_init);
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    //15.4 does not CRC the length (header) field
    append_crc_154(&tx_buf[header_len], payload_len, crc_init);
  }

  uint main_packet_size; //Main "packet" size (the payload sent thru the phy)
  bs_time_t packet_duration = 0; //Main packet duration (from preamble to CRC except for codedPhy which is just the FEC2)

  if (!tx_status.codedphy) {
    packet_duration = preamble_len*8 + address_len*8;
  } else {
    packet_duration = 3; //TERM2
  }
  packet_duration += header_len*8 + payload_len*8 + crc_len*8;
  packet_duration /= bits_per_us;
  main_packet_size = header_len + payload_len + crc_len;

  bs_time_t payload_start_time;
  bs_time_t main_packet_start_time;

  if (tx_status.codedphy) {
    tx_status.ADDRESS_end_time = nsi_hws_get_time() + (bs_time_t)(80 + 256 - nhwra_timings_get_TX_chain_delay());
    payload_start_time = tx_status.ADDRESS_end_time + 16 + 24;/* CI = 16us; TERM1= 24us */

    bs_time_t fec1_duration = 80 + 256 + 16 + 24;

    nhwra_prep_tx_request(&tx_status.tx_req_fec1, 1, fec1_duration, hwll_phy_time_from_dev(nsi_hws_get_time()), 8);
    update_abort_struct(&tx_status.tx_req_fec1.abort, &next_recheck_time);
    main_packet_start_time = tx_status.tx_req_fec1.end_tx_time + 1;
    tx_status.FEC2_start_time = main_packet_start_time; /* in air */
  } else {
    tx_status.ADDRESS_end_time = nsi_hws_get_time() + (bs_time_t)((preamble_len*8 + address_len*8)/bits_per_us) - nhwra_timings_get_TX_chain_delay();
    payload_start_time = tx_status.ADDRESS_end_time;
    main_packet_start_time = hwll_phy_time_from_dev(nsi_hws_get_time());
  }
  tx_status.PAYLOAD_end_time = payload_start_time + (bs_time_t)(8*(header_len + payload_len)/bits_per_us);
  tx_status.CRC_end_time = tx_status.PAYLOAD_end_time + (bs_time_t)(crc_len*8/bits_per_us);

  nhwra_prep_tx_request(&tx_status.tx_req, main_packet_size, packet_duration,
                        main_packet_start_time, main_packet_coding_rate);
  update_abort_struct(&tx_status.tx_req.abort, &next_recheck_time);

  int ret;
  if (tx_status.codedphy) {
    //Request the FEC1 Tx from the Phy:
    ret = p2G4_dev_req_txv2_nc_b(&tx_status.tx_req_fec1, &CI, &tx_status.tx_resp);
  } else { /* not codedphy */
    //Request the Tx from the Phy:
    ret = p2G4_dev_req_txv2_nc_b(&tx_status.tx_req, tx_buf, &tx_status.tx_resp);
  }
  handle_Tx_response(ret);

  radio_sub_state = TX_WAIT_FOR_ADDRESS_END;
  nhwra_set_Timer_RADIO(tx_status.ADDRESS_end_time);
}

static void start_Tx_FEC2(void) {
  int ret;
  update_abort_struct(&tx_status.tx_req.abort, &next_recheck_time);
  tx_status.tx_req.phy_address = 0; /* An invalid address */
  ret = p2G4_dev_req_txv2_nc_b(&tx_status.tx_req, tx_buf, &tx_status.tx_resp);
  handle_Tx_response(ret);
}

static void Rx_handle_CI_reception(void) {
  rx_status.CI = rx_buf[0] & 0x3;

  if ((rx_status.rx_resp.packet_size < 1) || (rx_status.CI > 1)) {
    bs_trace_warning_time_line("%s: Received supposed BLE CodedPhy FEC1 without CI or corrupted CI (%i, %i)\n",
        __func__, rx_status.rx_resp.packet_size, rx_status.CI);
  }

  NRF_RADIO_regs.PDUSTAT |= (rx_status.CI << RADIO_PDUSTAT_CISTAT_Pos) & RADIO_PDUSTAT_CISTAT_Msk;

  if (rx_status.rx_resp.status != P2G4_RXSTATUS_OK) {
    /* Error during CI decoding, we don't know how many bits, and if it would have recovered.
     * So, we just do a 50% drop of having each CI bit corrupted or not */
    int error;
    error = bs_random_Bern(RAND_PROB_1/2);
    NRF_RADIO_regs.PDUSTAT ^= error << (RADIO_PDUSTAT_CISTAT_Pos + 1); /* The don't-care bit in CI */

    error = bs_random_Bern(RAND_PROB_1/2);
    if (error) {
      NRF_RADIO_regs.PDUSTAT ^= 1 << (RADIO_PDUSTAT_CISTAT_Pos);
      rx_status.CI ^= 1;
      rx_status.CI_error = true;
    }
  }
}

static void Rx_handle_end_response(bs_time_t end_time) {

  if (rx_status.inFEC1 == true) { /* End of CodedPhy packet FEC1 */
    Rx_handle_CI_reception();

  } else { //Normal packet or end of FEC2
    if (rx_status.rx_resp.status != P2G4_RXSTATUS_HEADER_ERROR) {
        rx_status.CRC_End_Time = end_time + nhwra_timings_get_Rx_chain_delay();
    } //Otherwise we do not really now how the Nordic RADIO behaves depending on
    //where the biterrors are and so forth. So let's always behave like if the
    //packet lenght was received correctly, and just report a CRC error at the
    //end of the CRC

    if ( rx_status.rx_resp.status == P2G4_RXSTATUS_OK ){
      NRF_RADIO_regs.RXCRC = nhwra_get_rx_crc_value(rx_buf, rx_status.rx_resp.packet_size);
      rx_status.CRC_OK = 1;
      NRF_RADIO_regs.CRCSTATUS = 1;
    }
  }
}


static void Rx_handle_address_end_response(bs_time_t address_time) {

  rx_status.ADDRESS_End_Time = address_time + nhwra_timings_get_Rx_chain_delay();

  if ((rx_status.codedphy == true) && (rx_status.inFEC1)) {
    rx_status.FEC2_start_time = address_time + 16 + 24 + 1; /* It will be updated on the FEC1 Rx end */
    rx_status.packet_rejected = false; //We always accept the FEC1 part

    //Let's set a very provisional packet end time, in case the Tx aborts between FEC1 and FEC2:
    rx_status.PAYLOAD_End_Time = rx_status.FEC2_start_time + 2*8/bits_per_us
                                + nhwra_timings_get_Rx_chain_delay(); /* An empty packet */
    rx_status.CRC_End_Time = rx_status.PAYLOAD_End_Time + rx_status.CRC_duration;
    return;
  }
  //Otherwise, FEC2 or not Coded Phy

  uint length = nhwra_get_payload_length(rx_buf);
  uint max_length = nhwra_get_MAXLEN();

  if (length > max_length) {
    // We reject the packet right away, setting the CRC error, and timers as expected
    bs_trace_warning_time_line("NRF_RADIO: received a packet longer than the configured MAXLEN (%i>%i). Truncating it\n", length, max_length);
    length  = max_length;
    NRF_RADIO_regs.PDUSTAT |= RADIO_PDUSTAT_PDUSTAT_Msk;
    rx_status.packet_rejected = true;
  } else {
    rx_status.packet_rejected = false;
  }
  if (rx_status.CI_error) {
    /* Let's just stop the Phy reception here, as continuing does not give us anything anymore */
    rx_status.packet_rejected = true;
  }

  //TODO: Discard Ieee802154_250Kbit frames with length == 0

  bs_time_t payload_end = 0;

  if (nhwra_is_ble_mode(NRF_RADIO_regs.MODE)) {
    payload_end = rx_status.rx_resp.rx_time_stamp + (bs_time_t)((2+length)*8/bits_per_us);
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    payload_end = rx_status.rx_resp.rx_time_stamp + (bs_time_t)((1+length)*8/bits_per_us);
  } //Eventually this should be generalized with the packet configuration

  rx_status.PAYLOAD_End_Time = nhwra_timings_get_Rx_chain_delay() +
                               hwll_dev_time_from_phy(payload_end);

  int TERM2_duration = 0; /* See Note21.b */
  if (rx_status.codedphy) {
    if (rx_status.CI == 1) {
      TERM2_duration = 6; //S=2 * 3bits
    } else {
      TERM2_duration = 24;//S=8 * 3bits
    }
  }

  rx_status.CRC_End_Time = rx_status.PAYLOAD_End_Time + rx_status.CRC_duration + TERM2_duration; //Provisional value (if we are accepting the packet)

  //Copy the whole packet (S0, lenght, S1 & payload) excluding the CRC.
  if (nhwra_is_ble_mode(NRF_RADIO_regs.MODE)) {
    if (rx_status.rx_resp.packet_size >= 5) { /*At least the header and CRC, otherwise better to not try to copy it*/
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0] = rx_buf[0];
      ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1] = rx_buf[1];
      /* We cheat a bit and copy the whole packet already (The AAR block will look in Adv packets after 64 bits)*/
      memcpy(&((uint8_t*)NRF_RADIO_regs.PACKETPTR)[2 + rx_status.S1Offset],
          &rx_buf[2] , length);
    }
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    if (rx_status.rx_resp.packet_size >= 3) { /*At least the header and CRC, otherwise better to not try to copy it*/
            ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0] = rx_buf[0];
            memcpy(&((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1 + rx_status.S1Offset],
                &rx_buf[1] , length);
          }
  } //Eventually this should be generalized with the packet configuration

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    //The real HW only copies the LQI value after the payload in this mode
    //Note that doing it this early is a cheat
    double RSSI = p2G4_RSSI_value_to_dBm(rx_status.rx_resp.rssi.RSSI) + cheat_rx_power_offset;
    uint8_t LQI = nhwra_dBm_to_modem_LQIformat(RSSI);
    //Eventually this should be generalized with the packet configuration:
    ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[1 + rx_status.S1Offset + length] = LQI;
  }

}

/**
 * Handle all possible responses from the phy to a Rx request
 */
static void handle_Rx_response(int ret){
  if (ret == -1) {
    bs_trace_raw_manual_time(3,nsi_hws_get_time(),"Communication with the phy closed during Rx\n");
    hwll_disconnect_phy_and_exit();

  } else if (ret == P2G4_MSG_ABORTREEVAL) {

    phy_sync_ctrl_set_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = Rx_Abort_reeval;
    nhwra_set_Timer_abort_reeval( BS_MAX(next_recheck_time,nsi_hws_get_time()) );

  } else  if ((ret == P2G4_MSG_RXV2_ADDRESSFOUND) && (radio_state == RAD_RX /*if we havent aborted*/)) {

    bs_time_t addres_time = hwll_dev_time_from_phy(rx_status.rx_resp.rx_time_stamp); //this is the end of the sync word in air time
    phy_sync_ctrl_set_last_phy_sync_time(addres_time);
    Rx_handle_address_end_response(addres_time);

    if ((rx_status.codedphy == false) || (rx_status.inFEC1)) {
      radio_sub_state = RX_WAIT_FOR_ADDRESS_END;
      nhwra_set_Timer_RADIO(rx_status.ADDRESS_End_Time);
    } else { //FEC2
      radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
      nhwra_set_Timer_RADIO(TIME_NEVER);
      Rx_Addr_received();
      nhwra_set_Timer_RADIO(rx_status.PAYLOAD_End_Time);
    }
  } else if ((ret == P2G4_MSG_RXV2_END) && (radio_state == RAD_RX /*if we havent aborted*/)) {

    bs_time_t end_time = hwll_dev_time_from_phy(rx_status.rx_resp.end_time);
    phy_sync_ctrl_set_last_phy_sync_time(end_time);

    /* P2G4_RXSTATUS_NOSYNC during a simple packet or CodedPhy FEC1 cannot really happen
     * As that would mean we have run out of the "infinite" scan time.
     * It can happen though at the start of the CodedPhy FEC2, if the transmitter aborted
     * before starting the FEC2 */
    if (rx_status.rx_resp.status == P2G4_RXSTATUS_NOSYNC) {
      if ((rx_status.codedphy == false) || (rx_status.inFEC1 == true)) {
        bs_trace_error_time_line("Unexpected not-handled path\n");
      }
      /* Otherwise we just wait for the RX_WAIT_FOR_PAYLOAD_END handling in the main RADIO FSM
       * A provisional PAYLOAD_End_Time was set earlier */
      radio_sub_state = RX_WAIT_FOR_PAYLOAD_END;
      nhwra_set_Timer_RADIO(rx_status.PAYLOAD_End_Time);
      return;
    } else {
      Rx_handle_end_response(end_time);

      if (rx_status.inFEC1 == true) {
        /* To avoid issues with possible rounding errors in the phy<->dev timing conversion,
         * we ensure the FEC2 Rx will start in the next us in Phy time */
        rx_status.rx_req.start_time = rx_status.rx_resp.end_time + 1;
        nhwra_set_Timer_RADIO(rx_status.FEC2_start_time);
      }
    }
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the phy with our decision
 */
static void Rx_abort_eval_respond(void) {
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &rx_status.rx_req.abort;
  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_rxv2_abort_nc_b(abort);

  handle_Rx_response(ret);
}

/*
 * Actually start the Rx in this microsecond
 */
static void start_Rx(void) {
  #define RX_N_ADDR 8 /* How many addresses we can search in parallel */
  p2G4_address_t rx_addresses[RX_N_ADDR];

  nhwra_check_packet_conf();

  radio_state = RAD_RX;
  NRF_RADIO_regs.STATE = RAD_RX;
  NRF_RADIO_regs.CRCSTATUS = 0;
  NRF_RADIO_regs.PDUSTAT = 0;

  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ){
    rx_status.S1Offset = 1; /*1 byte offset in RAM (S1 length > 8 not supported)*/
  } else {
    rx_status.S1Offset = 0;
  }

  rx_status.codedphy = false;
  rx_status.inFEC1 = false;
  rx_status.CI_error = false;
  rx_status.CI = 0;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    bits_per_us = 1;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    bits_per_us = 2;
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
      || (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit)) {
    bits_per_us = 0.125; /* For FEC1 part */
    rx_status.codedphy = true;
    rx_status.inFEC1 = true;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    bits_per_us = 0.25;
  }
  rx_status.CRC_duration = nhwra_get_crc_length()*8/bits_per_us;
  rx_status.CRC_OK = false;
  rx_status.rx_resp.status = P2G4_RXSTATUS_NOSYNC;

  if (rx_status.codedphy) {
    nhwra_prep_rx_request_FEC1(&rx_status.rx_req_fec1, rx_addresses);
    update_abort_struct(&rx_status.rx_req_fec1.abort, &next_recheck_time);
  }
  nhwra_prep_rx_request(&rx_status.rx_req, rx_addresses);
  update_abort_struct(&rx_status.rx_req.abort, &next_recheck_time);

  //attempt to receive
  int ret;
  if (rx_status.codedphy) {
    ret = p2G4_dev_req_rxv2_nc_b(&rx_status.rx_req_fec1, rx_addresses,
                                 &rx_status.rx_resp, &rx_pkt_buffer_ptr,
                                 _NRF_MAX_PACKET_SIZE);
  } else {
    ret = p2G4_dev_req_rxv2_nc_b(&rx_status.rx_req, rx_addresses,
                                 &rx_status.rx_resp,&rx_pkt_buffer_ptr,
                                 _NRF_MAX_PACKET_SIZE);
  }

  radio_sub_state = SUB_STATE_INVALID;
  nhwra_set_Timer_RADIO(TIME_NEVER);

  handle_Rx_response(ret);
}

/*
 * Start the Rx for a CodedPhy FEC2 packet part in this microsecond
 */
static void start_Rx_FEC2(void) {

  rx_status.inFEC1 = false;

  if (rx_status.CI == 0) {
    rx_status.rx_req.coding_rate = 8;
    //error_calc_rate & header_duration preset in nhwra_prep_rx_request() are already correct
  } else { //0b01
    bits_per_us = 0.5;
    rx_status.rx_req.coding_rate = 2;
    rx_status.rx_req.error_calc_rate = 500000;
    rx_status.rx_req.header_duration = 2*8*2 /* 2 bytes at 500kbps */;
  }
  /* rx_req.start_time was set based on the FEC1 end */
  rx_status.rx_req.pream_and_addr_duration = 0;
  rx_status.rx_req.n_addr = 0;
  rx_status.rx_req.scan_duration = 1;
  rx_status.rx_req.prelocked_tx = true;

  rx_status.CRC_duration = nhwra_get_crc_length()*8/bits_per_us;

  update_abort_struct(&rx_status.rx_req.abort, &next_recheck_time);

  int ret;

  ret = p2G4_dev_req_rxv2_nc_b(&rx_status.rx_req, NULL,
                               &rx_status.rx_resp, &rx_pkt_buffer_ptr,
                               _NRF_MAX_PACKET_SIZE);

  handle_Rx_response(ret);
}

/**
 * This function is called at the time when the Packet address* would have been
 * completely received for simple packets, AND at the beginning of the FEC2
 * for CodedPhy packets.
 * (* at the time of the end of the last bit of the packet address)
 * To continue processing the reception (the Phy was left waiting for a response)
 *
 * Note that libPhyCom has already copied the whole packet into the input buffer
 */
static void Rx_Addr_received(void) {

  bool accept_packet = !rx_status.packet_rejected;

  if (rx_status.codedphy == false || rx_status.inFEC1 == true) {
    if ( rssi_sampling_on ){
      NRF_RADIO_regs.RSSISAMPLE = nhwra_RSSI_value_to_modem_format(
                                    p2G4_RSSI_value_to_dBm(rx_status.rx_resp.rssi.RSSI)
                                    + cheat_rx_power_offset
                                  );
      nhw_RADIO_signal_EVENTS_RSSIEND(0);
    }
  }

  if (rx_status.codedphy == false || rx_status.inFEC1 == false) {
    NRF_RADIO_regs.RXMATCH = 0; //The only we support so far

    if (NRF_RADIO_regs.DACNF & 0xFF) { /*If any of the addresses for device address match is enabled*/
      /*
       * NOTE: we cheat and we already check the advertisement addresses and
       * raise the event, even though we should wait for 16 + 48 bits more
       *
       * If this is a problem, add a new timer and Rx state and delay raising the event
       * until then
       */
      nhw_radio_device_address_match(rx_buf);
    }
  }

  update_abort_struct(&rx_status.rx_req.abort, &next_recheck_time);
  int ret = p2G4_dev_rxv2_cont_after_addr_nc_b(accept_packet, &rx_status.rx_req.abort);

  if ( accept_packet ){ /* Always true for CodedPhy FEC1 */
    handle_Rx_response(ret);
  } else {
    //We said we don't want to continue => there will be no response (ret==0 always). We just close the reception like if the phy finished on its own even though we finished it

    //We do what would correspond to Rx_handle_end_response() as it won't get called
    NRF_RADIO_regs.RXCRC = nhwra_get_rx_crc_value(rx_buf, rx_status.rx_resp.packet_size);
    nhw_ccm_radio_received_packet(!rx_status.CRC_OK);
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
static void nhw_radio_device_address_match(uint8_t rx_buf[]) {
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
    nhw_RADIO_signal_EVENTS_DEVMATCH(0);
  } else {
    nhw_RADIO_signal_EVENTS_DEVMISS(0);
  }
}

static void CCA_handle_end_response(void) {
  //Depending on mode, set status and registers
  //raising CCAIDLE, CCABUSY or EDEND will happen in the correct time in the main machine

  if (cca_status.CCA_notED) { //End a CCA procedure
    uint CCAMode = (NRF_RADIO_regs.CCACTRL & RADIO_CCACTRL_CCAMODE_Msk) >> RADIO_CCACTRL_CCAMODE_Pos;

    if ((CCAMode == RADIO_CCACTRL_CCAMODE_EdMode)
        || (CCAMode == RADIO_CCACTRL_CCAMODE_EdModeTest1)) {
      cca_status.is_busy = cca_status.cca_resp.rssi_overthreshold;
    } else if (CCAMode == RADIO_CCACTRL_CCAMODE_CarrierMode) {
      cca_status.is_busy = cca_status.cca_resp.mod_found;
    } else if (CCAMode == RADIO_CCACTRL_CCAMODE_CarrierAndEdMode) {
      cca_status.is_busy = cca_status.cca_resp.mod_found
          && cca_status.cca_resp.rssi_overthreshold;
    } else if (CCAMode == RADIO_CCACTRL_CCAMODE_CarrierOrEdMode) {
      cca_status.is_busy = cca_status.cca_resp.mod_found
          || cca_status.cca_resp.rssi_overthreshold;
    } else {
      bs_trace_error_time_line("%s, CCAMODE=%i suppport not yet implemented\n",
          __func__, CCAMode);
    }
  } else { // Ending an ED procedure
    double RSSI = p2G4_RSSI_value_to_dBm(cca_status.cca_resp.RSSI_max) + cheat_rx_power_offset;
    NRF_RADIO_regs.EDSAMPLE = nhwra_dBm_to_modem_LQIformat(RSSI);
  }
}

/**
 * Handle all possible responses to a CCA request from the Phy
 */
static void handle_CCA_response(int ret){
  if (ret == -1){
    bs_trace_raw_manual_time(3,nsi_hws_get_time(),"The Phy disconnected us during a CCA procedure\n");
    hwll_disconnect_phy_and_exit();
  } else if ( ret == P2G4_MSG_CCA_END  ) {
    bs_time_t end_time = hwll_dev_time_from_phy(cca_status.cca_resp.end_time);
    phy_sync_ctrl_set_last_phy_sync_time( end_time );
    cca_status.CCA_end_time = end_time;
    if (radio_state == RAD_CCA_ED) { /*if we haven't aborted*/
      nhwra_set_Timer_RADIO(cca_status.CCA_end_time);
    }
    CCA_handle_end_response();
  } else if ( ret == P2G4_MSG_ABORTREEVAL ) {
    phy_sync_ctrl_set_last_phy_sync_time( next_recheck_time );
    abort_fsm_state = CCA_Abort_reeval;
    nhwra_set_Timer_abort_reeval(next_recheck_time);
  }
}

/**
 * We have reached the time in which we wanted to reevaluate if we would abort or not
 * so we answer to the Phy with our decision
 */
static void CCA_abort_eval_respond(void) {
  //The abort must have been evaluated by now so we can respond to the waiting phy
  p2G4_abort_t *abort = &cca_status.cca_req.abort;

  update_abort_struct(abort, &next_recheck_time);

  int ret = p2G4_dev_provide_new_cca_abort_nc_b(abort);

  handle_CCA_response(ret);
}

/**
 * Start CCA or ED procedure right now.
 * input: CCA_not_ED = 1 for CCA, 0 for ED
 */
static void start_CCA_ED(bool CCA_not_ED){

  radio_state = RAD_CCA_ED;

  cca_status.CCA_notED = CCA_not_ED;
  cca_status.is_busy = false;

  nhwra_prep_cca_request(&cca_status.cca_req, CCA_not_ED, cheat_rx_power_offset);

  update_abort_struct(&cca_status.cca_req.abort, &next_recheck_time);

  //Expected end time; note that it may be shorter if detect over threshold is set
  cca_status.CCA_end_time = nsi_hws_get_time() + cca_status.cca_req.scan_duration;
  nhwra_set_Timer_RADIO(cca_status.CCA_end_time);

  //Request the CCA from the Phy:
  int ret = p2G4_dev_req_cca_nc_b(&cca_status.cca_req, &cca_status.cca_resp);
  handle_CCA_response(ret);
}

void hw_radio_testcheat_set_rx_power_gain(double power_offset){
  cheat_rx_power_offset = power_offset;
}
