/**
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BLE_CRYPT_IF_H
#define BLE_CRYPT_IF_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BLECrypt_if_enable_real_encryption(bool mode);

void BLECrypt_if_free();

void BLECrypt_if_encrypt_packet(uint8_t aad,
    const uint8_t* unecrypted_payload,
    uint8_t* encrypted_payload,
    int length,
    bool generate_mic,
    const uint8_t* sk,
    const uint8_t* nonce
);

void BLECrypt_if_decrypt_packet(uint8_t aad,
    const uint8_t* encrypted_packet_payload,
    uint8_t* decrypted_packet_payload,
    int length,
    bool has_mic,
    const uint8_t* sk,
    const uint8_t* nonce,
    uint8_t *mic_error
);

void BLECrypt_if_aes_128(
    const uint8_t *key_be,
    const uint8_t *plaintext_data_be,
    uint8_t *encrypted_data_be
);

#ifdef __cplusplus
}
#endif

#endif
