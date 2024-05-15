/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NHW_RRAMC_H
#define _NRF_HW_MODEL_NHW_RRAMC_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_RRAMC_regw_sideeffects_ERASEALL(uint inst);
void* nhw_RRAMC_get_RRAM_base_address(uint inst);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_RRAMC_H */
