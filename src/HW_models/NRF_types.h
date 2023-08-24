/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_TYPES_H
#define _NRF_TYPES_H

#include "NRF_HW_config.h"

#if NHW_USE_MDK_TYPES
  #include "nrf.h"
#else

  #if defined(NRF5340)
  #else
    #error "This platform is not supported (no HW register structure types defined)"
  #endif

#endif /* NHW_USE_MDK_TYPES */


#endif
