/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * AAR - Accelerated address resolver
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/aar.html?cp=4_1_0_5_1
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/aar.html?cp=4_0_0_6_3
 *
 * Notes:
 *  * Unlike in the real HW the AAR peripheral does not share resources with the CCM or ECB peripherals.
 *    They are actually 2 separate peripherals, so if they are used simultaneously nothing will fail.
 *    Therefore
 *    * Starting the AAR block while the ECB block is running will not abort the ECB and
 *      cause a ERRORECB
 *    * The AAR register map (including interrupt mask, enable, & task start) does not
 *      overlap the CCM one.
 *    * The AAR block could even be used in parallel to the CCM block without conflicts.
 *  * IMPORTANT: This may change in the future. No embedded SW or tests may rely on this,
 *    but instead they should behave like they would with real HW.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "NHW_config.h"
#include "NHW_common_types.h"
#include "NHW_templates.h"
#include "NHW_AAR.h"
#include "nsi_hw_scheduler.h"
#include "NHW_peri_types.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

#if NHW_AAR_TOTAL_INST > 1
#error "This model only supports 1 instance so far"
#endif

static bs_time_t Timer_AAR = TIME_NEVER; /* Time when the AAR will finish */

NRF_AAR_Type NRF_AAR_regs;
#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_AAR_dppi_map[NHW_AAR_TOTAL_INST] = NHW_AAR_DPPI_MAP;
#endif

static uint32_t AAR_INTEN = 0; //interrupt enable
static bool AAR_Running;
static int matching_irk;

static void nhw_aar_init(void) {
  memset(&NRF_AAR_regs, 0, sizeof(NRF_AAR_regs));
  AAR_INTEN = 0;
  Timer_AAR = TIME_NEVER;
  AAR_Running = false;
}

NSI_TASK(nhw_aar_init, HW_INIT, 100);

static int nhw_aar_resolve(int *good_irk);

static void nhw_AAR_eval_interrupt(uint inst) {
  static bool aar_int_line[NHW_AAR_TOTAL_INST]; /* Is the AAR currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_aar_irq_map[NHW_AAR_TOTAL_INST] = NHW_AAR_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(AAR, END, AAR_INTEN)
  NHW_CHECK_INTERRUPT_si(AAR, RESOLVED, AAR_INTEN)
  NHW_CHECK_INTERRUPT_si(AAR, NOTRESOLVED, AAR_INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&aar_int_line[inst],
                                       new_int_line,
                                       &nhw_aar_irq_map[inst]);
}

static NHW_SIGNAL_EVENT_si(AAR, END)
static NHW_SIGNAL_EVENT_si(AAR, RESOLVED)
static NHW_SIGNAL_EVENT_si(AAR, NOTRESOLVED)

void nhw_AAR_TASK_START(void) {
  int n_irks;

  if (NRF_AAR_regs.ENABLE != 0x3) {
    return;
  }

  AAR_Running = true;
  n_irks = nhw_aar_resolve(&matching_irk);

  Timer_AAR = nsi_hws_get_time() + 1 + NHW_AAR_t_AAR * n_irks; /*AAR delay*/
  nsi_hws_find_next_event();
}

void nhw_AAR_TASK_STOP(void) {
  if (!AAR_Running) {
    return;
  }

  AAR_Running = false;
  Timer_AAR = TIME_NEVER;
  nsi_hws_find_next_event();
  nhw_AAR_signal_EVENTS_END(0);
  //Does this actually signal an END?
  //and only an END?
}

NHW_SIDEEFFECTS_INTSET_si(AAR, NRF_AAR_regs., AAR_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(AAR, NRF_AAR_regs., AAR_INTEN)

NHW_SIDEEFFECTS_EVENTS(AAR)

NHW_SIDEEFFECTS_TASKS_si(AAR, START)
NHW_SIDEEFFECTS_TASKS_si(AAR, STOP)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(AAR, START)
NHW_SIDEEFFECTS_SUBSCRIBE_si(AAR, STOP)
#endif /* NHW_HAS_DPPI */

static void nhw_aar_timer_triggered(void) {
  AAR_Running = false;
  Timer_AAR = TIME_NEVER;
  nsi_hws_find_next_event();

  if (matching_irk != -1) {
    NRF_AAR_regs.STATUS = matching_irk;
    nhw_AAR_signal_EVENTS_RESOLVED(0);
  } else {
    nhw_AAR_signal_EVENTS_NOTRESOLVED(0);
  }
  nhw_AAR_signal_EVENTS_END(0);
}

NSI_HW_EVENT(Timer_AAR, nhw_aar_timer_triggered, 50);

static inline uint32_t read_3_bytes_value(const uint8_t *ptr) {
  uint32_t value = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16);
  return value;
}

/**
 * Try to resolve the address
 * Returns the number of IRKs it went thru before matching
 * (or if it did not, it returns NRF_AAR_regs.NIRK)
 *
 * It sets *good_irk to the index of the IRK that matched
 * or to -1 if none did.
 */
static int nhw_aar_resolve(int *good_irk) {
  int i;
  uint8_t prand_buf[16];
  uint8_t hash_check_buf[16];
  uint32_t hash, hash_check;
  uint32_t prand;
  const uint8_t *irkptr;
  /*
   * The AAR module always assumes the S0+Length+S1 occupy 3 bytes
   * independently of the RADIO config
   */
  uint8_t *address_ptr = (uint8_t*)NRF_AAR_regs.ADDRPTR + 3;

  *good_irk = -1;

  bs_trace_raw_time(9,"HW AAR address to match %02x:%02x:%02x:%02x:%02x:%02x\n",
      address_ptr[5], address_ptr[4], address_ptr[3],
      address_ptr[2], address_ptr[1], address_ptr[0]);

  prand = read_3_bytes_value(address_ptr+3);
  if (prand >> 22 != 0x01){
    /* Not a resolvable private address */
    bs_trace_raw_time(7,"HW AAR the address is not resolvable (0x%06X , %x)\n", prand, prand >> 22);
    return NRF_AAR_regs.NIRK;
  }

  memset(prand_buf,0,16);

  /* Endiannes reversal to bigendian */
  prand_buf[15] = prand & 0xFF;
  prand_buf[14] = (prand >> 8) & 0xFF;
  prand_buf[13] = (prand >> 16) & 0xFF;

  hash = read_3_bytes_value(address_ptr);

  for (i = 0 ; i < NRF_AAR_regs.NIRK; i++){
    /* The provided IRKs are assumed to be already big endian */
    irkptr = ((const uint8_t*)NRF_AAR_regs.IRKPTR) + 16*i;

    /* this aes_128 function takes and produces big endian results */
    BLECrypt_if_aes_128(
        irkptr,
        prand_buf,
        hash_check_buf);

    /* Endianess reversal to little endian */
    hash_check = hash_check_buf[15] | (uint32_t)hash_check_buf[14] << 8 | (uint32_t)hash_check_buf[13] << 16;

    bs_trace_raw_time(9,"HW AAR (%i): checking prand = 0x%06X, hash = 0x%06X, hashcheck = 0x%06X\n",i, prand, hash, hash_check);

    if (hash == hash_check) {
      bs_trace_raw_time(7,"HW AAR matched irk %i (of %i)\n",i, NRF_AAR_regs.NIRK);
      *good_irk = i;
      return i+1;
    }
  }

  bs_trace_raw_time(7,"HW AAR did not match any IRK of %i\n", NRF_AAR_regs.NIRK);
  return i;
}
