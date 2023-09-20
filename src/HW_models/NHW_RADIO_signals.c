/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes the RADIO event & signals functionality,
 * the registers writes sideeffects for tasks, subscriptions, events,
 * and interrupt masks.
 * For signals, when a signal is generated:
 *   Setting the EVENTS registers
 *   Sending the signal to the PPI
 *   Generating the corresponding interrupts
 *   Calling tasks if the corresponding shortcuts are enabled
 *
 *
 */

#include "NHW_common_types.h"
#include "NHW_templates.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_RADIO.h"
#include "NHW_RADIO_utils.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"

#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_RADIO_dppi_map[NHW_RADIO_TOTAL_INST] = NHW_RADIO_DPPI_MAP;
#endif
static uint32_t RADIO_INTEN; //interrupt enable
extern NRF_RADIO_Type NRF_RADIO_regs;

extern void nhw_RADIO_fake_task_TRXEN_TIFS(void);

void nhwra_signalif_reset(void){
  RADIO_INTEN = 0;
}

static void nhw_RADIO_eval_interrupt(uint inst) {
  static bool radio_int_line[NHW_RADIO_TOTAL_INST]; /* Is the RADIO currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_radio_irq_map[NHW_RADIO_TOTAL_INST] = NHW_RADIO_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(RADIO, READY, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, ADDRESS, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, PAYLOAD, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, END, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, DISABLED, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, DEVMATCH, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, DEVMISS, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, RSSIEND, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, BCMATCH, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CRCOK, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CRCERROR, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, FRAMESTART, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, EDEND, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, EDSTOPPED, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CCAIDLE, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CCABUSY, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CCASTOPPED, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, RATEBOOST, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, TXREADY, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, RXREADY, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, MHRMATCH, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, SYNC, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, PHYEND, RADIO_INTEN)
  NHW_CHECK_INTERRUPT_si(RADIO, CTEPRESENT, RADIO_INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&radio_int_line[inst],
                                       new_int_line,
                                       &nhw_radio_irq_map[inst]);
}

NHW_SIDEEFFECTS_INTSET_si(RADIO, NRF_RADIO_regs., RADIO_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(RADIO, NRF_RADIO_regs., RADIO_INTEN)

NHW_SIDEEFFECTS_EVENTS(RADIO)

static NHW_SIGNAL_EVENT_ns_si(RADIO, READY)
static NHW_SIGNAL_EVENT_ns_si(RADIO, ADDRESS)
NHW_SIGNAL_EVENT_si(RADIO, PAYLOAD)
static NHW_SIGNAL_EVENT_ns_si(RADIO, END)
static NHW_SIGNAL_EVENT_ns_si(RADIO, DISABLED)
NHW_SIGNAL_EVENT_si(RADIO, DEVMATCH)
NHW_SIGNAL_EVENT_si(RADIO, DEVMISS)
NHW_SIGNAL_EVENT_si(RADIO, RSSIEND)
NHW_SIGNAL_EVENT_si(RADIO, BCMATCH)
NHW_SIGNAL_EVENT_si(RADIO, CRCOK)
NHW_SIGNAL_EVENT_si(RADIO, CRCERROR)
static NHW_SIGNAL_EVENT_ns_si(RADIO, FRAMESTART)
static NHW_SIGNAL_EVENT_ns_si(RADIO, EDEND)
NHW_SIGNAL_EVENT_si(RADIO, EDSTOPPED)
static NHW_SIGNAL_EVENT_ns_si(RADIO, CCAIDLE)
static NHW_SIGNAL_EVENT_ns_si(RADIO, CCABUSY)
NHW_SIGNAL_EVENT_si(RADIO, CCASTOPPED)
NHW_SIGNAL_EVENT_si(RADIO, RATEBOOST)
static NHW_SIGNAL_EVENT_ns_si(RADIO, TXREADY)
static NHW_SIGNAL_EVENT_ns_si(RADIO, RXREADY)
NHW_SIGNAL_EVENT_si(RADIO, MHRMATCH)
NHW_SIGNAL_EVENT_si(RADIO, SYNC)
static NHW_SIGNAL_EVENT_ns_si(RADIO, PHYEND)
NHW_SIGNAL_EVENT_si(RADIO, CTEPRESENT)

void nhw_RADIO_signal_EVENTS_READY(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_READY_noshort(inst);

  NHW_SHORT_si(RADIO, READY, START)
  NHW_SHORT_si(RADIO, READY, EDSTART)
}

void nhw_RADIO_signal_EVENTS_ADDRESS(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_ADDRESS_noshort(inst);

  NHW_SHORT_si(RADIO, ADDRESS, RSSISTART)
  NHW_SHORT_si(RADIO, ADDRESS, BCSTART)
}

void nhw_RADIO_signal_EVENTS_END(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_END_noshort(inst);

  NHW_SHORT_si(RADIO, END, DISABLE)
  NHW_SHORT_si(RADIO, END, START)
}

void nhw_RADIO_signal_EVENTS_DISABLED(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_DISABLED_noshort(inst);

  /*
   * Everything indicates that, when the HW TIFS is enabled
   * what happens is that these DISABLED_[TR]XEN shorts are
   * effectively disabled and the tasks_[TR]XEN is instead
   * triggered when a HW counter/timer triggers a bit later
   */
  if (nhwra_is_HW_TIFS_enabled()) {
     /* This is a fake task meant to start a HW timer for the TIFS
      * which has effect only if the TIFS was enabled.
      * In that case, the TXEN or RXEN will be triggered in a small
      * while (as per the TIFS configuration/Timer_TIFS), instead of right now */
      nhw_RADIO_fake_task_TRXEN_TIFS();
  } else {
    /* Otherwise the normal TXEN/RXEN shortcuts apply */
    NHW_SHORT_si(RADIO, DISABLED, TXEN)
    NHW_SHORT_si(RADIO, DISABLED, RXEN)
  }

  NHW_SHORT_si(RADIO, DISABLED, RSSISTOP)
}

