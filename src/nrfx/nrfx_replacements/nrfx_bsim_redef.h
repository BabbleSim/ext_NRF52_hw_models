/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file redefines macros from nrfx that need to be different for simulated devices.
 */

#ifndef NRFX_BSIM_REDEF_H
#define NRFX_BSIM_REDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Redefine nrfx common macros that are not valid when using mocked peripherals */
#undef  NRFX_PERIPHERAL_ID_GET
#define NRFX_PERIPHERAL_ID_GET(base_addr)  (uint8_t)(nrfx_get_irq_number(base_addr))

#ifdef __cplusplus
}
#endif

#endif /* NRFX_BSIM_REDEF_H */
