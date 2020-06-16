/*
 * Part of the real nrf_ccm.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_CCM_H__
#define BS_NRF_CCM_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CCM tasks.
 */
typedef enum
{
    /*lint -save -e30*/
    NRF_CCM_TASK_KSGEN        = offsetof(NRF_CCM_Type, TASKS_KSGEN),        ///< Start generation of key-stream.
    NRF_CCM_TASK_CRYPT        = offsetof(NRF_CCM_Type, TASKS_CRYPT),        ///< Start encryption/decryption.
    NRF_CCM_TASK_STOP         = offsetof(NRF_CCM_Type, TASKS_STOP),         ///< Stop encryption/decryption.
#if defined(CCM_RATEOVERRIDE_RATEOVERRIDE_Pos) || defined(__NRFX_DOXYGEN__)
    NRF_CCM_TASK_RATEOVERRIDE = offsetof(NRF_CCM_Type, TASKS_RATEOVERRIDE), ///< Override DATARATE setting in MODE register.
#endif
    /*lint -restore*/
} nrf_ccm_task_t;

/** @brief CCM events. */
typedef enum
{
    NRF_CCM_EVENT_ENDKSGEN = offsetof(NRF_CCM_Type, EVENTS_ENDKSGEN), ///< Keystream generation complete.
    NRF_CCM_EVENT_ENDCRYPT = offsetof(NRF_CCM_Type, EVENTS_ENDCRYPT), ///< Encrypt/decrypt complete.
    NRF_CCM_EVENT_ERROR    = offsetof(NRF_CCM_Type, EVENTS_ERROR),    ///< CCM error event.
} nrf_ccm_event_t;

/** @brief CCM interrupts. */
typedef enum
{
    NRF_CCM_INT_ENDKSGEN_MASK  = CCM_INTENSET_ENDKSGEN_Msk, ///< Interrupt on ENDKSGEN event.
    NRF_CCM_INT_ENDCRYPT_MASK  = CCM_INTENSET_ENDCRYPT_Msk, ///< Interrupt on ENDCRYPT event.
    NRF_CCM_INT_ERROR_MASK     = CCM_INTENSET_ERROR_Msk,    ///< Interrupt on ERROR event.
} nrf_ccm_int_mask_t;

/** @brief CCM modes of operation. */
typedef enum
{
    NRF_CCM_MODE_ENCRYPTION = CCM_MODE_MODE_Encryption, ///< Encryption mode.
    NRF_CCM_MODE_DECRYPTION = CCM_MODE_MODE_Decryption, ///< Decryption mode.
} nrf_ccm_mode_t;

#if defined(CCM_MODE_DATARATE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief CCM data rates. */
typedef enum
{
    NRF_CCM_DATARATE_1M   = CCM_MODE_DATARATE_1Mbit,   ///< 1 Mbps.
    NRF_CCM_DATARATE_2M   = CCM_MODE_DATARATE_2Mbit,   ///< 2 Mbps.
#if defined(CCM_MODE_DATARATE_125Kbps) || defined(__NRFX_DOXYGEN__)
    NRF_CCM_DATARATE_125K = CCM_MODE_DATARATE_125Kbps, ///< 125 Kbps.
#endif
#if defined(CCM_MODE_DATARATE_500Kbps) || defined(__NRFX_DOXYGEN__)
    NRF_CCM_DATARATE_500K = CCM_MODE_DATARATE_500Kbps, ///< 500 Kbps.
#endif
} nrf_ccm_datarate_t;
#endif // defined(CCM_MODE_DATARATE_Pos) || defined(__NRFX_DOXYGEN__)

#if defined(CCM_MODE_LENGTH_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief CCM packet length options. */
typedef enum
{
    NRF_CCM_LENGTH_DEFAULT  = CCM_MODE_LENGTH_Default,  ///< Default length.
    NRF_CCM_LENGTH_EXTENDED = CCM_MODE_LENGTH_Extended, ///< Extended length.
} nrf_ccm_length_t;
#endif // defined(CCM_MODE_LENGTH_Pos) || defined(__NRFX_DOXYGEN__)

