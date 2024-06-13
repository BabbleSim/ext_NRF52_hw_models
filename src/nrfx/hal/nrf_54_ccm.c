/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ccm.h"
#include "bs_tracing.h"
#include "NHW_54_AAR_CCM_ECB.h"

static int ccm_inst_from_ptr(NRF_CCM_Type * p_reg)
{
  union NRF_AARCCM_regs *p = (union NRF_AARCCM_regs *)p_reg;

  int i = ( (int)p - (int)NRF_AARCCM_regs ) / sizeof(union NRF_AARCCM_Type);
  return i;
}

void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg, nrf_ccm_task_t task)
{
  int inst = ccm_inst_from_ptr(p_reg);
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  if (task == NRF_CCM_TASK_START) {
    nhw_CCM_regw_sideeffects_TASKS_START(inst);
  } else if (task == NRF_CCM_TASK_STOP) {
    nhw_CCM_regw_sideeffects_TASKS_STOP(inst);
  } else if (task == NRF_CCM_TASK_RATEOVERRIDE) {
    nhw_CCM_regw_sideeffects_TASKS_RATEOVERRIDE(inst);
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ccm\n");
  }
}

void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  int inst = ccm_inst_from_ptr(p_reg);
  p_reg->INTENSET = mask;
  nhw_CCM_regw_sideeffects_INTENSET(inst);
}

void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  int inst = ccm_inst_from_ptr(p_reg);
  p_reg->INTENCLR = mask;
  nhw_CCM_regw_sideeffects_INTENCLR(inst);
}

void nrf_ccm_enable(NRF_CCM_Type * p_reg)
{
  int inst = ccm_inst_from_ptr(p_reg);
  p_reg->ENABLE = CCM_ENABLE_ENABLE_Enabled << CCM_ENABLE_ENABLE_Pos;
  nhw_AARCCM_regw_sideeffects_ENABLE(inst);
}

void nrf_ccm_disable(NRF_CCM_Type * p_reg)
{
  int inst = ccm_inst_from_ptr(p_reg);
  p_reg->ENABLE = (CCM_ENABLE_ENABLE_Disabled << CCM_ENABLE_ENABLE_Pos);
  nhw_AARCCM_regw_sideeffects_ENABLE(inst);
}

void nrf_ccm_event_clear(NRF_CCM_Type * p_reg, nrf_ccm_event_t event)
{
  int inst = ccm_inst_from_ptr(p_reg);
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_CCM_regw_sideeffects_EVENTS_all(inst);
}

static void nrf_ccm_subscribe_common(NRF_CCM_Type * p_reg,
                                     nrf_ccm_task_t task)
{
  int inst = ccm_inst_from_ptr(p_reg);

  if (task == NRF_CCM_TASK_START) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_START(inst);
  } else if (task == NRF_CCM_TASK_STOP) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_STOP(inst);
  } else if (task == NRF_CCM_TASK_RATEOVERRIDE) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_RATEOVERRIDE(inst);
  } else {
      bs_trace_error_line_time("Attempted to subscribe to an not-supported task in the nrf_ccm (%i)\n",
                               task);
  }
}

void nrf_ccm_subscribe_set(NRF_CCM_Type * p_reg,
                           nrf_ccm_task_t task,
                           uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_ccm_subscribe_common(p_reg, task);
}

void nrf_ccm_subscribe_clear(NRF_CCM_Type * p_reg,
                             nrf_ccm_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_ccm_subscribe_common(p_reg, task);
}
