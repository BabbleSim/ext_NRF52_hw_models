/*
 * Copyright (c) 2024, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ppib.h"
#include "NHW_PPIB.h"
#include "bs_tracing.h"

static int ppib_number_from_ptr(NRF_PPIB_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_PPIB_regs[0] ) / sizeof(NRF_PPIB_Type);
  return i;
}

/* Note: The PPIB task and events registers are not functional,
 * and therefore have no HAL API */

void nrf_ppib_subscribe_set(NRF_PPIB_Type * p_reg,
                            nrf_ppib_task_t task,
                            uint8_t        channel)
{
  NRFX_ASSERT(p_reg);
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
          ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);

  int i = ppib_number_from_ptr(p_reg);
  int task_nbr = (task - NRF_PPIB_TASK_SEND_0)/sizeof(uint32_t);

  nhw_PPIB_regw_sideeffects_SUBSCRIBE_SEND(i, task_nbr);
}

void nrf_ppib_subscribe_clear(NRF_PPIB_Type * p_reg,
                              nrf_ppib_task_t task)
{
  NRFX_ASSERT(p_reg);
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;

  int i = ppib_number_from_ptr(p_reg);
  int task_nbr = (task - NRF_PPIB_TASK_SEND_0)/sizeof(uint32_t);

  nhw_PPIB_regw_sideeffects_SUBSCRIBE_SEND(i, task_nbr);
}