/** @brief CCM configuration. */
typedef struct {
    nrf_ccm_mode_t     mode;     ///< Operation mode.
#if defined(CCM_MODE_DATARATE_Pos) || defined(__NRFX_DOXYGEN__)
    nrf_ccm_datarate_t datarate; ///< Data rate.
#endif
#if defined(CCM_MODE_LENGTH_Pos) || defined(__NRFX_DOXYGEN__)
    nrf_ccm_length_t   length;   ///< Lenght of the CCM packet.
#endif
} nrf_ccm_config_t;

void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg,
                                          nrf_ccm_task_t task);

/**
 * @brief Function for clearing a specific CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ccm_event_clear(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event);

/**
 * @brief Function for retrieving the state of a specific CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ccm_event_check(NRF_CCM_Type const * p_reg,
                                           nrf_ccm_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Interrupts to be enabled.
 */
void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Interrupts to be disabled.
 */
void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling the CCM peripheral.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_ccm_enable(NRF_CCM_Type * p_reg);

/**
 * @brief Function for disabling the CCM peripheral.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_ccm_disable(NRF_CCM_Type * p_reg);

/**
 * @brief Function for setting the CCM peripheral configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with configuration to be set.
 */
NRF_STATIC_INLINE void nrf_ccm_configure(NRF_CCM_Type *           p_reg,
                                         nrf_ccm_config_t const * p_config);

/**
 * @brief Function for getting the MIC check result.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The MIC check passed.
 * @retval false The MIC check failed.
 */
NRF_STATIC_INLINE bool nrf_ccm_micstatus_get(NRF_CCM_Type const * p_reg);

/**
 * @brief Function for setting the pointer to the data structure
 *        holding the AES key and the CCM NONCE vector.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the data structure.
 */
NRF_STATIC_INLINE void nrf_ccm_cnfptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data);

/**
 * @brief Function for setting the input data pointer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Input data pointer.
 */
NRF_STATIC_INLINE void nrf_ccm_inptr_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_data);

/**
 * @brief Function for setting the output data pointer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Output data pointer.
 */
NRF_STATIC_INLINE void nrf_ccm_outptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data);

/**
 * @brief Function for setting the pointer to the scratch area used for
 *        temporary storage.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_area Pointer to the scratch area.
 */
NRF_STATIC_INLINE void nrf_ccm_scratchptr_set(NRF_CCM_Type *   p_reg,
                                              uint32_t const * p_area);

/*****************************/
/* Inlined functions bodies: */
/*****************************/

NRF_STATIC_INLINE void nrf_ccm_event_clear(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
#if __CORTEX_M == 0x04
    volatile uint32_t dummy = *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event));
    (void)dummy;
#endif
}

NRF_STATIC_INLINE bool nrf_ccm_event_check(NRF_CCM_Type const * p_reg,
                                           nrf_ccm_event_t      event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_ccm_enable(NRF_CCM_Type * p_reg)
{
    p_reg->ENABLE = (CCM_ENABLE_ENABLE_Enabled << CCM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_ccm_disable(NRF_CCM_Type * p_reg)
{
    p_reg->ENABLE = (CCM_ENABLE_ENABLE_Disabled << CCM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_ccm_configure(NRF_CCM_Type *           p_reg,
                                         nrf_ccm_config_t const * p_config)
{
    p_reg->MODE = (((uint32_t)p_config->mode     << CCM_MODE_MODE_Pos) |
#if defined(CCM_MODE_DATARATE_Pos)
                   ((uint32_t)p_config->datarate << CCM_MODE_DATARATE_Pos) |
#endif
#if defined(CCM_MODE_LENGTH_Pos)
                   ((uint32_t)p_config->length   << CCM_MODE_LENGTH_Pos) |
#endif
                   0);
}

NRF_STATIC_INLINE bool nrf_ccm_micstatus_get(NRF_CCM_Type const * p_reg)
{
    return (bool)(p_reg->MICSTATUS);
}

NRF_STATIC_INLINE void nrf_ccm_cnfptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data)
{
    p_reg->CNFPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE void nrf_ccm_inptr_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_data)
{
    p_reg->INPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE void nrf_ccm_outptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data)
{
    p_reg->OUTPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE void nrf_ccm_scratchptr_set(NRF_CCM_Type *   p_reg,
                                              uint32_t const * p_area)
{
    p_reg->SCRATCHPTR = (uint32_t)p_area;
}

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_CCM_H__ */
