/**
 * Part of the real nrf_ppi.h from Nordic's NRFx HAL
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


#ifndef BS_NRF_PPI_H__
#define BS_NRF_PPI_H__

#include "nrf_soc_if.h"
#include "../drivers/nrfx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ppi_hal PPI HAL
 * @{
 * @ingroup nrf_ppi
 * @brief   Hardware access layer for managing the Programmable Peripheral Interconnect (PPI)
 *          channels.
 */

#define NRF_PPI_TASK_SET    (1UL)

/**
 * @enum nrf_ppi_channel_t
 * @brief PPI channels.
 */
typedef enum
{
    NRF_PPI_CHANNEL0  = PPI_CHEN_CH0_Pos,  /**< Channel 0. */
    NRF_PPI_CHANNEL1  = PPI_CHEN_CH1_Pos,  /**< Channel 1. */
    NRF_PPI_CHANNEL2  = PPI_CHEN_CH2_Pos,  /**< Channel 2. */
    NRF_PPI_CHANNEL3  = PPI_CHEN_CH3_Pos,  /**< Channel 3. */
    NRF_PPI_CHANNEL4  = PPI_CHEN_CH4_Pos,  /**< Channel 4. */
    NRF_PPI_CHANNEL5  = PPI_CHEN_CH5_Pos,  /**< Channel 5. */
    NRF_PPI_CHANNEL6  = PPI_CHEN_CH6_Pos,  /**< Channel 6. */
    NRF_PPI_CHANNEL7  = PPI_CHEN_CH7_Pos,  /**< Channel 7. */
    NRF_PPI_CHANNEL8  = PPI_CHEN_CH8_Pos,  /**< Channel 8. */
    NRF_PPI_CHANNEL9  = PPI_CHEN_CH9_Pos,  /**< Channel 9. */
    NRF_PPI_CHANNEL10 = PPI_CHEN_CH10_Pos, /**< Channel 10. */
    NRF_PPI_CHANNEL11 = PPI_CHEN_CH11_Pos, /**< Channel 11. */
    NRF_PPI_CHANNEL12 = PPI_CHEN_CH12_Pos, /**< Channel 12. */
    NRF_PPI_CHANNEL13 = PPI_CHEN_CH13_Pos, /**< Channel 13. */
    NRF_PPI_CHANNEL14 = PPI_CHEN_CH14_Pos, /**< Channel 14. */
    NRF_PPI_CHANNEL15 = PPI_CHEN_CH15_Pos, /**< Channel 15. */
#if (PPI_CH_NUM > 16) || defined(__NRFX_DOXYGEN__)
    NRF_PPI_CHANNEL16 = PPI_CHEN_CH16_Pos, /**< Channel 16. */
    NRF_PPI_CHANNEL17 = PPI_CHEN_CH17_Pos, /**< Channel 17. */
    NRF_PPI_CHANNEL18 = PPI_CHEN_CH18_Pos, /**< Channel 18. */
    NRF_PPI_CHANNEL19 = PPI_CHEN_CH19_Pos, /**< Channel 19. */
#endif
    NRF_PPI_CHANNEL20 = PPI_CHEN_CH20_Pos, /**< Channel 20. */
    NRF_PPI_CHANNEL21 = PPI_CHEN_CH21_Pos, /**< Channel 21. */
    NRF_PPI_CHANNEL22 = PPI_CHEN_CH22_Pos, /**< Channel 22. */
    NRF_PPI_CHANNEL23 = PPI_CHEN_CH23_Pos, /**< Channel 23. */
    NRF_PPI_CHANNEL24 = PPI_CHEN_CH24_Pos, /**< Channel 24. */
    NRF_PPI_CHANNEL25 = PPI_CHEN_CH25_Pos, /**< Channel 25. */
    NRF_PPI_CHANNEL26 = PPI_CHEN_CH26_Pos, /**< Channel 26. */
    NRF_PPI_CHANNEL27 = PPI_CHEN_CH27_Pos, /**< Channel 27. */
    NRF_PPI_CHANNEL28 = PPI_CHEN_CH28_Pos, /**< Channel 28. */
    NRF_PPI_CHANNEL29 = PPI_CHEN_CH29_Pos, /**< Channel 29. */
    NRF_PPI_CHANNEL30 = PPI_CHEN_CH30_Pos, /**< Channel 30. */
    NRF_PPI_CHANNEL31 = PPI_CHEN_CH31_Pos  /**< Channel 31. */
} nrf_ppi_channel_t;

/**
 * @enum nrf_ppi_channel_group_t
 * @brief PPI channel groups.
 */
typedef enum
{
    NRF_PPI_CHANNEL_GROUP0 = 0, /**< Channel group 0. */
    NRF_PPI_CHANNEL_GROUP1 = 1, /**< Channel group 1. */
    NRF_PPI_CHANNEL_GROUP2 = 2, /**< Channel group 2. */
    NRF_PPI_CHANNEL_GROUP3 = 3, /**< Channel group 3. */
#if (PPI_GROUP_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_PPI_CHANNEL_GROUP4 = 4, /**< Channel group 4. */
    NRF_PPI_CHANNEL_GROUP5 = 5  /**< Channel group 5. */
#endif
} nrf_ppi_channel_group_t;

void nrf_ppi_channels_enable(NRF_PPI_Type * p_reg, uint32_t mask);
void nrf_ppi_channels_disable(NRF_PPI_Type * p_reg, uint32_t mask);
void nrf_ppi_channel_endpoint_setup(NRF_PPI_Type *    p_reg,
                                    nrf_ppi_channel_t channel,
                                    uint32_t          eep,
                                    uint32_t          tep);
void nrf_ppi_event_endpoint_setup(NRF_PPI_Type *    p_reg,
                                  nrf_ppi_channel_t channel,
                                  uint32_t          eep);
void nrf_ppi_task_endpoint_setup(NRF_PPI_Type *    p_reg,
                                 nrf_ppi_channel_t channel,
                                 uint32_t          tep);
void nrf_ppi_group_disable(NRF_PPI_Type * p_reg, nrf_ppi_channel_group_t group);

#ifdef __cplusplus
}
#endif

#endif /* BS_NRF_PPI_H__ */
