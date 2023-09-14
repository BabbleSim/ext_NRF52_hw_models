/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * SWI
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/swi.html?cp=4_0_0_6_32
 *
 * This file just allocates the RAM addresses booked for the SW interrupts
 * This is with all likelihood a model of no-HW
 */

#include <stdint.h>
#include "NHW_config.h"

int NRF_SWI_regs[NHW_SWI_TOTAL_INST];
