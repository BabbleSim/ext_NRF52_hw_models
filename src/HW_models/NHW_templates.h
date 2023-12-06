/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Template code (macros) for the most typical/repetitive
 * code of the HW models
 *
 * The _si versions are for peripheral which can only have one instance
 */
#ifndef _NRF_HW_MODEL_NHW_TEMPLATES_H
#define _NRF_HW_MODEL_NHW_TEMPLATES_H

#include "NHW_config.h"

/*
 * TASKS register write side-effects
 * (just calls thru to the task)
 */
#define NHW_SIDEEFFECTS_TASKS_si(peri, task) \
  void nhw_##peri##_regw_sideeffects_TASKS_##task(void) { \
    if ( NRF_##peri##_regs.TASKS_##task ) { \
      NRF_##peri##_regs.TASKS_##task = 0; \
      nhw_##peri##_TASK_##task(); \
    } \
  }

#define NHW_SIDEEFFECTS_TASKS(peri, peri_regs, task) \
  void nhw_##peri##_regw_sideeffects_TASKS_##task(unsigned int inst) { \
    if ( peri_regs TASKS_##task ) { \
      peri_regs TASKS_##task = 0; \
      nhw_##peri##_TASK_##task(inst); \
    } \
  }

/*
 * SUBSCRIBE register write side-effects
 */
#define NHW_SIDEEFFECTS_SUBSCRIBE_si(peri, task) \
  void nhw_##peri##_regw_sideeffects_SUBSCRIBE_##task(unsigned int inst) { \
    static struct nhw_subsc_mem task##_subscribed[NHW_##peri##_TOTAL_INST]; \
    nhw_dppi_common_subscribe_sideeffect(nhw_##peri##_dppi_map[inst], \
        NRF_##peri##_regs.SUBSCRIBE_##task, \
        &task##_subscribed[inst], \
        (dppi_callback_t)nhw_##peri##_TASK_##task, \
        DPPI_CB_NO_PARAM); \
  }

/*
 * Any EVENT register write side-effect
 */
#define NHW_SIDEEFFECTS_EVENTS(peri) \
  void nhw_##peri##_regw_sideeffects_EVENTS_all(unsigned int inst) { \
    nhw_##peri##_eval_interrupt(inst); \
  }


