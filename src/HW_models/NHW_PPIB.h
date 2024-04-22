/*
 * Copyright (c) 2024, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_PPIB_H
#define _NRF_HW_MODEL_PPIB_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_PPIB_regw_sideeffects_SUBSCRIBE_SEND(uint inst, uint ch);
/* Note: The PPIB task and events registers are not functional */

#ifdef __cplusplus
}
#endif

#endif
