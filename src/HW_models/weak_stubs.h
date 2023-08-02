/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HW_MODELS_WEAK_STUBS_H
#define HW_MODELS_WEAK_STUBS_H

#include "bs_types.h"
#include "bs_cmd_line.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The integrating program is expected to provide these. If not weak stubs are provided here */
extern void phy_sync_ctrl_set_last_phy_sync_time(bs_time_t time);
extern void bs_add_extra_dynargs(bs_args_struct_t *args_struct_toadd);
extern unsigned int bsim_args_get_global_device_nbr(void);

#ifdef __cplusplus
}
#endif

#endif /* HW_MODELS_WEAK_STUBS_H */
