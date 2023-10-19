/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The integrator should ensure this header is in
 * the include path before the real hal/nrf_dppi.h
 */

#ifndef HAL_REPLACEMENTS_HAL_NRF_DPPI_H
#define HAL_REPLACEMENTS_HAL_NRF_DPPI_H

#include_next "hal/nrf_dppi.h"

/*
 * Special API for the HW models, which allows setting or clearing
 * any subscribe register based on a pointer to the register
 *
 * These calls are experimental and may change at any point
 */
void nrf_dppi_hack_subscribe_set(void *sub_reg, unsigned int channel);
void nrf_dppi_hack_subscribe_clear(void *sub_reg);

#undef NRF_DPPI_ENDPOINT_SETUP
#define NRF_DPPI_ENDPOINT_SETUP(task_or_event, dppi_chan)                            \
  do {                                                                               \
    uint32_t tmp = task_or_event + NRF_SUBSCRIBE_PUBLISH_OFFSET(task_or_event);      \
    *(volatile uint32_t *)tmp = ((uint32_t)dppi_chan | NRF_SUBSCRIBE_PUBLISH_ENABLE);\
    nrf_dppi_hack_subscribe_set((void *)tmp, dppi_chan);                             \
  } while(0)

#undef NRF_DPPI_ENDPOINT_CLEAR
#define NRF_DPPI_ENDPOINT_CLEAR(task_or_event)                                  \
  do {                                                                          \
    uint32_t tmp = task_or_event + NRF_SUBSCRIBE_PUBLISH_OFFSET(task_or_event); \
    *(volatile uint32_t *)tmp = 0;                                              \
    nrf_dppi_hack_subscribe_clear((void *)tmp);                                 \
  } while(0)

#endif /* HAL_REPLACEMENTS_HAL_NRF_DPPI_H */
