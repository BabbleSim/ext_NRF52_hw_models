/*
 * Copyright (c) 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Interface the HW models expect from the time machine
 * (The time machine is provided by the board in Zephyr's case)
 */
#ifndef _TIME_MACHINE_IF_H
#define _TIME_MACHINE_IF_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bs_time_t tm_get_abs_time(void);
bs_time_t tm_get_hw_time_from_abs_time(bs_time_t abstime);
bs_time_t tm_get_hw_time();
bs_time_t tm_hw_time_to_abs_time(bs_time_t hwtime);
bs_time_t tm_abs_time_to_hw_time(bs_time_t abstime);

void tm_find_next_timer_to_trigger();
bs_time_t tm_get_next_timer_abstime();

void tm_update_last_phy_sync_time(bs_time_t abs_time);
void tm_set_phy_max_resync_offset(bs_time_t offset_in_us);

#ifdef __cplusplus
}
#endif

#endif
