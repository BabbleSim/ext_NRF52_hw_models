/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file includes miscellaneous utility functions used by the RADIO model
 * but no significant logic *
 */
#include <stdint.h>
#include <string.h>
#include "bs_tracing.h"
#include "bs_utils.h"
#include "bs_pc_2G4.h"
#include "bs_pc_2G4_utils.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NHW_RADIO.h"
#include "NHW_RADIO_timings.h"
#include "NRF_HWLowL.h"
#include "nsi_hw_scheduler.h"

extern NRF_RADIO_Type NRF_RADIO_regs;
static double cheat_tx_power_offset;

static void nrfra_check_crc_conf_ble(void) {
  if ( (NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk)
      != (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) ) {
    bs_trace_error_line_time(
        "NRF_RADIO: CRCCNF Only 3 bytes CRC is supported in BLE mode (CRCCNF=%u)\n",
        NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk);
  }
}

static void nrfra_check_pcnf1_ble(void) {
  int checked, check;
  checked = NRF_RADIO_regs.PCNF1 &
        (  RADIO_PCNF1_WHITEEN_Msk
         | RADIO_PCNF1_ENDIAN_Msk
         | RADIO_PCNF1_BALEN_Msk
         | RADIO_PCNF1_STATLEN_Msk );

  check = (1 << RADIO_PCNF1_WHITEEN_Pos)
          | (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos)
          | (3 << RADIO_PCNF1_BALEN_Pos) //4 bytes address
          | (0 << RADIO_PCNF1_STATLEN_Pos);

  if (checked != check) {
    bs_trace_error_line_time(
        "%s w LR|1|2Mbps BLE modulation only the BLE packet format is supported so far (PCNF1=%u)\n",
        __func__, NRF_RADIO_regs.PCNF1);
  }
}

static void nrfra_check_ble1M_conf(void){
  int checked =NRF_RADIO_regs.PCNF0 &
      (RADIO_PCNF0_PLEN_Msk
          | RADIO_PCNF0_S1LEN_Msk
          | RADIO_PCNF0_S0LEN_Msk
          | RADIO_PCNF0_LFLEN_Msk);

  int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
      | ( 1 << RADIO_PCNF0_S0LEN_Pos )
      | ( 0 << RADIO_PCNF0_S1LEN_Pos )
      | ( RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos ) );

  if (checked != check) {
    bs_trace_error_line_time(
        "NRF_RADIO: For 1 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
        NRF_RADIO_regs.PCNF0);
  }

  nrfra_check_pcnf1_ble();
  nrfra_check_crc_conf_ble();
}


static void nrfra_check_ble2M_conf(void){
  int checked =NRF_RADIO_regs.PCNF0 &
      (RADIO_PCNF0_PLEN_Msk
          | RADIO_PCNF0_S1LEN_Msk
          | RADIO_PCNF0_S0LEN_Msk
          | RADIO_PCNF0_LFLEN_Msk);

  int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
      | ( 1 << RADIO_PCNF0_S0LEN_Pos )
      | ( 0 << RADIO_PCNF0_S1LEN_Pos )
      | ( RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos ) );

  if (checked != check) {
    bs_trace_error_line_time(
        "NRF_RADIO: For 2 Mbps only BLE packet format is supported so far (PCNF0=%u)\n",
        NRF_RADIO_regs.PCNF0);
  }

  nrfra_check_pcnf1_ble();
  nrfra_check_crc_conf_ble();
}

static void nrfra_check_bleLR_conf(void){

  int checked =NRF_RADIO_regs.PCNF0 &
          ( RADIO_PCNF0_TERMLEN_Msk
          | RADIO_PCNF0_PLEN_Msk
          | RADIO_PCNF0_CILEN_Msk
          | RADIO_PCNF0_S1LEN_Msk
          | RADIO_PCNF0_S0LEN_Msk
          | RADIO_PCNF0_LFLEN_Msk);

  int check = ( ( 8 << RADIO_PCNF0_LFLEN_Pos )
      | ( 1 << RADIO_PCNF0_S0LEN_Pos )
      | ( 0 << RADIO_PCNF0_S1LEN_Pos )
      | ( 2 << RADIO_PCNF0_CILEN_Pos )
      | ( RADIO_PCNF0_PLEN_LongRange << RADIO_PCNF0_PLEN_Pos )
      | ( 3 << RADIO_PCNF0_TERMLEN_Pos) );

  if (checked != check) {
    bs_trace_error_line_time(
        "NRF_RADIO: For LR BLE mode only BLE packet format is supported so far (PCNF0=%u)\n",
        NRF_RADIO_regs.PCNF0);
  }

  nrfra_check_pcnf1_ble();
  nrfra_check_crc_conf_ble();
}


