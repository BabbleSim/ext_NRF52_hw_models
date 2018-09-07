/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This is a fake HW device model which is used by the k_busy_wait() function
 * replacement. It is a timer which will awake the CPU even if interrupts are
 * locked (it does not have an associated irq vector, it simply awakes the CPU)
 */

#include "bs_types.h"
#include "irq_ctrl.h"
#include "NRF_HW_model_top.h"

bs_time_t Timer_fake_timer = TIME_NEVER;

void fake_timer_init()
{
	Timer_fake_timer = TIME_NEVER;
}

void fake_timer_cleanup(void)
{

}

/**
 * The timer HW will awake the CPU (without an interrupt) at least when <time>
 * comes (it may awake it earlier)
 *
 * If there was a previous request for an earlier time, the old one will prevail
 *
 * This is meant for k_busy_wait() like functionality
 */
void fake_timer_wake_in_time(bs_time_t time)
{
	if (Timer_fake_timer > time) {
		Timer_fake_timer = time;
		nrf_hw_find_next_timer_to_trigger();
	}
}

void fake_timer_triggered(void)
{
	Timer_fake_timer = TIME_NEVER;
	nrf_hw_find_next_timer_to_trigger();
	hw_irq_ctrl_set_irq(PHONY_HARD_IRQ);
}
