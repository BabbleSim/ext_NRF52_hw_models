/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * EGU - Event generator unit
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/egu.html?cp=5_1_0_5_6
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/egu.html#concept_x4j_fx1_lr
 *
 * This file provides the implementation of the EGU peripherals,
 * and instantiates N of them (and initializes them at startup and frees them on exit),
 * as described in the configuration (NHW_config.h)
 *
 * Each EGU instance has a configurable number of tasks/events (up to 32)
 */

#include <string.h>
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "irq_ctrl.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_xPPI.h"
#include "NHW_EGU.h"
#include "nsi_tasks.h"


struct egu_status {
  NRF_EGU_Type *NRF_EGU_regs;
  uint n_events;  /* Number of events configured in this EGU instance */

#if (NHW_HAS_DPPI)
  uint dppi_map;   //To which DPPI instance are this EGU subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem* subscribed;   //[n_events]
#endif
};

static struct egu_status nhw_egu_st[NHW_EGU_TOTAL_INST];
NRF_EGU_Type NRF_EGU_regs[NHW_EGU_TOTAL_INST];

/**
 * Initialize the EGU model
 */
static void nhw_egu_init(void) {
#if (NHW_HAS_DPPI)
  /* Mapping of peripheral instance to DPPI instance */
  uint nhw_egu_dppi_map[NHW_EGU_TOTAL_INST] = NHW_EGU_DPPI_MAP;
#endif

  uint nhw_egu_n_events[NHW_EGU_TOTAL_INST] = NHW_EGU_N_EVENTS;

  memset(NRF_EGU_regs, 0, sizeof(NRF_EGU_Type) * NHW_EGU_TOTAL_INST);

  for (int i = 0; i< NHW_EGU_TOTAL_INST; i++) {
    nhw_egu_st[i].NRF_EGU_regs = &NRF_EGU_regs[i];
    nhw_egu_st[i].n_events = nhw_egu_n_events[i];

#if (NHW_HAS_DPPI)
    nhw_egu_st[i].dppi_map = nhw_egu_dppi_map[i];
    nhw_egu_st[i].subscribed = (struct nhw_subsc_mem*)bs_calloc(nhw_egu_n_events[i], sizeof(struct nhw_subsc_mem));
#endif
  }
}

NSI_TASK(nhw_egu_init, HW_INIT, 100);

#if (NHW_HAS_DPPI)
/*
 * Free all EGU instances resources before program exit
 */
static void nhw_egu_free(void)
{
  for (int i = 0; i< NHW_EGU_TOTAL_INST; i++) {
    free(nhw_egu_st[i].subscribed);
    nhw_egu_st[i].subscribed = NULL;
  }
}

NSI_TASK(nhw_egu_free, ON_EXIT_PRE, 100);
#endif /* (NHW_HAS_DPPI) */

/**
 * Check if the interrupt line for EGU instance needs to be raised or lowered
 * (as a result of either the interrupt mask or the event register having
 * been modified)
 */
static void nhw_egu_eval_interrupt(uint inst)
{
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_egu_irq_map[NHW_EGU_TOTAL_INST] = NHW_EGU_INT_MAP;
  static bool egu_int_line[NHW_EGU_TOTAL_INST] = {false}; //Is the EGU currently driving this interrupt line high

  struct egu_status *this = &nhw_egu_st[inst];

  NRF_EGU_Type *EGU_regs = this->NRF_EGU_regs;
  bool new_egu_int_line = false;

  for (int i = 0; i < this->n_events; i++) {
    int int_mask = (EGU_regs->INTEN >> i) & 1;
    if (int_mask && EGU_regs->EVENTS_TRIGGERED[i]) {
      new_egu_int_line = true;
    }
  }

  hw_irq_ctrl_toggle_level_irq_line_if(&egu_int_line[inst],
                                       new_egu_int_line,
                                       &nhw_egu_irq_map[inst]);
}

static inline void nhw_egu_check_inst_event(uint egu_inst, uint nbr, const char *type)
{
  if ((egu_inst >= NHW_EGU_TOTAL_INST) || (nbr >= nhw_egu_st[egu_inst].n_events) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u (>= %u) in EGU instance %u\n",
                             type, nbr, nhw_egu_st[egu_inst].n_events, egu_inst);
  }
}

static void nhw_egu_signal_EVENTS_TRIGGERED(uint inst, uint event_nbr)
{
  struct egu_status *this = &nhw_egu_st[inst];
  NRF_EGU_Type *EGU_regs = this->NRF_EGU_regs;

  EGU_regs->EVENTS_TRIGGERED[event_nbr] = 1;
  nhw_egu_eval_interrupt(inst);

#if (NHW_HAS_PPI)
  { //Signal the PPI
    const ppi_event_types_t PPI_EGU_EVENTS_base[NHW_EGU_TOTAL_INST] = {
      EGU0_EVENTS_TRIGGERED_0,
      EGU1_EVENTS_TRIGGERED_0,
      EGU2_EVENTS_TRIGGERED_0,
      EGU3_EVENTS_TRIGGERED_0,
      EGU4_EVENTS_TRIGGERED_0,
      EGU5_EVENTS_TRIGGERED_0,
    }; //The events for each EGU instance are assumed consecutive

    nrf_ppi_event(PPI_EGU_EVENTS_base[inst]+ event_nbr);
  }
#elif (NHW_HAS_DPPI)
  nhw_dppi_event_signal_if(this->dppi_map,
                           EGU_regs->PUBLISH_TRIGGERED[event_nbr]);
#endif
}