static void nrfra_check_802154_conf(void){
  int checked, check;

  //Overall packet structure:
  checked =NRF_RADIO_regs.PCNF0 &
         (  RADIO_PCNF0_TERMLEN_Msk
          | RADIO_PCNF0_CRCINC_Msk
          | RADIO_PCNF0_PLEN_Msk
          | RADIO_PCNF0_CILEN_Msk
          | RADIO_PCNF0_S1INCL_Msk
          | RADIO_PCNF0_S1LEN_Msk
          | RADIO_PCNF0_S0LEN_Msk
          | RADIO_PCNF0_LFLEN_Msk);

  check = (
      //TERM = 0
        RADIO_PCNF0_CRCINC_Msk
      | ( RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos )
      //CILEN = 0
      //S1INCL = 0
      | ( 0 << RADIO_PCNF0_S1LEN_Pos )
      | ( 0 << RADIO_PCNF0_S0LEN_Pos )
      | ( 8 << RADIO_PCNF0_LFLEN_Pos )
       );
  if (checked != check) {
    bs_trace_error_line_time(
        "%s w 15.4 modulation only the 802154 packet format is supported so far (PCNF0=%u)\n",
        __func__, NRF_RADIO_regs.PCNF0);
  }

  checked = NRF_RADIO_regs.PCNF1 &
        (  RADIO_PCNF1_WHITEEN_Msk
         | RADIO_PCNF1_ENDIAN_Msk
         | RADIO_PCNF1_BALEN_Msk
         | RADIO_PCNF1_STATLEN_Msk );

  check =   (0 << RADIO_PCNF1_WHITEEN_Pos)
          | (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos)
          | (0 << RADIO_PCNF1_BALEN_Pos) // => 1 byte for SFD
          | (0 << RADIO_PCNF1_STATLEN_Pos);

  if (checked != check) {
    bs_trace_error_line_time(
        "%s w 15.4 modulation only the 802154 packet format is supported so far (PCNF1=%u)\n",
        __func__, NRF_RADIO_regs.PCNF1);
  }

  //CRC:
  if ( (NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk)
      != (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos) ) {
    bs_trace_error_line_time(
        "%s CRCCNF Only 2 bytes CRC is supported in 15.4 mode (CRCCNF=%u)\n",
        __func__,
        NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk);
  }
}

/*
 * A few checks to ensure the model is only used with the currently supported packet format
 */
void nhwra_check_packet_conf(void){

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    nrfra_check_ble1M_conf();
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    nrfra_check_ble2M_conf();
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
      || (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit)){
    nrfra_check_bleLR_conf();
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    nrfra_check_802154_conf();
  } else {
    bs_trace_error_line_time(
        "NRF_RADIO: Only 1&2 Mbps BLE & 802.15.4 packet formats supported so far (MODE=%u)\n",
        NRF_RADIO_regs.MODE);
  }
}

uint32_t nhwra_RSSI_value_to_modem_format(double rssi_value){
  rssi_value = -BS_MAX(rssi_value,-127);
  rssi_value = BS_MAX(rssi_value,0);
  return (uint32_t)rssi_value;
}

uint8_t nhwra_dBm_to_modem_LQIformat(double rssi_value){
  //PRF[dBm] = ED_RSSIOFFS + VALHARDWARE
  //=> VALHARDWARE = PRF[dBm] - ED_RSSIOFFS
  rssi_value -= NHW_RADIO_ED_RSSIOFFS;
  rssi_value = BS_MAX(rssi_value,0);
  rssi_value = BS_MIN(rssi_value,255);
  return (uint8_t)rssi_value;
}

