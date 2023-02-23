/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note: This header is private to the RADIO HW model
 */
#ifndef _NRF_RADIO_SIGNALS_H
#define _NRF_RADIO_SIGNALS_H

#ifdef __cplusplus
extern "C"{
#endif

void nrf_radio_signal_DISABLED();
void nrf_radio_signal_READY();
//void nrf_radio_signal_TXREADY();
//void nrf_radio_signal_RXREADY();
void nrf_radio_signal_RSSIEND();
void nrf_radio_signal_ADDRESS();
void nrf_radio_signal_PAYLOAD();
void nrf_radio_signal_CRCOK();
void nrf_radio_signal_CRCERROR();
void nrf_radio_signal_END();
void nrf_radio_signal_DEVMATCH();
void nrf_radio_signal_DEVMISS();
void nrf_radio_signal_BCMATCH();
void nrf_radio_signal_SYNC();
void nrf_radio_signal_PHYEND();

#ifdef __cplusplus
}
#endif

#endif
