/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Provides a mocked nrf.h.
 * This reuses the nrf.h from the MDK, but defines base registers
 * and peripheral addresses to point to mocked peripherals.
 */

#ifndef NRF_BSIM_H__
#define NRF_BSIM_H__

/* Include the real nrf.h */
#include <mdk/nrf.h>

/* For BSIM we require redefining the pointers to the peripherals. */
#include "nrf_bsim_redef.h"

#endif
