/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * HW Interrupt Controller model
 *
 * Note: In principle this should have been a model of the ARM NVIC
 *  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html
 *
 * But so far it is just a general INT controller that provides
 * a reasonable emulation of the NVIC functionality used in Zephyr
 *
 * Implementation spec:
 *   This file provides the implementation of a generic interrupt controller,
 *   and instantiates N of them (and initializes them at startup),
 *   as described in the configuration (NHW_config.h).
 *
 *   This interrupt controller must be paired with dedicated interrupt handling
 *   code.
 *
 *   Each interrupt has a configurable priority (256 levels).
 *   Interrupts can be individually masked, apart from interrupts globally locked.
 *   With the appropriate interrupt handling code, interrupt nesting is supported.
 *
 *   The interrupt controller supports both level and edge/pulse interrupts.
 *   The level interrupts "lines" must be raised and *lowered* by the peripherals.
 *   ( hw_irq_ctrl_{raise|lower}_level_irq_line() )
 *   Pulse/edge interrupts are just events that pend the interrupt
 *   (calls to hw_irq_ctrl_set_irq()).
 *
 *   NOTE: At this point an interrupt controller may have at most 64 interrupt lines.
 *   Generalizing this is pending.
 */

#include <stdint.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "nsi_internal.h"
#include "nsi_cpun_if.h"
#include "bs_types.h"
#include "bs_tracing.h"
#include "irq_ctrl.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

#define IRQ_64s ((NHW_INTCTRL_MAX_INTLINES+63)/64)

struct intctrl_status {
  uint64_t irq_lines[IRQ_64s]; /*Level of interrupt lines from peripherals*/
  uint64_t irq_status[IRQ_64s];  /*pended and not masked interrupts*/
  uint64_t irq_premask[IRQ_64s]; /*pended interrupts before the mask*/
  /*
   * Mask of which interrupts will actually cause the cpu to vector into its
   * irq handler
   * If an interrupt is masked in this way, it will be pending in the premask in
   * case it is enabled later before clearing it.
   * If the irq_mask enables and interrupt pending in irq_premask, it will cause
   * the controller to raise the interrupt immediately
   */
  uint64_t irq_mask[IRQ_64s];

  uint8_t irq_prio[NHW_INTCTRL_MAX_INTLINES]; /*Priority of each interrupt*/
  /*note that prio = 0 == highest, prio=255 == lowest*/

  int currently_running_prio;

  /*
   * Interrupts lock/disable. When set, interrupts are registered
   * (in the irq_status) but do not awake the cpu. if when unlocked,
   * irq_status != 0 an interrupt will be raised immediately
   */
  bool irqs_locked;
  bool lock_ignore; /*For the hard fake IRQ, temporarily ignore lock*/

  bool awaking_CPU; /* Is this instance raising an interrupt to the CPU in a delta cycle */
};

static struct intctrl_status nhw_intctrl_st[NHW_INTCTRL_TOTAL_INST];

static bs_time_t Timer_irq_ctrl = TIME_NEVER;

static void hw_irq_ctrl_init(void)
{
  for (int i= 0; i < NHW_INTCTRL_TOTAL_INST; i++) {
    struct intctrl_status *el = &nhw_intctrl_st[i];

    memset(el->irq_prio, 255, NHW_INTCTRL_MAX_INTLINES);
    el->currently_running_prio = 256; /*255 is the lowest prio interrupt*/
  }
}

NSI_TASK(hw_irq_ctrl_init, HW_INIT, 100);

void hw_irq_ctrl_set_cur_prio(unsigned int inst, int new)
{
  nhw_intctrl_st[inst].currently_running_prio = new;
}

int hw_irq_ctrl_get_cur_prio(unsigned int inst)
{
  return nhw_intctrl_st[inst].currently_running_prio;
}

void hw_irq_ctrl_prio_set(unsigned int inst, unsigned int irq, unsigned int prio)
{
  nhw_intctrl_st[inst].irq_prio[irq] = prio;
}

