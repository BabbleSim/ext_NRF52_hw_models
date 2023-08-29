/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODELS_NHW_XPPI_H
#define _NRF_HW_MODELS_NHW_XPPI_H

#include "NHW_config.h"

#if (NHW_HAS_PPI)
#include "NRF_PPI.h"
#elif (NHW_HAS_DPPI)
#include "NHW_DPPI.h"
#endif

#endif /* _NRF_HW_MODELS_NHW_XPPI_H */
