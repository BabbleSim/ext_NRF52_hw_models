/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Backend for the UART(E) which connects to a compatible backend on another device.
 *
 * It accounts for the RTS|R/CTS flow control, and byte timing.
 *
 * Mismatches in the UART configuration and baudrate, only result in a warning
 * but the data is accepted without generating error events.
 *
 * When selected it *requires* the other end to be connected as soon as the UART HW is enabled.
 *
 * Notes:
 *   * This backend is heavy/slow, it is very strongly recommended to NOT use it
 *     in 2 devices which are also connected thru the Phy.
 *     It is also discouraged to use it for anything beyond testing of UART related code.
 *
 *   * The protocol this backend uses does not follow any standard, and it is not
 *     an API with any stability promise, if you connect your own UART model to its FIFO
 *     you should expect that to break eventually.
 *
 *   * When both Tx and Rx are off, CTS toggles will not generate the events in the right
 *     time but those events will be generated in bursts (as we are not monitoring constantly the input)
 *     Therefore for the UART(not E), a toggle of CTS will not trigger the StartRx/StopRx
 *     shortcuts in the right time.
 */

#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_utils.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "bs_pc_base_fifo_user.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_UART_backend_if.h"
#include "NHW_UART_backend_fifo.h"
#include "nsi_hw_scheduler.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

static bs_time_t Timer_UFIFO = TIME_NEVER;

/* Factor for the fastest reaction time to propagate an event to the other side
 * relative to a frame (byte transfer time) size.
 * This reaction time is frame_time / TX_MIN_DELTA_RATIO.
 * (Note the smallest byte time is 10 micros at 1Mbps no parity and 1 stop bit,
 * so for TX_MIN_DELTA_RATIO = 2 that is 5 micros )
 * The higher this value, the more accurate the flow control timing will be.
 * Byte timing will always be fully accurate.
 * But the higher this value the higher the processing overhead.
 */
#define TX_MIN_DELTA_RATIO 1

struct line_params {
  uint32_t config;
  uint32_t baud;
};

struct ufifo_st_t {
  bs_time_t Rx_timer;
  bs_time_t Tx_timer;

  char *fifo_Tx_path;
  char *fifo_Rx_path;

  int fifo_tx; /* File descriptor for tx */
  int fifo_rx; /* File descriptor for rx */

  bool enabled; /* Has it been chosen for this instance in the command line (and not yet disconnected) */
  bool tx_on;
  bool rx_on;
  bool rx_low_dutyc_mode;
  bool disconnected;

  struct line_params tx_line_params;

  char last_rx_msg[UFIFO_BIGGEST_MSG];
  bool last_rx_msg_pending; /* The message in last_rx_msg is between pre and post processing (1)
                                      or already processed (0) */


  struct line_params rx_line_params;
  bool cts; /* CTS signal we are getting from other device (starts high => not clear) */
  char rx_byte;
};

struct ufifo_st_t ufifo_st[NHW_UARTE_TOTAL_INST];

static bool uf_dont_terminate_on_disconnect;
static double uf_mdt;

static void nhw_ufifo_update_timer(void);
static void nhw_ufifo_create_fifos(uint inst, struct ufifo_st_t *u_el);
static void nhw_ufifo_tx_byte(uint inst, uint8_t data);
static void nhw_ufifo_RTS_pin_toggle(uint inst, bool new_level);
static void nhw_ufifo_enable_notify(uint inst, uint8_t tx_enabled, uint8_t rx_enabled);
static void uf_Rx_handle_old_input(uint inst, struct ufifo_st_t *u_el);

