/*
 * Copyright (c) 2017 Oticon A/S
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_NHW_AES_CCM_H
#define _NRF_HW_MODEL_NHW_AES_CCM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

void nhw_CCM_TASK_KSGEN(void);
void nhw_CCM_TASK_CRYPT(void);
void nhw_CCM_TASK_STOP(void);
void nhw_CCM_TASK_RATEOVERRIDE(void);
void nhw_ccm_radio_received_packet(bool crc_error);
void nhw_CCM_regw_sideeffects_INTENSET(void);
void nhw_CCM_regw_sideeffects_INTENCLR(void);
void nhw_CCM_regw_sideeffects_TASKS_KSGEN(void);
void nhw_CCM_regw_sideeffects_TASKS_CRYPT(void);
void nhw_CCM_regw_sideeffects_TASKS_STOP(void);
void nhw_CCM_regw_sideeffects_EVENTS_all(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_KSGEN(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_CRYPT(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_RATEOVERRIDE(unsigned int inst);

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_AES_CCM_H */
