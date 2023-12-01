/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ccm.h"
#include "bs_tracing.h"
#include "NHW_AES_CCM.h"

void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg, nrf_ccm_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  if ( task == NRF_CCM_TASK_KSGEN ) {
    nhw_CCM_regw_sideeffects_TASKS_KSGEN();
  } else if ( task == NRF_CCM_TASK_CRYPT ) {
    nhw_CCM_regw_sideeffects_TASKS_CRYPT();
  } else if ( task == NRF_CCM_TASK_STOP ) {
    nhw_CCM_regw_sideeffects_TASKS_STOP();
  } else {
    bs_trace_error_line_time("Not supported task started in nrf_ccm\n");
  }
}

void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;
  nhw_CCM_regw_sideeffects_INTENSET();
}

void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;
  nhw_CCM_regw_sideeffects_INTENCLR();
}

void nrf_ccm_event_clear(NRF_CCM_Type * p_reg, nrf_ccm_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  nhw_CCM_regw_sideeffects_EVENTS_all(0);
}

#if defined(DPPI_PRESENT)

static void nrf_ccm_subscribe_common(NRF_CCM_Type * p_reg,
                                     nrf_ccm_task_t task)
{
  if (task == NRF_CCM_TASK_KSGEN) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_KSGEN(0);
  } else if ( task == NRF_CCM_TASK_CRYPT ) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_CRYPT(0);
  } else if ( task == NRF_CCM_TASK_STOP ) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_STOP(0);
  } else if ( task == NRF_CCM_TASK_RATEOVERRIDE ) {
      nhw_CCM_regw_sideeffects_SUBSCRIBE_RATEOVERRIDE(0);
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

#endif /* defined(DPPI_PRESENT) */
