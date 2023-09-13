/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "NHW_config.h"

/*
 * Get the name of a core/domain
 *
 * Only for debugging/logging/tracing purposes.
 */
const char *nhw_get_core_name(unsigned int core_n)
{
  static const char *corenames[NHW_INTCTRL_TOTAL_INST] = NHW_CORE_NAMES;

  if (core_n < sizeof(corenames)/sizeof(corenames[0])) {
    return corenames[core_n];
  } else {
    return NULL;
  }
}
