/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Provides a minimalistic nrfx_glue to be used with Nordic nrfx and bsim.
 */

#ifndef NRFX_GLUE_BSIM_H__
#define NRFX_GLUE_BSIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro for placing a runtime assertion.
 *
 * @param expression Expression to be evaluated.
 */
#ifndef NRFX_ASSERT
#define NRFX_ASSERT(expression)
#endif

/** @brief Macro for entering into a critical section. */
#ifndef NRFX_CRITICAL_SECTION_ENTER
#define NRFX_CRITICAL_SECTION_ENTER()
#endif

/** @brief Macro for exiting from a critical section. */
#ifndef NRFX_CRITICAL_SECTION_EXIT
#define NRFX_CRITICAL_SECTION_EXIT()
#endif

#include "nrfx_bsim_redef.h"

#ifdef __cplusplus
}
#endif

#endif // NRFX_GLUE_BSIM_H__
