/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * EGU — Event generator unit
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/egu.html?cp=4_1_0_5_6
 */

#include <string.h>
#include "bs_tracing.h"
#include "irq_ctrl.h"
#include "NRF_EGU.h"
#include "NRF_PPI.h"

#define N_EGU 6
#define N_EGU_EVENTS 16
NRF_EGU_Type NRF_EGU_regs[N_EGU];
static bool egu_int_line[N_EGU] = {false}; //Is the EGU currently driving this interrupt line high

/**
 * Initialize the EGU model
 */
void nrf_egu_init(){
  memset(NRF_EGU_regs, 0, sizeof(NRF_EGU_Type)*N_EGU);
  memset(egu_int_line, 0, sizeof(egu_int_line));
}

/**
 * Clean up the EGU model before program exit
 */
void nrf_egu_clean_up(){
  /* Intentionally empty */
}

/**
 * Check if the interrupt line for EGU instance needs to be raised or lowered
 * (as a result of either the interrupt mask or the event register having
 * been modified)
 */
static void nrf_egu_eval_interrupt(int inst){
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  bool new_egu_int_line = false;

  for (int i = 0; i < N_EGU_EVENTS; i++) {
    int int_mask = (EGU_regs->INTEN >> i) & 1;
    if (int_mask && EGU_regs->EVENTS_TRIGGERED[i]) {
      new_egu_int_line = true;
    }
  }

  if (new_egu_int_line && (egu_int_line[inst] == false)) {
    egu_int_line[inst] = true;
    hw_irq_ctrl_raise_level_irq_line(SWI0_EGU0_IRQn + inst);
  } else if ((new_egu_int_line == false) && egu_int_line[inst]) {
    egu_int_line[inst] = false;
    hw_irq_ctrl_lower_level_irq_line(SWI0_EGU0_IRQn + inst);
  }
}

static inline void nrf_egu_check_inst_event(uint egu_inst, uint nbr, const char *type){
  if ((egu_inst >= N_EGU) || (nbr >= N_EGU_EVENTS) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u in EGU instance %u\n", type, nbr, egu_inst);
  }
}

/**
 * Do whatever is needed after the task has been triggered
 */
void nrf_egu_TASK_TRIGGER(uint inst, uint task_nbr){
  nrf_egu_check_inst_event(inst, task_nbr, "task");

  NRF_EGU_regs[inst].EVENTS_TRIGGERED[task_nbr] = 1;
  nrf_egu_eval_interrupt(inst);

  { //Signal the PPI
    const ppi_event_types_t PPI_EGU_EVENTS_base[N_EGU]  = {
      EGU0_EVENTS_TRIGGERED_0,
      EGU1_EVENTS_TRIGGERED_0,
      EGU2_EVENTS_TRIGGERED_0,
      EGU3_EVENTS_TRIGGERED_0,
      EGU4_EVENTS_TRIGGERED_0,
      EGU5_EVENTS_TRIGGERED_0,
    }; //The events for each EGU instance are assumed consecutive

    nrf_ppi_event(PPI_EGU_EVENTS_base[inst]+ task_nbr);
  }
}

void nrf_egu_regw_sideeffect_INTENSET(int inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  if ( EGU_regs->INTENSET ) {
    EGU_regs->INTEN   |= EGU_regs->INTENSET;
    EGU_regs->INTENSET = EGU_regs->INTEN;

    nrf_egu_eval_interrupt(inst);
  }
}

void nrf_egu_regw_sideeffect_INTENCLR(int inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  if ( EGU_regs->INTENCLR ) {
    EGU_regs->INTEN   &= ~EGU_regs->INTENCLR;
    EGU_regs->INTENSET = EGU_regs->INTEN;
    EGU_regs->INTENCLR = 0; //We do not model reading the INTEN register thru the INTENCLR one

    nrf_egu_eval_interrupt(inst);
  }
}

