/*
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Interface the HW models expect from the overall scheduler (this overall
 * scheduler is the time machine in the nrf52_bsim target board)
 */
#ifndef _TIME_MACHINE_IF_H
#define _TIME_MACHINE_IF_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the absolute simulated time (microseconds since start)
 * (as know by this device)
 */
bs_time_t tm_get_abs_time(void);
/**
 * Get the simulated time as presented to the HW models
 *
 * Note: In principle HW models should only rely on this time, but they
 * may access the absolute time for logging/tracing, or other purposes
 */
bs_time_t tm_get_hw_time();

/*
 * Convert an absolute time into HW time
 * (Absolute and HW time may be the same, but needs not be)
 */
bs_time_t tm_abs_time_to_hw_time(bs_time_t abstime);

/*
 * Convert a HW time into absolute time
 */
bs_time_t tm_hw_time_to_abs_time(bs_time_t hwtime);

/*
 * Notify the overall scheduler that an event time (timer) has been updated
 */
void tm_find_next_timer_to_trigger();

/*
 * Return the next time of the next event the overall scheduler knows about
 */
bs_time_t tm_get_next_timer_abstime();

/*
 * Note the time of the last interaction with the Babblesim Phy
 */
void tm_update_last_phy_sync_time(bs_time_t abs_time);

/*
 * Set the maximum amount of time we are willing to be without communicating
 * with the BabbleSim Phy. If at some point we would spend longer than that
 * without any Phy activity a "wait" command would be sent.
 */
void tm_set_phy_max_resync_offset(bs_time_t offset_in_us);

#ifdef __cplusplus
}
#endif

#endif
