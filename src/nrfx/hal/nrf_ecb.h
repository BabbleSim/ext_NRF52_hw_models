/*
 * Part of the real nrf_ecb.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_ECB_H__
#define BS_NRF_ECB_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ecb_hal AES ECB encryption HAL
 * @{
 * @ingroup nrf_ecb
 * @brief   Hardware access layer for managing the AES Electronic Codebook (ECB) peripheral.
 */

/**
 * @brief ECB tasks.
 */
typedef enum
{
    /*lint -save -e30 -esym(628,__INTADDR__)*/
    NRF_ECB_TASK_STARTECB = offsetof(NRF_ECB_Type, TASKS_STARTECB), /**< Task for starting ECB block encryption. */
    NRF_ECB_TASK_STOPECB  = offsetof(NRF_ECB_Type, TASKS_STOPECB),  /**< Task for stopping ECB block encryption. */
    /*lint -restore*/
} nrf_ecb_task_t;

/**
 * @brief ECB events.
 */
typedef enum
{
    /*lint -save -e30*/
    NRF_ECB_EVENT_ENDECB   = offsetof(NRF_ECB_Type, EVENTS_ENDECB),   /**< ECB block encrypt complete. */
    NRF_ECB_EVENT_ERRORECB = offsetof(NRF_ECB_Type, EVENTS_ERRORECB), /**< ECB block encrypt aborted because of a STOPECB task or due to an error. */
    /*lint -restore*/
} nrf_ecb_event_t;

/**
 * @brief ECB interrupts.
 */
typedef enum
{
    NRF_ECB_INT_ENDECB_MASK   = ECB_INTENSET_ENDECB_Msk,   ///< Interrupt on ENDECB event.
    NRF_ECB_INT_ERRORECB_MASK = ECB_INTENSET_ERRORECB_Msk, ///< Interrupt on ERRORECB event.
} nrf_ecb_int_mask_t;


/**
 * @brief Function for activating a specific ECB task.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] task  Task to activate.
 */
void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task);

/**
 * @brief Function for clearing the specified ECB event.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event);

/**
 * @brief Function for retrieving the state of the ECB event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ecb_event_check(NRF_ECB_Type const * p_reg, nrf_ecb_event_t event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Interrupts to enable.
 */
void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Interrupts to disable.
 */
void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask);

/**
 * @brief Function for setting the pointer to the ECB data buffer.
 *
 * @note The buffer has to be placed in the Data RAM region.
 *       For description of the data structure in this buffer, see the Product Specification.
 *
 * @param[in] p_reg    Pointer to the peripheral register structure.
 * @param[in] p_buffer Pointer to the ECB data buffer.
 */
NRF_STATIC_INLINE void nrf_ecb_data_pointer_set(NRF_ECB_Type * p_reg, void const * p_buffer);

/**
 * @brief Function for getting the pointer to the ECB data buffer.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 *
 * @return Pointer to the ECB data buffer.
 */
NRF_STATIC_INLINE void * nrf_ecb_data_pointer_get(NRF_ECB_Type const * p_reg);

/*****************************/
/* Inlined functions bodies: */
/*****************************/

NRF_STATIC_INLINE void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
#if __CORTEX_M == 0x04
    volatile uint32_t dummy = *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event));
    (void)dummy;
#endif
}

NRF_STATIC_INLINE bool nrf_ecb_event_check(NRF_ECB_Type const * p_reg, nrf_ecb_event_t event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_ecb_data_pointer_set(NRF_ECB_Type * p_reg, void const * p_buffer)
{
    p_reg->ECBDATAPTR = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE void * nrf_ecb_data_pointer_get(NRF_ECB_Type const * p_reg)
{
    return (void *)(p_reg->ECBDATAPTR);
}

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_ECB_H__ */
