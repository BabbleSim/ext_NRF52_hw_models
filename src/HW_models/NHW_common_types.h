/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file contains common types shared between peripherals models
 */
#ifndef _NRF_HW_MODELS_COMMON_TYPES_H
#define _NRF_HW_MODELS_COMMON_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

struct nhw_irq_mapping {
  unsigned int cntl_inst;
  unsigned int int_nbr;
};

struct nhw_dppi_mapping {
  unsigned int dppi_inst;
};

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODELS_COMMON_TYPES_H */
