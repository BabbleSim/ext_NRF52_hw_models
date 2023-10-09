/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_MUTEX_H
#define _NRF_HW_MODEL_MUTEX_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_MUTEX_Type NRF_MUTEX_regs;

void nhw_MUTEX_regw_sideeffects_MUTEX(uint n);
int nhw_MUTEX_regr_sideeffects_MUTEX(uint n);

#ifdef __cplusplus
}
#endif

#endif

