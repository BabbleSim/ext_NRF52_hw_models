/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HW_MODELS_BSTEST_TICKER_H
#define HW_MODELS_BSTEST_TICKER_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void bst_ticker_amp_set_period(uint inst, bs_time_t tick_period);
void bst_ticker_amp_set_next_tick_absolutelute(uint inst, bs_time_t absolute_time);
void bst_ticker_amp_set_next_tick_delta(uint inst, bs_time_t delta_time);
void bst_ticker_amp_awake_cpu_asap(uint inst);

#ifdef __cplusplus
}
#endif

#endif /* HW_MODELS_BSTEST_TICKER_H */
