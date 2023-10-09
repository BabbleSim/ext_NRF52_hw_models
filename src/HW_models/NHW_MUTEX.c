/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * MUTEX - Mutual exclusive peripheral
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/mutex.html?cp=4_0_0_6_18
 *
 * This file provides the implementation of the nRF5340 MUTEX peripheral
 */

#include <string.h>
#include <stdbool.h>
#include "bs_types.h"
#include "nsi_tasks.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"

NRF_MUTEX_Type NRF_MUTEX_regs;
static bool MUTEX_state[sizeof(NRF_MUTEX_regs.MUTEX)];

/**
 * Initialize the MUTEX model
 */
static void nhw_mutex_init(void) {
  memset(NRF_MUTEX_regs.MUTEX, 0, sizeof(NRF_MUTEX_regs.MUTEX));
  memset(MUTEX_state, 0, sizeof(MUTEX_state));
}

NSI_TASK(nhw_mutex_init, HW_INIT, 100);

/*
 * Handle the sideeffects of writing a 0 to the MUTEX register
 */
void nhw_MUTEX_regw_sideeffects_MUTEX(uint n) {
  MUTEX_state[n] = 0;
  NRF_MUTEX_regs.MUTEX[n] = 0;
}

/*
 * Returns the value that would have been read from the MUTEX register
 * and updates the MUTEX register itself
 */
int nhw_MUTEX_regr_sideeffects_MUTEX(uint n) {
  if (MUTEX_state[n] == 0) {
    MUTEX_state[n] = 1;
    NRF_MUTEX_regs.MUTEX[n] = 1;
    return 0;
  }
  return 1;
}