double nrfra_LQIformat_to_dBm(uint value){
  //PRF[dBm] = ED_RSSIOFFS + VALHARDWARE
  return (double)value + NHW_RADIO_ED_RSSIOFFS;
}

int nhwra_is_HW_TIFS_enabled(void) {
  if ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_END_DISABLE_Msk )
      && ( ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_RXEN_Msk )
          || ( NRF_RADIO_regs.SHORTS & RADIO_SHORTS_DISABLED_TXEN_Msk ) )
      && ( (NRF_RADIO_regs.MODECNF0 & RADIO_MODECNF0_RU_Msk) == 0) )
  {
    return 1;
  }
  return 0;
}

static uint64_t nhwra_get_address(uint logical_addr) {
  uint64_t address;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    address = NRF_RADIO_regs.SFD & RADIO_SFD_SFD_Msk;
  } else {
    if (logical_addr > 7) {
      bs_trace_error_time_line("programming error: Logical address out of range (%u > 7)\n", logical_addr);
    }
    int BALEN_bits = 8*((NRF_RADIO_regs.PCNF1 & RADIO_PCNF1_BALEN_Msk) >> RADIO_PCNF1_BALEN_Pos);
    uint32_t base;

    if (logical_addr == 0) {
      base = NRF_RADIO_regs.BASE0;
    } else {
      base = NRF_RADIO_regs.BASE1;
    }

    uint32_t *prefix_ptr = &NRF_RADIO_regs.PREFIX0;
    uint64_t prefix = prefix_ptr[logical_addr >> 2] >> (8*(logical_addr & 0x3));

    address = ((prefix & RADIO_PREFIX0_AP0_Msk) << BALEN_bits)
               | (base >> (32 - BALEN_bits));
  }
  return address;
}

