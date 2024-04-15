/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#define DPPIC_TASKS_CHG_Type NRF_DPPIC_TASKS_CHG_Type /* This type changed name between the 53 and 54 */

#include "hal/nrf_dppi.h"
#include "bs_tracing.h"
#include "NHW_DPPI.h"

static int get_dppi_inst_from_ptr(NRF_DPPIC_Type * p_reg)
{
    extern NRF_DPPIC_Type NRF_DPPIC_regs[];

    return ((uintptr_t)p_reg - (uintptr_t)NRF_DPPIC_regs) / sizeof(NRF_DPPIC_Type);
}

void nrf_dppi_task_trigger(NRF_DPPIC_Type * p_reg, nrf_dppi_task_t dppi_task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) dppi_task)) = 1;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    uint task_off = dppi_task - NRF_DPPI_TASK_CHG0_EN;
    uint group = task_off / sizeof(NRF_DPPIC_TASKS_CHG_Type);

    if (task_off % sizeof(NRF_DPPIC_TASKS_CHG_Type) == 0) {
        nhw_dppi_regw_sideeffects_TASK_CHGn_EN(dppi_inst, group);
    } else {
        nhw_dppi_regw_sideeffects_TASK_CHGn_DIS(dppi_inst, group);
    }
}

void nrf_dppi_channels_disable_all(NRF_DPPIC_Type * p_reg)
{
    p_reg->CHENCLR = 0xFFFFFFFFuL;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHENCLR(dppi_inst);
}

void nrf_dppi_channels_enable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
    p_reg->CHENSET = mask;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHENSET(dppi_inst);
}

void nrf_dppi_channels_disable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
    p_reg->CHENCLR = mask;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHENCLR(dppi_inst);
}

static void nrf_dppi_subscribe_common(NRF_DPPIC_Type * p_reg,
                                      nrf_dppi_task_t  task)
{
    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    uint task_off = task - NRF_DPPI_TASK_CHG0_EN;
    uint group = task_off / sizeof(NRF_DPPIC_TASKS_CHG_Type);

    if (task_off % sizeof(NRF_DPPIC_TASKS_CHG_Type) == 0) {
        nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_EN(dppi_inst, group);
    } else {
        nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_DIS(dppi_inst, group);
    }
}

void nrf_dppi_subscribe_set(NRF_DPPIC_Type * p_reg,
                            nrf_dppi_task_t  task,
                            uint8_t          channel)
{
    NRFX_ASSERT(channel < nrf_dppi_channel_number_get(p_reg));
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);

    nrf_dppi_subscribe_common(p_reg, task);
}

void nrf_dppi_subscribe_clear(NRF_DPPIC_Type * p_reg, nrf_dppi_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;

    nrf_dppi_subscribe_common(p_reg, task);
}

void nrf_dppi_channels_include_in_group(NRF_DPPIC_Type *         p_reg,
                                        uint32_t                 channel_mask,
                                        nrf_dppi_channel_group_t channel_group)
{
    p_reg->CHG[(uint32_t) channel_group] =
        p_reg->CHG[(uint32_t) channel_group] | (channel_mask);

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHGn(dppi_inst, channel_group);
}

void nrf_dppi_channels_remove_from_group(NRF_DPPIC_Type *         p_reg,
                                         uint32_t                 channel_mask,
                                         nrf_dppi_channel_group_t channel_group)
{
    p_reg->CHG[(uint32_t) channel_group] =
        p_reg->CHG[(uint32_t) channel_group] & ~(channel_mask);

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHGn(dppi_inst, channel_group);
}

void nrf_dppi_group_clear(NRF_DPPIC_Type *         p_reg,
                          nrf_dppi_channel_group_t group)
{
    p_reg->CHG[(uint32_t) group] = 0;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_CHGn(dppi_inst, group);
}

void nrf_dppi_group_enable(NRF_DPPIC_Type * p_reg, nrf_dppi_channel_group_t group)
{
    p_reg->TASKS_CHG[(uint32_t) group].EN = 1;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_TASK_CHGn_EN(dppi_inst, group);
}

void nrf_dppi_group_disable(NRF_DPPIC_Type *         p_reg,
                            nrf_dppi_channel_group_t group)
{
    p_reg->TASKS_CHG[(uint32_t) group].DIS = 1;

    uint dppi_inst = get_dppi_inst_from_ptr(p_reg);
    nhw_dppi_regw_sideeffects_TASK_CHGn_DIS(dppi_inst, group);

}