#if (NHW_HAS_PPI)
  #define _NHW_XPPI_EVENT(peri, peri_regs, inst, event)    \
    nrf_ppi_event(peri##_EVENTS_##event)
#elif (NHW_HAS_DPPI)
  #define _NHW_XPPI_EVENT(peri, peri_regs, inst, event)    \
     nhw_dppi_event_signal_if(nhw_##peri##_dppi_map[inst], \
                              peri_regs PUBLISH_##event)
#endif /* (NHW_HAS_PPI) / (NHW_HAS_DPPI)*/


#define _NHW_SIGNAL_EVENT_body(peri, peri_regs, event) \
  { \
    peri_regs EVENTS_##event = 1; \
    nhw_##peri##_eval_interrupt(inst); \
    _NHW_XPPI_EVENT(peri, peri_regs, inst, event); \
  }

/*
 * Signal an event:
 *  * Set the corresponding event register
 *  * Cause level interrupts to be reevaluated
 *  * Send the event either to the PPI or DPPI (if enabled)
 *
 * NOTE: Cannot be used for events with shortcuts
 * NOTE: Cannot be used for multi-instance peripherals connected to the PPI
 */
#define NHW_SIGNAL_EVENT(peri, peri_regs, event) \
  void nhw_##peri##_signal_EVENTS_##event(unsigned int inst) \
    _NHW_SIGNAL_EVENT_body(peri, peri_regs, event)

/*
 * Signal an event. Like NHW_SIGNAL_EVENT()
 * but when the event has shortcuts.
 *
 * NOTE: Cannot be used for multi-instance peripherals connected to the PPI
 */
#define NHW_SIGNAL_EVENT_ns(peri, peri_regs, event) \
  void nhw_##peri##_signal_EVENTS_##event##_noshort(unsigned int inst) \
    _NHW_SIGNAL_EVENT_body(peri, peri_regs, event)

#define NHW_SIGNAL_EVENT_si(peri, event) \
    NHW_SIGNAL_EVENT(peri, NRF_##peri##_regs. , event)

#define NHW_SIGNAL_EVENT_ns_si(peri, event) \
    NHW_SIGNAL_EVENT_ns(peri, NRF_##peri##_regs. , event)

#define NHW_SIDEEFFECTS_INTSET_si(peri, peri_regs, inten)\
  void nhw_##peri##_regw_sideeffects_INTENSET(void) { \
    if ( peri_regs INTENSET ){ /* LCOV_EXCL_BR_LINE */\
      inten |= peri_regs INTENSET;                    \
      peri_regs INTENSET = inten;                     \
      nhw_##peri##_eval_interrupt(0);                \
    }                                                 \
  }

#define NHW_SIDEEFFECTS_INTCLR_si(peri, peri_regs, inten) \
  void nhw_##peri##_regw_sideeffects_INTENCLR(void) { \
    if ( peri_regs INTENCLR ){/* LCOV_EXCL_BR_LINE */ \
       inten &= ~(peri_regs INTENCLR);                \
       peri_regs INTENSET = inten;                    \
       peri_regs INTENCLR = 0;                        \
       nhw_##peri## _eval_interrupt(0);               \
    }                                                 \
  }

#define NHW_SIDEEFFECTS_INTSET(peri, peri_regs, inten) \
  void nhw_##peri##_regw_sideeffects_INTENSET(uint inst) { \
    if ( peri_regs INTENSET ){ /* LCOV_EXCL_BR_LINE */\
      inten |= peri_regs INTENSET;                    \
      peri_regs INTENSET = inten;                     \
      nhw_##peri##_eval_interrupt(inst);              \
    }                                                 \
  }

#define NHW_SIDEEFFECTS_INTEN(peri, peri_regs, inten) \
  void nhw_##peri##_regw_sideeffects_INTEN(uint inst) { \
    peri_regs INTENSET = inten;                     \
    nhw_##peri##_eval_interrupt(inst);              \
  }

#define NHW_SIDEEFFECTS_INTCLR(peri, peri_regs, inten) \
  void nhw_##peri##_regw_sideeffects_INTENCLR(uint inst) { \
    if ( peri_regs INTENCLR ){/* LCOV_EXCL_BR_LINE */ \
       inten &= ~(peri_regs INTENCLR);                \
       peri_regs INTENSET = inten;                    \
       peri_regs INTENCLR = 0;                        \
       nhw_##peri## _eval_interrupt(inst);            \
    }                                                 \
  }

#define NHW_CHECK_INTERRUPT(peri, peri_regs, event, inten) \
  if (peri_regs EVENTS_##event && (inten &  peri##_INTENSET_##event##_Msk)){ \
    new_int_line = true; \
  }

#define NHW_CHECK_INTERRUPT_si(peri, event, inten) \
  if (NRF_##peri##_regs.EVENTS_##event && (inten &  peri##_INTENSET_##event##_Msk)){ \
    new_int_line = true; \
  }

#define NHW_SHORT_si(peri, event, task) \
  if (NRF_##peri##_regs.SHORTS & peri##_SHORTS_##event##_##task##_Msk) { \
    nhw_##peri##_TASK_##task(); \
  }

#define NHW_SHORT(peri, inst, peri_regs, event, task) \
  if (peri_regs SHORTS & peri##_SHORTS_##event##_##task##_Msk) { \
    nhw_##peri##_TASK_##task(inst); \
  }

#endif /* _NRF_HW_MODEL_NHW_TEMPLATES_H */
