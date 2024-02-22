/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Private types definitions for the RADIO peripheral
 */
#ifndef _NRF_RADIO_PRIVATE_H
#define _NRF_RADIO_PRIVATE_H

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {TIFS_DISABLE = 0, TIFS_WAITING_FOR_DISABLE, TIFS_TRIGGERING_TRX_EN} TIFS_state_t;
typedef enum {No_pending_abort_reeval = 0, Tx_Abort_reeval, Rx_Abort_reeval, CCA_Abort_reeval} abort_state_t;

typedef enum { //Note: This should match the real RADIO state values in the STATE register
  RAD_DISABLED = 0, //No operations are going on inside the radio and the power consumption is at a minimum
  RAD_RXRU, //The radio is ramping up and preparing for reception
  RAD_RXIDLE, //The radio is ready for reception to start
  RAD_RX, //Reception has been started and the addresses enabled in the RXADDRESSES register are being monitored
  RAD_RXDISABLE, //The radio is disabling the receiver

  RAD_TXRU = 9, //The radio is ramping up and preparing for transmission
  RAD_TXIDLE, //The radio is ready for transmission to start
  RAD_TXSTARTING, //The radio is starting to Tx (it will be in this state for TxChainDelay)
  RAD_TX, //The radio is transmitting a packet
  RAD_TXDISABLE, //The radio is disabling the transmitter

  RAD_CCA_ED, //We are in either a CCA or ED procedure
              //Not a real HW state. In real HW the RADIO is in RXIDLE or some other RX state. Seems the CCA and ED procedures as separate machines
} nrfra_state_t;

typedef enum {SUB_STATE_INVALID, /*The timer should not trigger in TX or RX state with this substate*/
  TX_WAIT_FOR_ADDRESS_END, TX_WAIT_FOR_FEC1_END, TX_WAIT_FOR_PAYLOAD_END, TX_WAIT_FOR_CRC_END,
  RX_WAIT_FOR_ADDRESS_END, RX_WAIT_FOR_FEC1_END, RX_WAIT_FOR_PAYLOAD_END, RX_WAIT_FOR_CRC_END
} nrfra_sub_state_t;

typedef struct {
  bs_time_t ADDRESS_End_Time;
  bs_time_t FEC2_start_time; /* In air */
  bs_time_t PAYLOAD_End_Time;
  bs_time_t CRC_End_Time;
  bs_time_t CRC_duration;
  p2G4_rxv2_t rx_req_fec1;
  p2G4_rxv2_t rx_req;
  p2G4_rxv2_done_t rx_resp;
  bool CRC_OK;
  bool packet_rejected;
  bool S1Offset;
  bool codedphy;
  uint8_t CI;
  bool inFEC1;
  bool CI_error;
} RADIO_Rx_status_t;

typedef struct {
  bs_time_t ADDRESS_end_time;
  bs_time_t FEC2_start_time; /* In air */
  bs_time_t PAYLOAD_end_time;
  bs_time_t CRC_end_time;
  p2G4_txv2_t tx_req_fec1;
  p2G4_txv2_t tx_req;
  p2G4_tx_done_t tx_resp;
  bool codedphy;
} RADIO_Tx_status_t;

typedef struct {
  bs_time_t CCA_end_time;
  p2G4_cca_t cca_req;
  p2G4_cca_done_t cca_resp;
  bool is_busy;
  bool CCA_notED; //Is it a CCA procedure (1), or ED procedure (0)
} RADIO_CCA_status_t;


#define _NRF_MAX_PACKET_SIZE (256+2+4)

#ifdef __cplusplus
}
#endif

#endif
