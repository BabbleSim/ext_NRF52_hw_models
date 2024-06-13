/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_54_AAR_CCM_ECB_H
#define _NRF_HW_MODEL_54_AAR_CCM_ECB_H

#ifdef __cplusplus
extern "C"{
#endif

void nhw_AAR_regw_sideeffects_INTENSET(unsigned int inst);
void nhw_AAR_regw_sideeffects_INTENCLR(unsigned int inst);
void nhw_AAR_regw_sideeffects_TASKS_START(unsigned int inst);
void nhw_AAR_regw_sideeffects_TASKS_STOP(unsigned int inst);
void nhw_AAR_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nhw_AAR_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_AAR_regw_sideeffects_EVENTS_all(unsigned int inst);

void nhw_AARCCM_regw_sideeffects_ENABLE(unsigned int inst);

void nhw_CCM_regw_sideeffects_INTENSET(unsigned int inst);
void nhw_CCM_regw_sideeffects_INTENCLR(unsigned int inst);
void nhw_CCM_regw_sideeffects_TASKS_START(unsigned int inst);
void nhw_CCM_regw_sideeffects_TASKS_STOP(unsigned int inst);
void nhw_CCM_regw_sideeffects_TASKS_RATEOVERRIDE(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_CCM_regw_sideeffects_SUBSCRIBE_RATEOVERRIDE(unsigned int inst);
void nhw_CCM_regw_sideeffects_EVENTS_all(unsigned int inst);

void nhw_ECB_regw_sideeffects_INTENSET(unsigned int inst);
void nhw_ECB_regw_sideeffects_INTENCLR(unsigned int inst);
void nhw_ECB_regw_sideeffects_SUBSCRIBE_START(unsigned int inst);
void nhw_ECB_regw_sideeffects_SUBSCRIBE_STOP(unsigned int inst);
void nhw_ECB_regw_sideeffects_TASKS_START(unsigned int inst);
void nhw_ECB_regw_sideeffects_TASKS_STOP(unsigned int inst);
void nhw_ECB_regw_sideeffects_EVENTS_all(unsigned int inst);

union NRF_AARCCM_Type {
  NRF_AAR_Type AAR;
  NRF_CCM_Type CCM;
};

extern union NRF_AARCCM_Type NRF_AARCCM_regs[];

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_54_AAR_CCM_ECB_H */
