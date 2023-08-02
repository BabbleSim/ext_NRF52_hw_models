/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_cmd_line.h"
#include "bs_tracing.h"

__attribute__((weak)) void bs_add_extra_dynargs(bs_args_struct_t *args_struct_toadd){
  bs_trace_warning_line("%s: The integrating program is expected to provide this API."
      "This weak function should not have been called\n", __func__);
}

__attribute__((weak)) void phy_sync_ctrl_set_last_phy_sync_time(bs_time_t time){
  /* Intentionally left empty */
  /*
   * There is no strong requirement for the integration program to provide this API.
   * Check Zephyr's bsim boards phy_sync_ctrl.c for more information.
   */
}

__attribute__((weak))  unsigned int bsim_args_get_global_device_nbr(void){
  return 0;
}