void nrf_egu_regw_sideeffect_INTEN(int inst) {
  NRF_EGU_Type *EGU_regs = &NRF_EGU_regs[inst];
  EGU_regs->INTENSET = EGU_regs->INTEN;
  nrf_egu_eval_interrupt(inst);
}

void nrf_egu_regw_sideeffect_EVENTS_TRIGGERED(int inst, uint event_nbr) {
  nrf_egu_check_inst_event(inst, event_nbr, "event");
  nrf_egu_eval_interrupt(inst);
}

void nrf_egu_regw_sideeffects_TASK_TRIGGER(uint inst, uint task_nbr){
  nrf_egu_check_inst_event(inst, task_nbr, "task");

  if (NRF_EGU_regs[inst].TASKS_TRIGGER[task_nbr] != 0) {
    NRF_EGU_regs[inst].TASKS_TRIGGER[task_nbr] = 0;
    nrf_egu_TASK_TRIGGER(inst, task_nbr);
  }
}

/*
 * Trampolines to automatically call from the PPI
 */
/* Generated with:
#! /usr/bin/env python3
#EGU.c
for inst in range(0,6):
    for i in range(0,16):
        print("void nrf_egu_%i_TASK_TRIGGER_%i(void){ nrf_egu_TASK_TRIGGER(%i,%i); }"%(inst,i,inst,i))
#EGU.h
for inst in range(0,6):
    for i in range(0,16):
        print("void nrf_egu_%i_TASK_TRIGGER_%i(void);"%(inst,i))
#PPI connections:
#PPI ppi_tasks_table:
for inst in range(0,6):
    for i in range(0,16):
        print("{(void*)&NRF_EGU_regs[%i].TASKS_TRIGGER[%i] , nrf_egu_%i_TASK_TRIGGER_%i},"%(inst,i,inst,i))
#PPI ppi_events_table:
for inst in range(0,6):
    for i in range(0,16):
        print("{EGU%i_EVENTS_TRIGGERED_%i, &NRF_EGU_regs[%i].EVENTS_TRIGGERED[%i]},"%(inst,i,inst,i))
 */
