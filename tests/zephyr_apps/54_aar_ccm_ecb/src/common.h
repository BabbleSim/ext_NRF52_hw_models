/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TESTS_ZEPHYR_APPS_AAR_CCM_ECB_SRC_COMMON_H
#define TESTS_ZEPHYR_APPS_AAR_CCM_ECB_SRC_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CCM_DMA_ATTR_ALEN 11
#define CCM_DMA_ATTR_MLEN 12
#define CCM_DMA_ATTR_ADATA 13
#define CCM_DMA_ATTR_MDATA 14

typedef struct __packed __aligned(sizeof(void*)) {
  uint8_t *job_ptr;
  uint32_t length : 24;
  uint8_t attr;
} job_t;

static inline void reverse_bytearray(uint8_t *p_array_in, uint8_t *p_array_out, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    p_array_out[size - i - 1] = p_array_in[i];
  }
}

#ifdef __cplusplus
}
#endif

#endif /* TESTS_ZEPHYR_APPS_AAR_CCM_ECB_SRC_COMMON_H */
