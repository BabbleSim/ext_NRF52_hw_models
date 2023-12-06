/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_UART_BACKEND_IF_H
#define _NRF_HW_MODEL_UART_BACKEND_IF_H

#include "bs_types.h"
#include <stdint.h>
#include "NHW_config.h"
#include "NHW_UART.h"

#ifdef __cplusplus
extern "C"{
#endif

/* Interface backends export to main UART module */

typedef void (*tx_byte_ft)(uint inst, uint8_t data);
typedef void (*RTS_pin_toggle_ft)(uint inst, bool new_level);
typedef void (*uart_enable_notify_ft)(uint inst, uint8_t tx_enabled, uint8_t rx_enabled);

struct backend_if {
  tx_byte_ft tx_byte_f; /* Note this is called at the beginning of the byte frame*/
  RTS_pin_toggle_ft RTS_pin_toggle_f;
  uart_enable_notify_ft uart_enable_notify_f;
};

/* Interface UART module exposes to backends */
void nhw_UARTE_digest_Rx_byte(uint inst, uint8_t byte);
bs_time_t nhw_uarte_one_byte_time(uint inst);
void nhw_UARTE_CTS_lowered(uint inst);
void nhw_UARTE_CTS_raised(uint inst);

void nhw_UARTE_backend_register(uint inst, struct backend_if *backend);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UART_BACKEND_IF_H */

