/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * TEMP - Temperature sensor
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/temp.html?cp=4_1_0_5_25
 *
 * A very simple and rough model
 *
 * Notes:
 *   * At this point the device is always at 25 C
 *   * The measurement result will just be 25 +- 0.25 C
 *   * There is no per device variability
 *   * There is no modeling of possible calibration errors or inaccuracies due to no non-linearities compensation
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "nsi_hw_scheduler.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_templates.h"
#include "NHW_TEMP.h"
#include "NHW_peri_types.h"
#include "NHW_xPPI.h"
#include "irq_ctrl.h"

#if NHW_TEMP_TOTAL_INST > 1
#error "This model only supports 1 instance so far"
#endif

NRF_TEMP_Type NRF_TEMP_regs;

#if (NHW_HAS_DPPI)
/* Mapping of peripheral instance to DPPI instance */
static uint nhw_TEMP_dppi_map[NHW_TEMP_TOTAL_INST] = NHW_TEMP_DPPI_MAP;
#endif

static bs_time_t Timer_TEMP = TIME_NEVER; //Time when the next temperature measurement will be ready

static bool TEMP_hw_started;
static uint32_t TEMP_INTEN; //interrupt enable

static double temperature = 25.0; /* Actual temperature the device is at */

/**
 * Initialize the TEMP model
 */
static void nhw_temp_init(void) {
  memset(&NRF_TEMP_regs, 0, sizeof(NRF_TEMP_regs));
#if defined(NRF52833)
  NRF_TEMP_regs.A0 = 0x00000326;
  NRF_TEMP_regs.A1 = 0x00000348;
  NRF_TEMP_regs.A2 = 0x000003AA;
  NRF_TEMP_regs.A3 = 0x0000040E;
  NRF_TEMP_regs.A4 = 0x000004BD;
  NRF_TEMP_regs.A5 = 0x000005A3;
  NRF_TEMP_regs.B0 = 0x00003FEF;
  NRF_TEMP_regs.B1 = 0x00003FBE;
  NRF_TEMP_regs.B2 = 0x00003FBE;
  NRF_TEMP_regs.B3 = 0x00000012;
  NRF_TEMP_regs.B4 = 0x00000124;
  NRF_TEMP_regs.B5 = 0x0000027C;
  NRF_TEMP_regs.T0 = 0x000000E2;
  NRF_TEMP_regs.T1 = 0x00000000;
  NRF_TEMP_regs.T2 = 0x00000019;
  NRF_TEMP_regs.T3 = 0x0000003C;
  NRF_TEMP_regs.T4 = 0x00000050;
#elif defined(NRF5340)
  NRF_TEMP_regs.A0 = 0x000002D9;
  NRF_TEMP_regs.A1 = 0x00000322;
  NRF_TEMP_regs.A2 = 0x00000355;
  NRF_TEMP_regs.A3 = 0x000003DF;
  NRF_TEMP_regs.A4 = 0x0000044E;
  NRF_TEMP_regs.A5 = 0x000004B7;
  NRF_TEMP_regs.B0 = 0x00000FC7;
  NRF_TEMP_regs.B1 = 0x00000F71;
  NRF_TEMP_regs.B2 = 0x00000F6C;
  NRF_TEMP_regs.B3 = 0x00000FCB;
  NRF_TEMP_regs.B4 = 0x0000004B;
  NRF_TEMP_regs.B5 = 0x000000F6;
  NRF_TEMP_regs.T0 = 0x000000E1;
  NRF_TEMP_regs.T1 = 0x000000F9;
  NRF_TEMP_regs.T2 = 0x00000010;
  NRF_TEMP_regs.T3 = 0x00000026;
  NRF_TEMP_regs.T4 = 0x0000003F;
#elif defined(NRF54L15)
  NRF_TEMP_regs.A0 = 0x000002D6;
  NRF_TEMP_regs.A1 = 0x0000032D;
  NRF_TEMP_regs.A2 = 0x00000384;
  NRF_TEMP_regs.A3 = 0x000003E9;
  NRF_TEMP_regs.A4 = 0x0000046F;
  NRF_TEMP_regs.A5 = 0x00000522;
  NRF_TEMP_regs.A6 = 0x000005B7;
  NRF_TEMP_regs.B0 = 0x00000FD6;
  NRF_TEMP_regs.B1 = 0x00000F76;
  NRF_TEMP_regs.B2 = 0x00000F8A;
  NRF_TEMP_regs.B3 = 0x00000FF8;
  NRF_TEMP_regs.B4 = 0x000000CC;
  NRF_TEMP_regs.B5 = 0x00000207;
  NRF_TEMP_regs.B6 = 0x00000558;
  NRF_TEMP_regs.T0 = 0x000000E2;
  NRF_TEMP_regs.T1 = 0x00000002;
  NRF_TEMP_regs.T2 = 0x0000001F;
  NRF_TEMP_regs.T3 = 0x00000038;
  NRF_TEMP_regs.T4 = 0x0000004F;
  NRF_TEMP_regs.T5 = 0x00000066;
#endif

  TEMP_hw_started = false;
  TEMP_INTEN = 0;
  Timer_TEMP = TIME_NEVER;
}

