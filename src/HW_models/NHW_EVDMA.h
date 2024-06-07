/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_EVDMA_H
#define _NRF_HW_MODEL_EVDMA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Job (list) entry (what FW programs in memory) */
struct job_list {
  char *job_buff;
  uint32_t attr_len;
};

typedef struct {
  struct job_list *joblist_ptr; /* Pointer to the *next* job entry to be processed */
  enum evdma_mode {EVDMA_idle = 0, EVDMA_read = 1, EVDMA_write = 2} mode; /* 0: nothing, 1: read, 2: write */

  /* Current job status: */
  char    *job_buff;          /* Pointer to job buffer in memory with whatever offset */
  uint32_t job_pend_length;   /* Pending length in current job */
  unsigned int job_attrib;    /* Attribute of the current job */
} EVDMA_status_t;

int nhw_EVDMA_start(EVDMA_status_t *st, void *joblist_ptr);
int nhw_EVDMA_access(EVDMA_status_t *st,
                     bool read_not_write,
                     uint8_t *periph_buf,
                     size_t nbytes,
                     size_t *n_actual,
                     bool new_job);

#define NHW_EVDMA_READ  true
#define NHW_EVDMA_WRITE false
#define NHW_EVDMA_NEWJOB  true
#define NHW_EVDMA_CONTINUEJOB  false

#ifdef __cplusplus
}
#endif

#endif
