/**
 * Part of the real nrf_radio.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_RADIO_H__
#define BS_NRF_RADIO_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum nrf_radio_task_t
 * @brief RADIO tasks.
 */
typedef enum /*lint -save -e30 -esym(628,__INTADDR__) */
{
    NRF_RADIO_TASK_TXEN      = offsetof(NRF_RADIO_Type, TASKS_TXEN),      /**< Enable radio transmitter. */
    NRF_RADIO_TASK_RXEN      = offsetof(NRF_RADIO_Type, TASKS_RXEN),      /**< Enable radio receiver. */
    NRF_RADIO_TASK_START     = offsetof(NRF_RADIO_Type, TASKS_START),     /**< Start radio transmission or reception. */
    NRF_RADIO_TASK_STOP      = offsetof(NRF_RADIO_Type, TASKS_STOP),      /**< Stop radio transmission or reception. */
    NRF_RADIO_TASK_DISABLE   = offsetof(NRF_RADIO_Type, TASKS_DISABLE),   /**< Disable radio transmitter and receiver. */
//    Only present in NRF52840
//    NRF_RADIO_TASK_CCASTART  = offsetof(NRF_RADIO_Type, TASKS_CCASTART),  /**< Start Clear Channel Assessment procedure. */
//    NRF_RADIO_TASK_CCASTOP   = offsetof(NRF_RADIO_Type, TASKS_CCASTOP),   /**< Stop Clear Channel Assessment procedure. */
//    NRF_RADIO_TASK_EDSTART   = offsetof(NRF_RADIO_Type, TASKS_EDSTART),   /**< Start Energy Detection procedure. */
    NRF_RADIO_TASK_RSSISTART = offsetof(NRF_RADIO_Type, TASKS_RSSISTART), /**< Start the RSSI and take one single sample of received signal strength. */
} nrf_radio_task_t;                                                       /*lint -restore */

/**
 * @enum nrf_radio_event_t
 * @brief RADIO events.
 */
typedef enum /*lint -save -e30 -esym(628,__INTADDR__) */
{
    NRF_RADIO_EVENT_READY      = offsetof(NRF_RADIO_Type, EVENTS_READY),      /**< Radio has ramped up and is ready to be started. */
    NRF_RADIO_EVENT_ADDRESS    = offsetof(NRF_RADIO_Type, EVENTS_ADDRESS),    /**< Address sent or received. */
    NRF_RADIO_EVENT_END        = offsetof(NRF_RADIO_Type, EVENTS_END),        /**< Packet transmitted or received. */
    NRF_RADIO_EVENT_DISABLED   = offsetof(NRF_RADIO_Type, EVENTS_DISABLED),   /**< Radio has been disabled. */
    NRF_RADIO_EVENT_CRCOK      = offsetof(NRF_RADIO_Type, EVENTS_CRCOK),      /**< Packet received with correct CRC. */
    NRF_RADIO_EVENT_CRCERROR   = offsetof(NRF_RADIO_Type, EVENTS_CRCERROR),   /**< Packet received with incorrect CRC. */
//    NRF_RADIO_EVENT_CCAIDLE    = offsetof(NRF_RADIO_Type, EVENTS_CCAIDLE),    /**< Wireless medium is idle. */
//    NRF_RADIO_EVENT_CCABUSY    = offsetof(NRF_RADIO_Type, EVENTS_CCABUSY),    /**< Wireless medium is busy. */
    NRF_RADIO_EVENT_RSSIEND    = offsetof(NRF_RADIO_Type, EVENTS_RSSIEND),    /**< Sampling of receive signal strength complete. */
//    NRF_RADIO_EVENT_MHRMATCH   = offsetof(NRF_RADIO_Type, EVENTS_MHRMATCH),   /**< MAC Header match found. */
//    NRF_RADIO_EVENT_FRAMESTART = offsetof(NRF_RADIO_Type, EVENTS_FRAMESTART), /**< IEEE 802.15.4 length field received. */
//    NRF_RADIO_EVENT_EDEND      = offsetof(NRF_RADIO_Type, EVENTS_EDEND),      /**< Energy Detection procedure ended. */
    NRF_RADIO_EVENT_BCMATCH    = offsetof(NRF_RADIO_Type, EVENTS_BCMATCH),    /**< Bit counter reached bit count value. */
} nrf_radio_event_t;                                                          /*lint -restore */

/**
 * @enum nrf_radio_int_mask_t
 * @brief RADIO interrupts.
 */
typedef enum
{
    NRF_RADIO_INT_READY_MASK      = RADIO_INTENSET_READY_Msk,      /**< Mask for enabling or disabling an interrupt on READY event.  */
    NRF_RADIO_INT_ADDRESS_MASK    = RADIO_INTENSET_ADDRESS_Msk,    /**< Mask for enabling or disabling an interrupt on ADDRESS event. */
    NRF_RADIO_INT_END_MASK        = RADIO_INTENSET_END_Msk,        /**< Mask for enabling or disabling an interrupt on END event. */
    NRF_RADIO_INT_DISABLED_MASK   = RADIO_INTENSET_DISABLED_Msk,   /**< Mask for enabling or disabling an interrupt on DISABLED event. */
    NRF_RADIO_INT_CRCOK_MASK      = RADIO_INTENSET_CRCOK_Msk,      /**< Mask for enabling or disabling an interrupt on CRCOK event. */
    NRF_RADIO_INT_CRCERROR_MASK   = RADIO_INTENSET_CRCERROR_Msk,   /**< Mask for enabling or disabling an interrupt on CRCERROR event. */
//    NRF_RADIO_INT_CCAIDLE_MASK    = RADIO_INTENSET_CCAIDLE_Msk,    /**< Mask for enabling or disabling an interrupt on CCAIDLE event. */
//    NRF_RADIO_INT_CCABUSY_MASK    = RADIO_INTENSET_CCABUSY_Msk,    /**< Mask for enabling or disabling an interrupt on CCABUSY event. */
    NRF_RADIO_INT_RSSIEND_MASK    = RADIO_INTENSET_RSSIEND_Msk,    /**< Mask for enabling or disabling an interrupt on RSSIEND event. */
//    NRF_RADIO_INT_FRAMESTART_MASK = RADIO_INTENSET_FRAMESTART_Msk, /**< Mask for enabling or disabling an interrupt on FRAMESTART event. */
//    NRF_RADIO_INT_EDEND_MASK      = RADIO_INTENSET_EDEND_Msk,      /**< Mask for enabling or disabling an interrupt on EDEND event. */
    NRF_RADIO_INT_BCMATCH_MASK    = RADIO_INTENSET_BCMATCH_Msk,    /**< Mask for enabling or disabling an interrupt on BCMATCH event. */
} nrf_radio_int_mask_t;

/**
 * @brief Function for activating a specific RADIO task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
void nrf_radio_task_trigger(NRF_RADIO_Type * p_reg, nrf_radio_task_t task);

/**
 * @brief Function for enabling specified RADIO interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 */
void nrf_radio_int_enable(NRF_RADIO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for setting Bit counter compare.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] radio_bcc Bit counter compare [bits].
 */
void nrf_radio_bcc_set(NRF_RADIO_Type * p_reg, uint32_t radio_bcc);

/**
 * @brief Function for setting power mode of the radio peripheral.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] radio_power If radio should be powered on.
 */
void nrf_radio_power_set(NRF_RADIO_Type * p_reg, bool radio_power);

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_RADIO_H__ */
