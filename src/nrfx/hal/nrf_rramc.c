/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include "hal/nrf_rramc.h"
#include "NHW_RRAMC.h"

void nrf_rramc_erase_all_set(NRF_RRAMC_Type * p_reg)
{
    p_reg->ERASE.ERASEALL = RRAMC_ERASE_ERASEALL_ERASE_Erase;
    nhw_RRAMC_regw_sideeffects_ERASEALL(0);
}

/* Note that tasks and events are not implemented in the HW model */
