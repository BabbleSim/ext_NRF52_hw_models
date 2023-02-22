/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes the radio timing related logic
 * That is, how long the different delays and ramp ups are
 */

#include "bs_types.h"
#include "bs_tracing.h"
#include "NRF_RADIO.h"
#include "NRF_RADIO_utils.h"

static struct {
  /*Ramp up times*/
  bs_time_t TX_RU_time[2][2][2];
  /* The versions are [1,2Mbps] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  bs_time_t RX_RU_time[2][2][2];

  /*Digital processing delay:*/
  bs_time_t TX_chain_delay;
  bs_time_t RX_chain_delay[2]; /*Indexed as 1Mbps, 2Mbps*/

  /*Ramp down times*/
  bs_time_t TX_RD_time;
  bs_time_t RX_RD_time;
} radio_timings;


void nrfra_timings_init(void) {
  /* The versions are [1,2Mbps] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  radio_timings.TX_RU_time[0][1][0] =  41; // 41000
  radio_timings.TX_RU_time[0][0][1] = 141; //141000
  radio_timings.TX_RU_time[0][0][0] = 130; //130000
  radio_timings.TX_RU_time[1][1][0] =  40; // 40000
  radio_timings.TX_RU_time[1][0][1] = 140; //140000
  radio_timings.TX_RU_time[1][0][0] = 129; //128900

  radio_timings.RX_RU_time[0][1][0] =  40; // 40000
  radio_timings.RX_RU_time[0][0][1] = 140; //140000
  radio_timings.RX_RU_time[0][0][0] = 129; //129000
  radio_timings.RX_RU_time[1][1][0] =  40; // 40000
  radio_timings.RX_RU_time[1][0][1] = 140; //140000
  radio_timings.RX_RU_time[1][0][0] = 129; //129000

  radio_timings.TX_chain_delay    = 1; //0.6 /*both 1 and 2Mbps*/
  radio_timings.RX_chain_delay[0] = 9; //9.4  /* 1Mbps */
  radio_timings.RX_chain_delay[1] = 5; //5.45 /* 2Mbps */

  radio_timings.TX_RD_time = 6;
  radio_timings.RX_RD_time = 0;
}

/**
 * Return the Rx chain delay given the configured MODE
 */
bs_time_t nrfra_timings_get_Rx_chain_delay(){
  int Mbps2 = 0;
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    Mbps2 = 1;
  }
  return radio_timings.RX_chain_delay[Mbps2];
}

/**
 * Return the rampup time given the configured MODE & MODECNF0
 * * TxNotRx should be set to 1 for Tx and 0 for Rx
 * * from_hw_TIFS should be set to 1 if the RADIO is automatically
 *                switching during its auto IFS mechanism
 * returns the requested rampup time
 */
bs_time_t nrfra_timings_get_rampup_time(bool TxNotRx, bool from_hw_TIFS) {
  int fast  = 0;
  int Mbps2 = 0;
  int HWTIFS= 0;

  if ( NRF_RADIO_regs.MODECNF0 & 1 ){ /* MODECNF0.RU */
    fast = 1;
  } else {
    HWTIFS = from_hw_TIFS | nrfra_is_HW_TIFS_enabled();
  }
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    Mbps2 = 1;
  }
  if (TxNotRx) {
    return radio_timings.TX_RU_time[Mbps2][fast][HWTIFS];
  } else {
    return radio_timings.RX_RU_time[Mbps2][fast][HWTIFS];
  }
}

bs_time_t nrfra_timings_get_RX_rampdown_time(void){
  return radio_timings.RX_RD_time;
}

bs_time_t nrfra_timings_get_TX_rampdown_time(void){
  return radio_timings.TX_RD_time;
}

bs_time_t nrfra_timings_get_TX_chain_delay(void){
  return radio_timings.TX_chain_delay;
}

