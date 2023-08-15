/**
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
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

#include <stdbool.h>
#include <dlfcn.h>
#include <string.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "nsi_tasks.h"

static bool Real_encryption_enabled = false;
static void *LibCryptoHandle = NULL;
//Note that if this library IF is changed, this function prototypes need to be updated:
//IF to the libCryptoBLE:
typedef enum { SLAVE_TO_MASTER_DIRECTION, MASTER_TO_SLAVE_DIRECTION } blecrypt_packet_direction_t;

typedef void (*blecrypt_packet_encrypt_f)(
    // Inputs
    uint8_t packet_1st_header_byte,
    uint8_t packet_payload_len,
    const uint8_t *packet_payload,
    const uint8_t *sk,
    const uint8_t *nonce,
    uint8_t *encrypted_packet_payload_and_mic);

typedef int (*blecrypt_packet_decrypt_f)(
    uint8_t packet_1st_header_byte,
    uint8_t packet_payload_len,
    const uint8_t *packet_payload_and_mic,
    const uint8_t *sk,
    const uint8_t *nonce,
    int no_mic,
    uint8_t *decrypted_packet_payload);

typedef void (*blecrypt_aes_128_f)(
    // Inputs
    const uint8_t *key_be,            // Key (KEY_LEN bytes, big-endian)
    const uint8_t *plaintext_data_be, // Plaintext data (KEY_LEN bytes, big-endian)
    // Outputs (the pointers themselves are inputs and must point to large enough areas)
    uint8_t *encrypted_data_be);      // Plaintext data (KEY_LEN bytes, big-endian)

static blecrypt_packet_encrypt_f blecrypt_packet_encrypt;
static blecrypt_packet_decrypt_f blecrypt_packet_decrypt;
static blecrypt_aes_128_f        blecrypt_aes_128;

static bool BLECrypt_if_args_useRealAES;

static void BLECrypt_if_register_cmd_args(void) {
  static bs_args_struct_t args_struct_toadd[] = {
  {
    .option = "RealEncryption",
    .name = "realAES",
    .type = 'b',
    .dest = (void*)&BLECrypt_if_args_useRealAES,
    .descript = "(0)/1 Use the real AES encryption for the LL or just send everything in "
                "plain text (default) (requires the ext_libCryptov1 component)"
  },
  ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(BLECrypt_if_register_cmd_args, PRE_BOOT_1, 90);

static void BLECrypt_if_enable_real_encryption(void) {
  if ( BLECrypt_if_args_useRealAES ) { //if the user tried to enable it
    //Attempt to load libCrypto
    char lib_name[128];
    char *error;
    snprintf(lib_name,128,"../lib/libCryptov1.so"); //Relative to the working directory which is expected to be the bin folder
    LibCryptoHandle = dlopen(lib_name, RTLD_NOW);
    if (!LibCryptoHandle) { //Not found
      snprintf(lib_name,128,"libCryptov1.so"); //Let's see if the user set LD_LIBRARY_PATH
      LibCryptoHandle = dlopen(lib_name, RTLD_NOW);
      if (!LibCryptoHandle) {
        bs_trace_warning_line("%s\n",dlerror());
        bs_trace_warning_line("Could not find the libCrypto library neither in ../lib or in LD_LIBRARY_PATH, is it compiled? => disabling real encryption\n");
        Real_encryption_enabled = false;
        return;
      } else {
        bs_trace_info_line(3, "This executable assumes the working directory is BabbleSim's bin/ folder, but it is not. libCrypto was found anyhow\n");
      }
    }
    if ((error = dlerror()) != NULL) {
      bs_trace_error_line("%s\n",error);
    }

    *(void **) (&blecrypt_packet_encrypt) = dlsym(LibCryptoHandle, "blecrypt_packet_encrypt");
    if ((error = dlerror()) != NULL) {
      bs_trace_error_line("%s\n",error);
    }
    *(void **) (&blecrypt_packet_decrypt) = dlsym(LibCryptoHandle, "blecrypt_packet_decrypt");
    if ((error = dlerror()) != NULL) {
      bs_trace_error_line("%s\n",error);
    }
    *(void **) (&blecrypt_aes_128) = dlsym(LibCryptoHandle, "blecrypt_aes_128");
    if ((error = dlerror()) != NULL) {
      bs_trace_error_line("%s\n",error);
    }
    Real_encryption_enabled = true;
  } else {
    Real_encryption_enabled = false;
  }
}

NSI_TASK(BLECrypt_if_enable_real_encryption, HW_INIT, 100);

static void BLECrypt_if_free(void){
  if ( LibCryptoHandle != NULL ){
    //#define DONTCLOSELIBRARIES
#ifndef DONTCLOSELIBRARIES /*To be able to profile time spent in libraries in callgrind*/
    dlclose(LibCryptoHandle);
    LibCryptoHandle = NULL;
