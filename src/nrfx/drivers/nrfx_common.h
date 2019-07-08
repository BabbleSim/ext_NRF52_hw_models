/*
 * Part of the real nrf_common.h from Nordic's NRFx drivers
 *
 * Note that there is a few changes compared to the original
 * Where relevant, these changes are:
 * Copyright (c) 2019 Oticon A/S
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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

#ifndef BS_NRFX_COMMON_H__
#define BS_NRFX_COMMON_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../hal/nrf_soc_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_common Common module
 * @{
 * @ingroup nrfx
 * @brief Common module.
 */

/**
 * @brief Macro for checking if the specified identifier is defined and it has
 *        a non-zero value.
 *
 * Normally, preprocessors treat all undefined identifiers as having the value
 * zero. However, some tools, like static code analyzers, can issue a warning
 * when such identifier is evaluated. This macro gives the possibility to suppress
 * such warnings only in places where this macro is used for evaluation, not in
 * the whole analyzed code.
 */
#define NRFX_CHECK(module_enabled)  (module_enabled)

/**
 * @brief Macro for concatenating two tokens in macro expansion.
 *
 * @note This macro is expanded in two steps so that tokens given as macros
 *       themselves are fully expanded before they are merged.
 *
 * @param[in] p1 First token.
 * @param[in] p2 Second token.
 *
 * @return The two tokens merged into one, unless they cannot together form
 *         a valid token (in such case, the preprocessor issues a warning and
 *         does not perform the concatenation).
 *
 * @sa NRFX_CONCAT_3
 */
#define NRFX_CONCAT_2(p1, p2)       NRFX_CONCAT_2_(p1, p2)

/** @brief Internal macro used by @ref NRFX_CONCAT_2 to perform the expansion in two steps. */
#define NRFX_CONCAT_2_(p1, p2)      p1 ## p2

/**
 * @brief Macro for concatenating three tokens in macro expansion.
 *
 * @note This macro is expanded in two steps so that tokens given as macros
 *       themselves are fully expanded before they are merged.
 *
 * @param[in] p1 First token.
 * @param[in] p2 Second token.
 * @param[in] p3 Third token.
 *
 * @return The three tokens merged into one, unless they cannot together form
 *         a valid token (in such case, the preprocessor issues a warning and
 *         does not perform the concatenation).
 *
 * @sa NRFX_CONCAT_2
 */
#define NRFX_CONCAT_3(p1, p2, p3)   NRFX_CONCAT_3_(p1, p2, p3)

/** @brief Internal macro used by @ref NRFX_CONCAT_3 to perform the expansion in two steps. */
#define NRFX_CONCAT_3_(p1, p2, p3)  p1 ## p2 ## p3

/**
 * @brief Macro for performing rounded integer division (as opposed to
 *        truncating the result).
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Rounded (integer) result of dividing @c a by @c b.
 */
#define NRFX_ROUNDED_DIV(a, b)  (((a) + ((b) / 2)) / (b))

/**
 * @brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details A typical use case for this macro is to compute the number of objects
 *          with size @c b required to hold @c a number of bytes.
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Integer result of dividing @c a by @c b, rounded up.
 */
#define NRFX_CEIL_DIV(a, b)  ((((a) - 1) / (b)) + 1)

/**
 * @brief Macro for getting the number of elements in an array.
 *
 * @param[in] array Name of the array.
 *
 * @return Array element count.
 */
#define NRFX_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))


unsigned int nrfx_peripheral_from_base_address(void const * p_reg);

/**
 * @brief Macro for getting the ID number of the specified peripheral.
 *
 * For peripherals in Nordic SoCs, there is a direct relationship between their
 * ID numbers and their base addresses. See the chapter "Peripheral interface"
 * (section "Peripheral ID") in the Product Specification.
 *
 * @param[in] base_addr Peripheral base address or pointer.
 *
 * @return ID number associated with the specified peripheral.
 */
#define NRFX_PERIPHERAL_ID_GET(base_addr)  (uint8_t)(nrfx_peripheral_from_base_address(base_addr))

/**
 * @brief Macro for getting the interrupt number assigned to a specific
 *        peripheral.
 *
 * For peripherals in Nordic SoCs, the IRQ number assigned to a peripheral is
 * equal to its ID number. See the chapter "Peripheral interface" (sections
 * "Peripheral ID" and "Interrupts") in the Product Specification.
 *
 * @param[in] base_addr Peripheral base address or pointer.
 *
 * @return Interrupt number associated with the specified peripheral.
 */
#define NRFX_IRQ_NUMBER_GET(base_addr)  NRFX_PERIPHERAL_ID_GET(base_addr)

static inline bool nrfx_is_word_aligned(void const * p_object)
{
    return ((((uint32_t)p_object) & 0x3u) == 0u);
}

static inline IRQn_Type nrfx_get_irq_number(void const * p_reg)
{
    return (IRQn_Type)NRFX_IRQ_NUMBER_GET(p_reg);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_COMMON_H__
