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
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_RADIO.h"
#include "NHW_RADIO_utils.h"

extern NRF_RADIO_Type NRF_RADIO_regs;

static struct {
  /*Ramp up times*/
  bs_time_t TX_RU_time[5][2][2];
  /* The versions are [1,2Mbps,CodedS=2,CodedS=8, 15.4] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  bs_time_t RX_RU_time[5][2][2];

  /*Digital processing delay:*/
  bs_time_t TX_chain_delay;    //Time between the START task and the bits start coming out of the antenna
  bs_time_t RX_chain_delay[5]; //Time between the bit ends in the antenna, and the corresponding event is generated (e.g. ADDRESS)
                               /*Indexed as 1Mbps, 2Mbps, CodedS=2, CodedS=8, 15.4*/

  /*Ramp down times*/
  bs_time_t TX_RD_time[5];
  bs_time_t RX_RD_time;
} radio_timings;


void nrfra_timings_init(void) {
  //All timings for a 52833
  /* The versions are [1, 2Mbps,CodedS=2,CodedS=8, 15.4] [Normal, Fast] [No_TIFS, HW_TIFS] */
  /* where HW TIFS only applies for Normal rampup */
  /* BLE 1 Mbps */
  radio_timings.TX_RU_time[0][1][0] =  41; // 41000
  radio_timings.TX_RU_time[0][0][1] = 141; //141000
  radio_timings.TX_RU_time[0][0][0] = 130; //130000
  /* BLE 2 Mbps */
  radio_timings.TX_RU_time[1][1][0] =  40; // 40000
  radio_timings.TX_RU_time[1][0][1] = 140; //140000
  radio_timings.TX_RU_time[1][0][0] = 129; //128900
  /* Coded S=2 */
  radio_timings.TX_RU_time[2][1][0] =  42; // 42000
  radio_timings.TX_RU_time[2][0][1] = 132; //132000
  radio_timings.TX_RU_time[2][0][0] = 132; //132000
  /* Coded S=8 */
  radio_timings.TX_RU_time[3][1][0] =  42; // 42000
  radio_timings.TX_RU_time[3][0][1] = 122; //122000
  radio_timings.TX_RU_time[3][0][0] = 132; //132000
  /* 15.4 */
  radio_timings.TX_RU_time[4][1][0] =  40; // 40000
  radio_timings.TX_RU_time[4][0][1] = 130; //130000 - Is this correct? or should it be 169us?
  radio_timings.TX_RU_time[4][0][0] = 129; //128900 ?? just copied from Ble 1Mbps

  /* BLE 1 Mbps */
  radio_timings.RX_RU_time[0][1][0] =  40; // 40000
  radio_timings.RX_RU_time[0][0][1] = 140; //140000
  radio_timings.RX_RU_time[0][0][0] = 129; //129000
  /* BLE 2 Mbps */
  radio_timings.RX_RU_time[1][1][0] =  40; // 40000
  radio_timings.RX_RU_time[1][0][1] = 140; //140000
  radio_timings.RX_RU_time[1][0][0] = 129; //129000
  /* Coded S=2 */ //The radio always ramps up with S=8
  radio_timings.RX_RU_time[2][1][0] =  40; // 40000
  radio_timings.RX_RU_time[2][0][1] = 120; //120000
  radio_timings.RX_RU_time[2][0][0] = 130; //130000
  /* Coded S=8 */
  radio_timings.RX_RU_time[3][1][0] =  40; // 40000
  radio_timings.RX_RU_time[3][0][1] = 120; //120000
  radio_timings.RX_RU_time[3][0][0] = 130; //130000
  /* 15.4 */
  radio_timings.RX_RU_time[4][1][0] =  40; // 40000
  radio_timings.RX_RU_time[4][0][1] = 130; //140000 - Is this correct? or should it be 169us?
  radio_timings.RX_RU_time[4][0][0] = 129; //129000 ?? just copied from Ble 1Mbps

  radio_timings.TX_chain_delay    = 1; //~1us /*both 1, 2Mbps and 15.4, for BLE coded phy it is ~2us*/
  radio_timings.RX_chain_delay[0] = 9; //9.4  /* 1Mbps */
  radio_timings.RX_chain_delay[1] = 5; //5.45 /* 2Mbps */
  radio_timings.RX_chain_delay[2] = 30; ///* BLE coded, S=2 */ /* For simplicity S=2 & S=8 are given the same chain delay */
  radio_timings.RX_chain_delay[3] = 30; ///* BLE coded, S=8 */
  radio_timings.RX_chain_delay[4] = 22; //22us /* 15.4 */

  //Note: TXEND is produced significantly earlier in 15.4 than the end of the bit in the air (~17.3us),
  //      while for 1/2M BLE it is ~1us, and for coded w S8 it is ~6us.
  //Note: TXPHYEND comes *after* the bit has finished in air.

  radio_timings.TX_RD_time[0] = 6;
  radio_timings.TX_RD_time[1] = 6; //According to the spec this should be 4us for the 52833. To avoid a behavior change we leave it as 6 by now
  radio_timings.TX_RD_time[2] = 10;
  radio_timings.TX_RD_time[3] = 10;
  radio_timings.TX_RD_time[4] = 21;
  radio_timings.RX_RD_time = 0; //In reality it seems modulation dependent at ~0, ~0 & ~0.5 us
}

/**
 * Return the Rx chain delay given the configured MODE
 */
bs_time_t nhwra_timings_get_Rx_chain_delay(void) {
  int mod_idx = 0;
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    mod_idx = 1;
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit)
       || (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit)) {
    /* To differentiate we'd need to check if we are in the FEC1 or FEC2 and the CI value */
    mod_idx = 2;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    mod_idx = 4;
  }
  return radio_timings.RX_chain_delay[mod_idx];
}

static int get_modidx(void) {
  int mod_idx = 0;
  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    mod_idx = 1;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit) {
    mod_idx = 2;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit) {
    mod_idx = 3;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    mod_idx = 4;
  }
  return mod_idx;
}
/**
 * Return the rampup time given the configured MODE & MODECNF0
 * * TxNotRx should be set to 1 for Tx and 0 for Rx
 * * from_hw_TIFS should be set to 1 if the RADIO is automatically
 *                switching during its auto IFS mechanism
 * returns the requested rampup time
 */
bs_time_t nhwra_timings_get_rampup_time(bool TxNotRx, bool from_hw_TIFS) {
  int fast  = 0;
  int mod_idx = get_modidx();
  int HWTIFS= 0;

  if ( NRF_RADIO_regs.MODECNF0 & 1 ){ /* MODECNF0.RU */
    fast = 1;
  } else {
    HWTIFS = from_hw_TIFS | nhwra_is_HW_TIFS_enabled();
  }

  if (TxNotRx) {
    return radio_timings.TX_RU_time[mod_idx][fast][HWTIFS];
  } else {
    return radio_timings.RX_RU_time[mod_idx][fast][HWTIFS];
  }
}

bs_time_t nhwra_timings_get_RX_rampdown_time(void){
  return radio_timings.RX_RD_time;
}

bs_time_t nhwra_timings_get_TX_rampdown_time(void){
  int mod_idx = get_modidx();
  return radio_timings.TX_RD_time[mod_idx];
}

bs_time_t nhwra_timings_get_TX_chain_delay(void){
  return radio_timings.TX_chain_delay;
}

