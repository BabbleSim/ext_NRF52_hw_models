/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "NRF_AES_ECB.h"
#include "NRF_PPI.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"

/*
 * AES electronic codebook mode encryption
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/ecb.html?cp=2_0_0_26#concept_vnj_kgy_xr
 */

bs_time_t Timer_ECB = TIME_NEVER; /* Time when the ECB will finish */

NRF_ECB_Type NRF_ECB_regs;

static uint32_t ECB_INTEN; /* interrupt enable */
static bool ECB_Running;

typedef struct {
	uint8_t KEY[16];        /* 16 byte AES key */
	uint8_t CLEARTEXT[16];  /* 16 byte AES cleartext input block */
	uint8_t CIPHERTEXT[16]; /* 16 byte AES ciphertext output block */
} ecbdata_t;

void nrf_aes_ecb_init(){
	memset(&NRF_ECB_regs, 0, sizeof(NRF_ECB_regs));
	Timer_ECB = TIME_NEVER;
	ECB_INTEN = 0;
	ECB_Running = false;
}

void nrf_aes_ecb_clean_up(){

}

static void signal_ENDECB(){
	NRF_ECB_regs.EVENTS_ENDECB = 1;
	nrf_ppi_event(ECB_EVENTS_ENDECB);

	if (ECB_INTEN & ECB_INTENSET_ENDECB_Msk){
		hw_irq_ctrl_set_irq(NRF5_IRQ_ECB_IRQn);
	}
}

static void signal_ERRORECB(){
	NRF_ECB_regs.EVENTS_ERRORECB = 1;
	nrf_ppi_event(ECB_EVENTS_ERRORECB);

	if (ECB_INTEN & ECB_INTENSET_ERRORECB_Msk){
		hw_irq_ctrl_set_irq(NRF5_IRQ_ECB_IRQn);
	}
}

void nrf_ecb_TASK_STOPECB(){
	if (!ECB_Running) {
		return;
	}

	ECB_Running = false;
	Timer_ECB = TIME_NEVER;
	nrf_hw_find_next_timer_to_trigger();
	signal_ERRORECB();
}

void nrf_ecb_TASK_STARTECB(){
	ECB_Running = true;
	Timer_ECB = tm_get_hw_time() + 7 /*ECB delay*/;
	nrf_hw_find_next_timer_to_trigger();
}

void nrf_ecb_regw_sideeffects_INTENSET(){
	if ( NRF_ECB_regs.INTENSET ) {
		ECB_INTEN |= NRF_ECB_regs.INTENSET;
		NRF_ECB_regs.INTENSET = ECB_INTEN;
	}
}

void nrf_ecb_regw_sideeffects_INTENCLEAR(){
	if ( NRF_ECB_regs.INTENCLR ) {
		ECB_INTEN &= ~NRF_ECB_regs.INTENCLR;
		NRF_ECB_regs.INTENSET = ECB_INTEN;
		NRF_ECB_regs.INTENCLR = 0;
	}
}

void nrf_ecb_regw_sideeffects_TASKS_STARTECB(){
	if ( NRF_ECB_regs.TASKS_STARTECB ) {
		NRF_ECB_regs.TASKS_STARTECB = 0;
		nrf_ecb_TASK_STARTECB();
	}
}

void nrf_ecb_regw_sideeffects_TASKS_STOPECB(){
	if ( NRF_ECB_regs.TASKS_STOPECB ) {
		NRF_ECB_regs.TASKS_STOPECB = 0;
		nrf_ecb_TASK_STOPECB();
	}
}

void nrf_ecb_timer_triggered(){

	ECB_Running = false;
	Timer_ECB = TIME_NEVER;
	nrf_hw_find_next_timer_to_trigger();

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
