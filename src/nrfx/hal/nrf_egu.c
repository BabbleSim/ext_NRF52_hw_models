/** Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */

#include "hal/nrf_egu.h"
#include "bs_tracing.h"
#include "NRF_EGU.h"
#include <zephyr/kernel.h>
#include <irq_ctrl.h>

uint32_t nrf_egu_channel_count(NRF_EGU_Type const *p_reg)
{
	return (sizeof(p_reg->EVENTS_TRIGGERED) / sizeof(p_reg->EVENTS_TRIGGERED[0]));
}

void nrf_egu_task_trigger(NRF_EGU_Type *p_reg, nrf_egu_task_t egu_task)
{
	*((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)egu_task)) = 0x1UL;
	nrf_egu_regw_sideeffects_TASKS_TRIGGER(p_reg, egu_task);
}

uint32_t nrf_egu_task_address_get(NRF_EGU_Type const *p_reg,
				  nrf_egu_task_t egu_task)
{
    return (uint32_t)((uint8_t *)p_reg + (uint32_t)egu_task);
}

nrf_egu_task_t nrf_egu_trigger_task_get(uint8_t channel)
{
	return (nrf_egu_task_t)offsetof(NRF_EGU_Type, TASKS_TRIGGER[channel]);
}

bool nrf_egu_event_check(NRF_EGU_Type const *p_reg, nrf_egu_event_t egu_event)
{
	return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)egu_event);
}

void nrf_egu_event_clear(NRF_EGU_Type *p_reg, nrf_egu_event_t egu_event)
{
	*((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)egu_event)) = 0x0UL;
	nrf_egu_regw_sideeffects_EVENTS_TRIGGERED(p_reg, egu_event);
}

uint32_t nrf_egu_event_address_get(NRF_EGU_Type const *p_reg,
				   nrf_egu_event_t egu_event)
{
	return (uint32_t)((uint8_t *)p_reg + (uint32_t)egu_event);
}

nrf_egu_event_t nrf_egu_triggered_event_get(uint8_t channel)
{
	return (nrf_egu_event_t)offsetof(NRF_EGU_Type, EVENTS_TRIGGERED[channel]);
}

void nrf_egu_int_enable(NRF_EGU_Type *p_reg, uint32_t mask)
{
	p_reg->INTENSET = mask;
	nrf_egu_regw_sideeffects_INTENSET(p_reg);
}

uint32_t nrf_egu_int_enable_check(NRF_EGU_Type const *p_reg, uint32_t mask)
{
	return p_reg->INTEN & mask;
}

void nrf_egu_int_disable(NRF_EGU_Type *p_reg, uint32_t mask)
{
	p_reg->INTENCLR = mask;
	nrf_egu_regw_sideeffects_INTENSET(p_reg);
}

nrf_egu_int_mask_t nrf_egu_channel_int_get(uint8_t channel)
{
	return (nrf_egu_int_mask_t)((uint32_t) (EGU_INTENSET_TRIGGERED0_Msk << channel));
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting the subscribe configuration for a given
 *        EGU task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
void nrf_egu_subscribe_set(NRF_EGU_Type *p_reg, nrf_egu_task_t task,
			   uint8_t channel)
{
	#error not implemented
}

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        EGU task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
void nrf_egu_subscribe_clear(NRF_EGU_Type *p_reg, nrf_egu_task_t task)
{
	#error not implemented
}

/**
 * @brief Function for setting the publish configuration for a given
 *        EGU event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
void nrf_egu_publish_set(NRF_EGU_Type *p_reg, nrf_egu_event_t event,
			 uint8_t channel)
{
	#error not implemented
}

/**
 * @brief Function for clearing the publish configuration for a given
 *        EGU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
void nrf_egu_publish_clear(NRF_EGU_Type *p_reg, nrf_egu_event_t event)
{
	#error not implemented
}

#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
