/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef TRIVIAL_XO_H
#define TRIVIAL_XO_H

#include "xo_if.h"
#include "bs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void trivial_xo_set_clock_drift(double cdrift);
void xo_model_set_toffset(double args_tOffset);

#ifdef __cplusplus
}
#endif

#endif
