/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef _NRF_HW_MODELS_NHW_DPPI_H
#define _NRF_HW_MODELS_NHW_DPPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef void (*dppi_callback_t)(void *);

void nhw_dppi_channel_subscribe(unsigned int dppi_inst,
                                unsigned int  channel_nbr,
                                dppi_callback_t callback,
                                void *param);
void nhw_dppi_event_signal(unsigned int  dppi_inst, unsigned int  channel_nbr);
void nhw_dppi_event_signal_if(unsigned int  dppi_inst, uint32_t publish_reg);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODELS_NHW_DPPI_H */
