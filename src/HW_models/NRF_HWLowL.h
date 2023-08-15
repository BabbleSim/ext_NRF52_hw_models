/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Very thin layer which hides a bit the low level libPhyCom IF
 * and handles the time differences
 */
#ifndef _NRF_HWLOWL_H
#define _NRF_HWLOWL_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

int hwll_connect_to_phy(unsigned int d, const char* s, const char* p);
void hwll_disconnect_phy(void);
void hwll_disconnect_phy_and_exit(void);
void hwll_terminate_simulation(void);
void hwll_set_nosim(bool new_nosim);

bs_time_t hwll_phy_time_from_dev(bs_time_t d_t);
bs_time_t hwll_dev_time_from_phy(bs_time_t phy_t);

void hwll_sync_time_with_phy(bs_time_t d_t);
void hwll_wait_for_phy_simu_time(bs_time_t phy_time);

#ifdef __cplusplus
}
#endif

#endif
