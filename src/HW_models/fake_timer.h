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

void nhw_fake_timer_wake_in_time(unsigned int inst, bs_time_t time);

#ifdef __cplusplus
}
#endif

#endif