void nrf_egu_0_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(0,0); }
void nrf_egu_0_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(0,1); }
void nrf_egu_0_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(0,2); }
void nrf_egu_0_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(0,3); }
void nrf_egu_0_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(0,4); }
void nrf_egu_0_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(0,5); }
void nrf_egu_0_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(0,6); }
void nrf_egu_0_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(0,7); }
void nrf_egu_0_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(0,8); }
void nrf_egu_0_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(0,9); }
void nrf_egu_0_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(0,10); }
void nrf_egu_0_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(0,11); }
void nrf_egu_0_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(0,12); }
void nrf_egu_0_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(0,13); }
void nrf_egu_0_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(0,14); }
void nrf_egu_0_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(0,15); }
void nrf_egu_1_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(1,0); }
void nrf_egu_1_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(1,1); }
void nrf_egu_1_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(1,2); }
void nrf_egu_1_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(1,3); }
void nrf_egu_1_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(1,4); }
void nrf_egu_1_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(1,5); }
void nrf_egu_1_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(1,6); }
void nrf_egu_1_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(1,7); }
void nrf_egu_1_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(1,8); }
void nrf_egu_1_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(1,9); }
void nrf_egu_1_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(1,10); }
void nrf_egu_1_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(1,11); }
void nrf_egu_1_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(1,12); }
void nrf_egu_1_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(1,13); }
void nrf_egu_1_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(1,14); }
void nrf_egu_1_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(1,15); }
void nrf_egu_2_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(2,0); }
void nrf_egu_2_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(2,1); }
void nrf_egu_2_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(2,2); }
void nrf_egu_2_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(2,3); }
void nrf_egu_2_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(2,4); }
void nrf_egu_2_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(2,5); }
void nrf_egu_2_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(2,6); }
void nrf_egu_2_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(2,7); }
void nrf_egu_2_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(2,8); }
void nrf_egu_2_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(2,9); }
void nrf_egu_2_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(2,10); }
void nrf_egu_2_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(2,11); }
void nrf_egu_2_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(2,12); }
void nrf_egu_2_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(2,13); }
void nrf_egu_2_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(2,14); }
void nrf_egu_2_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(2,15); }
void nrf_egu_3_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(3,0); }
void nrf_egu_3_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(3,1); }
void nrf_egu_3_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(3,2); }
void nrf_egu_3_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(3,3); }
void nrf_egu_3_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(3,4); }
void nrf_egu_3_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(3,5); }
void nrf_egu_3_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(3,6); }
void nrf_egu_3_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(3,7); }
void nrf_egu_3_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(3,8); }
void nrf_egu_3_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(3,9); }
void nrf_egu_3_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(3,10); }
void nrf_egu_3_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(3,11); }
void nrf_egu_3_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(3,12); }
void nrf_egu_3_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(3,13); }
void nrf_egu_3_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(3,14); }
void nrf_egu_3_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(3,15); }
void nrf_egu_4_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(4,0); }
void nrf_egu_4_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(4,1); }
void nrf_egu_4_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(4,2); }
void nrf_egu_4_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(4,3); }
void nrf_egu_4_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(4,4); }
void nrf_egu_4_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(4,5); }
void nrf_egu_4_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(4,6); }
void nrf_egu_4_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(4,7); }
void nrf_egu_4_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(4,8); }
void nrf_egu_4_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(4,9); }
void nrf_egu_4_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(4,10); }
void nrf_egu_4_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(4,11); }
void nrf_egu_4_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(4,12); }
void nrf_egu_4_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(4,13); }
void nrf_egu_4_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(4,14); }
void nrf_egu_4_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(4,15); }
void nrf_egu_5_TASK_TRIGGER_0(void){ nrf_egu_TASK_TRIGGER(5,0); }
void nrf_egu_5_TASK_TRIGGER_1(void){ nrf_egu_TASK_TRIGGER(5,1); }
void nrf_egu_5_TASK_TRIGGER_2(void){ nrf_egu_TASK_TRIGGER(5,2); }
void nrf_egu_5_TASK_TRIGGER_3(void){ nrf_egu_TASK_TRIGGER(5,3); }
void nrf_egu_5_TASK_TRIGGER_4(void){ nrf_egu_TASK_TRIGGER(5,4); }
void nrf_egu_5_TASK_TRIGGER_5(void){ nrf_egu_TASK_TRIGGER(5,5); }
void nrf_egu_5_TASK_TRIGGER_6(void){ nrf_egu_TASK_TRIGGER(5,6); }
void nrf_egu_5_TASK_TRIGGER_7(void){ nrf_egu_TASK_TRIGGER(5,7); }
void nrf_egu_5_TASK_TRIGGER_8(void){ nrf_egu_TASK_TRIGGER(5,8); }
void nrf_egu_5_TASK_TRIGGER_9(void){ nrf_egu_TASK_TRIGGER(5,9); }
void nrf_egu_5_TASK_TRIGGER_10(void){ nrf_egu_TASK_TRIGGER(5,10); }
void nrf_egu_5_TASK_TRIGGER_11(void){ nrf_egu_TASK_TRIGGER(5,11); }
void nrf_egu_5_TASK_TRIGGER_12(void){ nrf_egu_TASK_TRIGGER(5,12); }
void nrf_egu_5_TASK_TRIGGER_13(void){ nrf_egu_TASK_TRIGGER(5,13); }
void nrf_egu_5_TASK_TRIGGER_14(void){ nrf_egu_TASK_TRIGGER(5,14); }
void nrf_egu_5_TASK_TRIGGER_15(void){ nrf_egu_TASK_TRIGGER(5,15); }
