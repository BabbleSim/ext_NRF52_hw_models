/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Backend for the Non-Volatile Memory devices
 * which can use files on disk or just host RAM
 * to keep their content.
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "NHW_NVM_backend.h"

/**
 * At boot, do whatever is necessary for a given storage
 * (allocate memory, open files etc. )
 */
void nhw_nvm_initialize_data_storage(nvm_storage_state_t *st){
  struct stat f_stat;
  int rc;

  st->fd = -1;
  st->storage = NULL;

  if (st->in_ram == true) {
    st->storage = (uint8_t *)bs_malloc(st->size);

  } else {

    bs_create_folders_in_path(st->file_path);
    st->fd = open(st->file_path, O_RDWR | O_CREAT, (mode_t)0600);
    if (st->fd == -1) {
      bs_trace_error_line("%s: Failed to open %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    rc = fstat(st->fd, &f_stat);
    if (rc) {
      bs_trace_error_line("%s: Failed to get status of %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    if (ftruncate(st->fd, st->size) == -1) {
      bs_trace_error_line("%s: Failed to resize %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }

    st->storage = mmap(NULL, st->size, PROT_WRITE | PROT_READ, MAP_SHARED, st->fd, 0);
    if (st->storage == MAP_FAILED) {
      bs_trace_error_line("%s: Failed to mmap %s device file %s: %s\n",
          __func__, st->type_s, st->file_path, strerror(errno));
    }
  }

  if ((st->erase_at_start == true) || (st->in_ram == true) || (f_stat.st_size == 0)) {
    /* Erase the memory unit by pulling all bits to the configured erase value */
    (void)memset(st->storage, 0xFF, st->size);
  }
}

void nhw_nvm_init_storage(nvm_storage_state_t *st, struct nhw_nvm_st_args_t *args,
                          size_t size, char *type_s)
{
  st->file_path      = args->file;
  st->erase_at_start = args->erase;
  st->rm_at_exit     = args->rm;
  st->in_ram         = args->in_ram;
  st->size           = size;
  st->type_s         = type_s;

  nhw_nvm_initialize_data_storage(st);
}

/**
 * Before exiting the program, do whatever is
 * necessary for a given storage (free'ing heap,
 * closing file descriptors, etc)
 */
void nhw_nvm_clear_storage(nvm_storage_state_t *st){

  if (st->in_ram == true) {
    if (st->storage != NULL) {
      free(st->storage);
      st->storage = NULL;
    }
    return;
  }

  if ((st->storage != MAP_FAILED) && (st->storage != NULL)) {
    munmap(st->storage, st->size);
    st->storage = NULL;
  }

  if (st->fd != -1) {
    close(st->fd);
    st->fd = -1;
  }

  if ((st->rm_at_exit == true) && (st->file_path != NULL)) {
    /* We try to remove the file but do not error out if we can't */
    (void) remove(st->file_path);
    st->file_path = NULL;
  }
}
