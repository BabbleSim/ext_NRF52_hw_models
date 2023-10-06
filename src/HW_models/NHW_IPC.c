/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * IPC - Interprocessor communication
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/ipc.html?cp=4_0_0_6_15
 *
 * This file provides the implementation of the nRF5340 IPC peripherals,
 * and instantiates N of them (and initializes them at startup and frees them on exit),
 * as described in the configuration (NHW_config.h)
 *
 * Each IPC instance has a configurable number of channels/tasks/events (up to 32)
 *
 * Notes:
 *   * Close enough events in the IPC channels are not merged.
 *     Note the spec does not specify t_IPC.
 */

#include <string.h>
#include <stdint.h>
#include "nsi_tasks.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_templates.h"
#include "irq_ctrl.h"
#include "NHW_DPPI.h"

struct ipc_status {
  NRF_IPC_Type *NRF_IPC_regs;
  uint n_ch;  /* Number of events configured in this IPC instance */

  uint dppi_map;   //To which DPPI instance are this IPC subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem* subscribed;   //[n_ch]
};

static uint nhw_IPC_dppi_map[NHW_IPC_TOTAL_INST] = NHW_IPC_DPPI_MAP;
static struct ipc_status nhw_ipc_st[NHW_IPC_TOTAL_INST];
NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];

/**
 * Initialize the IPC model
 */
static void nhw_ipc_init(void) {
  /* Mapping of peripheral instance to DPPI instance */
  uint nhw_ipc_n_ch[NHW_IPC_TOTAL_INST] = NHW_IPC_N_CH;

  memset(NRF_IPC_regs, 0, sizeof(NRF_IPC_Type) * NHW_IPC_TOTAL_INST);

  for (int i = 0; i< NHW_IPC_TOTAL_INST; i++) {
    nhw_ipc_st[i].NRF_IPC_regs = &NRF_IPC_regs[i];
    nhw_ipc_st[i].n_ch = nhw_ipc_n_ch[i];

    nhw_ipc_st[i].dppi_map = nhw_IPC_dppi_map[i];
    nhw_ipc_st[i].subscribed = (struct nhw_subsc_mem*)bs_calloc(nhw_ipc_n_ch[i], sizeof(struct nhw_subsc_mem));
  }
}

NSI_TASK(nhw_ipc_init, HW_INIT, 100);

/*
 * Free all IPC instances resources before program exit
 */
static void nhw_ipc_free(void)
{
  for (int i = 0; i < NHW_IPC_TOTAL_INST; i++) {
    free(nhw_ipc_st[i].subscribed);
    nhw_ipc_st[i].subscribed = NULL;
  }
}

NSI_TASK(nhw_ipc_free, ON_EXIT_PRE, 100);

static void nhw_IPC_eval_interrupt(uint inst) {
  static bool ipc_int_line[NHW_IPC_TOTAL_INST]; /* Is this IPC currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_ipc_irq_map[NHW_IPC_TOTAL_INST] = NHW_IPC_INT_MAP;
  bool new_int_line = false;

  struct ipc_status *this = &nhw_ipc_st[inst];
  NRF_IPC_Type *IPC_regs = &NRF_IPC_regs[inst];

  uint32_t event_m = 0;

  for (int i = 0; i < this->n_ch; i++) {
    if (IPC_regs->EVENTS_RECEIVE[i]) {
      event_m |= 1<<i;
    }
  }

  IPC_regs->INTPEND = IPC_regs->INTEN & event_m;

  new_int_line = (IPC_regs->INTPEND != 0);

  hw_irq_ctrl_toggle_level_irq_line_if(&ipc_int_line[inst],
                                       new_int_line,
                                       &nhw_ipc_irq_map[inst]);
}

#define CHECK_VALID_CHANNEL(inst, ch, what) \
  if (nhw_ipc_st[inst].n_ch <= ch) { \
    bs_trace_error_time_line("Attempting to access IPC%i %s for channel %u>=%u\n", \
                             inst, what, ch, nhw_ipc_st[inst].n_ch); \
  }

static void nhw_IPC_signal_EVENT(uint inst, uint ch) {
  NRF_IPC_regs[inst].EVENTS_RECEIVE[ch] = 1;
  nhw_IPC_eval_interrupt(inst);
  nhw_dppi_event_signal_if(nhw_IPC_dppi_map[inst], NRF_IPC_regs[inst].PUBLISH_RECEIVE[ch]);
}

/*
 * Signal an event in channel <ch> in the IPC backbone
 * i.e. Check for all IPC peripherals, if they are receiving
 * in that channel, and if so have them signal the corresponding
 * EVENT RECEIVE
 */
static void nhw_IPC_notify_ipc_ch(uint ch) {
  for (uint inst = 0; inst <= NHW_IPC_TOTAL_INST; inst++) {
    uint ch_mask = 1 << ch;
    for (int i = 0; i < nhw_ipc_st[inst].n_ch; i++) {
      if (NRF_IPC_regs[inst].RECEIVE_CNF[i] & ch_mask) {
        nhw_IPC_signal_EVENT(inst, i);
      }
    }
  }
}

static void nhw_IPC_TASKS_SEND(uint inst, uint ch) {
  CHECK_VALID_CHANNEL(inst, ch, "TASK_SEND");

  uint32_t send_cnf = NRF_IPC_regs[inst].SEND_CNF[ch];

  for (int i = __builtin_ffs(send_cnf) - 1; i >= 0; i = __builtin_ffs(send_cnf) - 1) {
    nhw_IPC_notify_ipc_ch(i);
    send_cnf &= ~(1 << i);
  }
}

void nhw_IPC_regw_sideeffects_TASKS_SEND(uint inst, uint ch) {
  if (NRF_IPC_regs[inst].TASKS_SEND[ch]) {
    NRF_IPC_regs[inst].TASKS_SEND[ch] = 0;
    nhw_IPC_TASKS_SEND(inst, ch);
  }
}

static void nhw_IPC_tasks_send_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint n = (uintptr_t)param & 0xFFFF;
  nhw_IPC_TASKS_SEND(inst, n);
}

void nhw_IPC_regw_sideeffects_SUBSCRIBE_SEND(uint inst, uint ch) {
  struct ipc_status *this = &nhw_ipc_st[inst];

  CHECK_VALID_CHANNEL(inst, ch, "SUBSCRIBE");

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_IPC_regs->SUBSCRIBE_SEND[ch],
                                       &this->subscribed[ch],
                                       nhw_IPC_tasks_send_wrap,
                                       (void*)((inst << 16) + ch));
}

NHW_SIDEEFFECTS_INTEN(IPC, NRF_IPC_regs[inst]., NRF_IPC_regs[inst].INTEN)
NHW_SIDEEFFECTS_INTSET(IPC, NRF_IPC_regs[inst]., NRF_IPC_regs[inst].INTEN)
NHW_SIDEEFFECTS_INTCLR(IPC, NRF_IPC_regs[inst]., NRF_IPC_regs[inst].INTEN)

NHW_SIDEEFFECTS_EVENTS(IPC)