uint8_t hw_irq_ctrl_get_prio(unsigned int inst, unsigned int irq)
{
  return nhw_intctrl_st[inst].irq_prio[irq];
}

static inline bool irq_status_not_zero(unsigned int inst) {
  for (int i = 0; i < IRQ_64s; i++) {
    if (nhw_intctrl_st[inst].irq_status[i] != 0) {
      return true;
    }
  }
  return false;
}

/**
 * Get the currently pending highest priority interrupt which has a priority
 * higher than a possibly currently running interrupt
 *
 * If none, return -1
 */
int hw_irq_ctrl_get_highest_prio_irq(unsigned int inst)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  if (this->irqs_locked) {
    return -1;
  }

  int winner = -1;
  int winner_prio = 256;

  for (int i = 0; i < IRQ_64s; i++) {
    uint64_t irq_status = this->irq_status[i];
    while (irq_status != 0U) {
      int irq_bit = nsi_find_lsb_set64(irq_status) - 1;
      int irq_nbr = irq_bit + i*64;

      irq_status &= ~((uint64_t) 1 << irq_bit);
      if ((winner_prio > (int)this->irq_prio[irq_nbr])
          && (this->currently_running_prio > (int)this->irq_prio[irq_nbr])) {
        winner = irq_nbr;
        winner_prio = this->irq_prio[irq_nbr];
      }
    }
  }
  return winner;
}

uint32_t hw_irq_ctrl_get_current_lock(unsigned int inst)
{
  return nhw_intctrl_st[inst].irqs_locked;
}

/*
 * Change the overall interrupt controller "interrupt lock"
 * The interrupt lock is a flag that provisionally disables all interrupts
 * without affecting their status or their ability to be pended in the meanwhile
 */
uint32_t hw_irq_ctrl_change_lock(unsigned int inst, uint32_t new_lock)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];
  uint32_t previous_lock = this->irqs_locked;

  this->irqs_locked = new_lock;

  if ((previous_lock == true) && (new_lock == false)) {
    if (irq_status_not_zero(inst)) {
      nsif_cpun_irq_raised_from_sw(inst);
    }
  }
  return previous_lock;
}

void hw_irq_ctrl_clear_all_enabled_irqs(unsigned int inst)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  for (int i=0; i < IRQ_64s; i++) {
    this->irq_status[i]  = 0U;
    this->irq_premask[i] &= ~this->irq_mask[i];
  }
}

/*
 * Un-pend all interrupt from the interrupt controller.
 * Note: For level interrupts, the interrupts may be re-pended soon after.
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC)
 */
void hw_irq_ctrl_clear_all_irqs(unsigned int inst)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  for (int i=0; i < IRQ_64s; i++) {
    this->irq_status[i]  = 0U;
    this->irq_premask[i] = 0U;
  }
}

/*
 * Disable (mask) one interrupt
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC)
 */
void hw_irq_ctrl_disable_irq(unsigned int inst, unsigned int irq)
{
  nhw_intctrl_st[inst].irq_mask[irq/64] &= ~((uint64_t)1<<(irq%64));
}

/*
 * Check if an interrupt is enabled (not masked) (But not necessarily pended)
 * return 1 is enabled, 0 is disabled.
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC)
 */
int hw_irq_ctrl_is_irq_enabled(unsigned int inst, unsigned int irq)
{
  return (nhw_intctrl_st[inst].irq_mask[irq/64] & ((uint64_t)1 << (irq%64)))?1:0;
}

/*
 * Un-pend an interrupt from the interrupt controller.
 * Note: For level interrupts, the interrupt may be repended soon after.
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC)
 */
void hw_irq_ctrl_clear_irq(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];
  uint64_t irq_bit = ((uint64_t)1<<(irq%64));
  uint irq_idx = irq/64;

  this->irq_status[irq_idx]  &= ~irq_bit;
  this->irq_premask[irq_idx] &= ~irq_bit;
}

/*
 * Reevaluate if an interrupt still has its interrupt line up,
 * and if it does, re-pend immediately the interrupt
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC).
 * To properly model an NVIC behavior call it only when the MCU is exiting
 * the interrupt handler for this irq
 */
