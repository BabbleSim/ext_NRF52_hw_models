/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ecb.h"
#include "bs_tracing.h"
#include "NHW_54_AAR_CCM_ECB.h"

static int ecb_inst_from_ptr(NRF_ECB_Type * p_reg)
{
  int i = ( (int)p_reg - (int)NRF_ECB_regs ) / sizeof(NRF_ECB_Type);
  return i;
}

void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task)
{
  int inst = ecb_inst_from_ptr(p_reg);

  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  if (task == NRF_ECB_TASK_START) {
    nhw_ECB_regw_sideeffects_TASKS_START(inst);
  } else if (task == NRF_ECB_TASK_STOP) {
    nhw_ECB_regw_sideeffects_TASKS_STOP(inst);
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ecb\n");
  }
}

void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  int inst = ecb_inst_from_ptr(p_reg);
  p_reg->INTENSET = mask;
  nhw_ECB_regw_sideeffects_INTENSET(inst);
}

void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask)
{
  int inst = ecb_inst_from_ptr(p_reg);
  p_reg->INTENCLR = mask;
  nhw_ECB_regw_sideeffects_INTENCLR(inst);
}

void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event)
{
  int inst = ecb_inst_from_ptr(p_reg);
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_ECB_regw_sideeffects_EVENTS_all(inst);
}

static void nrf_ecb_subscribe_common(NRF_ECB_Type * p_reg,
                                     nrf_ecb_task_t task)
{
  int inst = ecb_inst_from_ptr(p_reg);

  if (task == NRF_ECB_TASK_START) {
      nhw_ECB_regw_sideeffects_SUBSCRIBE_START(inst);
  } else if (task == NRF_ECB_TASK_STOP) {
      nhw_ECB_regw_sideeffects_SUBSCRIBE_STOP(inst);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to a not-supported task in the nrf_ecb (%i)\n",
                               task);
  }
}

void nrf_ecb_subscribe_set(NRF_ECB_Type * p_reg,
                           nrf_ecb_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_ecb_subscribe_common(p_reg, task);
}

void nrf_ecb_subscribe_clear(NRF_ECB_Type * p_reg,
                             nrf_ecb_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_ecb_subscribe_common(p_reg, task);
}
