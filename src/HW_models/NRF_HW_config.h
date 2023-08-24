/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file configures the HW models based on variant being built
 */
#ifndef _NRF_HW_CONFIG_H
#define _NRF_HW_CONFIG_H

#if defined(NRF52833)

#define NHW_HAS_PPI 1
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */

#elif defined(NRF5340)

#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 0

#endif

#endif /* _NRF_HW_CONFIG_H */
