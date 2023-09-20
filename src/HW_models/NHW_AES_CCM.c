/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CCM - AES CCM mode encryption
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ccm.html?cp=5_1_0_5_3
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/ccm.html?cp=4_0_0_6_4
 *
 * Notes:
 *
 * 1. This model does not try to account for the delay the real CCM HW block has
 *    Instead:
 *    * The key stream generation will be instantaneous
 *      (really nothing will be done in this step)
 *    * In transmission mode the packet will be encrypted instantly
 *      after triggering TASKS_CRYPT
 *    * In reception TASKS_CRYPT will not do anything, instead a "cheat" call
 *      from the RADIO at the end of the packet will trigger the whole
 *      packet to be decoded (if the CRC was ok) instantaneously.
 *      If the CRC failed, nothing will be done with the payload, but micerror
 *      will be set
 *    Therefore we ignore the RATE register, RATEOVERRIDE register and TASK
 *
 * 2. MAXPACKETSIZE and MODE.LENGTH are ignored. Packets are decrypted correctly
 *    according to their length
 *
 * 3. TASKS_STOP is not really supported
 *
 * 4. TASK_RATEOVERRIDE and RATEOVERRIDE are not supported (just ignored)
 */

#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_templates.h"
#include "NHW_AES_CCM.h"
#include <string.h>
#include <stdbool.h>
#include "nsi_hw_scheduler.h"
#include "NHW_peri_types.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"

NRF_CCM_Type NRF_CCM_regs;
#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_CCM_dppi_map[NHW_CCM_TOTAL_INST] = NHW_CCM_DPPI_MAP;
#endif

static uint32_t CCM_INTEN; //interrupt enable
static bool decryption_ongoing;

static void nhw_aes_ccm_init(void) {
  memset(&NRF_CCM_regs, 0, sizeof(NRF_CCM_regs));
  NRF_CCM_regs.MODE = 0x01;
  NRF_CCM_regs.HEADERMASK= 0xE3;
  NRF_CCM_regs.MAXPACKETSIZE = 0xFB;
  CCM_INTEN = 0;
  decryption_ongoing = false;
}

NSI_TASK(nhw_aes_ccm_init, HW_INIT, 100);

static void nhw_CCM_eval_interrupt(uint inst) {
  static bool ccm_int_line[NHW_CCM_TOTAL_INST]; /* Is the CCM currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_ccm_irq_map[NHW_CCM_TOTAL_INST] = NHW_CCM_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(CCM, ENDKSGEN, CCM_INTEN)
  NHW_CHECK_INTERRUPT_si(CCM, ENDCRYPT, CCM_INTEN)
  NHW_CHECK_INTERRUPT_si(CCM, ERROR, CCM_INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&ccm_int_line[inst],
                                       new_int_line,
                                       &nhw_ccm_irq_map[inst]);
}

static NHW_SIGNAL_EVENT_si(CCM, ENDKSGEN)
static NHW_SIGNAL_EVENT_si(CCM, ENDCRYPT)
//Unused so far in this model: static NHW_SIGNAL_EVENT_si(CCM, ERROR)

static void signal_EVENTS_ENDKSGEN(void) {
  nhw_CCM_signal_EVENTS_ENDKSGEN(0);

  if (NRF_CCM_regs.SHORTS & CCM_SHORTS_ENDKSGEN_CRYPT_Msk) {
    nhw_CCM_TASK_CRYPT();
  }
}

#define IV_LEN      8
#define NONCE_LEN   13

// Calculates CCM nonce.
static void nonce_calc(
    // Inputs
    const uint8_t *iv,         // Initialization vector (IV_LEN bytes, little-endian)
    uint64_t packet_counter,   // 39-bit packet count (in given direction, excl. retransmissions and empty packets) since start of encryption
    uint8_t packet_direction,  // Direction of packet (1:master to slave, 0: slave to master)
    // Outputs (the pointers themselves are inputs and must point to large enough areas)
    uint8_t *ccm_nonce)        // Resulting nonce (NONCE_LEN bytes, little-endian)
{
  int i;
  // Copy 39-bit packet counter into first 5 bytes of nonce and set 40th bit depending on packet
  // direction
  for (i = 0; i < NONCE_LEN - IV_LEN - 1; i++)
  {
    ccm_nonce[i] = packet_counter & 0xFF;
    packet_counter >>= 8;
  }
  ccm_nonce[i] = (packet_counter & 0x7F) | (packet_direction == 1 /*master to slave*/ ? 0x80 : 0);
  // Copy initialization vector into remaining 8 bytes of nonce
  memcpy(&ccm_nonce[NONCE_LEN - IV_LEN], iv, IV_LEN);
}

