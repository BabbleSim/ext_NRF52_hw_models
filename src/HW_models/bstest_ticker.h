/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FW_test_ticker_H
#define _FW_test_ticker_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

//Interface towards the time_machine:
void bst_ticker_triggered(bs_time_t Now);

#ifdef __cplusplus
}
#endif

#endif
