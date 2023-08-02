/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * This interface is kept only for backwards compatibility with old applications/tests
 * All these are deprecated.
 */
#ifndef _TIME_MACHINE_IF_H
#define _TIME_MACHINE_IF_H

#include "bs_types.h"
#include "nsi_hw_scheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline bs_time_t tm_get_abs_time(void)
{
	return nsi_hws_get_time();
}

static inline bs_time_t tm_get_hw_time(void)
{
	return nsi_hws_get_time();
}

static inline bs_time_t tm_abs_time_to_hw_time(bs_time_t abstime)
{
	return abstime;
}

static inline bs_time_t tm_hw_time_to_abs_time(bs_time_t hwtime)
{
	return hwtime;
}

static inline void tm_find_next_timer_to_trigger(void)
{
	nsi_hws_find_next_event();
}

static inline bs_time_t tm_get_next_timer_abstime(void)
{
	return nsi_hws_get_next_event_time();
}

void tm_set_phy_max_resync_offset(bs_time_t offset_in_us);

#ifdef __cplusplus
}
#endif

#endif
