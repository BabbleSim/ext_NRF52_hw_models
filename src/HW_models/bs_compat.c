/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "bs_oswrap.h"

/*
 * Note: This is a replica of
 *   libUtilv1/bs_oswrap.c:bs_create_folders_in_path()
 * It exists here only to allow using it without requiring
 * users to update their simulator version.
 * It should be removed when libUtilv1 >= v1.12 becomes required
 * or common enough otherwise
 *
 * If missing, attempt to create all folders in a file path
 * Folders are expected to be separated by '/'
 * The path is assumed to be a path to either a folder or a file,
 * but if to a folder, it must end with a '/'.
 * The remainder of the string after '/' will be ignored
 * (assuming it is a file name)
 */
int _bs_create_folders_in_path(const char *path) {
  char sep='/';
  char *start = (char* )path;

  while (*start == sep) {
    start++;
  }
  char *end_folder = strchr(start, sep);

  while (end_folder != NULL) {
    *end_folder = 0;
    int ret = bs_createfolder(path);
    *end_folder = sep;
    if (ret != 0) {
      return 1;
    }
    end_folder = strchr(end_folder + 1, sep);
  }
  return 0;
}
