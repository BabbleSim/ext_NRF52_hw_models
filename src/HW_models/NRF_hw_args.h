/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef NRF_HW_ARGS_H
#define NRF_HW_ARGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  double xo_drift;
  double start_offset;
  bool useRealAES;
} nrf_hw_sub_args_t;

void nrf_hw_sub_cmline_set_defaults(nrf_hw_sub_args_t *ptr);
int nrf_hw_sub_cmdline_parse(char *argv_i, nrf_hw_sub_args_t *args);
void nrf_hw_sub_command_line_handle(nrf_hw_sub_args_t *args, double start_time);

extern double nrfhw_start_of;
void nrf_hw_cmd_starto_found(char * argv, int offset);
extern double nrfhw_drift;
void nrf_hw_cmd_drift_found(char * argv, int offset);
extern bool nrfhw_useRealAES;
void nrf_hw_cmd_useRealAES_found(char * argv, int offset);

#define _NRF_HW_SUB_CMD_ARG_STRUCT \
  /*manual,mandatory,switch, option,             name ,   type,  destination,                callback,                      , description*/ \
  { false  , false , false, "start_offset" ,  "start_of", 'f', (void*)&nrfhw_start_of, nrf_hw_cmd_starto_found,"Offset in time (at the start of the simulation) of this device. At time 0 of the device, the phy will be at <start_of>"}, \
  { false  , false , false, "xo_drift" ,      "xo_drift", 'f', (void*)&nrfhw_drift,    nrf_hw_cmd_drift_found, "Simple linear model of the XO drift of this device. For ex. for -30ppm set to -30e-6"}, \
  { false  , false , false, "RealEncryption", "realAES",  'b', (void*)&nrfhw_useRealAES, nrf_hw_cmd_useRealAES_found, "(0)/1 Use the real AES encryption for the LL or just send everything in plain text (default) (requires the ext_libCryptov1 component)"}

#ifdef __cplusplus
}
#endif

#endif
