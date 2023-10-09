/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_mutex.h"
#include "bs_tracing.h"
#include "NHW_MUTEX.h"

bool nrf_mutex_lock(NRF_MUTEX_Type * p_reg, uint8_t mutex)
{
    return (nhw_MUTEX_regr_sideeffects_MUTEX(mutex) == MUTEX_MUTEX_MUTEX_Unlocked);
}

void nrf_mutex_unlock(NRF_MUTEX_Type * p_reg, uint8_t mutex)
{
    nhw_MUTEX_regw_sideeffects_MUTEX(mutex);
}
