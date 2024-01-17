/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include <stdbool.h>

bool nrf_dma_accessible_check(void const * p_reg, void const * p_object)
{
  (void)p_reg;
  (void)p_object;
  return true; /* No restrictions in simulation */
}