static void nhw_ufifo_backend_init(void) {

  /* Let's make sure we ignore broken pipe signals even if we are not connected to a Phy*/
  signal(SIGPIPE, SIG_IGN);

  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct ufifo_st_t *u_el = &ufifo_st[i];
    u_el->Rx_timer = TIME_NEVER;
    u_el->Tx_timer = TIME_NEVER;

    u_el->fifo_tx = -1;
    u_el->fifo_rx = -1;

    u_el->tx_line_params.config = UINT32_MAX;
    u_el->tx_line_params.baud = UINT32_MAX;

    u_el->rx_line_params.config = UINT32_MAX;
    u_el->rx_line_params.baud = UINT32_MAX;

    u_el->cts = true;

    if (!u_el->enabled) {
      continue;
    }
    nhw_ufifo_create_fifos(i, u_el);

    struct backend_if st;
    st.tx_byte_f = nhw_ufifo_tx_byte;
    st.RTS_pin_toggle_f = nhw_ufifo_RTS_pin_toggle;
    st.uart_enable_notify_f = nhw_ufifo_enable_notify;

    nhw_UARTE_backend_register(i, &st);
    nhw_UARTE_CTS_raised(i);

    u_el->Tx_timer = 0;
    u_el->Rx_timer = uf_mdt;
    u_el->rx_low_dutyc_mode = true;
  }
  nhw_ufifo_update_timer();
}

NSI_TASK(nhw_ufifo_backend_init, HW_INIT, 100); /* this must be before the uart itself */

static void write_to_tx_fifo(struct ufifo_st_t *u_el, void *ptr, size_t size) {
  int res = write(u_el->fifo_tx, ptr, size);

  if (res != size) {
    u_el->disconnected = true;
    if ((res == -1) && (errno == EPIPE)) {
      /* The other side disconnected unexpectedly, let's terminate */
      bs_trace_exit_time_line("UART: Other end disconnected from its Rx FIFO, terminating\n");
    } else {
      bs_trace_error_time_line("UART: Unexpected error while writing (%i, %i) (or interrupted). "
                               "Terminating\n", res, errno);
    }
  }
}

static void tx_set_next_tx_timer(uint inst, struct ufifo_st_t *u_el, bs_time_t last_act) {
  u_el->Tx_timer = BS_MAX(last_act, u_el->Tx_timer);
  nhw_ufifo_update_timer();
}

static void tx_sync_line_params(uint inst, struct ufifo_st_t *u_el) {
  u_el->tx_line_params.config = NRF_UARTE_regs[inst].CONFIG;
  u_el->tx_line_params.baud = NRF_UARTE_regs[inst].BAUDRATE;

  struct ufifo_msg_mode_change msg;
  msg.header.time = nsi_hws_get_time();
  msg.header.size = sizeof(msg);
  msg.header.msg_type = ufifo_MODE_CHANGE;
  msg.config = u_el->tx_line_params.config;
  msg.baudrate = u_el->tx_line_params.baud;

  write_to_tx_fifo(u_el, (void *)&msg, sizeof(msg));
  tx_set_next_tx_timer(inst, u_el, msg.header.time);
}

static void tx_nop(uint inst, struct ufifo_st_t *u_el, bs_time_t t) {
  struct ufifo_msg_header msg;
  msg.time = t;
  msg.size = sizeof(msg);
  msg.msg_type = ufifo_NOP;

  write_to_tx_fifo(u_el, (void *)&msg, sizeof(msg));
  tx_set_next_tx_timer(inst, u_el, msg.time);
}

static void tx_disconnect(uint inst, struct ufifo_st_t *u_el) {
  struct ufifo_msg_header msg;
  msg.time = 0;
  msg.size = sizeof(msg);
  msg.msg_type = ufifo_DISCONNECT;

  write_to_tx_fifo(u_el, (void *)&msg, sizeof(msg));
}

static void nhw_ufifo_tx_byte(uint inst, uint8_t data) {
  struct ufifo_st_t *u_el = &ufifo_st[inst];

  if (!u_el->enabled) {
    bs_trace_error_time_line("Progamming error\n");
  }

  if ((NRF_UARTE_regs[inst].CONFIG != u_el->tx_line_params.config)
      || (NRF_UARTE_regs[inst].BAUDRATE != u_el->tx_line_params.baud)) {
    tx_sync_line_params(inst, u_el);
  }

  struct ufifo_msg_tx msg;
  msg.header.time = nsi_hws_get_time() + nhw_uarte_one_byte_time(inst);
  msg.header.size = sizeof(msg);
  msg.header.msg_type = ufifo_TX_BYTE;
  msg.data = data;

  write_to_tx_fifo(u_el, (void *)&msg, sizeof(msg));
  tx_set_next_tx_timer(inst, u_el, msg.header.time);
}

