/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HW_MODELS_WEAK_STUBS_H
#define HW_MODELS_WEAK_STUBS_H

#include "bs_cmd_line.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The integrating program is expected to provide these. If not weak stubs are provided here */
extern unsigned int get_device_nbr(void);
extern void bs_add_extra_dynargs(bs_args_struct_t *args_struct_toadd);

#ifdef __cplusplus
}
#endif

#endif /* HW_MODELS_WEAK_STUBS_H */
