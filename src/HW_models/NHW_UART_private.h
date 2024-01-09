/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_UART_PRIVATE_H
#define _NRF_HW_MODEL_UART_PRIVATE_H

#include "bs_types.h"
#include <stdint.h>
#include <stdio.h>
#include "NHW_UART_backend_if.h"

#ifdef __cplusplus
extern "C"{
#endif

enum uart_tx_status {Tx_Off = 0,
                     Tx_Idle,
                     Tx_Pend /* Waiting for CTS to lower to start Tx'ing */,
                     Txing,
                     Tx_Stopping /* Waiting for current frame to finish to finish a TASK STOP */};
enum uarte_dma_status {DMA_Off = 0, DMAing};
enum uart_rx_status {Rx_Off = 0, Rx_turning_off /* Waiting for RX TO */, Rx_On};

#define RX_FIFO_SIZE 6
#define RX_FIFO_RTS_THRESHOLD 2

struct uarte_status {
  bs_time_t Rx_TO_timer;
  bs_time_t Tx_byte_done_timer;

  uint inst;

  NRF_UART_Type *UART_regs[NHW_UARTE_TOTAL_INST];
  NRF_UARTE_Type *UARTE_regs[NHW_UARTE_TOTAL_INST];
#if (NHW_HAS_DPPI)
  /* Mapping of peripheral instance to DPPI instance */
  uint dppi_map;
#endif

  enum uart_tx_status tx_status;
  enum uart_rx_status rx_status;

  /* When was the last time the receiver was off (only valid if the receiver is currently On */
  bs_time_t Last_Rx_off_time;

  uint8_t Rx_FIFO[RX_FIFO_SIZE];
  int Rx_FIFO_cnt;

  uint8_t Tx_byte;

  bool RTSR; /* Logical level of RTS/R (false/lowered => Ready to receive)
                (this value is internal, and toggles even if the flow-control is disabled) */
  bool CTS_blocking; /* CTS is blocking the Tx (i.e. it is high),
                      * this value toggles even if flow control is disabled */

  /* DMA status including internally buffered/shadow version of the corresponding registers */
  uint32_t TXD_PTR;
  uint32_t TXD_MAXCNT;
  uint32_t TXD_AMOUNT;
  enum uarte_dma_status tx_dma_status;

  uint32_t RXD_PTR;
  uint32_t RXD_MAXCNT;
  uint32_t RXD_AMOUNT;
  enum uarte_dma_status rx_dma_status;

  struct backend_if backend;

  char *Rx_log_file_name;
  char *Tx_log_file_name;
  FILE *Tx_log_file;
  FILE *Rx_log_file;

  uart_rtxb_cb_f trx_callbacks[2];
};

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UART_PRIVATE_H */
