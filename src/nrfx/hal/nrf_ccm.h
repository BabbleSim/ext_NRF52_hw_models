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


void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg,
                                          nrf_ccm_task_t task);

void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask);

void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask);


#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_CCM_H__ */
