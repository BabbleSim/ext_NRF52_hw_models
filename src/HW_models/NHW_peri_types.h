/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This header provides the HW peripherals SW regiters interface
 * definitions.
 * For some devices this is done by including directly the MDK files
 * for others, by including dedicated files
 */
#ifndef _NRF_TYPES_H
#define _NRF_TYPES_H

#include "NHW_config.h"

#if NHW_USE_MDK_TYPES
  #define __I /* They cannot be const from the HW models POV */
  #define __IM
  #include "nrf.h"
#else

  #if defined(NRF5340)
    #include "NRF5340_peri_types.h"
  #else
    #error "This platform is not supported (no HW register structure types defined)"
  #endif

#endif /* NHW_USE_MDK_TYPES */


#endif