NSI_TASK(nhw_temp_init, HW_INIT, 100);

/**
 * TASK_START triggered handler
 */
void nhw_TEMP_TASK_START(void) {
  if (TEMP_hw_started) {
    return;
  }
  TEMP_hw_started = true;
  Timer_TEMP = nsi_hws_get_time() + NHW_TEMP_t_TEMP;
  nsi_hws_find_next_event();
}

/**
 * TASK_STOP triggered handler
 */
void nhw_TEMP_TASK_STOP(void) {
  TEMP_hw_started = false;
  Timer_TEMP = TIME_NEVER;
  nsi_hws_find_next_event();
}

static void nhw_TEMP_eval_interrupt(uint inst) {
  static bool temp_int_line[NHW_TEMP_TOTAL_INST]; /* Is the TEMP currently driving its interrupt line high */
  /* Mapping of peripheral instance to {int controller instance, int number} */
  static struct nhw_irq_mapping nhw_temp_irq_map[NHW_TEMP_TOTAL_INST] = NHW_TEMP_INT_MAP;
  bool new_int_line = false;

  NHW_CHECK_INTERRUPT_si(TEMP, DATARDY, TEMP_INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&temp_int_line[inst],
                                       new_int_line,
                                       &nhw_temp_irq_map[inst]);
}

NHW_SIDEEFFECTS_INTSET_si(TEMP, NRF_TEMP_regs., TEMP_INTEN)
NHW_SIDEEFFECTS_INTCLR_si(TEMP, NRF_TEMP_regs., TEMP_INTEN)

NHW_SIDEEFFECTS_EVENTS(TEMP)

NHW_SIDEEFFECTS_TASKS_si(TEMP, START)
NHW_SIDEEFFECTS_TASKS_si(TEMP, STOP)

#if (NHW_HAS_DPPI)
NHW_SIDEEFFECTS_SUBSCRIBE_si(TEMP, START)
NHW_SIDEEFFECTS_SUBSCRIBE_si(TEMP, STOP)
#endif /* NHW_HAS_DPPI */

static NHW_SIGNAL_EVENT_si(TEMP, DATARDY)

/**
 * Time has come when the temperature measurement is ready
 */
static void nhw_temp_timer_triggered(void) {

  NRF_TEMP_regs.TEMP = temperature*(1 << NHW_TEMP_FBITS) + bs_random_uniformRi(-1,1);

  TEMP_hw_started = false;
  Timer_TEMP = TIME_NEVER;
  nsi_hws_find_next_event();

  nhw_TEMP_signal_EVENTS_DATARDY(0);
}

NSI_HW_EVENT(Timer_TEMP, nhw_temp_timer_triggered, 50);
