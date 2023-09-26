/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NVMC_H
#define _NRF_HW_MODEL_NVMC_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

#define FLASH_PAGE_SIZE (4*1024)
#define FLASH_N_PAGES    128
#define FLASH_SIZE (FLASH_N_PAGES*FLASH_PAGE_SIZE)

#define UICR_DATA_SIZE 64 /* Words */
#define UICR_SIZE ( UICR_DATA_SIZE*4 )

void nrfhw_nvmc_regw_sideeffects_ERASEPAGE(void);
void nrf_nvmc_regw_sideeffects_ERASEPCR0(void);
void nrfhw_nvmc_regw_sideeffects_ERASEUICR(void);
void nrfhw_nvmc_regw_sideeffects_ERASEALL(void);
void nrfhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(void);
void nrfhw_nmvc_write_word(uint32_t address, uint32_t value);
uint32_t nrfhw_nmvc_read_word(uint32_t address);
uint16_t nrfhw_nmvc_read_halfword(uint32_t address);
uint8_t nrfhw_nmvc_read_byte(uint32_t address);
void nrfhw_nmvc_read_buffer(void *dest, uint32_t address, size_t size);
void* nrfhw_nmvc_flash_get_base_address(void);
bs_time_t nrfhw_nvmc_time_to_ready(void);

#ifdef __cplusplus
}
#endif

#endif
