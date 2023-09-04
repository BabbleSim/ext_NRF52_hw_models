/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This is a fake HW device timer (one instance per embedded CPU)
 * which aids in the execution of a test application which
 * runs in parallel to the normal embedded application
 */

#include "bs_types.h"
#include "irq_ctrl.h"
#include "nsi_tasks.h"
#include "nsi_cpun_if.h"
#include "nsi_hw_scheduler.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"

struct bs_ticker_status {
  bs_time_t ticker_timer;    /* Next time to awake either by the timer or a awake_cpu_asap() call */
  bs_time_t ticker_timer_lt; /* Next wake due to the plain bsticker timer */
  bs_time_t tick_period;
  bool awake_cpu_asap;
};

static bs_time_t bst_ticker_timer = TIME_NEVER;
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_bsticker_irq_map[NHW_BSTICKER_TOTAL_INST] = NHW_BSTICKER_TIMER_INT_MAP;
static struct bs_ticker_status bs_ticket_st[NHW_BSTICKER_TOTAL_INST];

/*
 * Initialize all fake timer instances for this SOC
 */
static void nhw_bst_ticker_init(void)
{
  for (int i = 0; i < NHW_BSTICKER_TOTAL_INST ; i++) {
    bs_ticket_st[i].ticker_timer = TIME_NEVER;
    bs_ticket_st[i].ticker_timer_lt = TIME_NEVER;
    bs_ticket_st[i].tick_period = TIME_NEVER;
  }
}

NSI_TASK(nhw_bst_ticker_init, HW_INIT, 10);

static void nhw_bst_ticker_update_main_timer(void)
{
  bst_ticker_timer = bs_ticket_st[0].ticker_timer;

  for (int i = 1; i < NHW_FAKE_TIMER_TOTAL_INST ; i++) {
    if (bs_ticket_st[i].ticker_timer < bst_ticker_timer) {
      bst_ticker_timer = bs_ticket_st[i].ticker_timer;
    }
  }

  nsi_hws_find_next_event();
}

static void nhw_bst_ticker_find_next_time_inner(struct bs_ticker_status *this)
{
  if (this->awake_cpu_asap == true) {
    this->ticker_timer = nsi_hws_get_time(); //We will awake it in this same microsecond
  } else {
    this->ticker_timer = this->ticker_timer_lt;
  }
}

static void nhw_bst_ticker_find_next_time(uint inst) {
  struct bs_ticker_status *this = &bs_ticket_st[inst];

  nhw_bst_ticker_find_next_time_inner(this);

  nhw_bst_ticker_update_main_timer();
}

/**
 * Set the FW_test ticker to trigger periodically, with a period of <tick_period>
 * Next trigger will occur tick_period (us= + current time
 */
void bst_ticker_amp_set_period(uint inst, bs_time_t tick_period)
{
  struct bs_ticker_status *this = &bs_ticket_st[inst];

  this->tick_period = tick_period;
  this->ticker_timer_lt = tick_period + nsi_hws_get_time();
  nhw_bst_ticker_find_next_time(inst);
}

/**
 * Set the next time the FW test ticker will trigger at <absolute_time> us
 */
void bst_ticker_amp_set_next_tick_absolutelute(uint inst, bs_time_t absolute_time)
{
  bs_ticket_st[inst].ticker_timer_lt = absolute_time;
  nhw_bst_ticker_find_next_time(inst);
}

/**
 * Set the next time the FW test ticker will trigger
 * at <delta_time> (us) + current Hw time
 */
void bst_ticker_amp_set_next_tick_delta(uint inst, bs_time_t delta_time)
{
  bs_ticket_st[inst].ticker_timer_lt = delta_time + nsi_hws_get_time();
  nhw_bst_ticker_find_next_time(inst);
}

static void nhw_bst_ticker_triggered(void)
{
  bs_time_t now = nsi_hws_get_time();

  for (int i = 0; i < NHW_BSTICKER_TOTAL_INST ; i++ ) {
    struct bs_ticker_status *this = &bs_ticket_st[i];

    if (this->awake_cpu_asap == true) {
      this->awake_cpu_asap = false;
      hw_irq_ctrl_raise_im(nhw_bsticker_irq_map[i].cntl_inst,
                           PHONY_HARD_IRQ);
    } else {
      if (this->tick_period != TIME_NEVER) {
        this->ticker_timer_lt = this->tick_period + now;
      } else {
        this->ticker_timer_lt = TIME_NEVER;
      }
      (void) nsif_cpun_test_hook(i, NULL);
    }

    nhw_bst_ticker_find_next_time_inner(this);
  }

  nhw_bst_ticker_update_main_timer();
}

NSI_HW_EVENT(bst_ticker_timer, nhw_bst_ticker_triggered, 1 /* Purposely the second */);

/**
 * Awake the MCU as soon as possible (in this same microsecond, in a following delta)
 */
void bst_ticker_amp_awake_cpu_asap(uint inst)
{
  bs_ticket_st[inst].awake_cpu_asap = true;
  nhw_bst_ticker_find_next_time(inst);
}
