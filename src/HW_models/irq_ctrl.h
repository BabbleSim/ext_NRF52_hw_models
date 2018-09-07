/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _IRQ_CTRL_H
#define _IRQ_CTRL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void hw_irq_ctrl_init();
void hw_irq_ctrl_raise_im(uint32_t irq);
uint64_t hw_irq_ctrl_get_irq_status();
void hw_irq_ctrl_clear_all_irqs();
void hw_irq_ctrl_clear_irq(unsigned int irq);
void hw_irq_controller_set_irq_mask(uint64_t mask);
uint64_t hw_irq_ctrl_get_irq_mask();
void hw_irq_ctrl_enable_irq(unsigned int irq);
void hw_irq_ctrl_disable_irq(unsigned int irq);
int hw_irq_ctrl_is_irq_enabled(unsigned int irq);
void hw_irq_ctrl_timer_triggered();
void hw_irq_ctrl_set_irq(unsigned int irq);
void hw_irq_ctrl_raise_im(unsigned int irq);
void hw_irq_ctrl_raise_im_from_sw(unsigned int irq);
uint32_t hw_irq_ctrl_get_current_lock(void);
int hw_irq_ctrl_get_highest_prio_irq(void);
int hw_irq_ctrl_get_cur_prio(void);
void hw_irq_ctrl_set_cur_prio(int new);
uint8_t hw_irq_ctrl_get_prio(unsigned int irq);
void hw_irq_ctrl_prio_set(unsigned int irq, unsigned int prio);
uint32_t hw_irq_ctrl_change_lock(uint32_t new_lock);

/*
 * This interrupt will awake the CPU if IRQs are not locked,
 * This interrupt does not have an associated status bit or handler
 */
#define PHONY_WEAK_IRQ 0xFFFE
/*
 * This interrupt will awake the CPU even if IRQs are locked,
 * This interrupt does not have an associated status bit or handler
 * (the lock is only ignored when the interrupt is raised from the HW models,
 * SW threads should not try to use this)
 */
#define PHONY_HARD_IRQ 0xFFFF

#define NRF_HW_NBR_IRQs 32

#ifdef __cplusplus
}
#endif

#endif