static p2G4_modulation_t nhra_modulation_from_mode(uint32_t MODE) {
  p2G4_modulation_t modulation;
  if (MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    modulation = P2G4_MOD_BLE;
  } else if (MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    modulation = P2G4_MOD_BLE2M;
  } else if (MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    modulation = P2G4_MOD_154_250K_DSS;
  } else if ((MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
             || (MODE == RADIO_MODE_MODE_Ble_LR500Kbit)) {
    modulation = P2G4_MOD_BLE_CODED;
  } else {
    bs_trace_error_time_line("programming error: Unsupported MODE %u\n", MODE);
  }
  return modulation;
}

bool nhwra_is_ble_mode(uint32_t MODE) {
  if ((MODE == RADIO_MODE_MODE_Ble_1Mbit)
      || (MODE == RADIO_MODE_MODE_Ble_2Mbit)
      || (MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
      || (MODE == RADIO_MODE_MODE_Ble_LR500Kbit)) {
    return true;
  } else {
    return false;
  }
}

/**
 * Prepare a Phy Rxv2 request structure
 * based on the radio registers configuration
 * (For CodedPhy only for the FEC2 part, and only provisional content assuming S=8)
 *
 * Note: The abort substructure is NOT filled.
 */
void nhwra_prep_rx_request(p2G4_rxv2_t *rx_req, p2G4_address_t *rx_addresses) {

  //TOLOW: Add support for other packet formats and bitrates
  uint8_t preamble_length = 0;
  uint8_t address_length = 0;
  uint8_t header_length = 0;
  bs_time_t pre_trunc = 0;
  uint16_t sync_threshold = 0;

  uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;
  double bits_per_us = 0;

  rx_addresses[0] = nhwra_get_address(0); /* We only support RXADDRESSES == 0x01 by now */

  rx_req->radio_params.modulation = nhra_modulation_from_mode(NRF_RADIO_regs.MODE);

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_1Mbit) {
    //Note that we only support BLE packet formats by now (so we ignore the configuration of the preamble and just assume it is what it needs to be)
    //we rely on the Tx side error/warning being enough to warn users that we do not support other formats
    preamble_length = 1; //1 byte
    address_length  = 4;
    header_length   = 2;
    bits_per_us = 1;
    pre_trunc = 0; //The modem can lose a lot of preamble and sync (~7us), we leave it as 0 by now to avoid a behavior change
    sync_threshold = 2; //(<) we tolerate less than 2 errors in the preamble and sync word together (old number, probably does not reflect the actual RADIO performance)
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_2Mbit) {
    preamble_length = 2; //2 bytes
    address_length  = 4;
    header_length   = 2;
    bits_per_us = 2;
    pre_trunc = 0; //The modem can lose a lot of preamble and sync (~7us), we leave it as 0 by now to avoid a behavior change
    sync_threshold = 2;
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR125Kbit)
      || (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ble_LR500Kbit)) {
    /* For the FEC2 part */
    preamble_length = 0;
    address_length  = 0;
    header_length   = 2;
    bits_per_us = 0.125; /* Provisional value assuming S=8 */
    pre_trunc = 0;
    sync_threshold = 0xFFFF;
  } else if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    preamble_length = 4;
    address_length  = 1;
    header_length   = 0;
    bits_per_us = 0.25;
    pre_trunc = 104; //The modem seems to be able to sync with just 3 sybmols of the preamble == lossing 13symbols|26bits|104us
    sync_threshold = 0;
  }

  rx_req->coding_rate = 0;

  p2G4_freq_t center_freq;
  p2G4_freq_from_d(freq_off, 1, &center_freq);
  rx_req->radio_params.center_freq = center_freq;

  rx_req->error_calc_rate = bits_per_us*1000000;
  rx_req->antenna_gain = 0;

  rx_req->header_duration  = header_length*8/bits_per_us;
  rx_req->header_threshold = 0; //(<=) we tolerate 0 bit errors in the header which will be found in the crc (we may want to tune this)
  rx_req->sync_threshold   = sync_threshold;
  rx_req->acceptable_pre_truncation = pre_trunc;

  rx_req->n_addr = 1;

  rx_req->pream_and_addr_duration = (preamble_length + address_length)*8/bits_per_us;

  rx_req->scan_duration = UINT32_MAX;
  rx_req->forced_packet_duration = UINT32_MAX; //we follow the transmitted packet (assuming no length errors by now)

  rx_req->start_time  = hwll_phy_time_from_dev(nsi_hws_get_time());

  rx_req->resp_type = 0;
  rx_req->prelocked_tx = false;
}

/**
 * Prepare a Phy Rxv2 request structure for CodedPhy's FEC1 part
 * based on the radio registers configuration.
 *
 * Note: The abort substructure is NOT filled.
 */
void nhwra_prep_rx_request_FEC1(p2G4_rxv2_t *rx_req, p2G4_address_t *rx_addresses) {

  uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;
  p2G4_freq_t center_freq;

  p2G4_freq_from_d(freq_off, 1, &center_freq);
  rx_req->radio_params.center_freq = center_freq;

  rx_addresses[0] = nhwra_get_address(0); /* We only support RXADDRESSES == 0x01 by now */
  rx_req->n_addr = 1;

  rx_req->radio_params.modulation = nhra_modulation_from_mode(NRF_RADIO_regs.MODE);

  rx_req->antenna_gain = 0;

  rx_req->coding_rate = 8;

  rx_req->error_calc_rate = 125000;

  rx_req->header_duration  = 16; /* CI duration */
  rx_req->header_threshold = 0xFFFF; /* We don't want "header"/CI errors in the FEC1 part in any case for design simplicity, we just check the "packet error" result */
  rx_req->sync_threshold   = 2;
  rx_req->acceptable_pre_truncation = 65; /* It seems the radio manages with ~15us of coded phy preamble in good signal conditions */

  rx_req->pream_and_addr_duration = 80 + 256;

  rx_req->scan_duration = UINT32_MAX;
  rx_req->forced_packet_duration = UINT32_MAX; //we follow the transmitted packet (assuming no length errors by now)

  rx_req->start_time  = hwll_phy_time_from_dev(nsi_hws_get_time());

  rx_req->resp_type = 0;
}

