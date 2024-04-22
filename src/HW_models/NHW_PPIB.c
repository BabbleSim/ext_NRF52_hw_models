/*
 * Copyright (c) 2024, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * PPIB - PPI Bridge
 *
 * This file provides the implementation of the nRF54 PPIB peripherals,
 * and instantiates N of them (and initializes them at startup and frees them on exit),
 * as described in the configuration (NHW_config.h)
 *
 * Each PPIB instance has a configurable number of channels
 *
 * Notes:
 *   * There is no handshake delay in between PPIB bridges,
 *     and therefore overflows cannot occur.
 *     OVERFLOW.SEND is just reset to 0 and never raised by the models.
 *
 *   * Just like in real HW, TASKS_SEND and EVENT_RECEIVE registers are not
 *     connected to anything. The HW model does not toggle them or react to
 *     them being written.
 */

#include <string.h>
#include <stdint.h>
#include "nsi_tasks.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_DPPI.h"

static void nhw_ppib_propagate_event_mate(uint inst, uint ppib_ch);

struct ppib_status {
  NRF_PPIB_Type *NRF_PPIB_regs;
  uint n_ch;  /* Number of channels */
  uint ppib_mate;

  uint dppi_map;   //To which DPPI instance are this PPIB subscribe&publish ports connected to
  //Which of the subscription ports are currently connected, and to which channel:
  struct nhw_subsc_mem* subscribed;   //[n_ch]
};

static struct ppib_status nhw_ppib_st[NHW_PPIB_TOTAL_INST];
NRF_PPIB_Type NRF_PPIB_regs[NHW_PPIB_TOTAL_INST];

/**
 * Initialize the PPIB model
 */
static void nhw_ppib_init(void) {
  /* Mapping of peripheral instance to DPPI instance */
  const uint nhw_ppib_n_ch[NHW_PPIB_TOTAL_INST] = NHW_PPIB_N_CH;
  const uint nhw_PPIB_dppi_map[NHW_PPIB_TOTAL_INST] = NHW_PPIB_DPPI_MAP;
  const uint nhw_PPIB_mates[NHW_PPIB_TOTAL_INST] = NHW_PPIB_MATE;

  memset(NRF_PPIB_regs, 0, sizeof(NRF_PPIB_Type) * NHW_PPIB_TOTAL_INST);

  for (int i = 0; i< NHW_PPIB_TOTAL_INST; i++) {
    nhw_ppib_st[i].NRF_PPIB_regs = &NRF_PPIB_regs[i];
    nhw_ppib_st[i].n_ch = nhw_ppib_n_ch[i];
    nhw_ppib_st[i].ppib_mate = nhw_PPIB_mates[i];

    nhw_ppib_st[i].dppi_map = nhw_PPIB_dppi_map[i];
    nhw_ppib_st[i].subscribed = (struct nhw_subsc_mem*)bs_calloc(nhw_ppib_n_ch[i], sizeof(struct nhw_subsc_mem));
  }
}

NSI_TASK(nhw_ppib_init, HW_INIT, 100);

/*
 * Free all PPIB instances resources before program exit
 */
static void nhw_ppib_free(void)
{
  for (int i = 0; i < NHW_PPIB_TOTAL_INST; i++) {
    free(nhw_ppib_st[i].subscribed);
    nhw_ppib_st[i].subscribed = NULL;
  }
}

NSI_TASK(nhw_ppib_free, ON_EXIT_PRE, 100);

/**
 * Check that this is an existing instance and ppib channel in that instance
 */
static inline void nhw_ppib_is_inst_ch_valid(uint ppib_inst, uint ppib_ch, const char *type)
{
  if ((ppib_inst >= NHW_PPIB_TOTAL_INST) || (ppib_ch >= nhw_ppib_st[ppib_inst].n_ch) ) {
    bs_trace_error_time_line("Attempted to access non existent %s %u (>= %u) in PPIB instance %u\n",
                             type, ppib_ch, nhw_ppib_st[ppib_inst].n_ch, ppib_inst);
  }
}

/**
 * Check that a given instance and channel is configured in a given direction
 *   source == 0 => must be a sink
 *   source == 1 => must be a source
 */
static void nhw_ppib_confirm_direction(uint inst, uint ppib_ch, bool source)
{
  bool subscribe_enable = (NRF_PPIB_regs[inst].SUBSCRIBE_SEND[ppib_ch] & (0x1UL << 31)) != 0;
  bool publish_enable = (NRF_PPIB_regs[inst].PUBLISH_RECEIVE[ppib_ch] & (0x1UL << 31)) != 0;

  if (subscribe_enable && publish_enable) {
    bs_trace_warning_time_line("PPIB channels must only be configured as sinks or sources, not both."
                               "PPIB %i channel %i\n",
                               inst, ppib_ch);
  }
  if (!publish_enable && !source) {
    bs_trace_warning_time_line("PPIB channel is expected to be a sink (it is receiving an event thru PPIB bus) but its publish register is not enabled"
                               "PPIB %i channel %i\n",
                               inst, ppib_ch);
  }
}

static void nhw_ppib_hardwired_check(uint32_t reg, uint ppib_ch){
  if (HWH_PPIB_HARDWIRESCHANNELS) {
    uint dppi_ch = reg & 0xFFU;
    if (ppib_ch != dppi_ch) {
      bs_trace_error_time_line("In this device PPIB channels are hardwired to DPPI channels => "
                               "The selected subscribe and publish DPPI channels must match the PPIB channel"
                               "%u != %u\n",
                               ppib_ch, dppi_ch);
    }
  }
}

/**
 * Transfer the event to the mate PPIB
 */
static void nhw_ppib_TASK_SEND(uint inst, uint ppib_ch)
{
  nhw_ppib_confirm_direction(inst, ppib_ch, true);
  nhw_ppib_propagate_event_mate(nhw_ppib_st[inst].ppib_mate, ppib_ch);
}

/*
 * Helper called by the DPPI when it gets an event is the subscribed DPPI channel
 */
static void nhw_ppib_tasksend_wrap(void* param) {
  unsigned int inst = (uintptr_t)param >> 16;
  uint ppib_ch = (uintptr_t)param & 0xFFFF;
  nhw_ppib_TASK_SEND(inst, ppib_ch);
}

void nhw_PPIB_regw_sideeffects_SUBSCRIBE_SEND(uint inst, uint ppib_ch){
  struct ppib_status *this = &nhw_ppib_st[inst];

  nhw_ppib_is_inst_ch_valid(inst, ppib_ch, "SUBSCRIBE_SEND");

  nhw_ppib_hardwired_check(NRF_PPIB_regs[inst].SUBSCRIBE_SEND[ppib_ch], ppib_ch);

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       this->NRF_PPIB_regs->SUBSCRIBE_SEND[ppib_ch],
                                       &this->subscribed[ppib_ch],
                                       nhw_ppib_tasksend_wrap,
                                       (void*)((inst << 16) + ppib_ch));
}

/*
 * This PPIB is receiving an event from its mate PPIB, publish it
 */
static void nhw_ppib_propagate_event_mate(uint inst, uint ppib_ch) {
  nhw_ppib_confirm_direction(inst, ppib_ch, false);

  nhw_ppib_hardwired_check(NRF_PPIB_regs[inst].PUBLISH_RECEIVE[ppib_ch], ppib_ch);

  nhw_dppi_event_signal_if(nhw_ppib_st[inst].dppi_map,
                           NRF_PPIB_regs[inst].PUBLISH_RECEIVE[ppib_ch]);
}
