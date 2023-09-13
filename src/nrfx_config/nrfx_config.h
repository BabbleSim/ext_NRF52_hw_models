/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Provides a minimalistic nrfx_config to be used with Nordic nrfx
 * when not used together with Zephyr.
 */

#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

#ifdef __ZEPHYR__
#error This file should not be included in builds with Zephyr. Use the one provided by the nordic_hal zephyr module in that case.
#endif

#include "nrfx_config_bsim.h"

#ifndef NO_NRFX_TEMPLATE_CONFIG

#include <templates/nrfx_config_common.h>
#if defined(NRF5340_XXAA_APPLICATION)
    #include <templates/nrfx_config_nrf5340_application.h>
#elif defined(NRF5340_XXAA_NETWORK)
    #include <templates/nrfx_config_nrf5340_network.h>
#endif

#endif /* NO_NRFX_TEMPLATE_CONFIG */

#endif // NRFX_CONFIG_H__
