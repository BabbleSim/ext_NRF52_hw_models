/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_TOP_H
#define _NRF_HW_MODEL_TOP_H

#include "bs_types.h"
#include "NRF_hw_args.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * API towards the user of this HW models
 */

/*
 * Simulated time in microseconds in which the next HW models event is to occur
 * The overall scheduler must call `nrf_hw_some_timer_reached()` whenver this
 * simulated time is reached
 */
extern bs_time_t timer_nrf_main_timer;

/*
 * Run any pre-initialization tasks which needs to be done very early
 * Like registering command line arguments or dump files
 */
void nrf_hw_pre_init();

/*
 * Initialize all HW models and set registers to defaults
 */
void nrf_hw_initialize(nrf_hw_sub_args_t *args);

/*
 * Cleanup, close any resources and free any memory allocated by the models
 * No other HW models function may be called after this.
 */
void nrf_hw_models_free_all();

/*
 * The simulated time indicated in `timer_nrf_main_timer` has just been reached.
 * The "overall scheduler" calls this function so the models can run
 * whatever work they had to run at this point in time.
 */
void nrf_hw_some_timer_reached();


/*
 * Internal API to the HW models
 */

/**
 * Each time a HW peripheral model
 */
void nrf_hw_find_next_timer_to_trigger();

#ifdef __cplusplus
}
#endif

#endif