void nhw_RADIO_signal_EVENTS_FRAMESTART(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_FRAMESTART_noshort(inst);

  NHW_SHORT_si(RADIO, FRAMESTART, BCSTART)
}

void nhw_RADIO_signal_EVENTS_EDEND(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_EDEND_noshort(inst);

  NHW_SHORT_si(RADIO, EDEND, DISABLE)
}

void nhw_RADIO_signal_EVENTS_CCAIDLE(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_CCAIDLE_noshort(inst);

  NHW_SHORT_si(RADIO, CCAIDLE, STOP)
  NHW_SHORT_si(RADIO, CCAIDLE, TXEN)
}

void nhw_RADIO_signal_EVENTS_CCABUSY(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_CCABUSY_noshort(inst);

  NHW_SHORT_si(RADIO, CCABUSY, DISABLE)
}

void nhw_RADIO_signal_EVENTS_TXREADY(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_TXREADY_noshort(inst);

  NHW_SHORT_si(RADIO, TXREADY, START)
}

void nhw_RADIO_signal_EVENTS_RXREADY(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_RXREADY_noshort(inst);

  NHW_SHORT_si(RADIO, RXREADY, START)
  NHW_SHORT_si(RADIO, RXREADY, CCASTART)
}

void nhw_RADIO_signal_EVENTS_PHYEND(unsigned int inst) {
  nhw_RADIO_signal_EVENTS_PHYEND_noshort(inst);

  NHW_SHORT_si(RADIO, PHYEND, DISABLE)
  NHW_SHORT_si(RADIO, PHYEND, START)
}


extern NRF_RADIO_Type NRF_RADIO_regs;


NHW_SIDEEFFECTS_TASKS_si(RADIO, TXEN)
NHW_SIDEEFFECTS_TASKS_si(RADIO, RXEN)
NHW_SIDEEFFECTS_TASKS_si(RADIO, START)
NHW_SIDEEFFECTS_TASKS_si(RADIO, STOP)
NHW_SIDEEFFECTS_TASKS_si(RADIO, DISABLE)

void nhw_RADIO_regw_sideeffects_TASKS_RSSISTART(void) {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTART ){
    NRF_RADIO_regs.TASKS_RSSISTART = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTART register is not supported by the model yet\n");
  }
}

void nhw_RADIO_regw_sideeffects_TASKS_RSSISTOP(void) {
  //We don't need to model this yet
  if ( NRF_RADIO_regs.TASKS_RSSISTOP ){
    NRF_RADIO_regs.TASKS_RSSISTOP = 0;
    bs_trace_warning_line_time("RADIO: Sampling RSSI by writing to TASK_RSSISTOP register is not supported by the model yet\n");
  }
}

NHW_SIDEEFFECTS_TASKS_si(RADIO, BCSTART)
NHW_SIDEEFFECTS_TASKS_si(RADIO, BCSTOP)
NHW_SIDEEFFECTS_TASKS_si(RADIO, EDSTART)
NHW_SIDEEFFECTS_TASKS_si(RADIO, EDSTOP)
NHW_SIDEEFFECTS_TASKS_si(RADIO, CCASTART)
NHW_SIDEEFFECTS_TASKS_si(RADIO, CCASTOP)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, TXEN)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, RXEN)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, START)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, STOP)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, DISABLE)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, RSSISTART)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, RSSISTOP)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, BCSTART)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, BCSTOP)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, EDSTART)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, EDSTOP)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, CCASTART)
NHW_SIDEEFFECTS_SUBSCRIBE_si(RADIO, CCASTOP)
#endif /* NHW_HAS_DPPI */
