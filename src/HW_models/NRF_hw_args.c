/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <math.h>
#include <string.h>
#include <strings.h>
#include "NRF_hw_args.h"
#include "bs_types.h"

static nrf_hw_sub_args_t *args_g_hw;

void nrf_hw_sub_cmline_set_defaults(nrf_hw_sub_args_t *args){
  args->useRealAES = 0;
  args_g_hw = args;
}

extern void trivial_xo_set_toffset(bs_time_t t_off);
extern void trivial_xo_set_drift(double xo_d);

void nrf_hw_cmd_starto_found(char * argv, int offset){
  trivial_xo_set_toffset(args_g_hw->start_offset);
}

void nrf_hw_cmd_drift_found(char * argv, int offset){
  trivial_xo_set_drift(args_g_hw->xo_drift);
}
