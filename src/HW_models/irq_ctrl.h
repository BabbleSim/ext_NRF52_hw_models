/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_SRC_HW_MODELS_IRQ_CTRL_H
#define _NRF_HW_MODEL_SRC_HW_MODELS_IRQ_CTRL_H

#include <stdint.h>
#include "NHW_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void hw_irq_ctrl_clear_all_irqs(unsigned int inst);
void hw_irq_ctrl_clear_irq(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_reeval_level_irq(unsigned int inst, unsigned int irq);
void nhw_irq_controller_set_irq_mask(unsigned int inst, uint64_t mask);
void hw_irq_ctrl_enable_irq(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_disable_irq(unsigned int inst, unsigned int irq);
int hw_irq_ctrl_is_irq_enabled(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_raise_im(unsigned int inst, uint32_t irq);
void hw_irq_ctrl_set_irq(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_raise_level_irq_line(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_lower_level_irq_line(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_raise_im_from_sw(unsigned int inst, unsigned int irq);
uint32_t hw_irq_ctrl_get_current_lock(unsigned int inst);
int hw_irq_ctrl_get_highest_prio_irq(unsigned int inst);
int hw_irq_ctrl_get_cur_prio(unsigned int inst);
void hw_irq_ctrl_set_cur_prio(unsigned int inst, int new);
uint8_t hw_irq_ctrl_get_prio(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_prio_set(unsigned int inst, unsigned int irq, unsigned int prio);
uint32_t hw_irq_ctrl_change_lock(unsigned int inst, uint32_t new_lock);
const char *hw_irq_ctrl_get_name(unsigned int inst, unsigned int irq);
void hw_irq_ctrl_toggle_level_irq_line_if(bool *int_line,
                                          bool new_int_line,
                                          struct nhw_irq_mapping *irq_map);

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

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_SRC_HW_MODELS_IRQ_CTRL_H */
