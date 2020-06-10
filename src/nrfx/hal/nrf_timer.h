/**
 * Part of the real nrf_timer.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_TIMER_H__
#define BS_NRF_TIMER_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer tasks.
 */
typedef enum
{
    /*lint -save -e30 -esym(628,__INTADDR__)*/
    NRF_TIMER_TASK_START    = offsetof(NRF_TIMER_Type, TASKS_START),      ///< Task for starting the timer.
    NRF_TIMER_TASK_STOP     = offsetof(NRF_TIMER_Type, TASKS_STOP),       ///< Task for stopping the timer.
    NRF_TIMER_TASK_COUNT    = offsetof(NRF_TIMER_Type, TASKS_COUNT),      ///< Task for incrementing the timer (in counter mode).
    NRF_TIMER_TASK_CLEAR    = offsetof(NRF_TIMER_Type, TASKS_CLEAR),      ///< Task for resetting the timer value.
    NRF_TIMER_TASK_SHUTDOWN = offsetof(NRF_TIMER_Type, TASKS_SHUTDOWN),   ///< Task for powering off the timer.
    NRF_TIMER_TASK_CAPTURE0 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[0]), ///< Task for capturing the timer value on channel 0.
    NRF_TIMER_TASK_CAPTURE1 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[1]), ///< Task for capturing the timer value on channel 1.
    NRF_TIMER_TASK_CAPTURE2 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[2]), ///< Task for capturing the timer value on channel 2.
    NRF_TIMER_TASK_CAPTURE3 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[3]), ///< Task for capturing the timer value on channel 3.
#if defined(TIMER_INTENSET_COMPARE4_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_TASK_CAPTURE4 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[4]), ///< Task for capturing the timer value on channel 4.
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_TASK_CAPTURE5 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[5]), ///< Task for capturing the timer value on channel 5.
#endif
#if defined(TIMER_INTENSET_COMPARE6_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_TASK_CAPTURE6 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[6]), ///< Task for capturing the timer value on channel 6.
#endif
#if defined(TIMER_INTENSET_COMPARE7_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_TASK_CAPTURE7 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[7]), ///< Task for capturing the timer value on channel 7.
#endif
    /*lint -restore*/
} nrf_timer_task_t;

/** @brief Timer events. */
typedef enum
{
    NRF_TIMER_EVENT_COMPARE0 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[0]), ///< Event from compare channel 0.
    NRF_TIMER_EVENT_COMPARE1 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[1]), ///< Event from compare channel 1.
    NRF_TIMER_EVENT_COMPARE2 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[2]), ///< Event from compare channel 2.
    NRF_TIMER_EVENT_COMPARE3 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[3]), ///< Event from compare channel 3.
#if defined(TIMER_INTENSET_COMPARE4_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_EVENT_COMPARE4 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[4]), ///< Event from compare channel 4.
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_EVENT_COMPARE5 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[5]), ///< Event from compare channel 5.
#endif
#if defined(TIMER_INTENSET_COMPARE6_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_EVENT_COMPARE6 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[6]), ///< Event from compare channel 6.
#endif
#if defined(TIMER_INTENSET_COMPARE7_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_EVENT_COMPARE7 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[7]), ///< Event from compare channel 7.
#endif
} nrf_timer_event_t;

/** @brief Timer modes. */
typedef enum
{
    NRF_TIMER_MODE_TIMER             = TIMER_MODE_MODE_Timer,           ///< Timer mode: timer.
    NRF_TIMER_MODE_COUNTER           = TIMER_MODE_MODE_Counter,         ///< Timer mode: counter.
#if defined(TIMER_MODE_MODE_LowPowerCounter) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_MODE_LOW_POWER_COUNTER = TIMER_MODE_MODE_LowPowerCounter, ///< Timer mode: low-power counter.
#endif
} nrf_timer_mode_t;

/** @brief Timer bit width. */
typedef enum
{
    NRF_TIMER_BIT_WIDTH_8  = TIMER_BITMODE_BITMODE_08Bit, ///< Timer bit width 8 bit.
    NRF_TIMER_BIT_WIDTH_16 = TIMER_BITMODE_BITMODE_16Bit, ///< Timer bit width 16 bit.
    NRF_TIMER_BIT_WIDTH_24 = TIMER_BITMODE_BITMODE_24Bit, ///< Timer bit width 24 bit.
    NRF_TIMER_BIT_WIDTH_32 = TIMER_BITMODE_BITMODE_32Bit  ///< Timer bit width 32 bit.
} nrf_timer_bit_width_t;

/** @brief Timer prescalers. */
typedef enum
{
    NRF_TIMER_FREQ_16MHz = 0, ///< Timer frequency 16 MHz.
    NRF_TIMER_FREQ_8MHz,      ///< Timer frequency 8 MHz.
    NRF_TIMER_FREQ_4MHz,      ///< Timer frequency 4 MHz.
    NRF_TIMER_FREQ_2MHz,      ///< Timer frequency 2 MHz.
    NRF_TIMER_FREQ_1MHz,      ///< Timer frequency 1 MHz.
    NRF_TIMER_FREQ_500kHz,    ///< Timer frequency 500 kHz.
    NRF_TIMER_FREQ_250kHz,    ///< Timer frequency 250 kHz.
    NRF_TIMER_FREQ_125kHz,    ///< Timer frequency 125 kHz.
    NRF_TIMER_FREQ_62500Hz,   ///< Timer frequency 62500 Hz.
    NRF_TIMER_FREQ_31250Hz    ///< Timer frequency 31250 Hz.
} nrf_timer_frequency_t;

