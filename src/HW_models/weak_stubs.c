/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_cmd_line.h"
#include "bs_tracing.h"

__attribute__((weak)) unsigned int get_device_nbr(void) {
	return 0;
}

__attribute__((weak)) void bs_add_extra_dynargs(bs_args_struct_t *args_struct_toadd){
  bs_trace_warning_line("%s: The integrating program is expected to provide this API."
      "This weak function should not have been called\n", __func__);
}
