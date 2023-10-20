/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Provides a minimalistic nrfx_glue to be used with Nordic nrfx
 * when not used together with Zephyr.
 */

#ifndef NRFX_GLUE_H__
#define NRFX_GLUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ZEPHYR__
#error This file should not be included in builds with Zephyr. Use the one provided by the nordic_hal zephyr module in that case.
#endif

#ifndef NRFX_DELAY_US
#define NRFX_DELAY_US(x)
#endif

#include "nrfx_glue_bsim.h"

#ifdef __cplusplus
}
#endif

#endif // NRFX_GLUE_H__