/**
 * @brief Timer capture/compare channels.
 */
typedef enum
{
    NRF_TIMER_CC_CHANNEL0 = 0, ///< Timer capture/compare channel 0.
    NRF_TIMER_CC_CHANNEL1,     ///< Timer capture/compare channel 1.
    NRF_TIMER_CC_CHANNEL2,     ///< Timer capture/compare channel 2.
    NRF_TIMER_CC_CHANNEL3,     ///< Timer capture/compare channel 3.
#if defined(TIMER_INTENSET_COMPARE4_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_CC_CHANNEL4,     ///< Timer capture/compare channel 4.
#endif
#if defined(TIMER_INTENSET_COMPARE5_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_TIMER_CC_CHANNEL5,     ///< Timer capture/compare channel 5.
#endif
} nrf_timer_cc_channel_t;

/**
 * @brief Function for activating the specified timer task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
void nrf_timer_task_trigger(NRF_TIMER_Type * p_reg,
                            nrf_timer_task_t task);

/**
 * @brief Function for clearing the specified timer event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_timer_event_clear(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event);

/**
 * @brief Function for retrieving the state of the TIMER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_timer_event_check(NRF_TIMER_Type const * p_reg,
                                             nrf_timer_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_timer_shorts_enable(NRF_TIMER_Type * p_reg,
                                               uint32_t         mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_timer_shorts_disable(NRF_TIMER_Type * p_reg,
                                                uint32_t         mask);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 */
void nrf_timer_int_enable(NRF_TIMER_Type * p_reg,
                          uint32_t         mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 */
void nrf_timer_int_disable(NRF_TIMER_Type * p_reg,
                           uint32_t         mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_int_enable_check(NRF_TIMER_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for setting the timer mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Timer mode.
 */
void nrf_timer_mode_set(NRF_TIMER_Type * p_reg,
                        nrf_timer_mode_t mode);

/**
 * @brief Function for setting the timer bit width.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] bit_width Timer bit width.
 */
NRF_STATIC_INLINE void nrf_timer_bit_width_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_bit_width_t bit_width);

/**
 * @brief Function for setting the timer frequency.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency Timer frequency.
 */
NRF_STATIC_INLINE void nrf_timer_frequency_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_frequency_t frequency);

/**
 * @brief Function for setting the capture/compare register for the specified channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 * @param[in] cc_value   Value to write to the capture/compare register.
 */
void nrf_timer_cc_set(NRF_TIMER_Type *       p_reg,
                      nrf_timer_cc_channel_t cc_channel,
                      uint32_t               cc_value);

/**
 * @brief Function for retrieving the capture/compare value for a specified channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 *
 * @return Value from the specified capture/compare register.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_cc_get(NRF_TIMER_Type const * p_reg,
                                            nrf_timer_cc_channel_t cc_channel);

/**
 * @brief Function for getting the specified timer capture task.
 *
 * @param[in] channel Capture channel.
 *
 * @return Capture task.
 */
NRF_STATIC_INLINE nrf_timer_task_t nrf_timer_capture_task_get(uint32_t channel);

/**
 * @brief Function for getting the specified timer compare event.
 *
 * @param[in] channel Compare channel.
 *
 * @return Compare event.
 */
NRF_STATIC_INLINE nrf_timer_event_t nrf_timer_compare_event_get(uint32_t channel);

/*****************************/
/* Inlined functions bodies: */
/*****************************/

NRF_STATIC_INLINE void nrf_timer_event_clear(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
#if __CORTEX_M == 0x04
    volatile uint32_t dummy = *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event));
    (void)dummy;
#endif
}

NRF_STATIC_INLINE bool nrf_timer_event_check(NRF_TIMER_Type const * p_reg,
                                             nrf_timer_event_t      event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_timer_shorts_enable(NRF_TIMER_Type * p_reg,
                                               uint32_t         mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_timer_shorts_disable(NRF_TIMER_Type * p_reg,
                                                uint32_t         mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE uint32_t nrf_timer_int_enable_check(NRF_TIMER_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_timer_bit_width_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_bit_width_t bit_width)
{
    p_reg->BITMODE = (p_reg->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) |
                     ((bit_width << TIMER_BITMODE_BITMODE_Pos) &
                     TIMER_BITMODE_BITMODE_Msk);
}

NRF_STATIC_INLINE void nrf_timer_frequency_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_frequency_t frequency)
{
    p_reg->PRESCALER = (p_reg->PRESCALER & ~TIMER_PRESCALER_PRESCALER_Msk) |
                       ((frequency << TIMER_PRESCALER_PRESCALER_Pos) &
                       TIMER_PRESCALER_PRESCALER_Msk);
}

NRF_STATIC_INLINE uint32_t nrf_timer_cc_get(NRF_TIMER_Type const * p_reg,
                                            nrf_timer_cc_channel_t cc_channel)
{
    return (uint32_t)p_reg->CC[cc_channel];
}

NRF_STATIC_INLINE nrf_timer_task_t nrf_timer_capture_task_get(uint32_t channel)
{
    return (nrf_timer_task_t)NRFX_OFFSETOF(NRF_TIMER_Type, TASKS_CAPTURE[channel]);
}

NRF_STATIC_INLINE nrf_timer_event_t nrf_timer_compare_event_get(uint32_t channel)
{
    return (nrf_timer_event_t)NRFX_OFFSETOF(NRF_TIMER_Type, EVENTS_COMPARE[channel]);
}

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_TIMER_H__ */