/**
 * Prepare a Phy Tx request structure
 * based on the radio registers configuration.
 *
 * Note: The abort substructure is NOT filled.
 */
void nhwra_prep_tx_request(p2G4_txv2_t *tx_req, uint packet_size, bs_time_t packet_duration,
                           bs_time_t start_time, uint16_t coding_rate) {

  tx_req->radio_params.modulation = nhra_modulation_from_mode(NRF_RADIO_regs.MODE);

  tx_req->phy_address = nhwra_get_address(NRF_RADIO_regs.TXADDRESS);

  {
    double TxPower = (int8_t)( NRF_RADIO_regs.TXPOWER & RADIO_TXPOWER_TXPOWER_Msk); //the cast is to sign extend it
    tx_req->power_level = p2G4_power_from_d(TxPower + cheat_tx_power_offset); //Note that any possible Tx antenna or PA gain would need to be included here
  }

  { //Note only default freq. map supported
    uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;
    p2G4_freq_t center_freq;
    p2G4_freq_from_d(freq_off, 1, &center_freq);
    tx_req->radio_params.center_freq = center_freq;
  }
  tx_req->packet_size  = packet_size; //Not including preamble or address

  {
    tx_req->start_tx_time = start_time;
    tx_req->start_packet_time = tx_req->start_tx_time;
    tx_req->end_tx_time = tx_req->start_tx_time + packet_duration - 1;
    tx_req->end_packet_time = tx_req->end_tx_time;
  }
  tx_req->coding_rate = coding_rate;
}

/**
 * Prepare a Phy CCA request structure
 * based on the radio registers configuration.
 *
 * Note: The abort substructure is NOT filled.
 */
void nhwra_prep_cca_request(p2G4_cca_t *cca_req, bool CCA_not_ED, double rx_pow_offset) {

  cca_req->start_time  = hwll_phy_time_from_dev(nsi_hws_get_time()); //We start right now
  cca_req->antenna_gain = 0;

  uint32_t freq_off = NRF_RADIO_regs.FREQUENCY & RADIO_FREQUENCY_FREQUENCY_Msk;
  p2G4_freq_t center_freq;
  p2G4_freq_from_d(freq_off, 1, &center_freq);
  cca_req->radio_params.center_freq = center_freq;

  if (NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit) {
    cca_req->radio_params.modulation = P2G4_MOD_154_250K_DSS;
  } else {
    bs_trace_error_line_time("CCA procedure only supported with 15.4 modulation\n");
  }

  int symbol_time = 16; //micros

  if (CCA_not_ED) { //CCA request
    uint edthreshold = (NRF_RADIO_regs.CCACTRL & RADIO_CCACTRL_CCAEDTHRES_Msk) >> RADIO_CCACTRL_CCAEDTHRES_Pos;
    uint CCAMode = (NRF_RADIO_regs.CCACTRL & RADIO_CCACTRL_CCAMODE_Msk) >> RADIO_CCACTRL_CCAMODE_Pos;
    double carrier_detect_level = -110 - rx_pow_offset; //dBm : Any detectable signal by the modem
    double edthreshold_dBm = nrfra_LQIformat_to_dBm(edthreshold) - rx_pow_offset;

    cca_req->scan_duration  = 8*symbol_time;
    cca_req->scan_period    = 2*symbol_time; //let's measure 4 times (model design choice)

    if (CCAMode == RADIO_CCACTRL_CCAMODE_EdMode) {
      cca_req->rssi_threshold = p2G4_RSSI_value_from_dBm(edthreshold_dBm);
      cca_req->mod_threshold  = p2G4_RSSI_value_from_dBm(100/*dBm*/); //not used
      cca_req->stop_when_found = 0;
    } else if (CCAMode == RADIO_CCACTRL_CCAMODE_CarrierMode) {
      cca_req->stop_when_found = 1;
      cca_req->rssi_threshold = p2G4_RSSI_value_from_dBm(100/*dBm*/); //not used
      cca_req->mod_threshold  = p2G4_RSSI_value_from_dBm(carrier_detect_level);
      //The Phy does not support detecting just based on something close enough in the correlator output
      //so we ignore CCACTRL.CCACORRCNT & CCACTRL.CCACORRTHRES
    } else if ((CCAMode == RADIO_CCACTRL_CCAMODE_CarrierAndEdMode)
        || (CCAMode == RADIO_CCACTRL_CCAMODE_CarrierOrEdMode) ) {
      cca_req->stop_when_found = 1;
      cca_req->rssi_threshold = p2G4_RSSI_value_from_dBm(edthreshold_dBm);
      cca_req->mod_threshold  = p2G4_RSSI_value_from_dBm(carrier_detect_level);
    } else if (CCAMode == RADIO_CCACTRL_CCAMODE_EdModeTest1) {
      cca_req->rssi_threshold = p2G4_RSSI_value_from_dBm(edthreshold_dBm);
      cca_req->mod_threshold  = p2G4_RSSI_value_from_dBm(100/*dBm*/); //not used
      cca_req->stop_when_found = 2;
    } else {
      bs_trace_error_time_line("%s, CCAMODE=%i support not yet implemented\n",
          __func__, CCAMode);
    }
  } else { //ED request
    cca_req->scan_duration = 8 * symbol_time * ((NRF_RADIO_regs.EDCNT & RADIO_EDCNT_EDCNT_Msk) + 1);
    cca_req->scan_period   = 2 * symbol_time; //let's measure 4 times per EDCNT (model design choice)
    cca_req->rssi_threshold = p2G4_RSSI_value_from_dBm(100/*dBm*/); //not used
    cca_req->mod_threshold  = p2G4_RSSI_value_from_dBm(100/*dBm*/); //not used
    cca_req->stop_when_found = 0;
  }
}


