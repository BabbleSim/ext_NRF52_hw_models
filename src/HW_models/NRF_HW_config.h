/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file configures the HW models based on the variant being built
 */
#ifndef _NRF_HW_CONFIG_H
#define _NRF_HW_CONFIG_H

/*
 * For each peripheral the following may be defined
 * NHW_<PERIPH_TYPE>_TOTAL_INST <val> : Total number of instances of the peripheral in the whole SOC
 * NHW_<PERIPH_TYPE>_<CORE>_<INST> <val> : Index of that peripheral instant in list of peripherals
 */

#if defined(NRF52833)

#define NHW_HAS_PPI  1
#define NHW_HAS_DPPI 0
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */


#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_0 0
#define NHW_RNG_INT_MAP  {{0 , 13}} /*Only core,RNG_IRQn*/

#elif defined(NRF5340)

/*
 * The Application core/domain is indexed as domain 0
 * The Network core/domain is indexed as domain 1
 */

#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 0


#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_NET_0 0
#define NHW_RNG_INT_MAP  {{1, 9}} /*Network core, "RNG_IRQn"*/
#define NHW_RNG_DPPI_MAP {1} /*Network core*/

#else
#error "No valid platform was selected"
#endif

#endif /* _NRF_HW_CONFIG_H */
