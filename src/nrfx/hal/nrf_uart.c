/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdint.h>
#include "hal/nrf_uart.h"
#include "bs_tracing.h"
#include "NHW_UART.h"

static int uart_number_from_ptr(NRF_UART_Type * p_reg){
  int i = ( (int)p_reg - (int)NRF_UARTE_regs ) / sizeof(NRF_UARTE_Type);
  return i;
}

void nrf_uart_task_trigger(NRF_UART_Type * p_reg, nrf_uart_task_t task)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;

  int i = uart_number_from_ptr(p_reg);

#define CASE_TASK(x) \
  case NRF_UART_TASK_##x: nhw_UARTE_regw_sideeffects_TASKS_##x(i); break

  switch (task) {
    CASE_TASK(STARTRX);
    CASE_TASK(STOPRX);
    CASE_TASK(STARTTX);
    CASE_TASK(STOPTX);
    CASE_TASK(SUSPEND);
    default:
      bs_trace_error_line_time("Not supported task started in nrf_clock, %d\n", task);
      break;
  }
#undef CASE_TASK
}

void nrf_uart_int_enable(NRF_UART_Type * p_reg, uint32_t mask)
{
  int i = uart_number_from_ptr(p_reg);
  p_reg->INTENSET = mask;
  nhw_UARTE_regw_sideeffects_INTENSET(i);
}

void nrf_uart_int_disable(NRF_UART_Type * p_reg, uint32_t mask)
{
  int i = uart_number_from_ptr(p_reg);
  p_reg->INTENCLR = mask;
  nhw_UARTE_regw_sideeffects_INTENCLR(i);
}

void nrf_uart_event_clear(NRF_UART_Type * p_reg, nrf_uart_event_t event)
{
  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
  int i = uart_number_from_ptr(p_reg);
  nhw_UARTE_regw_sideeffects_EVENTS_all(i);
}

uint32_t nrf_uart_errorsrc_get_and_clear(NRF_UART_Type * p_reg)
{
  int i = uart_number_from_ptr(p_reg);
  return nhw_UARTE_regr_sideeffects_ERRORSRC(i);
}

uint8_t nrf_uart_rxd_get(NRF_UART_Type const * p_reg)
{
  int i = uart_number_from_ptr((NRF_UART_Type *)p_reg);
  return nhw_UARTE_regr_sideeffects_RXD(i);
}

void nrf_uart_txd_set(NRF_UART_Type * p_reg, uint8_t txd)
{
  p_reg->TXD = txd;
  int i = uart_number_from_ptr(p_reg);
  nhw_UARTE_regw_sideeffects_TXD(i);
}

void nrf_uart_disable(NRF_UART_Type * p_reg)
{
  int i = uart_number_from_ptr(p_reg);
  p_reg->ENABLE = UART_ENABLE_ENABLE_Disabled;
  nhw_UARTE_regw_sideeffects_ENABLE(i);
}

void nrf_uart_enable(NRF_UART_Type * p_reg)
{
  int i = uart_number_from_ptr(p_reg);
  p_reg->ENABLE = UART_ENABLE_ENABLE_Enabled;
  nhw_UARTE_regw_sideeffects_ENABLE(i);
}

void nrf_uart_configure(NRF_UART_Type           * p_reg,
                        nrf_uart_config_t const * p_cfg)
{
  int i = uart_number_from_ptr(p_reg);
  p_reg->CONFIG = (uint32_t)p_cfg->parity
#if defined(UART_CONFIG_STOP_Msk)
                  | (uint32_t)p_cfg->stop
#endif
#if defined(UART_CONFIG_PARITYTYPE_Msk)
                  | (uint32_t)p_cfg->paritytype
#endif
                  | (uint32_t)p_cfg->hwfc;
  nhw_UARTE_regw_sideeffects_CONFIG(i);
}

#if defined(DPPI_PRESENT)

static void nrf_uart_subscribe_common(NRF_UART_Type * p_reg,
                                       nrf_uart_task_t task)
{
  int i = uart_number_from_ptr(p_reg);

#define CASE_TASK(TASKN) \
  case NRF_UART_TASK_##TASKN: nhw_uart_regw_sideeffects_SUBSCRIBE_##TASKN(i); break;

  switch (task) {
    CASE_TASK(STARTRX);
    CASE_TASK(STOPRX);
    CASE_TASK(STARTTX);
    CASE_TASK(STOPTX);
    CASE_TASK(SUSPEND);
    default:
      bs_trace_error_line_time("Attempted to subscribe to a not-supported task in the nrf_uart(%i)\n",
                                task);
      break;
  }
#undef CASE_TASK
}

void nrf_uart_subscribe_set(NRF_UART_Type * p_reg,
                             nrf_uart_task_t task,
                             uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
    nrf_uart_subscribe_common(p_reg, task);
}

void nrf_uart_subscribe_clear(NRF_UART_Type * p_reg,
                               nrf_uart_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
    nrf_uart_subscribe_common(p_reg, task);
}
#endif /* defined(DPPI_PRESENT) */
