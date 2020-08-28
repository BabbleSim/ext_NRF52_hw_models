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

#include "nrfx_bsim_redef.h"

#ifdef __cplusplus
}
#endif

#endif // NRFX_GLUE_BSIM_H__