static void nhw_ufifo_RTS_pin_toggle(uint inst, bool new_level) {
  struct ufifo_st_t *u_el = &ufifo_st[inst];

  if (!u_el->enabled) {
    bs_trace_error_time_line("Progamming error\n");
  }

  struct ufifo_msg_rts_cts msg;

  msg.header.time = nsi_hws_get_time();
  msg.header.size = sizeof(msg);
  msg.header.msg_type = ufifo_RTS_CTS_TOGGLE;
  msg.level = new_level;

  write_to_tx_fifo(u_el, (void *)&msg, sizeof(msg));
  tx_set_next_tx_timer(inst, u_el, msg.header.time);
}

static void nhw_ufifo_enable_notify(uint inst, uint8_t tx_enabled, uint8_t rx_enabled) {
  struct ufifo_st_t *u_el = &ufifo_st[inst];

  if (!u_el->enabled) {
    bs_trace_error_time_line("Progamming error\n");
  }

  u_el->tx_on = tx_enabled;
  u_el->rx_on = rx_enabled;

  if ((u_el->rx_low_dutyc_mode == true) &&
      ((tx_enabled == true) || (rx_enabled == true)) ) {
    /* If we are in low duty cycle mode, let's get out of it right away */
    u_el->Rx_timer = nsi_hws_get_time();
    nhw_ufifo_update_timer();
  }
}

static void nhw_ufifo_update_timer(void) {
  Timer_UFIFO = TIME_NEVER;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct ufifo_st_t * u_el = &ufifo_st[i];
    if (!u_el->enabled) {
      continue;
    }
    bs_time_t smaller = BS_MIN(u_el->Rx_timer, u_el->Tx_timer);
    Timer_UFIFO = BS_MIN(Timer_UFIFO, smaller);
  }
  nsi_hws_find_next_event();
}

static void uf_propage_cts(uint inst, struct ufifo_st_t *u_el) {
  if (u_el->cts) {
    nhw_UARTE_CTS_raised(inst);
  } else {
    nhw_UARTE_CTS_lowered(inst);
  }
}

static int uf_rx_lowlevel_read(struct ufifo_st_t *u_el, void *buf, size_t size) {
  int ret = read(u_el->fifo_rx, buf, size);
  if (ret != size) {
    u_el->disconnected = true;
    if (ret == 0) {
      bs_trace_error_time_line("UART: Other end disconnected unexpectedly\n");
    } else {
      bs_trace_error_time_line("UART: Read interrupted, or other unexpected error => terminating (%i, %s)\n",
                               ret, strerror(errno));
    }
  }
  return ret;
}

/*
 * Get one message from the Rx FIFO
 * And for
 *   NOP: do nothing
 *   MODE_CHANGE: update u_el->rx_line_params
 *   TX_BYTE: copy byte to u_el->rx_byte
 *   RTS_CTS_TOGGLE: update u_el->cts
 *   DISCONNECT: Disconnect this FIFO instance (set timers in never, and disconnect from peripheral)
 *
 * returns message type
 */
