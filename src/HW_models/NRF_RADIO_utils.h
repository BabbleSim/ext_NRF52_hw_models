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

#ifdef __cplusplus
extern "C"{
#endif

void nrfra_check_packet_conf(void);
uint32_t nrfra_RSSI_value_to_modem_format(double rssi_value);
int nrfra_is_HW_TIFS_enabled();

#ifdef __cplusplus
}
#endif

#endif