void hw_irq_ctrl_reeval_level_irq(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];
  uint64_t irq_bit = ((uint64_t)1<<(irq%64));
  uint irq_idx = irq/64;

  if ((this->irq_lines[irq_idx] & irq_bit) != 0) {
    this->irq_premask[irq_idx] |= irq_bit;

    if (this->irq_mask[irq_idx] & irq_bit) {
      this->irq_status[irq_idx] |= irq_bit;
    }
  }
}

/**
 * Enable an interrupt
 *
 * This function may only be called from SW threads
 *
 * If the enabled interrupt is pending, it will immediately vector to its
 * interrupt handler and continue (maybe with some swap() before)
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC).
 */
void hw_irq_ctrl_enable_irq(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];
  uint64_t irq_bit = ((uint64_t)1<<(irq%64));
  uint irq_idx = irq/64;

  this->irq_mask[irq_idx] |= irq_bit;
  if (this->irq_premask[irq_idx] & irq_bit) { /*if the interrupt is pending*/
    hw_irq_ctrl_raise_im_from_sw(inst, irq);
  }
}

static inline void hw_irq_ctrl_irq_raise_prefix(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  if ( irq < NHW_INTCTRL_MAX_INTLINES ) {
    uint64_t irq_bit = ((uint64_t)1<<(irq%64));
    uint irq_idx = irq/64;

    this->irq_premask[irq_idx] |= irq_bit;

    if (this->irq_mask[irq_idx] & irq_bit) {
      this->irq_status[irq_idx] |= irq_bit;
    }
  } else if (irq == PHONY_HARD_IRQ) {
    this->lock_ignore = true;
  }
}

/**
 * Set/Raise/Pend an interrupt
 *
 * This function is meant to be used by either the SW manual IRQ raising
 * or by HW which wants the IRQ to be raised in one delta cycle from now
 *
 * Note that this is equivalent to a HW peripheral sending a *pulse* interrupt
 * to the interrupt controller
 *
 * This is an API towards the HW models. Embedded SW may not call it.
 */
void hw_irq_ctrl_set_irq(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  hw_irq_ctrl_irq_raise_prefix(inst, irq);
  if ((this->irqs_locked == false) || (this->lock_ignore)) {
    /*
     * Awake CPU in 1 delta
     * Note that we awake the CPU even if the IRQ is disabled
     * => we assume the CPU is always idling in a WFE() like
     * instruction and the CPU is allowed to awake just with the irq
     * being marked as pending
     */
    this->awaking_CPU = true;
    Timer_irq_ctrl = nsi_hws_get_time();
    nsi_hws_find_next_event();
  }
}

/**
 * Raise an interrupt line from a HW peripheral to the interrupt controller
 *
 * This function is meant to be used only from a HW model which wants
 * to emulate level interrupts.
 * An IRQ will be raised in one delta cycle from now
 *
 * Any call from the hardware models to this function must be eventually
 * followed by a call to hw_irq_ctrl_lower_level_irq_line(), otherwise
 * the interrupt controller will keep interrupting the CPU and causing it to
 * re-enter the interrupt handler
 *
 * This is an API towards the HW models. Embedded SW may not call it.
 */
void hw_irq_ctrl_raise_level_irq_line(unsigned int inst, unsigned int irq)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];

  if ( irq >= NHW_INTCTRL_MAX_INTLINES ) {
    bs_trace_error_line_time("Phony interrupts cannot use this API\n");
  }

  uint64_t irq_bit = ((uint64_t)1<<(irq%64));
  uint irq_idx = irq/64;

  if ((this->irq_lines[irq_idx] & irq_bit) == 0) {
    this->irq_lines[irq_idx] |= irq_bit;
    hw_irq_ctrl_set_irq(inst, irq);
  }
}

/**
 * Lower an interrupt line from a HW peripheral to the interrupt controller
 *
 * This function is meant to be used only from a HW model which wants
 * to emulate level interrupts.
 *
 * This is an API towards the HW models. Embedded SW may not call it.
 */
