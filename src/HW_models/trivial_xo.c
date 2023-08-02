/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//Simple model of a constant XO drift

#include "bs_tracing.h"
#include "bs_types.h"
#include "nsi_tasks.h"
#include "weak_stubs.h"

static double xo_drift = 0.0; //Crystal oscillator drift relative to the phy
static double time_off = 0.0; //Time offset relative to the phy

long double __attribute__((weak)) dev_time_from_phy(long double p_t){
  long double dev_time;
  dev_time = ((1.0 + xo_drift)*(long double)p_t) - time_off;
  //Note: in x86 compiling with GCC for Linux long double is 80bit (64bits
  // mantissa), and therefore precision should be kept
  return dev_time;
}

long double __attribute__((weak)) phy_time_from_dev(long double d_t){
  long double phy_time;
  phy_time =  ((long double)(d_t + time_off))/(1.0 + xo_drift);
  //Note: in x86 compiling with GCC for Linux long double is 80bit (64bits
  // mantissa), and therefore precision should be kept
  return phy_time;
}

void xo_model_set_toffset(double t_off) {
  time_off = ((double)t_off)/1e6;
}

static double tmp_xo_drift;

static void arg_xo_drift_found(char * argv, int offset) {
	xo_drift = tmp_xo_drift;
}

static void trivial_xo_register_cmd_args(void) {

  static bs_args_struct_t args_struct_toadd[] = {
    {
      .option = "xo_drift",
      .name = "xo_drift",
      .type = 'f',
      .dest = (void*)&tmp_xo_drift,
      .call_when_found = arg_xo_drift_found,
      .descript = "Simple linear model of the XO drift of this device. For ex. for -30ppm set to -30e-6"
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(trivial_xo_register_cmd_args, PRE_BOOT_1, 100);

static void trivial_xo_init(void) {
  if ((xo_drift < -3e-4) || (xo_drift > 3e-4)) {
    bs_trace_warning_line("Very high clock drift set (%0.f ppm > 300ppm)\n", xo_drift*1e6);
    if ((xo_drift < -1e-2) || (xo_drift > 1e-2)) {
      bs_trace_error_line("Insane clock drift set (%0.f ppm)\n", xo_drift*1e6);
    }
  }
}

NSI_TASK(trivial_xo_init, HW_INIT, 100);
