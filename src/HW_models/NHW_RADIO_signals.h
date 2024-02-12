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

void nhw_RADIO_signal_EVENTS_READY(unsigned int inst);
void nhw_RADIO_signal_EVENTS_ADDRESS(unsigned int inst);
void nhw_RADIO_signal_EVENTS_PAYLOAD(unsigned int inst);
void nhw_RADIO_signal_EVENTS_END(unsigned int inst);
void nhw_RADIO_signal_EVENTS_DISABLED(unsigned int inst);
void nhw_RADIO_signal_EVENTS_DEVMATCH(unsigned int inst);
void nhw_RADIO_signal_EVENTS_DEVMISS(unsigned int inst);
void nhw_RADIO_signal_EVENTS_RSSIEND(unsigned int inst);
void nhw_RADIO_signal_EVENTS_BCMATCH(unsigned int inst);
void nhw_RADIO_signal_EVENTS_CRCOK(unsigned int inst);
void nhw_RADIO_signal_EVENTS_CRCERROR(unsigned int inst);
void nhw_RADIO_signal_EVENTS_FRAMESTART(unsigned int inst);
void nhw_RADIO_signal_EVENTS_EDEND(unsigned int inst);
void nhw_RADIO_signal_EVENTS_EDSTOPPED(unsigned int inst);
void nhw_RADIO_signal_EVENTS_CCAIDLE(unsigned int inst);
void nhw_RADIO_signal_EVENTS_CCABUSY(unsigned int inst);
void nhw_RADIO_signal_EVENTS_CCASTOPPED(unsigned int inst);
void nhw_RADIO_signal_EVENTS_RATEBOOST(unsigned int inst);
void nhw_RADIO_signal_EVENTS_TXREADY(unsigned int inst);
void nhw_RADIO_signal_EVENTS_RXREADY(unsigned int inst);
//void nhw_RADIO_signal_EVENTS_MHRMATCH(unsigned int inst);
void nhw_RADIO_signal_EVENTS_SYNC(unsigned int inst);
void nhw_RADIO_signal_EVENTS_PHYEND(unsigned int inst);
//void nhw_RADIO_signal_EVENTS_CTEPRESENT(unsigned int inst);

void nhwra_signalif_reset(void);

#ifdef __cplusplus
}
#endif

#endif