/**
 * Do whatever is needed after the task has been triggered
 */
void nhw_egu_TASK_TRIGGER(uint inst, uint task_nbr)
{
  nhw_egu_check_inst_event(inst, task_nbr, "task");
  nhw_egu_signal_EVENTS_TRIGGERED(inst, task_nbr);
}

#if (NHW_HAS_DPPI)
static void nhw_egu_tasktrigger_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint n = (uintptr_t)param & 0xFFFF;
  nhw_egu_TASK_TRIGGER(inst, n);
}

void nhw_egu_regw_sideeffects_SUBSCRIBE_TRIGGER(uint inst, uint n) {
  struct egu_status *this = &nhw_egu_st[inst];

  nhw_egu_check_inst_event(inst, n, "subscribe");

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_EGU_regs->SUBSCRIBE_TRIGGER[n],
                                       &this->subscribed[n],
                                       nhw_egu_tasktrigger_wrap,
                                       (void*)((inst << 16) + n));
}
#endif /* NHW_HAS_DPPI */

void nhw_egu_regw_sideeffect_INTENSET(uint inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  if ( EGU_regs->INTENSET ) {
    EGU_regs->INTEN   |= EGU_regs->INTENSET;
    EGU_regs->INTENSET = EGU_regs->INTEN;

    nhw_egu_eval_interrupt(inst);
  }
}

void nhw_egu_regw_sideeffect_INTENCLR(uint inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  if ( EGU_regs->INTENCLR ) {
    EGU_regs->INTEN   &= ~EGU_regs->INTENCLR;
    EGU_regs->INTENSET = EGU_regs->INTEN;
    EGU_regs->INTENCLR = 0; //We do not model reading the INTEN register thru the INTENCLR one

    nhw_egu_eval_interrupt(inst);
  }
}

void nhw_egu_regw_sideeffect_INTEN(uint inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  EGU_regs->INTENSET = EGU_regs->INTEN;
  nhw_egu_eval_interrupt(inst);
}

void nhw_egu_regw_sideeffect_EVENTS_TRIGGERED(uint inst, uint event_nbr) {
  nhw_egu_check_inst_event(inst, event_nbr, "event");
  nhw_egu_eval_interrupt(inst);
}

void nhw_egu_regw_sideeffects_TASK_TRIGGER(uint inst, uint task_nbr){
  nhw_egu_check_inst_event(inst, task_nbr, "task");

  if (NRF_EGU_regs[inst].TASKS_TRIGGER[task_nbr] != 0) {
    NRF_EGU_regs[inst].TASKS_TRIGGER[task_nbr] = 0;
    nhw_egu_TASK_TRIGGER(inst, task_nbr);
  }
}

#if (NHW_HAS_PPI)
/*
 * Trampolines to automatically call from the PPI
 */
/* Generated with:
#! /usr/bin/env python3
#EGU.c
for inst in range(0,6):
    for i in range(0,16):
        print("void nhw_egu_%i_TASK_TRIGGER_%i(void){ nhw_eguTASK_TRIGGER(%i,%i); }"%(inst,i,inst,i))
#EGU.h
for inst in range(0,6):
    for i in range(0,16):
        print("void nhw_egu%i_TASK_TRIGGER_%i(void);"%(inst,i))
#PPI connections:
#PPI ppi_tasks_table:
for inst in range(0,6):
    for i in range(0,16):
        print("{(void*)&NRF_EGU_regs[%i].TASKS_TRIGGER[%i] , nhw_egu%i_TASK_TRIGGER_%i},"%(inst,i,inst,i))
#PPI ppi_events_table:
for inst in range(0,6):
    for i in range(0,16):
        print("{EGU%i_EVENTS_TRIGGERED_%i, &NRF_EGU_regs[%i].EVENTS_TRIGGERED[%i]},"%(inst,i,inst,i))
 */
