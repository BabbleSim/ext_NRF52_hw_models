/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
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
#include "nsi_hw_scheduler.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"

static bs_time_t Timer_fake_timer = TIME_NEVER;
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_faketimer_irq_map[NHW_FAKE_TIMER_TOTAL_INST] = NHW_FAKE_TIMER_INT_MAP;

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
		nsi_hws_find_next_event();
	}
}

static void fake_timer_triggered(void)
{
	Timer_fake_timer = TIME_NEVER;
	nsi_hws_find_next_event();

	int inst = 0;
	nhw_irq_ctrl_set_irq(nhw_faketimer_irq_map[inst].cntl_inst,
	                     PHONY_HARD_IRQ);
}

NSI_HW_EVENT(Timer_fake_timer, fake_timer_triggered, 0 /* Purposely the first */);
