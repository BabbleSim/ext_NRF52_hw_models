/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Provides a minimalistic nrfx_config to be used with Nordic nrfx and bsim.
 */

#ifndef NRFX_CONFIG_BSIM_H__
#define NRFX_CONFIG_BSIM_H__

/* When the real nrfx HAL headers are included, this header will be included
 * from them, with the two definitions below. The normal users of the nrfx HAL
 * will only get the prototype of the functions from the real headers.
 * The HW models, will include again the real nrfx headers once more,
 * but setting NRF_DONT_DECLARE_ONLY, and replacing NRF_STATIC_INLINE.
 * In this way, the HW models will include 1 copy of the functions bodies,
 * but as normal, globally available functions, instead of inlined versions.
 * (See src/nrfx/hal/nrf_hal_originals.c for more info)
 */
#ifndef NRF_DONT_DECLARE_ONLY
#define NRF_DECLARE_ONLY
#endif

#ifndef NRF_STATIC_INLINE
#define NRF_STATIC_INLINE
#endif

/*
 * When nrfx drivers are compiled for a real SoC, this macro is inherited from
 * CMSIS. The below definition is needed when those drivers are compiled for
 * the simulated target.
 */
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif

/*
 * Suppress use of anomaly workarounds in nrfx drivers that would directly
 * access hardware registers.
 */
#define USE_WORKAROUND_FOR_ANOMALY_132 0

#endif // NRFX_CONFIG_BSIM_H__

