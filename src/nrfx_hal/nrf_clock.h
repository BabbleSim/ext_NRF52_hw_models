/*
 * Part of the real nrf_clock.h from Nordic's NRFx HAL
 *
 * Note that there is a few changes compared to the original
 * Where relevant, these changes are:
 * Copyright (c) 2017 Oticon A/S
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef BS_NRF_CLOCK_H__
#define BS_NRF_CLOCK_H__

#include "nrf_soc_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_clock_hal Clock HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the CLOCK peripheral.
 *
 * This code can be used to managing low-frequency clock (LFCLK) and the high-frequency clock
 * (HFCLK) settings.
 */

#define NRF_CLOCK_TASK_TRIGGER (1UL)
#define NRF_CLOCK_EVENT_CLEAR  (0UL)

/**
 * @brief Interrupts.
 */
typedef enum
{
    NRF_CLOCK_INT_HF_STARTED_MASK = CLOCK_INTENSET_HFCLKSTARTED_Msk, /**< Interrupt on HFCLKSTARTED event. */
    NRF_CLOCK_INT_LF_STARTED_MASK = CLOCK_INTENSET_LFCLKSTARTED_Msk, /**< Interrupt on LFCLKSTARTED event. */
    NRF_CLOCK_INT_DONE_MASK       = CLOCK_INTENSET_DONE_Msk,         /**< Interrupt on DONE event. */
    NRF_CLOCK_INT_CTTO_MASK       = CLOCK_INTENSET_CTTO_Msk          /**< Interrupt on CTTO event. */
} nrf_clock_int_mask_t;


/**
 * @brief Tasks.
 *
 * @details The NRF_CLOCK_TASK_LFCLKSTOP task cannot be set when the low-frequency clock is not running.
 * The NRF_CLOCK_TASK_HFCLKSTOP task cannot be set when the high-frequency clock is not running.
 */
typedef enum /*lint -save -e30 -esym(628,__INTADDR__) */
{
    NRF_CLOCK_TASK_HFCLKSTART = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTART), /**< Start HFCLK clock source.*/
    NRF_CLOCK_TASK_HFCLKSTOP  = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTOP),  /**< Stop HFCLK clock source.*/
    NRF_CLOCK_TASK_LFCLKSTART = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTART), /**< Start LFCLK clock source.*/
    NRF_CLOCK_TASK_LFCLKSTOP  = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTOP),  /**< Stop LFCLK clock source.*/
    NRF_CLOCK_TASK_CAL        = offsetof(NRF_CLOCK_Type, TASKS_CAL),        /**< Start calibration of LFCLK RC oscillator.*/
    NRF_CLOCK_TASK_CTSTART    = offsetof(NRF_CLOCK_Type, TASKS_CTSTART),    /**< Start calibration timer.*/
    NRF_CLOCK_TASK_CTSTOP     = offsetof(NRF_CLOCK_Type, TASKS_CTSTOP)      /**< Stop calibration timer.*/
} nrf_clock_task_t;                                                         /*lint -restore */

void nrf_clock_int_enable(uint32_t int_mask);

void nrf_clock_int_disable(uint32_t int_mask);

void nrf_clock_task_trigger(nrf_clock_task_t task);

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_CLOCK_H__ */
