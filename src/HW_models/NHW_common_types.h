/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file contains common types shared between peripherals models
 */
#ifndef _NRF_HW_MODELS_COMMON_TYPES_H
#define _NRF_HW_MODELS_COMMON_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Type used to keep the mapping of a peripheral instance to its interrupt controller line and instance */
struct nhw_irq_mapping {
  unsigned int cntl_inst;
  unsigned int int_nbr;
};

/* Type used to keep the mapping of a peripheral instance to its DPPI instance */
struct nhw_dppi_mapping {
  unsigned int dppi_inst;
};

/* Type used to keep the current/last status of each subscribable task */
struct nhw_subsc_mem {
  bool is_subscribed;
  uint8_t subscribed_ch;
};

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODELS_COMMON_TYPES_H */
