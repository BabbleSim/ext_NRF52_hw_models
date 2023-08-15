/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * AES electronic codebook mode encryption
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ecb.html?cp=5_1_0_5_5
 */

#include "NRF_AES_ECB.h"
#include "NRF_PPI.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "nsi_hw_scheduler.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

static bs_time_t Timer_ECB = TIME_NEVER; /* Time when the ECB will finish */

NRF_ECB_Type NRF_ECB_regs;

static uint32_t ECB_INTEN; /* interrupt enable */
static bool ECB_Running;

#define DEFAULT_t_ECB 7
static uint ECB_t_ECB = DEFAULT_t_ECB;

typedef struct {
	uint8_t KEY[16];        /* 16 byte AES key */
	uint8_t CLEARTEXT[16];  /* 16 byte AES cleartext input block */
	uint8_t CIPHERTEXT[16]; /* 16 byte AES ciphertext output block */
} ecbdata_t;

static void nrf_aes_ecb_init(void) {
	memset(&NRF_ECB_regs, 0, sizeof(NRF_ECB_regs));
	Timer_ECB = TIME_NEVER;
	ECB_INTEN = 0;
	ECB_Running = false;
}

NSI_TASK(nrf_aes_ecb_init, HW_INIT, 100);

/*
 * Cheat interface to adjust the time in microseconds it takes
 * for a 16byte AES ECB block to be computed
 */
void nrf_aes_ecb_cheat_set_t_ecb(unsigned int new_t){
	ECB_t_ECB = new_t;
}

/*
 * Cheat interface to reset the time it takes
 * for a 16byte AES ECB block to be computed
 * to the value specified in the infocenter spec.
 */
void nrf_aes_ecb_cheat_reset_t_ecb(void){
	ECB_t_ECB = DEFAULT_t_ECB;
}

static void signal_ENDECB(void) {
	NRF_ECB_regs.EVENTS_ENDECB = 1;
	nrf_ppi_event(ECB_EVENTS_ENDECB);

	if (ECB_INTEN & ECB_INTENSET_ENDECB_Msk){
		hw_irq_ctrl_set_irq(ECB_IRQn);
	}
}

static void signal_ERRORECB(void) {
	NRF_ECB_regs.EVENTS_ERRORECB = 1;
	nrf_ppi_event(ECB_EVENTS_ERRORECB);

	if (ECB_INTEN & ECB_INTENSET_ERRORECB_Msk){
		hw_irq_ctrl_set_irq(ECB_IRQn);
	}
}

void nrf_ecb_TASK_STOPECB(void) {
	if (!ECB_Running) {
		return;
	}

	ECB_Running = false;
	Timer_ECB = TIME_NEVER;
	nsi_hws_find_next_event();
	signal_ERRORECB();
}

void nrf_ecb_TASK_STARTECB(void) {
	ECB_Running = true;
	Timer_ECB = nsi_hws_get_time() + ECB_t_ECB;
	nsi_hws_find_next_event();
}

void nrf_ecb_regw_sideeffects_INTENSET(void) {
	if ( NRF_ECB_regs.INTENSET ) {
		ECB_INTEN |= NRF_ECB_regs.INTENSET;
		NRF_ECB_regs.INTENSET = ECB_INTEN;
	}
}

void nrf_ecb_regw_sideeffects_INTENCLEAR(void) {
	if ( NRF_ECB_regs.INTENCLR ) {
		ECB_INTEN &= ~NRF_ECB_regs.INTENCLR;
		NRF_ECB_regs.INTENSET = ECB_INTEN;
		NRF_ECB_regs.INTENCLR = 0;
	}
}

void nrf_ecb_regw_sideeffects_TASKS_STARTECB(void) {
	if ( NRF_ECB_regs.TASKS_STARTECB ) {
		NRF_ECB_regs.TASKS_STARTECB = 0;
		nrf_ecb_TASK_STARTECB();
	}
}

void nrf_ecb_regw_sideeffects_TASKS_STOPECB(void) {
	if ( NRF_ECB_regs.TASKS_STOPECB ) {
		NRF_ECB_regs.TASKS_STOPECB = 0;
		nrf_ecb_TASK_STOPECB();
	}
}

static void nrf_ecb_timer_triggered(void) {

	ECB_Running = false;
	Timer_ECB = TIME_NEVER;
	nsi_hws_find_next_event();

	ecbdata_t *ecbptr = (ecbdata_t *)NRF_ECB_regs.ECBDATAPTR;

	if (!ecbptr) {
		bs_trace_error_time_line("NRF_ECB_regs.ECBDATAPT is NULL\n");
	} else {
		/* Note all KEY, and data are assumed to be big endian ordered */
		BLECrypt_if_aes_128(
				ecbptr->KEY,
				ecbptr->CLEARTEXT,
				ecbptr->CIPHERTEXT);
		signal_ENDECB();
	}
}

NSI_HW_EVENT(Timer_ECB, nrf_ecb_timer_triggered, 50);
