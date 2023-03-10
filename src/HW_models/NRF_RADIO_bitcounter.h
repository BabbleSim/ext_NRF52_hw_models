/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Private header between the RADIO and its bitcounter
 */
#ifndef _NRF_RADIO_BITCOUNTER_H
#define _NRF_RADIO_BITCOUNTER_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_radio_bitcounter_reset();
void nrf_radio_bitcounter_cleanup();
void nrf_radio_stop_bit_counter();

#ifdef __cplusplus
}
#endif

#endif
