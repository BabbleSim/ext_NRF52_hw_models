/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRC_BLE_H
#define _CRC_BLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void append_crc_ble(uint8_t* buf, unsigned int len, uint32_t crc_init);
void append_crc_154(uint8_t* buf, unsigned int len, uint16_t crc_init);

#ifdef __cplusplus
}
#endif

#endif
