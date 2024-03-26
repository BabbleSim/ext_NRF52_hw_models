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

void nhwra_check_packet_conf(void);
uint32_t nhwra_RSSI_value_to_modem_format(double rssi_value);
uint8_t nhwra_dBm_to_modem_LQIformat(double rssi_value);
int nhwra_is_HW_TIFS_enabled(void);
void nhwra_prep_rx_request(p2G4_rxv2_t *ongoing_rx, p2G4_address_t *rx_addresses);
void nhwra_prep_rx_request_FEC1(p2G4_rxv2_t *rx_req, p2G4_address_t *rx_addresses);
void nhwra_prep_tx_request(p2G4_txv2_t *tx_req, uint packet_size, bs_time_t packet_duration,
                           bs_time_t start_time, uint16_t coding_rate);
void nhwra_prep_cca_request(p2G4_cca_t *cca_req, bool CCA_not_ED, double rx_pow_offset);

uint nhwra_tx_copy_payload(uint8_t *tx_buf);
uint nhwra_get_payload_length(uint8_t *buf);
uint32_t nhwra_get_rx_crc_value(uint8_t *rx_buf, size_t rx_packet_size);
uint nhwra_get_crc_length(void);
uint nhwra_get_MAXLEN(void);
bool nhwra_is_ble_mode(uint32_t MODE);

#ifdef __cplusplus
}
#endif

#endif
