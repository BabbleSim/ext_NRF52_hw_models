/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//Simple model of a constant XO drift

#include "bs_tracing.h"
#include "bs_types.h"

static double xo_drift = 0.0; //Cristal oscillator drift relative to the phy
static double time_off = 0.0; //Time offset relative to the phy

long double __attribute__((weak)) dev_time_from_phy(long double p_t){
  long double dev_time;
  dev_time = ((1.0 + xo_drift)*(long double)p_t) - time_off;
  //Note: in x86 compiling with GCC for linux long double is 80bit (64bits
  // mantissa), and therefore precision should be kept
  return dev_time;
}

long double __attribute__((weak)) phy_time_from_dev(long double d_t){
  long double phy_time;
  phy_time =  ((long double)(d_t + time_off))/(1.0 + xo_drift);
  //Note: in x86 compiling with GCC for linux long double is 80bit (64bits
  // mantissa), and therefore precision should be kept
  return phy_time;
}

void trivial_xo_set_drift(double xo_d){
  if ((xo_d < -3e-4) || (xo_d > 3e-4)) {
    bs_trace_warning_line("Very high clock drift set (%0.f ppm > 300ppm)\n", xo_d*1e6);
    if ((xo_d < -1e-2) || (xo_d > 1e-2)) {
      bs_trace_error_line("Insane clock drift set (%0.f ppm)\n", xo_d*1e6);
    }
  }
  xo_drift = xo_d;
}

void trivial_xo_set_toffset(bs_time_t t_off){
  if (t_off < 0) {
    bs_trace_error_line("Time offset (%lf) cannot be smaller than 0\n", t_off);
  }
  time_off = ((double)t_off)/1e6;
}
