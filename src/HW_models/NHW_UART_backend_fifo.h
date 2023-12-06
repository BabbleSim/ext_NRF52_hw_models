/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_BACKEND_FIFO_H
#define _NRF_HW_MODEL_BACKEND_FIFO_H

#include <stdint.h>
#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Protocol
 *
 *   The device creates (if not there) and opens its Tx side FIFO at boot
 *   The device tries to opens its Rx side FIFO when the UART is enabled first time
 *
 *   Messages:
 *    <uint64_t time> <uint8_t msg_type> <uint16_t msg_size (including time and msg_type)>
 *          <...>
 *
 *    <uint64_t time> MODE_CHANGE <msg_size>
 *          <uint32_t baudrate> <uint32_t config>
 *          (note the time stamp is ignored, as only the next Tx is affected by the rate change)
 *
 *    <uint64_t time> TX_BYTE <msg_size>
 *          <uint8_t data >
 *          (note time is when the frame ends)
 *
 *    <uint64_t time> RTS_CTS_TOGGLE <msg_size>
 *          <uint8_t level {0,1}>
 *
 *    <uint64_t time> NOP <msg_size>
 *
 *    <uint64_t time> DISCONNECT <msg_size>
 *          The other side is disconnecting gracefully
 */

struct ufifo_msg_header {
  bs_time_t time;
  enum {ufifo_NOP=0, ufifo_MODE_CHANGE, ufifo_TX_BYTE, ufifo_RTS_CTS_TOGGLE, ufifo_DISCONNECT} msg_type;
  uint16_t size;
} __attribute__ ((packed));

struct ufifo_msg_tx {
  struct ufifo_msg_header header;
  uint8_t data;
} __attribute__ ((packed));

struct ufifo_msg_rts_cts {
  struct ufifo_msg_header header;
  uint8_t level;
} __attribute__ ((packed));

struct ufifo_msg_mode_change {
  struct ufifo_msg_header header;
  uint32_t baudrate;
  uint32_t config;
} __attribute__ ((packed));

#define UFIFO_MSG_HEADER_SIZE (sizeof(struct ufifo_msg_header))
#define UFIFO_MSG_TXL_BODY_SIZE (sizeof(struct ufifo_msg_tx) - sizeof(struct ufifo_msg_header))
#define UFIFO_MSG_RTS_CTS_BODY_SIZE (sizeof(struct ufifo_msg_rts_cts) - sizeof(struct ufifo_msg_header))
#define UFIFO_MSG_MODE_CHANGE_BODY_SIZE (sizeof(struct ufifo_msg_mode_change) - sizeof(struct ufifo_msg_header))

#define UFIFO_BIGGEST_MSG (sizeof(struct ufifo_msg_mode_change))

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UART_H */