static void nrf_ccm_encrypt_tx(void) {
  const uint8_t* cnfptr;
  const uint8_t* sk;
  const uint8_t* iv;
  uint64_t tx_pkt_ctr;
  uint8_t pkt_direction;
  const uint8_t* inptr;
  uint8_t* outptr;
  int length;
  uint8_t ccm_nonce[NONCE_LEN];
  uint8_t aad;

  cnfptr = (const uint8_t*)NRF_CCM_regs.CNFPTR;
  sk = cnfptr;
  tx_pkt_ctr = *(uint64_t*)(cnfptr + 16) & 0x7FFFFFFFFFULL;
  iv = &cnfptr[25];

  inptr = (const uint8_t*)NRF_CCM_regs.INPTR;
  outptr =  (uint8_t*)NRF_CCM_regs.OUTPTR;

  length = inptr[1];
  if (length > 0) {
    length +=4;
  }
  outptr[0] = inptr[0];
  outptr[1] = length;
  pkt_direction = cnfptr[24] & 1;
  /* Note that outptr[2] is reserved for S1 in the HW (but unused) */

  nonce_calc(iv, tx_pkt_ctr, pkt_direction, ccm_nonce);

  aad = inptr[0] & NRF_CCM_regs.HEADERMASK;

  BLECrypt_if_encrypt_packet(aad, //Additional Authentication data
      (uint8_t*)&inptr[3],      // Packet payload to be encrypted
      &outptr[3],  //encrypted payload (and MIC if generate_mic==1)
      length,      //including MIC length if ( generate_mic == 1 ) ; [ just  the length in the output packet header ]
      true, //we have MIC, or not
      sk, // Session key (16 bytes, BIG-ENDIAN)
      ccm_nonce
  );

  nhw_CCM_signal_EVENTS_ENDCRYPT(0);
}

static void nrf_ccm_decrypt_rx(bool crc_error) {
  const uint8_t* cnfptr;
  const uint8_t* sk;
  const uint8_t* iv;
  uint64_t rx_pkt_ctr;
  uint8_t pkt_direction;
  const uint8_t* inptr;
  uint8_t* outptr;
  int length;
  uint8_t mic_error;
  uint8_t ccm_nonce[NONCE_LEN];
  uint8_t aad;

  if (crc_error) {
    NRF_CCM_regs.MICSTATUS = 0;
    nhw_CCM_signal_EVENTS_ENDCRYPT(0);
    return;
  }

  cnfptr = (const uint8_t*)NRF_CCM_regs.CNFPTR;
  sk = cnfptr;
  rx_pkt_ctr = *(uint64_t*)(cnfptr + 16) & 0x7FFFFFFFFFULL;
  iv = &cnfptr[25];

  inptr = (const uint8_t*)NRF_CCM_regs.INPTR;
  outptr = (uint8_t*)NRF_CCM_regs.OUTPTR;

  length = inptr[1];
  if (length > 4) {
    length -=4;
  }
  outptr[0] = inptr[0];
  outptr[1] = length;
  pkt_direction = cnfptr[24] & 1;
  /* Note that outptr[2] is reserved for S1 in the HW (but unused) */

  nonce_calc(iv, rx_pkt_ctr, pkt_direction, ccm_nonce);

  aad = inptr[0] & NRF_CCM_regs.HEADERMASK;

  BLECrypt_if_decrypt_packet(aad, //Additional Authentication data
      (uint8_t*)&inptr[3], //as received from the air (including a MIC if has_mic)
      &outptr[3], //buffer for decrypted payload
      inptr[1],   //including MIC lenght if (has_mic == 1) ; [ just  the length in the packet header ]
      true, //we have MIC, or not
      sk, // Session key (16 bytes, BIG-ENDIAN)
      ccm_nonce,
      &mic_error
  );

  NRF_CCM_regs.MICSTATUS = !mic_error;

  nhw_CCM_signal_EVENTS_ENDCRYPT(0);
}

void nhw_CCM_TASK_KSGEN(void) {
  if (NRF_CCM_regs.ENABLE != CCM_ENABLE_ENABLE_Enabled) {
    return;
  }
  /* In this model we cheat and we do it instantly */
  signal_EVENTS_ENDKSGEN();
}

void nhw_CCM_TASK_CRYPT(void) {
  if (NRF_CCM_regs.ENABLE != CCM_ENABLE_ENABLE_Enabled) {
    return;
  }

  if ((NRF_CCM_regs.MODE & CCM_MODE_MODE_Msk) >> CCM_MODE_MODE_Pos
      == CCM_MODE_MODE_Encryption) {
    nrf_ccm_encrypt_tx();
  } else {
    decryption_ongoing = true;
  }
}

void nhw_CCM_TASK_STOP(void) {
  bs_trace_warning_line_time("CCM: TASK_STOP functionality is not implemented\n");
  decryption_ongoing = false;
}

void nhw_CCM_TASK_RATEOVERRIDE(void) {
  /* We ignore the RATEOVERRIDE task */
  bs_trace_warning_line_time("%s ignored\n", __func__);
}

NHW_SIDEEFFECTS_INTSET_si(CCM, NRF_CCM_regs., CCM_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(CCM, NRF_CCM_regs., CCM_INTEN)

NHW_SIDEEFFECTS_EVENTS(CCM)

NHW_SIDEEFFECTS_TASKS_si(CCM, KSGEN)
NHW_SIDEEFFECTS_TASKS_si(CCM, CRYPT)
NHW_SIDEEFFECTS_TASKS_si(CCM, STOP)
NHW_SIDEEFFECTS_TASKS_si(CCM, RATEOVERRIDE)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(CCM, KSGEN)
NHW_SIDEEFFECTS_SUBSCRIBE_si(CCM, CRYPT)
NHW_SIDEEFFECTS_SUBSCRIBE_si(CCM, STOP)
NHW_SIDEEFFECTS_SUBSCRIBE_si(CCM, RATEOVERRIDE)
#endif /* NHW_HAS_DPPI */

void nhw_ccm_radio_received_packet(bool crc_error) {
  if (!decryption_ongoing) {
    return;
  }
  decryption_ongoing = false;
  nrf_ccm_decrypt_rx(crc_error);
}
