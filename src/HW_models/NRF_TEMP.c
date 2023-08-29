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

#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_TEMP.h"
#include <string.h>
#include <stdbool.h>
#include "nsi_hw_scheduler.h"
#include "NHW_peri_types.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_rand_main.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

NRF_TEMP_Type NRF_TEMP_regs;
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_temp_irq_map[NHW_TEMP_TOTAL_INST] = NHW_TEMP_INT_MAP;

static bs_time_t Timer_TEMP = TIME_NEVER; //Time when the next temperature measurement will be ready

static bool TEMP_hw_started = false;
static bool TEMP_INTEN = false; //interrupt enable
#define T_TEMP 36 /* microseconds */
#define TEMP_FBITS 2 /* fractional bits */

static double temperature = 25.0; /* Actual temperature the device is at */

/**
 * Initialize the TEMP model
 */
static void nrf_temp_init(void) {
  memset(&NRF_TEMP_regs, 0, sizeof(NRF_TEMP_regs));
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

  TEMP_hw_started = false;
  TEMP_INTEN = false;
  Timer_TEMP = TIME_NEVER;
}

NSI_TASK(nrf_temp_init, HW_INIT, 100);

/**
 * TASK_START triggered handler
 */
void nrf_temp_task_start(void) {
  if (TEMP_hw_started) {
    return;
  }
  TEMP_hw_started = true;
  Timer_TEMP = nsi_hws_get_time() + T_TEMP;
  nsi_hws_find_next_event();
}

/**
 * TASK_STOP triggered handler
 */
void nrf_temp_task_stop(void) {
  TEMP_hw_started = false;
  Timer_TEMP = TIME_NEVER;
  nsi_hws_find_next_event();
}

void nrf_temp_regw_sideeffects_TASK_START(void) {

  NRF_TEMP_regs.TASKS_START &= 1;

  if ( NRF_TEMP_regs.TASKS_START ) {
    NRF_TEMP_regs.TASKS_START = 0;
    nrf_temp_task_start();
  }
}

void nrf_temp_regw_sideeffects_TASK_STOP(void) {

  NRF_TEMP_regs.TASKS_STOP &= 1;

  if ( NRF_TEMP_regs.TASKS_STOP ) {
    NRF_TEMP_regs.TASKS_STOP = 0;
    nrf_temp_task_stop();
  }
}

void nrf_temp_regw_sideeffects_INTENSET(void) {

  NRF_TEMP_regs.INTENSET &= 1;

  if ( NRF_TEMP_regs.INTENSET ) {
    TEMP_INTEN = true;
  }
}

void nrf_temp_regw_sideeffects_INTENCLEAR(void) {

  NRF_TEMP_regs.INTENCLR &= 1;

  if ( NRF_TEMP_regs.INTENCLR ) {
    TEMP_INTEN = false;
    NRF_TEMP_regs.INTENSET = 0;
    NRF_TEMP_regs.INTENCLR = 0;
  }
}

/**
 * Time has come when the temperature measurement is ready
 */
static void nrf_temp_timer_triggered(void) {

  NRF_TEMP_regs.TEMP = temperature*(1 << TEMP_FBITS) + bs_random_uniformRi(-1,1);

  TEMP_hw_started = false;
  Timer_TEMP = TIME_NEVER;
  nsi_hws_find_next_event();

  NRF_TEMP_regs.EVENTS_DATARDY = 1;
  nrf_ppi_event(TEMP_EVENTS_DATARDY);

  int inst = 0;
  if ( TEMP_INTEN ){
    nhw_irq_ctrl_set_irq(nhw_temp_irq_map[inst].cntl_inst,
                         nhw_temp_irq_map[inst].int_nbr);
  }
}

NSI_HW_EVENT(Timer_TEMP, nrf_temp_timer_triggered, 50);
