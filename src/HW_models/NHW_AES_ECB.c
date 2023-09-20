/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * AES electronic codebook mode encryption
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/ecb.html?cp=5_1_0_5_5
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/ecb.html?cp=4_0_0_6_9
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "NHW_common_types.h"
#include "NHW_templates.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_AES_ECB.h"
#include "NHW_xPPI.h"
#include "nsi_hw_scheduler.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

#if NHW_ECB_TOTAL_INST > 1
#error "This model only supports 1 instance so far"
#endif

static bs_time_t Timer_ECB = TIME_NEVER; /* Time when the ECB will finish */

NRF_ECB_Type NRF_ECB_regs;

#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_ECB_dppi_map[NHW_ECB_TOTAL_INST] = NHW_ECB_DPPI_MAP;
#endif

static uint32_t ECB_INTEN; /* interrupt enable */
static bool ECB_Running;

static uint ECB_t_ECB = NHW_ECB_t_ECB;

typedef struct {
  uint8_t KEY[16];        /* 16 byte AES key */
  uint8_t CLEARTEXT[16];  /* 16 byte AES cleartext input block */
  uint8_t CIPHERTEXT[16]; /* 16 byte AES ciphertext output block */
} ecbdata_t;

static void nhw_aes_ecb_init(void) {
  memset(&NRF_ECB_regs, 0, sizeof(NRF_ECB_regs));
  Timer_ECB = TIME_NEVER;
  ECB_INTEN = 0;
  ECB_Running = false;
}

NSI_TASK(nhw_aes_ecb_init, HW_INIT, 100);

/*
 * Cheat interface to adjust the time in microseconds it takes
 * for a 16byte AES ECB block to be computed
 */
void nhw_aes_ecb_cheat_set_t_ecb(unsigned int new_t){
  ECB_t_ECB = new_t;
}
void nrf_aes_ecb_cheat_set_t_ecb(unsigned int new_t) {
  nhw_aes_ecb_cheat_set_t_ecb(new_t);
}

/*
 * Cheat interface to reset the time it takes
 * for a 16byte AES ECB block to be computed
 * to the value specified in the infocenter spec.
 */
void nhw_aes_ecb_cheat_reset_t_ecb(void){
  ECB_t_ECB = NHW_ECB_t_ECB;
}
void nrf_aes_ecb_cheat_reset_t_ecb(void){
  nhw_aes_ecb_cheat_reset_t_ecb();
}

static void nhw_ECB_eval_interrupt(uint inst) {
  static bool ecb_int_line[NHW_ECB_TOTAL_INST]; /* Is the ECB currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_ecb_irq_map[NHW_ECB_TOTAL_INST] = NHW_ECB_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(ECB, ENDECB, ECB_INTEN)
  NHW_CHECK_INTERRUPT_si(ECB, ERRORECB, ECB_INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&ecb_int_line[inst],
                                       new_int_line,
                                       &nhw_ecb_irq_map[inst]);
}

static NHW_SIGNAL_EVENT_si(ECB, ENDECB)
static NHW_SIGNAL_EVENT_si(ECB, ERRORECB)

void nhw_ECB_TASK_STOPECB(void) {
  if (!ECB_Running) {
    return;
  }

  ECB_Running = false;
  Timer_ECB = TIME_NEVER;
  nsi_hws_find_next_event();
  nhw_ECB_signal_EVENTS_ERRORECB(0);
}

void nhw_ECB_TASK_STARTECB(void) {
  ECB_Running = true;
  Timer_ECB = nsi_hws_get_time() + ECB_t_ECB;
  nsi_hws_find_next_event();
}

NHW_SIDEEFFECTS_INTSET_si(ECB, NRF_ECB_regs., ECB_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(ECB, NRF_ECB_regs., ECB_INTEN)

NHW_SIDEEFFECTS_EVENTS(ECB)

NHW_SIDEEFFECTS_TASKS_si(ECB, STARTECB)
NHW_SIDEEFFECTS_TASKS_si(ECB, STOPECB)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(ECB, STARTECB)
NHW_SIDEEFFECTS_SUBSCRIBE_si(ECB, STOPECB)
#endif /* NHW_HAS_DPPI */

static void nhw_ecb_timer_triggered(void) {

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
    nhw_ECB_signal_EVENTS_ENDECB(0);
  }
}

NSI_HW_EVENT(Timer_ECB, nhw_ecb_timer_triggered, 50);
