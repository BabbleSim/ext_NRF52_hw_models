/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NHW_NVMC_H
#define _NRF_HW_MODEL_NHW_NVMC_H

#include "bs_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_nvmc_regw_sideeffects_ERASEPAGE(uint inst);
void nhw_nvmc_regw_sideeffects_ERASEPCR0(uint inst);
void nhw_nvmc_regw_sideeffects_ERASEUICR(uint inst);
void nhw_nvmc_regw_sideeffects_ERASEALL(uint inst);
void nhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(uint inst);
void* nrfhw_nmvc_flash_get_base_address(uint inst);
bs_time_t nhw_nvmc_time_to_ready(uint inst);

void nhw_nvmc_erase_page(uint inst, uint32_t address);
void nhw_nvmc_erase_page_partial(uint inst, uint32_t address);

void nhw_nmvc_write_word(uint32_t address, uint32_t value);
uint32_t nhw_nmvc_read_word(uint32_t address);
uint16_t nhw_nmvc_read_halfword(uint32_t address);
uint8_t nhw_nmvc_read_byte(uint32_t address);
void nhw_nmvc_read_buffer(void *dest, uint32_t address, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_NVMC_H */
