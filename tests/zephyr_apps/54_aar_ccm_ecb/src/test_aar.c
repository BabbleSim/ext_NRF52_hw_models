/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "nrf.h"
#include <hal/nrf_aar.h>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include <common.h>

//ZTEST(nrf_aar_tests, aar_test1)
//}

static void test_clean_aar(void *ignored)
{
  ARG_UNUSED(ignored);

  nrf_aar_event_clear(NRF_AAR00, NRF_AAR_EVENT_END);
  nrf_aar_event_clear(NRF_AAR00, NRF_AAR_EVENT_RESOLVED);
  nrf_aar_event_clear(NRF_AAR00, NRF_AAR_EVENT_NOTRESOLVED);
  nrf_aar_event_clear(NRF_AAR00, NRF_AAR_EVENT_ERROR);
}

ZTEST_SUITE(nrf_aar_tests, NULL, NULL, test_clean_aar, test_clean_aar, NULL);
