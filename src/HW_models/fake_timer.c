/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This is a model of a fake HW device which is used by the k_busy_wait() function
 * replacement. It is a timer which will awake the embedded CPU even if interrupts are
 * locked (it does not have an associated irq vector, it simply awakes the CPU)
 *
 * N instances of this timer are present, one for each embedded CPU.
 */

#include "bs_types.h"
#include "irq_ctrl.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"

struct ftimer_status_t {
  bs_time_t event_time;
};

static bs_time_t Timer_fake_timer = TIME_NEVER;
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_faketimer_irq_map[NHW_FAKE_TIMER_TOTAL_INST] = NHW_FAKE_TIMER_INT_MAP;
static struct ftimer_status_t ftimer_st[NHW_FAKE_TIMER_TOTAL_INST];

/*
 * Initialize all fake timer instances for this SOC
 */
static void nhw_fake_timer_init(void)
{
  for (int i = 0; i < NHW_FAKE_TIMER_TOTAL_INST ; i++ ) {
    ftimer_st[i].event_time = TIME_NEVER;
  }
}

NSI_TASK(nhw_fake_timer_init, HW_INIT, 10);

static void nhw_fake_timer_update_main_timer(void)
{
  Timer_fake_timer = ftimer_st[0].event_time;

#if (NHW_FAKE_TIMER_TOTAL_INST > 1)
  for (int i = 1; i < NHW_FAKE_TIMER_TOTAL_INST ; i++ ) {
    if (ftimer_st[i].event_time < Timer_fake_timer){
      Timer_fake_timer = ftimer_st[i].event_time;
    }
  }
#endif

  nsi_hws_find_next_event();
}

/**
 * The timer HW will awake the CPU (without an interrupt) at least when <time>
 * comes (it may awake it earlier)
 *
 * If there was a previous request for an earlier time, the old one will prevail
 *
 * This is meant for k_busy_wait() like functionality
 */
void nhw_fake_timer_wake_in_time(unsigned int inst, bs_time_t time)
{
  if (ftimer_st[inst].event_time > time) {
    ftimer_st[inst].event_time = time;
    nhw_fake_timer_update_main_timer();
  }
}

static void nhw_fake_timer_triggered(void)
{
  for (int i = 0; i < NHW_FAKE_TIMER_TOTAL_INST ; i++ ) {
    if (Timer_fake_timer >= ftimer_st[i].event_time) {
      ftimer_st[i].event_time = TIME_NEVER;
      hw_irq_ctrl_set_irq(nhw_faketimer_irq_map[i].cntl_inst,
                          PHONY_HARD_IRQ);
    }
  }

  nhw_fake_timer_update_main_timer();
}

NSI_HW_EVENT(Timer_fake_timer, nhw_fake_timer_triggered, 0 /* Purposely the first */);
