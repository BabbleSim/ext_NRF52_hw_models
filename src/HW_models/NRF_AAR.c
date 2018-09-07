/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * AR — Accelerated address resolver
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52840.ps/aar.html?cp=2_0_0_28#frontpage_aar
 */
#include "NRF_AAR.h"
#include <string.h>
#include <stdbool.h>
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "irq_sources.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"

bs_time_t Timer_AAR = TIME_NEVER; /* Time when the AAR will finish */

NRF_AAR_Type NRF_AAR_regs;
static uint32_t AAR_INTEN = 0; //interrupt enable
static bool AAR_Running;
static int matching_irk;

void nrf_aar_init(){
  memset(&NRF_AAR_regs, 0, sizeof(NRF_AAR_regs));
  AAR_INTEN = 0;
  Timer_AAR = TIME_NEVER;
  AAR_Running = false;
}

void nrf_aar_clean_up(){

}

static int nrf_aar_resolve(int *good_irk);

static void signal_EVENTS_END(){
  NRF_AAR_regs.EVENTS_END = 1;
  nrf_ppi_event(AAR_EVENTS_END);

  if (AAR_INTEN & AAR_INTENSET_END_Msk){
    hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
  }
}

static void signal_EVENTS_RESOLVED(){
  NRF_AAR_regs.EVENTS_RESOLVED = 1;
  nrf_ppi_event(AAR_EVENTS_RESOLVED);

  if (AAR_INTEN & AAR_INTENCLR_RESOLVED_Msk){
    hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
  }
}

static void signal_EVENTS_NOTRESOLVED(){
  NRF_AAR_regs.EVENTS_NOTRESOLVED = 1;
  nrf_ppi_event(AAR_EVENTS_NOTRESOLVED);

  if (AAR_INTEN & AAR_INTENCLR_NOTRESOLVED_Msk){
    hw_irq_ctrl_set_irq(NRF5_IRQ_CCM_AAR_IRQn);
  }
}

void nrf_aar_TASK_START(){
  int n_irks;

  if (NRF_AAR_regs.ENABLE != 0x3) {
    return;
  }

  AAR_Running = true;
  n_irks = nrf_aar_resolve(&matching_irk);

  Timer_AAR = tm_get_hw_time() + 1 + 6 * n_irks; /*AAR delay*/
  nrf_hw_find_next_timer_to_trigger();
}

void nrf_aar_TASK_STOP(){
  if (!AAR_Running) {
    return;
  }

  AAR_Running = false;
  Timer_AAR = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();
  signal_EVENTS_END();
  //Does this actually signal an END?
  //and only an END?
}

void nrf_aar_regw_sideeffects_INTENSET(){
  if ( NRF_AAR_regs.INTENSET ){
    AAR_INTEN |= NRF_AAR_regs.INTENSET;
    NRF_AAR_regs.INTENSET = AAR_INTEN;
  }
}

void nrf_aar_regw_sideeffects_INTENCLR(){
  if ( NRF_AAR_regs.INTENCLR ){
    AAR_INTEN  &= ~NRF_AAR_regs.INTENCLR;
    NRF_AAR_regs.INTENSET = AAR_INTEN;
    NRF_AAR_regs.INTENCLR = 0;
  }
}

void nrf_aar_regw_sideeffects_TASKS_START(){
  if ( NRF_AAR_regs.TASKS_START ) {
    NRF_AAR_regs.TASKS_START = 0;
    nrf_aar_TASK_START();
  }
}

void nrf_aar_regw_sideeffects_TASKS_STOP(){
  if ( NRF_AAR_regs.TASKS_STOP ) {
    NRF_AAR_regs.TASKS_STOP = 0;
    nrf_aar_TASK_STOP();
  }
}

void nrf_aar_timer_triggered(){
  AAR_Running = false;
  Timer_AAR = TIME_NEVER;
  nrf_hw_find_next_timer_to_trigger();

  if (matching_irk != -1) {
    NRF_AAR_regs.STATUS = matching_irk;
    signal_EVENTS_RESOLVED();
  } else {
    signal_EVENTS_NOTRESOLVED();
  }
  signal_EVENTS_END();
}

/**
 * Try to resolve the address
 * Returns the number of IRKs it went thru before matching
 * (or if it did not, it returns NRF_AAR_regs.NIRK)
 *
 * It sets *good_irk to the index of the IRK that matched
 * or to -1 if none did.
 */
static int nrf_aar_resolve(int *good_irk) {
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

  prand = *(uint32_t*)(address_ptr+3) & 0xFFFFFF;
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

    hash = *(uint32_t*)address_ptr & 0xFFFFFF;

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
