/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <irq_ctrl.h>
#include "NRF_EGU.h"
#include "NRF_PPI.h"

#include "bs_tracing.h"

#define PPI_SIDEEFFECT_TASK(EGU_ID, CHANNEL) \
do {\
	NRF_EGU_regs[EGU_ID].TASKS_TRIGGER[CHANNEL] = 1;\
	nrf_egu_regw_sideeffects_TASKS_TRIGGER(&NRF_EGU_regs[EGU_ID], NRF_EGU_TASK_TRIGGER ## CHANNEL);\
	nrf_ppi_event(PPI_EGU_EVENTS[EGU_ID][CHANNEL]);\
} while(0)

#define NRF_EGU_TASK_TRIGGER_DEFINE(id, channel) \
void nrf_egu_ ## id ## _TASK_TRIGGER_ ## channel(void) {PPI_SIDEEFFECT_TASK(id, channel);}

#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(0, channel)
X_EGU_CHANNELS
#undef X
#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(1, channel)
X_EGU_CHANNELS
#undef X
#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(2, channel)
X_EGU_CHANNELS
#undef X
#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(3, channel)
X_EGU_CHANNELS
#undef X
#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(4, channel)
X_EGU_CHANNELS
#undef X
#define X(channel) NRF_EGU_TASK_TRIGGER_DEFINE(5, channel)
X_EGU_CHANNELS
#undef X


const ppi_event_types_t PPI_EGU_EVENTS[EGU_COUNT][EGU0_CH_NUM] = {
	{
		#define X(channel) EGU0_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
	{
		#define X(channel) EGU1_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
	{
		#define X(channel) EGU2_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
	{
		#define X(channel) EGU3_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
	{
		#define X(channel) EGU4_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
	{
		#define X(channel) EGU5_EVENTS_TRIGGERED_ ## channel ,
		X_EGU_CHANNELS
		#undef X
	},
};

/**
 * @brief return instance number for EGU peripheral
 *
 * @param p_reg NRF_EGU_Type pointer
 * @return Number of EGU instance
 *
 */
#define REG_TO_ID(p_reg)    (((size_t)p_reg - (size_t)(&NRF_EGU_regs[0])) / sizeof(NRF_EGU_Type))

/**
 * @brief return ID of task in EGU
 *
 * @param task nrf_egu_task_t 
 * @return channel of the task
 *
 */
#define TASK_TO_ID(task)    ((size_t)task - NRF_EGU_TASK_TRIGGER0) / sizeof(NRF_EGU_regs[0].TASKS_TRIGGER[0])

/**
 * @brief return ID of event in EGU
 *
 * @param event nrf_egu_event_t 
 * @return channel of the event
 *
 */
#define EVENT_TO_ID(event)    ((size_t)event - NRF_EGU_EVENT_TRIGGERED0) / sizeof(NRF_EGU_regs[0].EVENTS_TRIGGERED[0])

/**
 * @brief Get Interrupt Id of corresponding EGU peripheral
 *
 * @param p_reg NRF_EGU_Type pointer
 * @return IRQn for p_reg
 */
#define REG_TO_IRQ(p_reg)   (REG_TO_ID(p_reg) + SWI0_EGU0_IRQn)

/**
 * @brief Convert TASK to EVENT
 *
 * @param task nrf_egu_task_t 
 * @return nrf_egu_event_t connected to the same channel as task
 */
#define TASK_TO_EVENT(task)  offsetof(NRF_EGU_Type, EVENTS_TRIGGERED[TASK_TO_ID(task)])

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

void nrf_egu_regw_sideeffects_TASKS_TRIGGER(NRF_EGU_Type *p_reg, nrf_egu_task_t egu_task)
{
	// task has been moved to trigger, zero out the register for future use
	p_reg->EVENTS_TRIGGERED[TASK_TO_ID(egu_task)] = p_reg->TASKS_TRIGGER[TASK_TO_ID(egu_task)];
	memset(p_reg->TASKS_TRIGGER, 0, sizeof(p_reg->TASKS_TRIGGER));
	nrf_egu_regw_sideeffects_EVENTS_TRIGGERED(p_reg, TASK_TO_EVENT(egu_task));
}

void nrf_egu_regw_sideeffects_EVENTS_TRIGGERED(NRF_EGU_Type *p_reg, nrf_egu_event_t egu_channel)
{
	if(p_reg->EVENTS_TRIGGERED[EVENT_TO_ID(egu_channel)]){
		if (CHANNEL_INTERRUPT_FLAG(p_reg, EVENT_TO_ID(egu_channel))){
			hw_irq_ctrl_set_irq(REG_TO_IRQ(p_reg));
		}
		nrf_ppi_event(PPI_EGU_EVENTS[REG_TO_ID(p_reg)][EVENT_TO_ID(egu_channel)]);
	}
	else {
		// this event has been disabled, check if irq can be cleared
		for(uint8_t i = 0; i< nrf_egu_channel_count(p_reg); i++)
		{
			if (CHANNEL_INTERRUPT_FLAG(p_reg, i) && p_reg->EVENTS_TRIGGERED[i]){
				return; // there are still unhandled events with interrupts enabled
			}
		}
		hw_irq_ctrl_clear_irq(REG_TO_IRQ(p_reg));
	}
}

void nrf_egu_regw_sideeffects_INTEN(NRF_EGU_Type *p_reg )
{
	// check if event has been triggered in the past without interrupt
	for(uint8_t i = 0; i<nrf_egu_channel_count(p_reg); i++) {
		if (CHANNEL_INTERRUPT_FLAG(p_reg, i) && p_reg->EVENTS_TRIGGERED[i]) {
			hw_irq_ctrl_set_irq(REG_TO_IRQ(p_reg));
			// there is only one interrupt for every channel, 
			// if at list one event has been found, we already know we have to enable interrupts
			return; 
		}
	}
}

void nrf_egu_regw_sideeffects_INTENSET(NRF_EGU_Type *p_reg)
{
	if (p_reg->INTENSET) {
		p_reg->INTEN |= p_reg->INTENSET;
		p_reg->INTENSET = p_reg->INTEN;
	}
	nrf_egu_regw_sideeffects_INTEN(p_reg);
}

void nrf_egu_regw_sideeffects_INTENCLR(NRF_EGU_Type *p_reg)
{
	if (p_reg->INTENCLR) {
		p_reg->INTEN &= ~p_reg->INTENSET;
		p_reg->INTENSET = p_reg->INTEN;
		p_reg->INTENCLR = 0;
	}
	nrf_egu_regw_sideeffects_INTEN(p_reg);
}
