/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes miscellaneous utility functions used by the RADIO model
 * but no significant logic *
 */
#include "NRF_RADIO.h"
#include "bs_tracing.h"
#include "bs_utils.h"

void nrfra_check_packet_conf(void){
  // a few checks to ensure the model is only used with the currently supported packet format
  int checked =NRF_RADIO_regs.PCNF0 &
      (RADIO_PCNF0_PLEN_Msk
          | RADIO_PCNF0_S1LEN_Msk
          | RADIO_PCNF0_S0LEN_Msk
          | RADIO_PCNF0_LFLEN_Msk);

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
        | ( 1 << RADIO_PCNF0_S0LEN_Pos )
        | ( 0 << RADIO_PCNF0_S1LEN_Pos )
        | ( RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos ) );

    if (checked != check) {
      bs_trace_error_line_time(
          "NRF_RADIO: For 1 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
          NRF_RADIO_regs.PCNF0);
    }

  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {

    int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
        | ( 1 << RADIO_PCNF0_S0LEN_Pos )
        | ( 0 << RADIO_PCNF0_S1LEN_Pos )
        | ( RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos ) );

    if (checked != check) {
      bs_trace_error_line_time(
          "NRF_RADIO: For 2 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
          NRF_RADIO_regs.PCNF0);
    }

  } else {
    bs_trace_error_line_time(
        "NRF_RADIO: Only 1&2 Mbps BLE packet format supported so far (MODE=%u)\n",
        NRF_RADIO_regs.MODE);
  }

  if ( (NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk)
      != (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) ) {
    bs_trace_error_line_time(
        "NRF_RADIO: CRCCNF Only 3 bytes CRC is supported (CRCCNF=%u)\n",
        NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk);
  }
}

uint32_t nrfra_RSSI_value_to_modem_format(double rssi_value){
  rssi_value = -BS_MAX(rssi_value,-127);
  rssi_value = BS_MAX(rssi_value,0);
  return (uint32_t)rssi_value;
}

int nrfra_is_HW_TIFS_enabled() {
  if ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_END_DISABLE_Msk )
      && ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_RXEN_Msk )
          || ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ) )
  ){
    return 1;
  }
  return 0;
}
