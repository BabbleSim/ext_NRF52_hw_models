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

void BLECrypt_if_encrypt_packet(uint8_t packet_first_header_byte, // First byte of packet header
    const uint8_t* unecrypted_payload,      // Packet payload to be encrypted
    uint8_t* encrypted_payload,  //encrypted payload (and MIC if generate_mic==1)
    int length,        //including MIC length if ( generate_mic == 1 ) ; [ just  the length in the packet header ]
    bool generate_mic, //we have MIC, or not
    const uint8_t* sk, // Session key (16 bytes, BIG-ENDIAN)
    const uint8_t* nonce   // CCM Nonce (NONCE_LEN bytes, little-endian)
);

void BLECrypt_if_decrypt_packet(uint8_t packet_1st_header_byte, // First byte of packet header (or just LLID and RFU (RFU=0 for BLE v4.x) - other bits are ignored)
    const uint8_t* encrypted_packet_payload,      //as received from the air (including a MIC if has_mic)
    uint8_t* decrypted_packet_payload,
    int length,       //including MIC lenght if (has_mic == 1) ; [ just  the length in the packet header ]
    bool has_mic,
    const uint8_t* sk,      // Session key (16 bytes, BIG-ENDIAN)
    const uint8_t* nonce,   // CCM Nonce (NONCE_LEN bytes, little-endian)
    uint8_t *mic_error       /*was there a mic error in the packet (only if has_mic==1)*/
);

void BLECrypt_if_aes_128(
    // Inputs
    const uint8_t *key_be,                          // Key (KEY_LEN bytes, big-endian)
    const uint8_t *plaintext_data_be,               // Plaintext data (KEY_LEN bytes, big-endian)
    // Outputs (the pointers themselves are inputs and must point to large enough areas)
    uint8_t *encrypted_data_be);                    // Plaintext data (KEY_LEN bytes, big-endian)

#ifdef __cplusplus
}
#endif

#endif
