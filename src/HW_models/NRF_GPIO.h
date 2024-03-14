/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_GPIO_H
#define _NRF_HW_MODEL_GPIO_H

#include "NRF_GPIO_backend.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

#define NRF_GPIOS 2
#define NRF_GPIO_MAX_PINS_PER_PORT 32
#define NRF_GPIO_PORTS_PINS {32, 10} /* Number of IOs per port */

/*
 * Callback another peripheral or tests can register if it wants to be called when an
 * input/output is toggled.
 *
 * The callback will get as inputs:
 *  * port: the GPIO port which toggled,
 *  * n: the pin in the port
 *  * value: the new value (true: high, false: low)
 */
typedef void (*nrf_gpio_input_callback_t)(unsigned int port, unsigned int n, bool value);


unsigned int nrf_gpio_get_number_pins_in_port(int port);

void nrf_gpio_test_register_in_callback(nrf_gpio_input_callback_t fptr);
void nrf_gpio_test_register_out_callback(nrf_gpio_input_callback_t fptr);
void nrf_gpio_test_change_pin_level(unsigned int port, unsigned int n, bool value);
bool nrf_gpio_get_pin_level(unsigned int port, unsigned int n);

void nrf_gpio_peri_pin_control(unsigned int port, unsigned int n,
			      int override_output, int override_input, int override_dir,
			      nrf_gpio_input_callback_t fptr, int new_level);
void nrf_gpio_peri_change_output(unsigned int port, unsigned int n, bool value);

bool nrf_gpio_get_detect_level(unsigned int port);

void nrf_gpio_regw_sideeffects_OUT(unsigned int port);
void nrf_gpio_regw_sideeffects_OUTSET(unsigned int port);
void nrf_gpio_regw_sideeffects_OUTCLR(unsigned int port);
void nrf_gpio_regw_sideeffects_DIR(unsigned int port);
void nrf_gpio_regw_sideeffects_DIRSET(unsigned int port);
void nrf_gpio_regw_sideeffects_DIRCLR(unsigned int port);
void nrf_gpio_regw_sideeffects_LATCH(unsigned int port);
void nrf_gpio_regw_sideeffects_DETECTMODE(unsigned int port);
void nrf_gpio_regw_sideeffects_PIN_CNF(unsigned int port,unsigned  int n);

void nrf_gpio_eval_input(unsigned int port, unsigned int n, bool value);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_GPIO_H */