#endif
  }
}

NSI_TASK(BLECrypt_if_free, ON_EXIT_POST, 100);

void BLECrypt_if_encrypt_packet(uint8_t packet_first_header_byte, // First byte of packet header
    const uint8_t* unecrypted_payload,      // Packet payload to be encrypted
    uint8_t* encrypted_payload,  //encrypted payload (and MIC if generate_mic==1)
    int length,        //including MIC length if ( generate_mic == 1 ) ; [ just the length in the packet header ]
    bool generate_mic, //we have MIC, or not
    const uint8_t* sk, // Session key (16 bytes, BIG-ENDIAN)
    const uint8_t* nonce   // CCM Nonce (NONCE_LEN bytes, little-endian)
){

  if ( length > 0 ) { //otherwise there is nothing to encrypt and for sure no MIC to add
    uint8_t packet_payload_len = length - generate_mic*4;

    if ( Real_encryption_enabled ) {

      blecrypt_packet_encrypt(
          packet_first_header_byte,
          packet_payload_len,
          unecrypted_payload,
          sk,
          nonce,
          encrypted_payload);
      //this generates always the MIC at the end, but for MIC less cases we just wont transmit it

    } else {
      /*
       * we could do something like this:
       *   Generate quickly a "nonce" as a concatenation of the tx_packet_counter and IV
       *   KK = XOR it with the sk
       *   data to transmit = KK XOR data,
       *   MIC = 4 lower bytes of KK
       * so the MIC would be broken and the data scrambled if the security initialization was not proper
       */
      memcpy(encrypted_payload, unecrypted_payload, packet_payload_len /*payload excluding possible mic*/);
      if ( generate_mic ) { //the MIC:
        encrypted_payload[length - 4 ] = 0;
        encrypted_payload[length - 3 ] = 'M';
        encrypted_payload[length - 2 ] = 'I';
        encrypted_payload[length - 1 ] = 'C';
      }
    }
  }
}

void BLECrypt_if_decrypt_packet(uint8_t packet_1st_header_byte, // First byte of packet header (or just LLID and RFU (RFU=0 for BLE v4.x) - other bits are ignored)
    const uint8_t* encrypted_packet_payload,      //as received from the air (including a MIC if has_mic)
    uint8_t* decrypted_packet_payload,
    int length,       //including MIC lenght if (has_mic == 1) ; [ just  the length in the packet header ]
    bool has_mic,
    const uint8_t* sk, // Session key (16 bytes, BIG-ENDIAN)
    const uint8_t* nonce,   // CCM Nonce (NONCE_LEN bytes, little-endian)
    uint8_t *mic_error       /*was there a mic error in the packet (only if has_mic==1)*/
){

  if ( length == 0 ){ //otherwise there is nothing to decrypt and no MIC to check
    *mic_error = 0;
    return;
  }

  if ( has_mic && ( length < 5 )  ){
    //if the receiver thinks we have a packet with MIC but the transmitter did
    //not add the MIC the packet length could be 0..4 bytes long, which the
    //real AES block should mark as a mic_error.
    //In that case there should not be an attempt to decrypt
    *mic_error = 1;
    return;
  }

  uint8_t packet_payload_len = length - has_mic*4;

  if ( Real_encryption_enabled ) {
    *mic_error = !blecrypt_packet_decrypt(
        packet_1st_header_byte,
        packet_payload_len,
        encrypted_packet_payload,
        sk,
        nonce,
        !has_mic,
        decrypted_packet_payload);
  } else {
    //exactly the same we do in BLECrypt_if_encrypt_packet()
    memcpy(decrypted_packet_payload, encrypted_packet_payload, packet_payload_len);
    *mic_error = 0;
  }
}

void BLECrypt_if_aes_128(
    // Inputs
    const uint8_t *key_be,            // Key (KEY_LEN bytes, big-endian)
    const uint8_t *plaintext_data_be, // Plaintext data (KEY_LEN bytes, big-endian)
    // Outputs (the pointers themselves are inputs and must point to large enough areas)
    uint8_t *encrypted_data_be)
{
  if ( Real_encryption_enabled ) {
    blecrypt_aes_128(key_be,
        plaintext_data_be,
        encrypted_data_be);
  } else {
    /* we just copy the data */
    memcpy(encrypted_data_be, plaintext_data_be, 16);
  }
}
