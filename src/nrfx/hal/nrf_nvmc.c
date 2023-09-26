/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include "hal/nrf_nvmc.h"
#include "bs_tracing.h"
#include "bs_utils.h"
#include "NHW_NVMC.h"
#include <nrfx_glue.h>


void nrf_nvmc_page_erase_start(NRF_NVMC_Type * p_reg, uint32_t page_addr)
{
    p_reg->ERASEPAGE = page_addr;
    nrfhw_nvmc_regw_sideeffects_ERASEPAGE();
}

void nrf_nvmc_uicr_erase_start(NRF_NVMC_Type * p_reg)
{
    p_reg->ERASEUICR = 1;
    nrfhw_nvmc_regw_sideeffects_ERASEUICR();
}

void nrf_nvmc_erase_all_start(NRF_NVMC_Type * p_reg)
{
    p_reg->ERASEALL = 1;
    nrfhw_nvmc_regw_sideeffects_ERASEALL();
}

void nrf_nvmc_page_partial_erase_start(NRF_NVMC_Type * p_reg, uint32_t page_addr)
{
    p_reg->ERASEPAGEPARTIAL = page_addr;
    nrfhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL();
}

uint8_t nrf_nvmc_byte_read(uint32_t addr)
{
    return nrfhw_nmvc_read_byte(addr);
}

uint16_t nrf_nvmc_halfword_read(uint32_t addr)
{
    return nrfhw_nmvc_read_halfword(addr);
}

uint32_t nrf_nvmc_word_read(uint32_t addr)
{
    return nrfhw_nmvc_read_word(addr);
}

void nrf_nvmc_word_write(uint32_t addr, uint32_t value)
{
    nrfhw_nmvc_write_word(addr, value);
}

void nrf_nvmc_buffer_read(void *dest, uint32_t addr, size_t n)
{
    nrfhw_nmvc_read_buffer(dest, addr, n);
}

/*
 * The nvmc driver uses the HAL ready checks in quite a few places
 * in busy wait loops. Some of the driver maintainers were
 * not keen on adding conditionally compiled delays on those
 * so we hide the delay here.
 * Under the assumption that the ready_check functions
 * are used only in such busy wait loops until ready,
 * we actually wait more or less time depending on the pending
 * time.
 */
static void nrf_nvmc_ready_wait(){
#if defined(NRFX_DELAY_US) && !defined(NO_NVMC_READY_AUTO_DELAY)
        bs_time_t wait = nrfhw_nvmc_time_to_ready() + 1;
        wait = BS_MAX(BS_MIN(wait,100),1);
        NRFX_DELAY_US(wait);
#endif
}

bool nrf_nvmc_ready_check(NRF_NVMC_Type const * p_reg)
{
    bool ready = (bool)(p_reg->READY & NVMC_READY_READY_Msk);
    if (!ready) {
        nrf_nvmc_ready_wait();
    }
    return ready;
}

bool nrf_nvmc_write_ready_check(NRF_NVMC_Type const * p_reg)
{
    bool ready = (bool)(p_reg->READY & NVMC_READY_READY_Msk);
    if (!ready) {
        nrf_nvmc_ready_wait();
    }
    return ready;
}
