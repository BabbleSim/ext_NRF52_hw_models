/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Backend for the UART(E) which connects the Tx and Rx of the same UART instance in loopback
 */

#include <stdint.h>
#include "bs_tracing.h"
#include "bs_cmd_line.h"
#include "bs_utils.h"
#include "bs_dynargs.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_UART_backend_if.h"
#include "nsi_tasks.h"
#include "nsi_hw_scheduler.h"
#include "nsi_hws_models_if.h"

bs_time_t nhw_Timer_ULoopback = TIME_NEVER;

struct ublb_st_t {
  bool enabled;
  bs_time_t Timer;

  char rx_byte;
} ublb_st[NHW_UARTE_TOTAL_INST];

void nhw_uarte_update_common_timer(void);

static void nhw_ublb_tx_byte(uint inst, uint8_t data);
static void nhw_ublb_RTS_pin_toggle(uint inst, bool new_level);

static void nhw_ublb_init(void) {

  struct backend_if st;
  st.tx_byte_f = nhw_ublb_tx_byte;
  st.RTS_pin_toggle_f = nhw_ublb_RTS_pin_toggle;
  st.uart_enable_notify_f = NULL;

  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {

    ublb_st[i].Timer = TIME_NEVER;

    if (!ublb_st[i].enabled) {
      continue;
    }

    nhw_UARTE_backend_register(i, &st);
    nhw_UARTE_CTS_raised(i);
  }
}

NSI_TASK(nhw_ublb_init, HW_INIT, 100); /* this must be before the uart itself */

static void nhw_ublb_register_cmdline(void) {
  static bs_args_struct_t args[NHW_UARTE_TOTAL_INST + 1 /* End marker */];
  static char descr[] = "Connect this UART instance in loopback (Tx->Rx, RTS->CTS)";
#define OPTION_LEN (4 + 2 + 9 + 1)
  static char options[NHW_UARTE_TOTAL_INST][OPTION_LEN];

  for (int i = 0 ; i < NHW_UARTE_TOTAL_INST; i++) {
    snprintf(options[i], OPTION_LEN, "uart%i_loopback", i);

    args[i].is_switch = true;
    args[i].option = options[i];
    args[i].type = 'b';
    args[i].dest = &ublb_st[i].enabled;
    args[i].descript = descr;
  }

  bs_add_extra_dynargs(args);
}

NSI_TASK(nhw_ublb_register_cmdline, PRE_BOOT_1, 200);

static void nhw_ublb_update_timer(void) {
  nhw_Timer_ULoopback = TIME_NEVER;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    if (!ublb_st[i].enabled) {
      continue;
    }
    nhw_Timer_ULoopback = BS_MIN(ublb_st[i].Timer, nhw_Timer_ULoopback);
  }
  nhw_uarte_update_common_timer();
}

static void nhw_ublb_tx_byte(uint inst, uint8_t data) {
  if (ublb_st[inst].Timer != TIME_NEVER) {
    bs_trace_error_time_line("%s: Unexpected error\n", __func__);
  }
  ublb_st[inst].rx_byte = data;
  ublb_st[inst].Timer = nsi_hws_get_time() + nhw_uarte_one_byte_time(inst);
  nhw_ublb_update_timer();
}

static void nhw_ublb_RTS_pin_toggle(uint inst, bool new_level) {
  if (new_level){
    nhw_UARTE_CTS_raised(inst);
  } else {
    nhw_UARTE_CTS_lowered(inst);
  }
}

void nhw_ublb_timer_triggered(void) {
  bs_time_t current_time = nhw_Timer_ULoopback;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    if (ublb_st[i].Timer == current_time) {
      nhw_UARTE_digest_Rx_byte(i, ublb_st[i].rx_byte);
      ublb_st[i].Timer = TIME_NEVER;
    }
  }
  nhw_ublb_update_timer();
}
