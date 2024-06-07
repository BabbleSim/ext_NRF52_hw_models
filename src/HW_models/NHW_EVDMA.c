/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Common EVDMA functionality models
 *
 * Notes:
 *   * Attributes with special meaning/signaling that data should be
 *     processed by the DMA are not yet implemented.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "NHW_EVDMA.h"
#include <bs_tracing.h>

/**
 * Read the next job from the joblist into the EVDMA status structure
 * and move the joblist_ptr to point to the one after.
 * returns
 *    true if the joblist ended (no new job)
 *    false otherwise
 */
static bool nhw_EVDMA_read_next_job(EVDMA_status_t *st) {
  st->job_buff = st->joblist_ptr->job_buff;

  if (st->job_buff == NULL) {
    return true;
  }

  uint32_t at_len = st->joblist_ptr->attr_len;

  st->job_attrib = at_len >> 24;
  st->job_pend_length = at_len & (0xFFFFFF);

  st->joblist_ptr++;

  return false;
}

/**
 * Peek on the next job from the joblist
 * and set *job_attrib with that next job attribute
 * returns
 *    true if the joblist ends in the next one (no new job)
 *    false otherwise
 */
static bool nhw_EVDMA_peek_next_job_attrib(EVDMA_status_t *st, uint *job_attrib) {
  char *job_buff = st->joblist_ptr->job_buff;

  if (job_buff == NULL) {
    return true;
  }

  uint32_t at_len = st->joblist_ptr->attr_len;
  *job_attrib = at_len >> 24;

  return false;
}

/**
 * Initialize the EVDMA for a new joblist
 *
 *   joblist_ptr: pointer to the job list
 *   st : EVDMA internal status to be initialized
 *
 *   return: 0 on success
 *           != 0 if joblist_ptr is NULL
 */
int nhw_EVDMA_start(EVDMA_status_t *st, void *joblist_ptr) {
  st->joblist_ptr = joblist_ptr;
  st->mode = EVDMA_idle;

  st->job_buff = NULL;
  st->job_pend_length = 0;
  st->job_attrib = 0;

  if (joblist_ptr == NULL) {
    bs_trace_warning_time_line("FW error: Job list pointer is NULL\n");
    return -1;
  } else {
    return 0;
  }
}

/*
 * Do an EVMA read/write of <nbytes>
 *
 * Returns the number of read/written bytes if all requested where read or a negative number otherwise
 *
 *  st             EVDMA internal status (will be updated)
 *  read_not_write True for read, false for write access.
 *  periph_buf     Buffer in the peripheral where the data will be written to/read from
 *  nbytes         Number of bytes to read/write
 *  n_actual       Number of bytes actually read/written
 *  new_job        Skip to the next job even if the current one has pending data
 *
 *  returns:
 *    >=0 (nbytes) read as many as requested
 *    -1 partial read/write
 *    -2 end of job list reached (n_actual may be != 0 on a partial read/write)
 *    -3 error, joblist_ptr is invalid
 */
int nhw_EVDMA_access(EVDMA_status_t *st,
                     bool read_not_write,
                     uint8_t *periph_buf,
                     size_t nbytes,
                     size_t *n_actual,
                     bool start_new_job) {

  *n_actual = 0;

  if (st->joblist_ptr == NULL) {
    bs_trace_warning_time_line("FW error: Job list pointer is NULL\n");
    return -3;
  }

  if (   ( read_not_write && (st->mode == EVDMA_write))
      || (!read_not_write && (st->mode == EVDMA_read)) ) {
    bs_trace_error_time_line("Programming error\n");
  } else {
    if (read_not_write) {
      st->mode = EVDMA_read;
    } else {
      st->mode = EVDMA_write;
    }
  }

  if ((st->job_pend_length == 0) || start_new_job) {
    bool list_end = nhw_EVDMA_read_next_job(st);
    if (list_end) {
      return -2;
    }
  }

  do {
    uint to_read = BS_MIN(nbytes - *n_actual, st->job_pend_length);

    if (read_not_write) {
      memcpy(periph_buf, st->job_buff, to_read);
    } else {
      memcpy(st->job_buff, periph_buf, to_read);
    }
    st->job_pend_length -= to_read;
    st->job_buff += to_read;
    *n_actual += to_read;
    periph_buf += to_read;

    if (*n_actual == nbytes) {
      return nbytes;
    }

    uint next_job_attrib;
    bool list_end = nhw_EVDMA_peek_next_job_attrib(st, &next_job_attrib);
    if (list_end) {
      return -2;
    } else if (next_job_attrib != st->job_attrib) {
      return -1;
    }

    nhw_EVDMA_read_next_job(st);

  } while (true);
}
