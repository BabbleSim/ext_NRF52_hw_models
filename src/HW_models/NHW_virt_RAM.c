/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Allocate buffers which can be used in place of the real device RAMs.
 * See NHW_misc.c, and nhw_convert_RAM_addr()
 */

#include "NHW_config.h"

#if defined(NRF5340) || defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)
__attribute__((__section__("virt_RAM.appcore_RAM"))) char NHW_appcore_RAM[NHW_APPCORE_RAM_SIZE];
#endif
