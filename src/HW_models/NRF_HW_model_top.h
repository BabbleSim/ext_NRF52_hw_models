/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TOP_H
#define _NRF_HW_MODEL_TOP_H

#include "NRF_hw_args.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrf_hw_models_free_all();
void nrf_hw_pre_init();
void nrf_hw_initialize(nrf_hw_sub_args_t *args);
void nrf_hw_some_timer_reached();
void nrf_hw_find_next_timer_to_trigger();

#ifdef __cplusplus
}
#endif

#endif
