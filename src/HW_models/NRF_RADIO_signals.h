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

void nrf_radio_signal_READY(void);
void nrf_radio_signal_ADDRESS(void);
void nrf_radio_signal_PAYLOAD(void);
void nrf_radio_signal_END(void);
void nrf_radio_signal_DISABLED(void);
void nrf_radio_signal_DEVMATCH(void);
void nrf_radio_signal_DEVMISS(void);
void nrf_radio_signal_RSSIEND(void);
void nrf_radio_signal_BCMATCH(void);
void nrf_radio_signal_CRCOK(void);
void nrf_radio_signal_CRCERROR(void);
void nrf_radio_signal_FRAMESTART(void);
void nrf_radio_signal_EDEND(void);
void nrf_radio_signal_EDSTOPPED(void);
void nrf_radio_signal_CCAIDLE(void);
void nrf_radio_signal_CCABUSY(void);
void nrf_radio_signal_CCASTOPPED(void);
//void nrf_radio_signal_RATEBOOST(void);
void nrf_radio_signal_TXREADY(void);
void nrf_radio_signal_RXREADY(void);
//void nrf_radio_signal_MHRMATCH(void);
void nrf_radio_signal_SYNC(void);
void nrf_radio_signal_PHYEND(void);
//void nrf_radio_signal_CTEPRESENT(void);

#ifdef __cplusplus
}
#endif

#endif
