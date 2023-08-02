/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _XO_IF_H
#define _XO_IF_H

#ifdef __cplusplus
extern "C"{
#endif

long double phy_time_from_dev(long double dev_t);
long double dev_time_from_phy(long double phy_t);
void xo_model_set_toffset(double t_off);

#ifdef __cplusplus
}
#endif

#endif
