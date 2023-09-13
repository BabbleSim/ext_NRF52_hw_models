/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Mocks the CMSIS dependency of the nRF MDK headers.
 * And the bare minimum of required content.
 * The proper CMSIS headers replacements are provided by the integrating program
 * In the case of Zephyr, this is by the nrf52_bsim.
 */

#ifndef HW_MODELS_CORE_CM4_H
#define HW_MODELS_CORE_CM4_H

#if defined(CONFIG_SOC_SERIES_BSIM_NRFXX) || defined(INTEGRATOR_PROVIDES_CMSIS)
  /* The real core_cm4.h includes a very significant number of definitions
   * and further CMSIS includes. Which may be needed by the integrating program.
   * We provide this include as a means to allow the integrating to add those.
   * */
#include "cmsis.h"
#endif

#ifndef __I
#define __I
#endif
#ifndef __IO
#define __IO
#endif
#ifndef __O
#define __O
#endif

/**
 * The model of the CPU & IRQ controller driver must provide
 * these function. These HW models do not provide them.
 */
extern void __WFE(void);

#ifndef __DSB
#define __DSB()
#endif

#ifndef __ISB
#define __ISB()
#endif

#endif /* HW_MODELS_CORE_CM4_H */
