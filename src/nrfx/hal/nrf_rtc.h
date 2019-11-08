/**
 * Part of the real nrf_rtc.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_RTC_H__
#define BS_NRF_RTC_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum nrf_rtc_task_t
 * @brief RTC tasks.
 */
typedef enum
{
    /*lint -save -e30*/
    NRF_RTC_TASK_START            = offsetof(NRF_RTC_Type,TASKS_START),     /**< Start. */
    NRF_RTC_TASK_STOP             = offsetof(NRF_RTC_Type,TASKS_STOP),      /**< Stop. */
    NRF_RTC_TASK_CLEAR            = offsetof(NRF_RTC_Type,TASKS_CLEAR),     /**< Clear. */
    NRF_RTC_TASK_TRIGGER_OVERFLOW = offsetof(NRF_RTC_Type,TASKS_TRIGOVRFLW),/**< Trigger overflow. */
    /*lint -restore*/
} nrf_rtc_task_t;

/**
 * @enum nrf_rtc_event_t
 * @brief RTC events.
 */
typedef enum
{
    /*lint -save -e30*/
    NRF_RTC_EVENT_TICK        = offsetof(NRF_RTC_Type,EVENTS_TICK),       /**< Tick event. */
    NRF_RTC_EVENT_OVERFLOW    = offsetof(NRF_RTC_Type,EVENTS_OVRFLW),     /**< Overflow event. */
    NRF_RTC_EVENT_COMPARE_0   = offsetof(NRF_RTC_Type,EVENTS_COMPARE[0]), /**< Compare 0 event. */
    NRF_RTC_EVENT_COMPARE_1   = offsetof(NRF_RTC_Type,EVENTS_COMPARE[1]), /**< Compare 1 event. */
    NRF_RTC_EVENT_COMPARE_2   = offsetof(NRF_RTC_Type,EVENTS_COMPARE[2]), /**< Compare 2 event. */
    NRF_RTC_EVENT_COMPARE_3   = offsetof(NRF_RTC_Type,EVENTS_COMPARE[3])  /**< Compare 3 event. */
    /*lint -restore*/
} nrf_rtc_event_t;

/**
 * @enum nrf_rtc_int_t
 * @brief RTC interrupts.
 */
typedef enum
{
    NRF_RTC_INT_TICK_MASK     = RTC_INTENSET_TICK_Msk,     /**< RTC interrupt from tick event. */
    NRF_RTC_INT_OVERFLOW_MASK = RTC_INTENSET_OVRFLW_Msk,   /**< RTC interrupt from overflow event. */
    NRF_RTC_INT_COMPARE0_MASK = RTC_INTENSET_COMPARE0_Msk, /**< RTC interrupt from compare event on channel 0. */
    NRF_RTC_INT_COMPARE1_MASK = RTC_INTENSET_COMPARE1_Msk, /**< RTC interrupt from compare event on channel 1. */
    NRF_RTC_INT_COMPARE2_MASK = RTC_INTENSET_COMPARE2_Msk, /**< RTC interrupt from compare event on channel 2. */
    NRF_RTC_INT_COMPARE3_MASK = RTC_INTENSET_COMPARE3_Msk  /**< RTC interrupt from compare event on channel 3. */
} nrf_rtc_int_t;



/**
 * @brief Function for setting a compare value for a channel.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] ch     Channel.
 * @param[in] cc_val Compare value to be set.
 */
void nrf_rtc_cc_set(NRF_RTC_Type * p_reg, uint32_t ch, uint32_t cc_val);

/**
 * @brief Function for returning the compare value for a channel.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ch    Channel.
 *
 * @return COMPARE[ch] value.
 */
__STATIC_INLINE uint32_t nrf_rtc_cc_get(NRF_RTC_Type const * p_reg, uint32_t ch);

/**
 * @brief Function for enabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Interrupt mask to be enabled.
 */
void nrf_rtc_int_enable(NRF_RTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Interrupt mask to be disabled.
 */
void nrf_rtc_int_disable(NRF_RTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *
 * @return Mask of enabled interrupts.
 */
__STATIC_INLINE uint32_t nrf_rtc_int_enable_check(NRF_RTC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of the RTC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
__STATIC_INLINE bool nrf_rtc_event_check(NRF_RTC_Type const * p_reg, nrf_rtc_event_t event);

/**
 * @brief Function for clearing an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
__STATIC_INLINE void nrf_rtc_event_clear(NRF_RTC_Type * p_reg, nrf_rtc_event_t event);

/**
 * @brief Function for returning a counter value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Counter value.
 */
uint32_t nrf_rtc_counter_get(NRF_RTC_Type const * p_reg);

/**
 * @brief Function for setting a prescaler value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] val   Value to set the prescaler to.
 */
__STATIC_INLINE void nrf_rtc_prescaler_set(NRF_RTC_Type * p_reg, uint32_t val);

/**
 * @brief Function for returning the address of an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the requested event register.
 */
__STATIC_INLINE uint32_t nrf_rtc_event_address_get(NRF_RTC_Type const * p_reg,
                                                     nrf_rtc_event_t      event);

/**
 * @brief Function for returning the address of a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the requested task register.
 */
__STATIC_INLINE uint32_t nrf_rtc_task_address_get(NRF_RTC_Type const * p_reg,
                                                    nrf_rtc_task_t       task);

/**
 * @brief Function for starting a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 */
void nrf_rtc_task_trigger(NRF_RTC_Type * p_reg, nrf_rtc_task_t task);

/**
 * @brief Function for enabling events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of event flags to be enabled.
 */
void nrf_rtc_event_enable(NRF_RTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 */
void nrf_rtc_event_disable(NRF_RTC_Type * p_reg, uint32_t event);

/*****************************/
/* Inlined functions bodies: */
/*****************************/

__STATIC_INLINE uint32_t nrf_rtc_cc_get(NRF_RTC_Type const * p_reg, uint32_t ch)
{
    return p_reg->CC[ch];
}

__STATIC_INLINE uint32_t nrf_rtc_int_enable_check(NRF_RTC_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

__STATIC_INLINE bool nrf_rtc_event_check(NRF_RTC_Type const * p_reg, nrf_rtc_event_t event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event);
}

__STATIC_INLINE void nrf_rtc_event_clear(NRF_RTC_Type * p_reg, nrf_rtc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
}

__STATIC_INLINE void nrf_rtc_prescaler_set(NRF_RTC_Type * p_reg, uint32_t val)
{
    p_reg->PRESCALER = val;
}

__STATIC_INLINE uint32_t nrf_rtc_event_address_get(NRF_RTC_Type const * p_reg,
                                                   nrf_rtc_event_t      event)
{
    return (uint32_t)p_reg + event;
}

__STATIC_INLINE uint32_t nrf_rtc_task_address_get(NRF_RTC_Type const * p_reg,
                                                  nrf_rtc_task_t       task)
{
    return (uint32_t)p_reg + task;
}

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_RTC_H__ */
