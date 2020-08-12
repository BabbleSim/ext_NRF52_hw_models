/*
 * Copyright (c) 2019 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef BS_NRF_POWER_H__
#define BS_NRF_POWER_H__

#include "../drivers/nrfx_common.h"

NRF_STATIC_INLINE uint32_t nrf_power_int_enable_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->INTENSET;
}

#endif /* BS_NRF_POWER_H__ */
