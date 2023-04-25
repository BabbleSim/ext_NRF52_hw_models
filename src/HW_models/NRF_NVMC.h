/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NVMC_H
#define _NRF_HW_MODEL_NVMC_H

#include "nrfx.h"
#include "NRF_hw_args.h"
#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

#define FLASH_PAGE_SIZE (4*1024)
#define FLASH_N_PAGES    128
#define FLASH_SIZE (FLASH_N_PAGES*FLASH_PAGE_SIZE)

#define UICR_DATA_SIZE 64 /* Words */
#define UICR_SIZE ( UICR_DATA_SIZE*4 )

extern NRF_NVMC_Type NRF_NVMC_regs;

void nrfhw_nvmc_uicr_pre_init(void);
void nrfhw_nvmc_uicr_init(void);
void nrfhw_nvmc_uicr_clean_up(void);
void nrfhw_nvmc_timer_triggered(void);
void nrfhw_nvmc_regw_sideeffects_ERASEPAGE();
void nrf_nvmc_regw_sideeffects_ERASEPCR0();
void nrfhw_nvmc_regw_sideeffects_ERASEUICR();
void nrfhw_nvmc_regw_sideeffects_ERASEALL();
void nrfhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL();
void nrfhw_nmvc_write_word(uint32_t address, uint32_t value);
uint32_t nrfhw_nmvc_flash_read_word(uint32_t address);
void nrfhw_nmvc_read_buffer(void *dest, uint32_t address, size_t size);
void* nrfhw_nmvc_flash_get_base_address(void);
bs_time_t nrfhw_nvmc_time_to_ready(void);

#ifdef __cplusplus
}
#endif

#endif
