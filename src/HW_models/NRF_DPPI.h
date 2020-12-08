/*
 * Copyright (c) 2020 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_DPPI_H
#define _NRF_HW_MODEL_DPPI_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

extern NRF_DPPIC_Type NRF_DPPI_regs;

typedef void (*task_function_t)(void); //typedef for function pointer

void nrf_dppi_subscriber_add(uint8_t channel, task_function_t function);
void nrf_dppi_subscriber_remove(task_function_t function);
void nrf_dppi_publish(uint8_t channel);

void nrf_dppi_init();
void nrf_dppi_clean_up();

void nrf_dppi_TASK_CHG0_EN();
void nrf_dppi_TASK_CHG1_EN();
void nrf_dppi_TASK_CHG2_EN();
void nrf_dppi_TASK_CHG3_EN();
void nrf_dppi_TASK_CHG4_EN();
void nrf_dppi_TASK_CHG5_EN();
void nrf_dppi_TASK_CHG0_DIS();
void nrf_dppi_TASK_CHG1_DIS();
void nrf_dppi_TASK_CHG2_DIS();
void nrf_dppi_TASK_CHG3_DIS();
void nrf_dppi_TASK_CHG4_DIS();
void nrf_dppi_TASK_CHG5_DIS();

void nrf_dppi_regw_sideeffects_CHENSET();
void nrf_dppi_regw_sideeffects_CHENCLR();

#ifdef __cplusplus
}
#endif

#endif
