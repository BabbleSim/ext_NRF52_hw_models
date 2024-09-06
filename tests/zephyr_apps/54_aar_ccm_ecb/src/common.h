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

static inline void print_byte_array(uint8_t *in, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    if (i > 0) {
      printk(", ");
    }
    printk("0x%02X", in[i]);
  }
  printk("\n");
}

static inline void print_uint32_array(uint32_t *in, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    printk("0x%08X\n", in[i]);
  }
  printk("\n");
}

static inline void print_job(job_t *job, bool with_data) {
  if (job == NULL) {
    return;
  }
#define PRINT_JOB printk("job_ptr=0x%p\tlength= %i\t attr=%i\n", job->job_ptr, job->length, job->attr);
  while (job->job_ptr != 0) {
    PRINT_JOB
    if (with_data) {
      printk("   ^--> ");
      print_byte_array(job->job_ptr, job->length);
    }
    job++;
  }
  PRINT_JOB
  printk("=> End of Joblist\n");
}

#ifdef __cplusplus
}
#endif

#endif /* TESTS_ZEPHYR_APPS_AAR_CCM_ECB_SRC_COMMON_H */
