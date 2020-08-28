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

#endif // NRFX_CONFIG_H__

