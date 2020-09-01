/**
 * Part of the real nrf_aar.h from Nordic's NRFx HAL
 *
 * Note that there is a few changes compared to the original
 * Where relevant, these changes are:
 * Copyright (c) 2020 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Copyright (c) 2019 - 2020, Nordic Semiconductor ASA
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

#ifndef BS_NRF_AAR_H__
#define BS_NRF_AAR_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief AAR events. */
typedef enum
{
    NRF_AAR_EVENT_END         = offsetof(NRF_AAR_Type, EVENTS_END),         ///< Address resolution procedure complete.
    NRF_AAR_EVENT_RESOLVED    = offsetof(NRF_AAR_Type, EVENTS_RESOLVED),    ///< Address resolved.
    NRF_AAR_EVENT_NOTRESOLVED = offsetof(NRF_AAR_Type, EVENTS_NOTRESOLVED), ///< Address not resolved.
} nrf_aar_event_t;

/** @brief AAR tasks. */
typedef enum
{
    NRF_AAR_TASK_START = offsetof(NRF_AAR_Type, TASKS_START), ///< Start address resolution procedure.
    NRF_AAR_TASK_STOP  = offsetof(NRF_AAR_Type, TASKS_STOP),  ///< Stop address resolution procedure.
} nrf_aar_task_t;

/**
 * @brief Function for retrieving the state of the AAR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  Event is set.
 * @retval false Event is not set.
 */
NRF_STATIC_INLINE bool nrf_aar_event_check(NRF_AAR_Type const * p_reg,
                                           nrf_aar_event_t      event);

/**
 * @brief Function for clearing the specified AAR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_aar_event_clear(NRF_AAR_Type *  p_reg,
                                           nrf_aar_event_t event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 */
void nrf_aar_int_enable(NRF_AAR_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 */
void nrf_aar_int_disable(NRF_AAR_Type * p_reg, uint32_t mask);

/**
 * @brief Function for starting an AAR task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
void nrf_aar_task_trigger(NRF_AAR_Type * p_reg, nrf_aar_task_t task);

/**
 * @brief Function for enabling AAR.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_aar_enable(NRF_AAR_Type * p_reg);

/**
 * @brief Function for disabling AAR.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_aar_disable(NRF_AAR_Type * p_reg);

/**
 * @brief Function for setting the pointer to the Identity Resolving Keys (IRK) data structure.
 *
 * The size of the provided data structure must correspond to the number of keys available.
 * Each key occupies 16 bytes.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irk_ptr Pointer to the IRK data structure. Must point to the Data RAM region.
 *
 * @sa nrf_aar_irk_number_set
 */
NRF_STATIC_INLINE void nrf_aar_irk_pointer_set(NRF_AAR_Type * p_reg, uint8_t const * irk_ptr);

/**
 * @brief Function for setting the number of keys available in the Identity Resolving Keys
 *        data structure.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irk_num Number of keys available in the IRK data structure. Maximum is 16.
 *                    Must correspond to the size of the provided IRK data structure.
 *
 * @sa nrf_aar_irk_pointer_set
 */
NRF_STATIC_INLINE void nrf_aar_irk_number_set(NRF_AAR_Type * p_reg, uint8_t irk_num);

/**
 * @brief Function for setting the pointer to the resolvable address.
 *
 * The resolvable address must consist of 6 bytes.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] addr_ptr Pointer to the address to resolve using the available IRK keys.
 *                     Must point to the Data RAM region.
 */
NRF_STATIC_INLINE void nrf_aar_addr_pointer_set(NRF_AAR_Type * p_reg, uint8_t const * addr_ptr);

/**
 * @brief Function for setting the pointer to the scratch data area.
 *
 * The scratch data area is used for temporary storage during the address resolution procedure.
 * A space of minimum 3 bytes must be reserved for the scratch data area.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] scratch_ptr Pointer to the scratch data area. Must point to the Data RAM region.
 */
NRF_STATIC_INLINE void nrf_aar_scratch_pointer_set(NRF_AAR_Type * p_reg, uint8_t * scratch_ptr);

/**
 * @brief Function for getting the index of the Identity Resolving Key that was used
 *        the last time an address was resolved.
 *
 * This function can be used to get the IRK index that matched the resolvable address,
 * provided that @ref NRF_AAR_EVENT_RESOLVED occured. Otherwise, it will return
 * the index of the last IRK stored in the IRK data structure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The index of the IRK that was used the last time an address was resolved.
 */
NRF_STATIC_INLINE uint8_t nrf_aar_resolution_status_get(NRF_AAR_Type const * p_reg);

/*****************************/
/* Inlined functions bodies: */
/*****************************/

NRF_STATIC_INLINE bool nrf_aar_event_check(NRF_AAR_Type const * p_reg,
                                           nrf_aar_event_t      aar_event)
{
    return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)aar_event);
}

NRF_STATIC_INLINE void nrf_aar_event_clear(NRF_AAR_Type *  p_reg,
                                           nrf_aar_event_t aar_event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)aar_event)) = 0;
#if __CORTEX_M == 0x04
    volatile uint32_t dummy = *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)aar_event));
    (void)dummy;
#endif
}

NRF_STATIC_INLINE void nrf_aar_enable(NRF_AAR_Type * p_reg)
{
    p_reg->ENABLE = AAR_ENABLE_ENABLE_Enabled << AAR_ENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE void nrf_aar_disable(NRF_AAR_Type * p_reg)
{
    p_reg->ENABLE = AAR_ENABLE_ENABLE_Disabled << AAR_ENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE void nrf_aar_irk_pointer_set(NRF_AAR_Type * p_reg, uint8_t const * irk_ptr)
{
    p_reg->IRKPTR = (uint32_t)irk_ptr;
}

NRF_STATIC_INLINE void nrf_aar_irk_number_set(NRF_AAR_Type * p_reg, uint8_t irk_num)
{
    p_reg->NIRK = irk_num;
}

NRF_STATIC_INLINE void nrf_aar_addr_pointer_set(NRF_AAR_Type * p_reg, uint8_t const * addr_ptr)
{
    p_reg->ADDRPTR = (uint32_t)addr_ptr;
}

NRF_STATIC_INLINE void nrf_aar_scratch_pointer_set(NRF_AAR_Type * p_reg, uint8_t * scratch_ptr)
{
    p_reg->SCRATCHPTR = (uint32_t)scratch_ptr;
}

NRF_STATIC_INLINE uint8_t nrf_aar_resolution_status_get(NRF_AAR_Type const * p_reg)
{
    return (uint8_t)(p_reg->STATUS);
}

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_AAR_H__ */