static int uf_rx_get_one_msg(uint inst, struct ufifo_st_t *u_el) {
  struct ufifo_msg_header *buf = (struct ufifo_msg_header *)u_el->last_rx_msg;

  uf_rx_lowlevel_read(u_el, (void *)buf, UFIFO_MSG_HEADER_SIZE);

  switch (buf->msg_type) {
    case ufifo_MODE_CHANGE:
      uf_rx_lowlevel_read(u_el, (char *)buf + UFIFO_MSG_HEADER_SIZE, UFIFO_MSG_MODE_CHANGE_BODY_SIZE);
      u_el->rx_line_params.baud = ((struct ufifo_msg_mode_change *)buf)->baudrate;
      u_el->rx_line_params.config = ((struct ufifo_msg_mode_change *)buf)->config;
      break;
    case ufifo_DISCONNECT:
      u_el->disconnected = true;
      if (!uf_dont_terminate_on_disconnect) {
        bs_trace_exit_line_time("UART%i: Other end disconnected. Terminating\n", inst);
      } else {
        bs_trace_raw_time(3, "UART%i: Other end disconnected, UART%i backend disabled\n", inst, inst);
        u_el->enabled = false;
        u_el->Rx_timer = TIME_NEVER;
        u_el->Tx_timer = TIME_NEVER;
        nhw_ufifo_update_timer();

        // Unregister from UART peri this instance so we never get another call:
        struct backend_if st;
        memset(&st, 0, sizeof(st));
        nhw_UARTE_backend_register(inst, &st);
      }
      break;
    case ufifo_TX_BYTE:
      uf_rx_lowlevel_read(u_el, (char *)buf + UFIFO_MSG_HEADER_SIZE, UFIFO_MSG_TXL_BODY_SIZE);
      u_el->rx_byte = ((struct ufifo_msg_tx *)buf)->data;;
      break;
    case ufifo_NOP:
      break;
    case ufifo_RTS_CTS_TOGGLE:
      uf_rx_lowlevel_read(u_el, (char *)buf + UFIFO_MSG_HEADER_SIZE, UFIFO_MSG_RTS_CTS_BODY_SIZE);
      u_el->cts = ((struct ufifo_msg_rts_cts *)buf)->level;
      break;
    default:
      bs_trace_error_time_line("Corrupted stream\n");
      break;
  }
  return buf->msg_type;
}

/*
 * Process the last received msg
 * (Which may result in a pended call to nhw_ufifo_handle_RxTimer() )
 */
static void uf_rx_process_last_msg_pre(uint inst, struct ufifo_st_t *u_el) {
  struct ufifo_msg_header *buf = (struct ufifo_msg_header *)u_el->last_rx_msg;
  switch (buf->msg_type) {
    case ufifo_DISCONNECT:
      return; /* Was already processed */
    case ufifo_MODE_CHANGE:
    case ufifo_NOP:
      /* TOLOW: We can do a optimization here, by checking what is {ONT = the next schedueled event in the nsi hws}
       * And if (ONT > buf->time) get another message from the Rx queue right away, as we anyhow would not do anything in the meanwhile
       * (doing so would avoid possible abort reevaluations towards the phy, and loops thru the UART itself)
       */
    case ufifo_TX_BYTE:
    case ufifo_RTS_CTS_TOGGLE:
      u_el->last_rx_msg_pending = true;
      u_el->Rx_timer = BS_MAX(buf->time, nsi_hws_get_time());
      nhw_ufifo_update_timer();
      break;
    default:
      bs_trace_error_time_line("Programming error\n");
      break;
  }
}

static void uf_rx_check_config_match(uint inst, struct ufifo_st_t *u_el) {
  if (( (NRF_UARTE_regs[inst].CONFIG & ~UART_CONFIG_HWFC_Msk)
        != (u_el->rx_line_params.config & ~UART_CONFIG_HWFC_Msk)
      )
      || (NRF_UARTE_regs[inst].BAUDRATE != u_el->rx_line_params.baud)) {
    bs_trace_warning_time_line("UART%i: Receiving a byte with mismatched configuration. "
        "This would result in a line error in a real UART. Here you just get this warning "
        "(UARTE_regs.CONFIG = %"PRIu32" !=? %"PRIu32 " in other side ;"
        "(UARTE_regs.BAUDRATE = %"PRIu32" !=? %"PRIu32" in other side\n",
        inst,
        NRF_UARTE_regs[inst].CONFIG, u_el->rx_line_params.config,
        NRF_UARTE_regs[inst].BAUDRATE, u_el->rx_line_params.baud
        );
  }
}

/*
 * Finish processing the last received msg if needed (at the RxTimer time)
 */