/**
 * Return the CRC length in bytes
 */
uint nhwra_get_crc_length(void) {
  return (NRF_RADIO_regs.CRCCNF & RADIO_CRCCNF_LEN_Msk) >> RADIO_CRCCNF_LEN_Pos;
}


uint nhwra_get_MAXLEN(void) {
  return (NRF_RADIO_regs.PCNF1 & RADIO_PCNF1_MAXLEN_Msk) >> RADIO_PCNF1_MAXLEN_Pos;
}

/*
 * Return the payload length, NOT including the CRC length
 * (and NOT adding S0 or S1 lengths)
 */
uint nhwra_get_payload_length(uint8_t *buf){
  int S0Len;
  int LFLenb, LFLenB;
  uint payload_len = 0;
  S0Len = (NRF_RADIO_regs.PCNF0 &  RADIO_PCNF0_S0LEN_Msk) >> RADIO_PCNF0_S0LEN_Pos;

  LFLenb = (NRF_RADIO_regs.PCNF0 & RADIO_PCNF0_LFLEN_Msk) >> RADIO_PCNF0_LFLEN_Pos;
  LFLenB = (LFLenb + 7)/8;

  for (int i = 0; i < LFLenB; i++){
    payload_len += buf[S0Len+i] << i*8;
  }

  if (NRF_RADIO_regs.PCNF0 & RADIO_PCNF0_CRCINC_Msk) {
    int crc_len = nhwra_get_crc_length();
    if (payload_len >= crc_len) {
      payload_len -= crc_len;
    } else {
      bs_trace_error_time_line("Programmed payload length (%i) smaller than CRC length (%i), "
          "while it was configured as including the CRC.. => SW programming error\n",
          payload_len, crc_len);
    }
  }
  return payload_len;
}

uint nrfra_get_capped_payload_length(uint8_t *buf) {
  uint payload_lenght = nhwra_get_payload_length(buf);
  uint max_length = nhwra_get_MAXLEN();
  return BS_MIN(payload_lenght, max_length);
}

/*
 * Get the received CRC value from the received buffer
 * paramters:
 *    rx_buf: Pointer to the received buffer
 *    rx_packet_size: Number of input bytes in rx_buf
 */
