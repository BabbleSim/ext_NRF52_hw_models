/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note: This header is private to the RADIO HW model
 */
#ifndef _NRF_RADIO_UTILS_H
#define _NRF_RADIO_UTILS_H

#include <stdint.h>
#include "bs_pc_2G4_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrfra_check_packet_conf(void);
uint32_t nrfra_RSSI_value_to_modem_format(double rssi_value);
uint8_t nrfra_RSSI_value_to_modem_LQIformat(double rssi_value);
int nrfra_is_HW_TIFS_enabled();
void nrfra_prep_rx_request(p2G4_rxv2_t *ongoing_rx, p2G4_address_t *rx_addresses);
void nrfra_prep_tx_request(p2G4_txv2_t *ongoing_tx, uint packet_size, bs_time_t packet_duration);

uint nrfra_tx_copy_payload(uint8_t *tx_buf);
uint nrfra_get_payload_length(uint8_t *buf);
uint nrfra_get_crc_length();

#ifdef __cplusplus
}
#endif

#endif