static void uf_rx_process_last_msg_post(uint inst, struct ufifo_st_t *u_el) {
  struct ufifo_msg_header *buf = (struct ufifo_msg_header *)u_el->last_rx_msg;

  if (u_el->last_rx_msg_pending == false) {
    return;
  }
  u_el->last_rx_msg_pending = false;

  switch (buf->msg_type) {
  case ufifo_NOP:
  case ufifo_MODE_CHANGE:
    /* Nothing left to be done */
    break;
  case ufifo_TX_BYTE:
    if (u_el->rx_on) {
      bs_trace_info_time(8, "UART%i: Received byte (0x%02X)\n",
          inst, ((struct ufifo_msg_tx *)buf)->data);
      uf_rx_check_config_match(inst, u_el);
      nhw_UARTE_digest_Rx_byte(inst, u_el->rx_byte);
    } else {
      bs_trace_info_time(3, "UART%i: Received byte (0x%02X) while Rx was off => ignored\n",
          inst, ((struct ufifo_msg_tx *)buf)->data);
    }
    break;
  case ufifo_RTS_CTS_TOGGLE:
    uf_propage_cts(inst, u_el);
    break;
  case ufifo_DISCONNECT:
    /* We should not have reached this point */
  default:
    bs_trace_error_time_line("Programming error\n");
    break;
  }
}

/*
 * Dequeue all old input in the Rx FIFO until now
 */
static void uf_Rx_handle_old_input(uint inst, struct ufifo_st_t *u_el) {
  struct ufifo_msg_header *buf = (struct ufifo_msg_header *)u_el->last_rx_msg;
  bs_time_t now = nsi_hws_get_time();

  do {
    uf_rx_get_one_msg(inst, u_el);
    if (buf->time < now) {
      switch (buf->msg_type) {
        case ufifo_NOP: //We just ignore them while clearing
        case ufifo_MODE_CHANGE:
          break;
        case ufifo_TX_BYTE:
          if ((buf->time < now)) {
            char byte = ((struct ufifo_msg_tx *)buf)->data;
            bs_trace_info_time(3, "UART%i: Received byte (0x%02X) while Rx was off => ignored\n", inst, byte);
          }
          break;
        case ufifo_RTS_CTS_TOGGLE:
          /* We propagate past toggles right away to the UART */
          uf_propage_cts(inst, u_el); //Note both Tx and Rx are disabled right now
          break;
        case ufifo_DISCONNECT:
          return;
        default:
          bs_trace_error_time_line("Programming error\n");
          break;
      }
    } else {
      break;
    }
  } while (true);

  uf_rx_process_last_msg_pre(inst, u_el);
}

static void nhw_ufifo_handle_RxTimer(int inst, struct ufifo_st_t *u_el) {

  if (u_el->rx_low_dutyc_mode) {
    /*
     * Low duty cycle:
     * (1) We pick all old messages, and the first that points at now or the future
     * And we schedule nhw_ufifo_handle_RxTimer() to parse that last one normally
     * (we get out of low duty cycle mode, and maybe we get in again after)
     */
    u_el->rx_low_dutyc_mode = false;
    uf_Rx_handle_old_input(inst, u_el);
    return;
  }

  uf_rx_process_last_msg_post(inst, u_el);

  if (u_el->tx_on || u_el->rx_on) {
    uf_rx_get_one_msg(inst, u_el);
    uf_rx_process_last_msg_pre(inst, u_el);
  } else {
    /* Low duty cycle (2):
     * If the Tx and Rx are off, we go into low duty cycle mode:
     * Instead of picking the next Rx msg now, we postpone picking it for uf_mdt (10ms)
     * => when we come back to nhw_ufifo_handle_RxTimer() thru (1)
     *
     * Low dutycycle mode may be sped up at any point by setting the Rx_timer to a earlier value
     */
    u_el->rx_low_dutyc_mode = true;
    u_el->Rx_timer = nsi_hws_get_time() + uf_mdt;
    nhw_ufifo_update_timer();
  }
}

static void nhw_ufifo_handle_TxTimer(int inst, struct ufifo_st_t *u_el) {
  bs_time_t t;

  u_el->Tx_timer = TIME_NEVER;
  nsi_hws_find_next_event();
  t = nsi_hws_get_next_event_time() + nhw_uarte_one_byte_time(inst)/TX_MIN_DELTA_RATIO;
  u_el->Tx_timer = t;

  tx_nop(inst, u_el, t);
}