uint32_t nhwra_get_rx_crc_value(uint8_t *rx_buf, size_t rx_packet_size) {
  uint32_t crc = 0;
  uint crc_len = nhwra_get_crc_length();
  uint payload_len = nrfra_get_capped_payload_length(rx_buf);

  //Eventually this should be generalized with the packet configuration
  if (nhwra_is_ble_mode(NRF_RADIO_regs.MODE)
      && ( rx_packet_size >= 5 ) ){
    memcpy((void*)&crc, &rx_buf[2 + payload_len], crc_len);
  } else if ((NRF_RADIO_regs.MODE == RADIO_MODE_MODE_Ieee802154_250Kbit)
      && ( rx_packet_size >= 3 ) ){
    memcpy((void*)&crc, &rx_buf[1 + payload_len], crc_len);
  }
  return crc;
}

/**
 * Assemble a packet to be transmitted out thru the air into tx_buf[]
 * Omitting the preamble and address/sync flag
 *
 * Return copied payload size (after S0 + len + S1) into tx_buf[]
 * (without the CRC)
 *
 * Note: PCNF1.MAXLEN is taken into account to cap len
 *
 * Note: When adding support for CodedPhy and or other packet formats,
 * this needs to be reworked together with the start_Tx()
 * function, as it is all way too interdependent
 */
uint nhwra_tx_copy_payload(uint8_t *tx_buf){
  int S0Len, S1LenB, LFLenB; //All in bytes
  int LFLenb, S1LenAirb;
  int i;
  uint payload_len;
  int maxlen;

  S0Len = (NRF_RADIO_regs.PCNF0 &  RADIO_PCNF0_S0LEN_Msk) >> RADIO_PCNF0_S0LEN_Pos;


  S1LenAirb = (NRF_RADIO_regs.PCNF0 & RADIO_PCNF0_S1LEN_Msk) >> RADIO_PCNF0_S1LEN_Pos;
  S1LenB = (S1LenAirb + 7)/8; //This is just : S1Offset = ceil(PCNF0.S1LEN / 8)

  LFLenb = (NRF_RADIO_regs.PCNF0 & RADIO_PCNF0_LFLEN_Msk) >> RADIO_PCNF0_LFLEN_Pos;
  LFLenB = (LFLenb + 7)/8;

  //copy from RAM to Tx buffer
  i = 0;
  if (S0Len) {
    tx_buf[0] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[0];
    i++;
  }
  for (int j = 0; j < LFLenB; j++){ //Copy up to 2 Length bytes
    tx_buf[i] = ((uint8_t*)NRF_RADIO_regs.PACKETPTR)[i];
    i++;
  }
  int S1Off = 0;
  if ( NRF_RADIO_regs.PCNF0 & ( RADIO_PCNF0_S1INCL_Include << RADIO_PCNF0_S1INCL_Pos ) ) {
    if (S1LenB == 0) {
      S1Off = 1; //We skip 1 S1 byte in RAM
    }
    /*
     * If S1INCL and S1LEN > 0, the assumption is that the
     * the size in RAM will just be the same as in air
     * TODO: this behavior needs to be confirmed
     */
  }

  payload_len = nhwra_get_payload_length(tx_buf);
  /* Note that we assume if CRCINC=1, CRCLEN is deducted from the length field
   * before capping the length to MAXLEN */
  maxlen = nhwra_get_MAXLEN();
  if (payload_len > maxlen) {
    bs_trace_error_time_line("NRF_RADIO: Transmitting a packet longer than the configured MAXLEN (%i>%i). "
        "This would truncate it and a corrupted packet will be transmitted. "
        "Assuming this is a controller programming error, so we stop here. "
        "If you did really intend this, please request this error to be converted into a warning "
        "(the model handles this properly)\n", payload_len, maxlen);
    payload_len = maxlen;
    NRF_RADIO_regs.PDUSTAT = RADIO_PDUSTAT_PDUSTAT_Msk;
  } else {
    NRF_RADIO_regs.PDUSTAT = 0;
  }

  int copy_len = payload_len + S1LenB;
  memcpy(&tx_buf[i], &((uint8_t*)NRF_RADIO_regs.PACKETPTR)[i + S1Off], copy_len);
  return payload_len;
}

void hw_radio_testcheat_set_tx_power_gain(double power_offset_i) {
  cheat_tx_power_offset = power_offset_i;
}
