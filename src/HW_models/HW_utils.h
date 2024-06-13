/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_UTILS_H
#define _NRF_HW_MODEL_UTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

void hwu_reverse_byte_order(const unsigned char *in_data, unsigned char *out_data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UTILS_H */
