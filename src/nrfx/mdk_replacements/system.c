/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#if defined(NRF5340_XXAA_NETWORK) || defined(NRF52833) \
	|| defined(NRF54L15_XXAA)

#define __SYSTEM_CLOCK_DEFAULT      (64000000UL)

__attribute__((weak)) uint32_t SystemCoreClock = __SYSTEM_CLOCK_DEFAULT;

__attribute__((weak)) void SystemCoreClockUpdate(void)
{
	SystemCoreClock = __SYSTEM_CLOCK_DEFAULT;
}

#elif defined(NRF5340_XXAA_APPLICATION)

#include "nrfx.h"

/* NRF5340 application core uses a variable System Clock Frequency that starts at 64MHz */
#define __SYSTEM_CLOCK_MAX      (128000000UL)
#define __SYSTEM_CLOCK_DEFAULT  ( 64000000UL)

__attribute__((weak)) uint32_t SystemCoreClock = __SYSTEM_CLOCK_DEFAULT;

__attribute__((weak)) void SystemCoreClockUpdate(void)
{
	SystemCoreClock = __SYSTEM_CLOCK_MAX >> (NRF_CLOCK_S->HFCLKCTRL & (CLOCK_HFCLKCTRL_HCLK_Msk));
}

#endif
