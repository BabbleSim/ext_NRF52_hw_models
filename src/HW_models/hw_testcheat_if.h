/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file specifies a set of interfaces meant to allow tests
 * to modify the HW behavior.
 * This APIs should only be used for test purposes, to facilitate testing.
 *
 * This API is specified in a relatively HW agnostic way, so as to facilitate
 * other vendors providing an implementation for it, but
 * it should be used with extra care as the functionality is likely to not be
 * provided by other build targets than the ones based on this HW models.
 */

#ifndef _NRF_HWMODELS_HW_TESTCHEAT_IF_H
#define _NRF_HWMODELS_HW_TESTCHEAT_IF_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Offset the Tx power by <power_offset_i> dBs
 *
 * For example, set to -20 to lower the Tx power by 20dBs compared
 * to what it would have been otherwise.
 */
void hw_radio_testcheat_set_tx_power_gain(double power_offset);

/**
 * Offset the measured Rx power (RSSI) by <power_offset_i> dBs
 *
 * For example, set to 20 to raise the Rx power by 20dBs compared
 * to what it would have been otherwise.
 */
void hw_radio_testcheat_set_rx_power_gain(double power_offset);

#ifdef __cplusplus
}
#endif

#endif
