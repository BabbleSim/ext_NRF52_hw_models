/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_FICR_H
#define _NRF_HW_MODEL_FICR_H

#include "NRF_regs.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_ficr_init();
void nrf_ficr_clean_up();

#ifdef __cplusplus
}
#endif

#endif
