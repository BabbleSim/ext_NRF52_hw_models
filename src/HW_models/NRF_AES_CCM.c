/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CCM — AES CCM mode encryption
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/ccm.html?cp=2_0_0_27#topic
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
 */

#include "NRF_AES_CCM.h"
#include <string.h>
#include <stdbool.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"

NRF_CCM_Type NRF_CCM_regs;
static uint32_t CCM_INTEN; //interrupt enable
static bool decryption_ongoing;

void nrf_aes_ccm_init(){
  memset(&NRF_CCM_regs, 0, sizeof(NRF_CCM_regs));
  CCM_INTEN = 0;
  decryption_ongoing = false;
}

void nrf_aes_ccm_clean_up(){

}


void nrf_ccm_TASK_CRYPT ();
static void signal_EVENTS_ENDCRYPT();

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


static void nrf_ccm_encrypt_tx() {
  const uint8_t* cnfptr;
  const uint8_t* sk;
  const uint8_t* iv;
  uint64_t tx_pkt_ctr;
  uint8_t pkt_direction;
  const uint8_t* inptr;
  uint8_t* outptr;
  int length;
  uint8_t ccm_nonce[NONCE_LEN];

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
  /* Note that outptr[2] is reserverd for S1 in the HW (but unused) */

  nonce_calc(iv, tx_pkt_ctr, pkt_direction, ccm_nonce);

  BLECrypt_if_encrypt_packet(inptr[0], // First byte of packet header
      (uint8_t*)&inptr[3],      // Packet payload to be encrypted
      &outptr[3],  //encrypted payload (and MIC if generate_mic==1)
      length,      //including MIC length if ( generate_mic == 1 ) ; [ just  the length in the output packet header ]
      true, //we have MIC, or not
      sk, // Session key (16 bytes, BIG-ENDIAN)
      ccm_nonce
  );

  signal_EVENTS_ENDCRYPT();
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

  if (crc_error) {
    NRF_CCM_regs.MICSTATUS = 0;
    signal_EVENTS_ENDCRYPT();
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
  /* Note that outptr[2] is reserverd for S1 in the HW (but unused) */

  nonce_calc(iv, rx_pkt_ctr, pkt_direction, ccm_nonce);

  BLECrypt_if_decrypt_packet(inptr[0], // First byte of packet header
      (uint8_t*)&inptr[3], //as received from the air (including a MIC if has_mic)
      &outptr[3], //buffer for decrypted payload
      inptr[1],   //including MIC lenght if (has_mic == 1) ; [ just  the length in the packet header ]
      true, //we have MIC, or not
      sk, // Session key (16 bytes, BIG-ENDIAN)
      ccm_nonce,
      &mic_error
  );

  NRF_CCM_regs.MICSTATUS = !mic_error;

  signal_EVENTS_ENDCRYPT();
}

static void signal_EVENTS_ENDKSGEN() {
  NRF_CCM_regs.EVENTS_ENDKSGEN = 1;
  nrf_ppi_event(CCM_EVENTS_ENDKSGEN);

  if (CCM_INTEN & CCM_INTENSET_ENDKSGEN_Msk) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
  }

  if (NRF_CCM_regs.SHORTS & CCM_SHORTS_ENDKSGEN_CRYPT_Msk) {
    nrf_ccm_TASK_CRYPT();
  }
}

static void signal_EVENTS_ENDCRYPT(){
  NRF_CCM_regs.EVENTS_ENDCRYPT = 1;
  nrf_ppi_event(CCM_EVENTS_ENDCRYPT);

  if (CCM_INTEN & CCM_INTENSET_ENDCRYPT_Msk) {
    hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
  }
}

/* static void signal_EVENTS_ERROR(){
	NRF_CCM_regs.EVENTS_ERROR = 1;
	NRF_PPI_Event(CCM_EVENTS_ERROR);

	if (CCM_INTEN & CCM_INTENSET_ERROR_Msk) {
		hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
	}
} */

void nrf_ccm_TASK_KSGEN() {
  if (NRF_CCM_regs.ENABLE != CCM_ENABLE_ENABLE_Enabled) {
    return;
  }
  /* In this model we cheat and we do it instantly */
  signal_EVENTS_ENDKSGEN();
}

void nrf_ccm_TASK_CRYPT() {
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

void nrf_ccm_TASK_STOP() {
  decryption_ongoing = false;
}

void nrf_ccm_TASK_RATEOVERRIDE() {
  /* We ignore the RATEOVERRIDE task */
  bs_trace_warning_line_time("%s ignored\n", __func__);
}


void nrf_ccm_regw_sideeffects_INTENSET(){
  if (NRF_CCM_regs.INTENSET) {
    CCM_INTEN |= NRF_CCM_regs.INTENSET;
    NRF_CCM_regs.INTENSET = CCM_INTEN;
  }
}

void nrf_ccm_regw_sideeffects_INTENCLR(){
  if (NRF_CCM_regs.INTENCLR) {
    CCM_INTEN  &= ~NRF_CCM_regs.INTENCLR;
    NRF_CCM_regs.INTENSET = CCM_INTEN;
    NRF_CCM_regs.INTENCLR = 0;
  }
}

void nrf_ccm_regw_sideeffects_TASKS_KSGEN(){
  if (NRF_CCM_regs.TASKS_KSGEN) {
    NRF_CCM_regs.TASKS_KSGEN = 0;
    nrf_ccm_TASK_KSGEN();
  }
}


void nrf_ccm_radio_received_packet(bool crc_error) {
  if (!decryption_ongoing) {
    return;
  }
  decryption_ongoing = false;
  nrf_ccm_decrypt_rx(crc_error);
}
