/*
 * Copyright (c) 2023, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_IPC_H
#define _NRF_HW_MODEL_IPC_H

#include "bs_types.h"
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_IPC_Type NRF_IPC_regs[NHW_IPC_TOTAL_INST];

void nhw_IPC_regw_sideeffects_TASKS_SEND(uint inst, uint ch);
void nhw_IPC_regw_sideeffects_EVENTS_all(uint inst);
void nhw_IPC_regw_sideeffects_INTEN(uint inst);
void nhw_IPC_regw_sideeffects_INTENSET(uint inst);
void nhw_IPC_regw_sideeffects_INTENCLR(uint inst);
void nhw_IPC_regw_sideeffects_SUBSCRIBE_SEND(uint inst, uint ch);

#ifdef __cplusplus
}
#endif

#endif

