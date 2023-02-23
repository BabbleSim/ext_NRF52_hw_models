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
typedef enum {No_pending_abort_reeval = 0, Tx_Abort_reeval, Rx_Abort_reeval} abort_state_t;

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
} nrfra_state_t;

typedef enum {SUB_STATE_INVALID, /*The timer should not trigger in TX or RX state with this substate*/
  TX_WAIT_FOR_ADDRESS_END, TX_WAIT_FOR_PAYLOAD_END, TX_WAIT_FOR_CRC_END,
  RX_WAIT_FOR_ADDRESS_END, RX_WAIT_FOR_PAYLOAD_END, RX_WAIT_FOR_CRC_END
} nrfra_sub_state_t;

typedef struct {
  bs_time_t CRC_duration;
  bool CRC_OK;
  bs_time_t ADDRESS_End_Time;
  bs_time_t PAYLOAD_End_Time;
  bs_time_t CRC_End_Time;
  bool packet_rejected;
  bool S1Offset;
} ongoing_rx_RADIO_status_t;

#define _NRF_MAX_PACKET_SIZE (256+2+4)

#ifdef __cplusplus
}
#endif

#endif
