/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_GPIO_BACKEND_H
#define _NRF_HW_MODEL_GPIO_BACKEND_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

void nrf_gpio_backend_init(void);
void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value);
void nrf_gpio_backend_write_output_change(unsigned int port, unsigned int n, bool value);

void nrf_gpio_backend_register_short(uint8_t X, uint8_t x, uint8_t Y, uint8_t y);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_GPIO_BACKEND_H */
