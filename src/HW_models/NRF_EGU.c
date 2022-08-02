/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <irq_ctrl.h>
#include "NRF_EGU.h"

#include "bs_tracing.h"

/**
 * @brief return instance number for EGU peripheral
 *
 * @param p_reg NRF_EGU_Type pointer
 * @return Number of EGU instance
 * 
 */
#define REG_TO_ID(p_reg)    ((size_t)p_reg - (size_t)(&NRF_EGU_regs[0])) / sizeof(NRF_EGU_Type)

/**
 * @brief Get Interrupt Id of corresponding EGU peripheral
 *
 * @param p_reg NRF_EGU_Type pointer
 * @return IRQn for p_reg
 */
#define REG_TO_IRQ(p_reg)   REG_TO_ID(p_reg) + SWI0_EGU0_IRQn

/**
 * @brief Check if channel has enabled interrupts
 * 
 * @param p_reg NRF_EGU_Type pointer
 * @param channel uint8_t channel number to check
 * @return True if interrupt is enabled for channel in p_reg, otherwise return False
 */
#define CHANNEL_INTERRUPT_FLAG(p_reg, channel)    (p_reg->INTEN & nrf_egu_channel_int_get(channel))


NRF_EGU_Type NRF_EGU_regs[EGU_COUNT];
// prevent enabling already enabled IRQ
bool EGU_interrupt_enabled[EGU_COUNT];

void nrf_egu_regw_sideeffects_TASKS_TRIGGERED(NRF_EGU_Type * p_reg)
{
    // move tasks to triggers
    memcpy(p_reg->EVENTS_TRIGGERED, p_reg->TASKS_TRIGGER, sizeof(p_reg->EVENTS_TRIGGERED)); 
    // task has been moved to trigger, zero out the register for future use
    memset(p_reg->TASKS_TRIGGER, 0, sizeof(p_reg->TASKS_TRIGGER)); 
    nrf_egu_regw_sideeffects_EVENTS_TRIGGERED_INTEN(p_reg);
}

void nrf_egu_regw_sideeffects_EVENTS_TRIGGERED_INTEN(NRF_EGU_Type * p_reg)
{

    for(uint8_t i = 0; i < nrf_egu_channel_count(p_reg); i++)
    {
        if(CHANNEL_INTERRUPT_FLAG(p_reg, i) && p_reg->EVENTS_TRIGGERED[i])
        {
            // if there is some task, and interrupt is enabled - set irq
            if(! EGU_interrupt_enabled[REG_TO_ID(p_reg)])
            {
                EGU_interrupt_enabled[REG_TO_ID(p_reg)] = true;
                hw_irq_ctrl_set_irq(REG_TO_IRQ(p_reg));
            }
            return;
        }
    }
    // if there are not active tasks with enabled interrupt - clear irq
    if(EGU_interrupt_enabled[REG_TO_ID(p_reg)])
    {
        EGU_interrupt_enabled[REG_TO_ID(p_reg)] = false;
        hw_irq_ctrl_clear_irq(REG_TO_IRQ(p_reg));
    }
}

void nrf_egu_regw_sideeffects_INTENSET(NRF_EGU_Type * p_reg)
{
    if(p_reg->INTENSET)
    {
        p_reg->INTEN |= p_reg->INTENSET;
        p_reg->INTENSET = p_reg->INTEN;
    }
    nrf_egu_regw_sideeffects_EVENTS_TRIGGERED_INTEN(p_reg);
}

void nrf_egu_regw_sideeffects_INTENCLR(NRF_EGU_Type * p_reg)
{
    if(p_reg->INTENCLR)
    {
        p_reg->INTEN &= ~p_reg->INTENSET;
        p_reg->INTENSET = p_reg->INTEN;
        p_reg->INTENCLR = 0;
    }
    nrf_egu_regw_sideeffects_EVENTS_TRIGGERED_INTEN(p_reg);
}