void hw_irq_ctrl_lower_level_irq_line(unsigned int inst, unsigned int irq)
{
  if ( irq >= NHW_INTCTRL_MAX_INTLINES ) {
    bs_trace_error_line_time("Phony interrupts cannot use this API\n");
  }
  uint64_t irq_bit = ((uint64_t)1<<(irq%64));
  uint irq_idx = irq/64;

  nhw_intctrl_st[inst].irq_lines[irq_idx] &= ~irq_bit;
}


static void irq_raising_from_hw_now(unsigned int inst)
{
  struct intctrl_status *this = &nhw_intctrl_st[inst];
  /*
   * We always awake the CPU even if the IRQ was masked,
   * but not if irqs are locked unless this is due to a
   * PHONY_HARD_IRQ
   */
  if ((this->irqs_locked == false) || (this->lock_ignore)) {
    this->lock_ignore = false;
    nsif_cpun_irq_raised(inst);
  }
}

/**
 * Set/Raise/Pend an interrupt immediately.
 * Like hw_irq_ctrl_set_irq() but awake immediately the CPU instead of in
 * 1 delta cycle
 *
 * Call only from HW threads; Should be used with care
 *
 * This is an API towards the HW models. Embedded SW may not call it.
 */
void hw_irq_ctrl_raise_im(unsigned int inst, unsigned int irq)
{
  hw_irq_ctrl_irq_raise_prefix(inst, irq);
  irq_raising_from_hw_now(inst);
}

/**
 * Like hw_irq_ctrl_raise_im() but for SW threads
 *
 * Call only from SW threads; Should be used with care
 */
void hw_irq_ctrl_raise_im_from_sw(unsigned int inst, unsigned int irq)
{
  hw_irq_ctrl_irq_raise_prefix(inst, irq);

  if (nhw_intctrl_st[inst].irqs_locked == false) {
    nsif_cpun_irq_raised_from_sw(inst);
  }
}

/*
 * Event timer handler for the IRQ controller HW model
 */
static void hw_irq_ctrl_timer_triggered(void)
{
  Timer_irq_ctrl = TIME_NEVER;
  for (int i = 0; i < NHW_INTCTRL_TOTAL_INST; i++) {
    if (nhw_intctrl_st[i].awaking_CPU) {
      nhw_intctrl_st[i].awaking_CPU = false;
      irq_raising_from_hw_now(i);
    }
  }
  nsi_hws_find_next_event();
}

NSI_HW_EVENT(Timer_irq_ctrl, hw_irq_ctrl_timer_triggered, 10 /* Purposely a low value */);

/*
 * Get the name of an interrupt given the interrupt controller instance
 * and the int line number.
 *
 * Only for debugging/logging/tracing purposes.
 */
const char *hw_irq_ctrl_get_name(unsigned int inst, unsigned int irq)
{
  static const char *irqnames[NHW_INTCTRL_TOTAL_INST][NHW_INTCTRL_MAX_INTLINES] = NHW_INT_NAMES;

  if ((inst < NHW_INTCTRL_TOTAL_INST) && (irq < NHW_INTCTRL_MAX_INTLINES)) {
    return irqnames[inst][irq];
  } else {
    return NULL;
  }
}

/*
 * NOTE: This is not a Interrupt controller function per se, but a common function
 * for all peripherals to evaluate if they want to raise or not
 * their interrupt line
 *
 * int_line      Previous/Current interrutp line level
 * new_int_line  new interrupt line level
 * irq_map       Interrupt mapping (interrupt controller and line)
 */
void hw_irq_ctrl_toggle_level_irq_line_if(bool *int_line,
                                          bool new_int_line,
                                          struct nhw_irq_mapping *irq_map)
{
  if (*int_line == false && new_int_line == true) {
    *int_line = true;
    hw_irq_ctrl_raise_level_irq_line(irq_map->cntl_inst,
                                     irq_map->int_nbr);
  } else if (*int_line == true && new_int_line == false) {
    *int_line = false;
    hw_irq_ctrl_lower_level_irq_line(irq_map->cntl_inst,
                                     irq_map->int_nbr);
  }

}
