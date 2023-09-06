/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef _NRF_HW_MODELS_NHW_DPPI_H
#define _NRF_HW_MODELS_NHW_DPPI_H

#include <stdint.h>
#include <stdbool.h>
#include "bs_types.h"
#include "NHW_common_types.h"

#ifdef __cplusplus
extern "C"{
#endif

#define DPPI_CB_NO_PARAM ((void*)INTPTR_MAX)

typedef void (*dppi_callback_t)(void *);

void nhw_dppi_channel_subscribe(unsigned int dppi_inst,
                                unsigned int  channel_nbr,
                                dppi_callback_t callback,
                                void *param);
void nhw_dppi_channel_unsubscribe(unsigned int dppi_inst,
                                  unsigned int channel_nbr,
                                  dppi_callback_t callback,
                                  void *param);
void nhw_dppi_event_signal(unsigned int  dppi_inst, unsigned int  channel_nbr);
void nhw_dppi_event_signal_if(unsigned int  dppi_inst, uint32_t publish_reg);

void nhw_dppi_common_subscribe_sideeffect(unsigned int  dppi_inst,
                                          uint32_t SUBSCRIBE_reg,
                                          struct nhw_subsc_mem *last,
                                          dppi_callback_t callback,
                                          void *param);

void nhw_dppi_regw_sideeffects_TASK_CHGn_EN(uint dppi_inst, uint n);
void nhw_dppi_regw_sideeffects_TASK_CHGn_DIS(uint dppi_inst, uint n);
void nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_EN(unsigned int dppi_inst, uint n);
void nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_DIS(unsigned int dppi_inst, uint n);
void nhw_dppi_regw_sideeffects_CHENSET(uint dppi_inst);
void nhw_dppi_regw_sideeffects_CHENCLR(uint dppi_inst);
void nhw_dppi_regw_sideeffects_CHEN(uint dppi_inst);
void nhw_dppi_regw_sideeffects_CHGn(uint dppi_inst, uint n);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODELS_NHW_DPPI_H */