static void nhw_ufifo_timer_triggered(void) {
  bs_time_t current_time = Timer_UFIFO;
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct ufifo_st_t *u_el = &ufifo_st[i];
    if (u_el->Tx_timer == current_time) {
      nhw_ufifo_handle_TxTimer(i, u_el);
    }
    if (u_el->Rx_timer == current_time) {
      nhw_ufifo_handle_RxTimer(i, u_el);
    }
  }
  nhw_ufifo_update_timer();
}

NSI_HW_EVENT(Timer_UFIFO, nhw_ufifo_timer_triggered, 900); /* Let's let as many timers as possible evaluate before this one */

static void uf_parse_mdt(char *argv, int offset) {
  if (uf_mdt < 1 || uf_mdt > 1e6) {
    bs_trace_error_line("uart_fifob_mdt must be set to a value between 1 and 1e6 (%s)\n", argv);
  }
}

static void nhw_ufifo_backend_register_cmdline(void) {
  static bs_args_struct_t args2[2*NHW_UARTE_TOTAL_INST + 1 /* End marker */];
  static char descr_tx[] = "Path to the FIFO to be used for Tx (it will be created automatically). "
                           "Remember to cross them between devices. "
                           "Setting this option enables the FIFO backend for this UART";
  static char descr_rx[] = "Path to the FIFO to be used for Rx (it will be created automatically)";
#define OPTION_LEN (4 + 2 + 13 + 1)
  static char options[NHW_UARTE_TOTAL_INST][2][OPTION_LEN];
  static char opt_name[]= "path";

  for (int i = 0 ; i < NHW_UARTE_TOTAL_INST; i++) {
    snprintf(options[i][0], OPTION_LEN, "uart%i_fifob_txfile", i);
    snprintf(options[i][1], OPTION_LEN, "uart%i_fifob_rxfile", i);

    args2[2*i].option = options[i][0];
    args2[2*i].name = opt_name;
    args2[2*i].type = 's';
    args2[2*i].dest = &ufifo_st[i].fifo_Tx_path;
    args2[2*i].descript = descr_tx;

    args2[2*i + 1].option = options[i][1];
    args2[2*i + 1].name = opt_name;
    args2[2*i + 1].type = 's';
    args2[2*i + 1].dest = &ufifo_st[i].fifo_Rx_path;
    args2[2*i + 1].descript = descr_rx;
  }

  bs_add_extra_dynargs(args2);

  static bs_args_struct_t args1[] = {
    { .is_switch = true,
      .option = "uart_fifob_no_terminate",
      .type = 'b',
      .dest = (void *)&uf_dont_terminate_on_disconnect,
      .descript = "Do NOT terminate execution of this device when the other end disconnects "
                  "gracefully"
    },
    { .option = "uart_fifob_mdt",
      .type = 'd',
      .call_when_found = uf_parse_mdt,
      .dest = (void *)&uf_mdt,
      .descript = "(By default 10e3=10ms) Maximum amount of time the backend will spend without dequeuing"
                  "the Rx FIFO when the UART is not running in Tx or Rx mode. "
                  "This needs to be small enough to avoid the pipe from filling up and causing"
                  "a deadlock. It can also be set to a very small value (order of 10micros) if you want to"
                  "ensure the UART keeps registering the CTS signal toggles even when it is not running"
                  "(This only makes sense if you have registered a short or a PPI event in the CTS signal"
                  "events). The smaller the value the greater the overhead."
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args1);
}

NSI_TASK(nhw_ufifo_backend_register_cmdline, PRE_BOOT_1, 200);

static void nhw_ufifo_backend_post_cmdline(void) {
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct ufifo_st_t *u_el = &ufifo_st[i];
    if ((u_el->fifo_Rx_path == NULL) && (u_el->fifo_Rx_path == NULL)) {
      continue;
    }
    if (u_el->fifo_Tx_path == NULL) {
      bs_trace_error_line("UART%i: uart_fifob_rxfile was provided but not uart_fifob_txfile. "
                          "If you want to use the FIFO backend you must provide both\n",i);
    }
    if (u_el->fifo_Rx_path == NULL) {
      bs_trace_error_line("UART%i: uart_fifob_txfile was provided but not uart_fifob_rxfile. "
                          "If you want to use the FIFO backend you must provide both\n",i);
    }
    u_el->enabled = true;
  }
  if (isnan(uf_mdt)) {
    uf_mdt = 10000;
  }
}