void nhw_egu_0_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(0,0); }
void nhw_egu_0_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(0,1); }
void nhw_egu_0_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(0,2); }
void nhw_egu_0_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(0,3); }
void nhw_egu_0_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(0,4); }
void nhw_egu_0_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(0,5); }
void nhw_egu_0_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(0,6); }
void nhw_egu_0_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(0,7); }
void nhw_egu_0_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(0,8); }
void nhw_egu_0_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(0,9); }
void nhw_egu_0_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(0,10); }
void nhw_egu_0_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(0,11); }
void nhw_egu_0_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(0,12); }
void nhw_egu_0_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(0,13); }
void nhw_egu_0_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(0,14); }
void nhw_egu_0_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(0,15); }
void nhw_egu_1_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(1,0); }
void nhw_egu_1_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(1,1); }
void nhw_egu_1_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(1,2); }
void nhw_egu_1_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(1,3); }
void nhw_egu_1_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(1,4); }
void nhw_egu_1_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(1,5); }
void nhw_egu_1_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(1,6); }
void nhw_egu_1_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(1,7); }
void nhw_egu_1_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(1,8); }
void nhw_egu_1_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(1,9); }
void nhw_egu_1_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(1,10); }
void nhw_egu_1_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(1,11); }
void nhw_egu_1_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(1,12); }
void nhw_egu_1_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(1,13); }
void nhw_egu_1_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(1,14); }
void nhw_egu_1_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(1,15); }
void nhw_egu_2_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(2,0); }
void nhw_egu_2_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(2,1); }
void nhw_egu_2_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(2,2); }
void nhw_egu_2_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(2,3); }
void nhw_egu_2_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(2,4); }
void nhw_egu_2_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(2,5); }
void nhw_egu_2_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(2,6); }
void nhw_egu_2_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(2,7); }
void nhw_egu_2_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(2,8); }
void nhw_egu_2_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(2,9); }
void nhw_egu_2_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(2,10); }
void nhw_egu_2_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(2,11); }
void nhw_egu_2_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(2,12); }
void nhw_egu_2_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(2,13); }
void nhw_egu_2_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(2,14); }
void nhw_egu_2_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(2,15); }
void nhw_egu_3_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(3,0); }
void nhw_egu_3_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(3,1); }
void nhw_egu_3_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(3,2); }
void nhw_egu_3_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(3,3); }
void nhw_egu_3_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(3,4); }
void nhw_egu_3_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(3,5); }
void nhw_egu_3_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(3,6); }
void nhw_egu_3_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(3,7); }
void nhw_egu_3_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(3,8); }
void nhw_egu_3_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(3,9); }
void nhw_egu_3_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(3,10); }
void nhw_egu_3_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(3,11); }
void nhw_egu_3_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(3,12); }
void nhw_egu_3_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(3,13); }
void nhw_egu_3_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(3,14); }
void nhw_egu_3_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(3,15); }
void nhw_egu_4_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(4,0); }
void nhw_egu_4_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(4,1); }
void nhw_egu_4_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(4,2); }
void nhw_egu_4_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(4,3); }
void nhw_egu_4_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(4,4); }
void nhw_egu_4_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(4,5); }
void nhw_egu_4_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(4,6); }
void nhw_egu_4_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(4,7); }
void nhw_egu_4_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(4,8); }
void nhw_egu_4_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(4,9); }
void nhw_egu_4_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(4,10); }
void nhw_egu_4_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(4,11); }
void nhw_egu_4_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(4,12); }
void nhw_egu_4_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(4,13); }
void nhw_egu_4_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(4,14); }
void nhw_egu_4_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(4,15); }
void nhw_egu_5_TASK_TRIGGER_0(void){ nhw_egu_TASK_TRIGGER(5,0); }
void nhw_egu_5_TASK_TRIGGER_1(void){ nhw_egu_TASK_TRIGGER(5,1); }
void nhw_egu_5_TASK_TRIGGER_2(void){ nhw_egu_TASK_TRIGGER(5,2); }
void nhw_egu_5_TASK_TRIGGER_3(void){ nhw_egu_TASK_TRIGGER(5,3); }
void nhw_egu_5_TASK_TRIGGER_4(void){ nhw_egu_TASK_TRIGGER(5,4); }
void nhw_egu_5_TASK_TRIGGER_5(void){ nhw_egu_TASK_TRIGGER(5,5); }
void nhw_egu_5_TASK_TRIGGER_6(void){ nhw_egu_TASK_TRIGGER(5,6); }
void nhw_egu_5_TASK_TRIGGER_7(void){ nhw_egu_TASK_TRIGGER(5,7); }
void nhw_egu_5_TASK_TRIGGER_8(void){ nhw_egu_TASK_TRIGGER(5,8); }
void nhw_egu_5_TASK_TRIGGER_9(void){ nhw_egu_TASK_TRIGGER(5,9); }
void nhw_egu_5_TASK_TRIGGER_10(void){ nhw_egu_TASK_TRIGGER(5,10); }
void nhw_egu_5_TASK_TRIGGER_11(void){ nhw_egu_TASK_TRIGGER(5,11); }
void nhw_egu_5_TASK_TRIGGER_12(void){ nhw_egu_TASK_TRIGGER(5,12); }
void nhw_egu_5_TASK_TRIGGER_13(void){ nhw_egu_TASK_TRIGGER(5,13); }
void nhw_egu_5_TASK_TRIGGER_14(void){ nhw_egu_TASK_TRIGGER(5,14); }
void nhw_egu_5_TASK_TRIGGER_15(void){ nhw_egu_TASK_TRIGGER(5,15); }
#endif /* NHW_HAS_PPI */
