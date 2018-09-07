/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FAKE_TIMER_H
#define _FAKE_TIMER_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void fake_timer_init();
void fake_timer_cleanup(void);
void fake_timer_wake_in_time(bs_time_t time);
void fake_timer_triggered(void);

#ifdef __cplusplus
}
#endif

#endif
