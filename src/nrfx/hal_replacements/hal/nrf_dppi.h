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

/*#undef NRF_DPPI_ENDPOINT_SETUP
#define NRF_DPPI_ENDPOINT_SETUP(task_or_event, dppi_chan) \
  nrf_dppi_hack_subscribe_set((void *)task_or_event, dppi_chan); */
//For this to work, we would need to change the hack version so it would get a task or event instead of adding here NRF_SUBSCRIBE_PUBLISH_OFFSET

/*#undef NRF_DPPI_ENDPOINT_CLEAR
#define NRF_DPPI_ENDPOINT_CLEAR(task_or_event) \
  nrf_dppi_hack_subscribe_clear((void *)task_or_event);*/
//For this to work, we would need to change the hack version so it would get a task or event instead of adding here NRF_SUBSCRIBE_PUBLISH_OFFSET

#endif /* HAL_REPLACEMENTS_HAL_NRF_DPPI_H */