NSI_TASK(nhw_ufifo_backend_post_cmdline, PRE_BOOT_2, 200);

/*
 * Create FIFOs towards the other device and open both from our end.
 * But do not block. The Tx FIFO is as nonblocking, and the Rx
 * FIFO is reconfigured as blocking.
 */
static void nhw_ufifo_create_fifos(uint inst, struct ufifo_st_t *u_el) {
  bs_trace_raw_time(9, "Creating UART%i backend FIFOs, and connecting Tx end\n", inst);

  bs_create_folders_in_path(u_el->fifo_Tx_path);
  bs_create_folders_in_path(u_el->fifo_Rx_path);

  if (pb_create_fifo_if_not_there(u_el->fifo_Tx_path) != 0) {
    bs_trace_error_line("Couldn't create UART backend Tx FIFOs\n");
  }
  if (pb_create_fifo_if_not_there(u_el->fifo_Rx_path) != 0) {
    bs_trace_error_line("Couldn't create UART backend Rx FIFOs\n");
  }

  /* 1) We first open our read side non-blocking to avoid a deadlock.
   * (This open read side, even if provisional, allows the other side to safely start writing to the
   * pipe even if it overtakes our step 3) )
   */
  int prov_descr = open(u_el->fifo_Rx_path, O_RDONLY | O_NONBLOCK);
  if (prov_descr == -1) {
    bs_trace_error_line("Couldn't open UART backend Rx FIFO (%i, %s)\n", errno, strerror(errno));
  }

  /* 2) We block opening our Tx side until the other side has reached 1) */
  u_el->fifo_tx = open(u_el->fifo_Tx_path, O_WRONLY);
  if (u_el->fifo_tx == -1) {
    bs_trace_error_line("Couldn't open UART backend Tx FIFO (%i, %s)\n", errno, strerror(errno));
  }

  /* 3) And now that we know the other side reached 1), we can block until it reaches 2)
   * while creating the descriptor we will actually use */
  u_el->fifo_rx = open(u_el->fifo_Rx_path, O_RDONLY);
  if (u_el->fifo_rx == -1) {
    bs_trace_error_line("Couldn't open UART backend Rx FIFO (%i, %s)\n", errno, strerror(errno));
  }

  /* At this point we know both pipes are open in both sides.
   * We can now close our previous Rx descriptor as we won't use it for anything anymore. */
  (void)close(prov_descr);

}

static void nhw_ufifo_backend_cleanup(void) {
  bs_trace_raw_time(9, "Cleaning up UART backend FIFOs\n");
  for (int i = 0; i < NHW_UARTE_TOTAL_INST; i++) {
    struct ufifo_st_t *u_el = &ufifo_st[i];
    if ((u_el->fifo_Tx_path) && (u_el->fifo_tx != -1)) {
      if (!u_el->disconnected) {
        tx_disconnect(i, u_el);
      }
      (void)close(u_el->fifo_tx);
      u_el->fifo_tx = -1;
      (void)remove(u_el->fifo_Tx_path); /* The last one closing manages to remove it (so we don't check for success) */
      u_el->fifo_Tx_path = NULL;
    }
    if ((u_el->fifo_Rx_path) && (u_el->fifo_rx != -1)) {
      (void)close(u_el->fifo_rx);
      u_el->fifo_rx = -1;
      (void)remove(u_el->fifo_Rx_path); /* The last one closing manages to remove it (so we don't check for success) */
      u_el->fifo_Rx_path = NULL;
    }
  }
}

NSI_TASK(nhw_ufifo_backend_cleanup, ON_EXIT_PRE, 100);
