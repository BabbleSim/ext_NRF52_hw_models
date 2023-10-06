/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_ipc.h"
#include "bs_tracing.h"
#include "NHW_IPC.h"

static int ipc_number_from_ptr(NRF_IPC_Type const * p_reg){
  int i = ( (int)p_reg - (int)&NRF_IPC_regs[0] ) / sizeof(NRF_IPC_Type);
  return i;
}

void nrf_ipc_task_trigger(NRF_IPC_Type * p_reg, nrf_ipc_task_t ipc_task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)ipc_task)) = 0x1UL;

  int i = ipc_number_from_ptr(p_reg);
  int task_nbr = (ipc_task - NRF_IPC_TASK_SEND_0)/sizeof(uint32_t);
  nhw_IPC_regw_sideeffects_TASKS_SEND(i, task_nbr);
}

void nrf_ipc_event_clear(NRF_IPC_Type * p_reg, nrf_ipc_event_t ipc_event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)ipc_event)) = 0x0UL;

  int i = ipc_number_from_ptr(p_reg);
  nhw_IPC_regw_sideeffects_EVENTS_all(i);
}

void nrf_ipc_int_enable(NRF_IPC_Type * p_reg, uint32_t mask)
{
  p_reg->INTENSET = mask;

  int i = ipc_number_from_ptr(p_reg);
  nhw_IPC_regw_sideeffects_INTENSET(i);
}

void nrf_ipc_int_disable(NRF_IPC_Type * p_reg, uint32_t mask)
{
  p_reg->INTENCLR = mask;

  int i = ipc_number_from_ptr(p_reg);
  nhw_IPC_regw_sideeffects_INTENCLR(i);
}

#if defined(DPPI_PRESENT)
void nrf_ipc_subscribe_set(NRF_IPC_Type * p_reg,
                           nrf_ipc_task_t task,
                           uint8_t        channel)
{
  NRFX_ASSERT(p_reg);
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
          ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);

  int i = ipc_number_from_ptr(p_reg);
  int task_nbr = (task - NRF_IPC_TASK_SEND_0)/sizeof(uint32_t);

  nhw_IPC_regw_sideeffects_SUBSCRIBE_SEND(i, task_nbr);
}

void nrf_ipc_subscribe_clear(NRF_IPC_Type * p_reg,
                             nrf_ipc_task_t task)
{
  NRFX_ASSERT(p_reg);
  *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;

  int i = ipc_number_from_ptr(p_reg);
  int task_nbr = (task - NRF_IPC_TASK_SEND_0)/sizeof(uint32_t);

  nhw_IPC_regw_sideeffects_SUBSCRIBE_SEND(i, task_nbr);
}
#endif // defined(DPPI_PRESENT)
