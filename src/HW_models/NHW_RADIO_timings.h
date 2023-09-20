/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note: This header is private to the RADIO HW model
 */
#ifndef _NRF_RADIO_TIMINGS_H
#define _NRF_RADIO_TIMINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nrfra_timings_init(void);
bs_time_t nhwra_timings_get_rampup_time(bool TxNotRx, bool from_hw_TIFS);
bs_time_t nhwra_timings_get_Rx_chain_delay(void);
bs_time_t nhwra_timings_get_RX_rampdown_time(void);
bs_time_t nhwra_timings_get_TX_rampdown_time(void);
bs_time_t nhwra_timings_get_TX_chain_delay(void);

#ifdef __cplusplus
}
#endif

#endif
