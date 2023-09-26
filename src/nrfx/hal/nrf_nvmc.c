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

static int nvmc_number_from_ptr(NRF_NVMC_Type * p_reg)
{
    int i = ((intptr_t)p_reg - (intptr_t)NRF_NVMC_regs_p[0]) / sizeof(NRF_NVMC_Type);
    return i;
}

void nrf_nvmc_page_erase_start(NRF_NVMC_Type * p_reg, uint32_t page_addr)
{
    int i = nvmc_number_from_ptr(p_reg);

#if defined(NRF52_SERIES)
    p_reg->ERASEPAGE = page_addr;
    nhw_nvmc_regw_sideeffects_ERASEPAGE(i);
#elif defined(NRF53_SERIES) || defined(NRF91_SERIES)
    nhw_nvmc_erase_page(i, page_addr);
#endif
}

#if defined(NVMC_ERASEUICR_ERASEUICR_Msk)
void nrf_nvmc_uicr_erase_start(NRF_NVMC_Type * p_reg)
{
    int i = nvmc_number_from_ptr(p_reg);

    p_reg->ERASEUICR = 1;
    nhw_nvmc_regw_sideeffects_ERASEUICR(i);
}
#endif

void nrf_nvmc_erase_all_start(NRF_NVMC_Type * p_reg)
{
    int i = nvmc_number_from_ptr(p_reg);

    p_reg->ERASEALL = 1;
    nhw_nvmc_regw_sideeffects_ERASEALL(i);
}

void nrf_nvmc_page_partial_erase_start(NRF_NVMC_Type * p_reg, uint32_t page_addr)
{
    int i = nvmc_number_from_ptr(p_reg);

#if defined(NVMC_ERASEPAGEPARTIAL_ERASEPAGEPARTIAL_Msk)
    p_reg->ERASEPAGEPARTIAL = page_addr;
    nhw_nvmc_regw_sideeffects_ERASEPAGEPARTIAL(i);
#elif defined(NRF53_SERIES) || defined(NRF91_SERIES)
    nhw_nvmc_erase_page_partial(i, page_addr);
#endif
}

uint8_t nrf_nvmc_byte_read(uint32_t addr)
{
    return nhw_nmvc_read_byte(addr);
}

uint16_t nrf_nvmc_halfword_read(uint32_t addr)
{
    return nhw_nmvc_read_halfword(addr);
}

uint32_t nrf_nvmc_word_read(uint32_t addr)
{
    return nhw_nmvc_read_word(addr);
}

void nrf_nvmc_word_write(uint32_t addr, uint32_t value)
{
    nhw_nmvc_write_word(addr, value);
}

void nrf_nvmc_buffer_read(void *dest, uint32_t addr, size_t n)
{
    nhw_nmvc_read_buffer(dest, addr, n);
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
static void nrf_nvmc_ready_wait(unsigned int inst)
{
#if defined(NRFX_DELAY_US) && !defined(NO_NVMC_READY_AUTO_DELAY)
        bs_time_t wait = nhw_nvmc_time_to_ready(inst) + 1;
        wait = BS_MAX(BS_MIN(wait,100),1);
        NRFX_DELAY_US(wait);
#endif
}

bool nrf_nvmc_ready_check(NRF_NVMC_Type const * p_reg)
{
    bool ready = (bool)(p_reg->READY & NVMC_READY_READY_Msk);
    if (!ready) {
        int i = nvmc_number_from_ptr((NRF_NVMC_Type *)p_reg);
        nrf_nvmc_ready_wait(i);
    }
    return ready;
}

bool nrf_nvmc_write_ready_check(NRF_NVMC_Type const * p_reg)
{
    return nrf_nvmc_ready_check(p_reg);
}
