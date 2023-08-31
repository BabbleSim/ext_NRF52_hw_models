/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * CCM - AES CCM mode encryption
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ccm.html?cp=5_1_0_5_3
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
 * 4. TASK_RATEOVERRIDE and RATEOVERRIDE are not supported
 */

#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_AES_CCM.h"
#include <string.h>
#include <stdbool.h>
#include "nsi_hw_scheduler.h"
#include "NHW_peri_types.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"

NRF_CCM_Type NRF_CCM_regs;
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_ccm_irq_map[NHW_CCM_TOTAL_INST] = NHW_CCM_INT_MAP;

static uint32_t CCM_INTEN; //interrupt enable
static bool decryption_ongoing;

static void nrf_aes_ccm_init(void) {
  memset(&NRF_CCM_regs, 0, sizeof(NRF_CCM_regs));
  NRF_CCM_regs.MODE = 0x01;
  NRF_CCM_regs.HEADERMASK= 0xE3;
  NRF_CCM_regs.MAXPACKETSIZE = 0xFB;
  CCM_INTEN = 0;
  decryption_ongoing = false;
}

NSI_TASK(nrf_aes_ccm_init, HW_INIT, 100);

void nrf_ccm_TASK_CRYPT(void);
static void signal_EVENTS_ENDCRYPT(void);

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
  uint8_t aad;

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

  signal_EVENTS_ENDCRYPT();
}

static void signal_EVENTS_ENDKSGEN(void) {
  NRF_CCM_regs.EVENTS_ENDKSGEN = 1;
  nrf_ppi_event(CCM_EVENTS_ENDKSGEN);

  int inst = 0;
  if (CCM_INTEN & CCM_INTENSET_ENDKSGEN_Msk) {
    hw_irq_ctrl_set_irq(nhw_ccm_irq_map[inst].cntl_inst,
                         nhw_ccm_irq_map[inst].int_nbr);
  }

  if (NRF_CCM_regs.SHORTS & CCM_SHORTS_ENDKSGEN_CRYPT_Msk) {
    nrf_ccm_TASK_CRYPT();
  }
}

static void signal_EVENTS_ENDCRYPT(void) {
  NRF_CCM_regs.EVENTS_ENDCRYPT = 1;
  nrf_ppi_event(CCM_EVENTS_ENDCRYPT);

  int inst = 0;
  if (CCM_INTEN & CCM_INTENSET_ENDCRYPT_Msk) {
    hw_irq_ctrl_set_irq(nhw_ccm_irq_map[inst].cntl_inst,
                         nhw_ccm_irq_map[inst].int_nbr);
  }
}

/* static void signal_EVENTS_ERROR(){
	NRF_CCM_regs.EVENTS_ERROR = 1;
	NRF_PPI_Event(CCM_EVENTS_ERROR);

	if (CCM_INTEN & CCM_INTENSET_ERROR_Msk) {
		hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
	}
} */

void nrf_ccm_TASK_KSGEN(void) {
  if (NRF_CCM_regs.ENABLE != CCM_ENABLE_ENABLE_Enabled) {
    return;
  }
  /* In this model we cheat and we do it instantly */
  signal_EVENTS_ENDKSGEN();
}

void nrf_ccm_TASK_CRYPT(void) {
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

void nrf_ccm_TASK_STOP(void) {
  decryption_ongoing = false;
}

void nrf_ccm_TASK_RATEOVERRIDE(void) {
  /* We ignore the RATEOVERRIDE task */
  bs_trace_warning_line_time("%s ignored\n", __func__);
}


void nrf_ccm_regw_sideeffects_INTENSET(void) {
  if (NRF_CCM_regs.INTENSET) {
    CCM_INTEN |= NRF_CCM_regs.INTENSET;
    NRF_CCM_regs.INTENSET = CCM_INTEN;
  }
}

void nrf_ccm_regw_sideeffects_INTENCLR(void) {
  if (NRF_CCM_regs.INTENCLR) {
    CCM_INTEN  &= ~NRF_CCM_regs.INTENCLR;
    NRF_CCM_regs.INTENSET = CCM_INTEN;
    NRF_CCM_regs.INTENCLR = 0;
  }
}

void nrf_ccm_regw_sideeffects_TASKS_KSGEN(void) {
  if (NRF_CCM_regs.TASKS_KSGEN) {
    NRF_CCM_regs.TASKS_KSGEN = 0;
    nrf_ccm_TASK_KSGEN();
  }
}

void nrf_ccm_regw_sideeffects_TASKS_CRYPT(void) {
  if (NRF_CCM_regs.TASKS_CRYPT) {
    NRF_CCM_regs.TASKS_CRYPT = 0;
    nrf_ccm_TASK_CRYPT();
  }
}

void nrf_ccm_regw_sideeffects_TASKS_STOP(void) {
  if (NRF_CCM_regs.TASKS_STOP) {
    NRF_CCM_regs.TASKS_STOP = 0;
    nrf_ccm_TASK_STOP();
    bs_trace_warning_line_time("CCM: TASK_STOP functionality is not implemented\n");
  }
}

void nrf_ccm_radio_received_packet(bool crc_error) {
  if (!decryption_ongoing) {
    return;
  }
  decryption_ongoing = false;
  nrf_ccm_decrypt_rx(crc_error);
}
