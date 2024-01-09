/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_UART_H
#define _NRF_HW_MODEL_UART_H

#include "bs_types.h"
#include <stdint.h>
#include "NHW_config.h"

#ifdef __cplusplus
extern "C"{
#endif

void nhw_UARTE_regw_sideeffects_TASKS_STARTRX(uint i);
void nhw_UARTE_regw_sideeffects_TASKS_STOPRX(uint i);
void nhw_UARTE_regw_sideeffects_TASKS_STARTTX(uint i);
void nhw_UARTE_regw_sideeffects_TASKS_STOPTX(uint i);
void nhw_UARTE_regw_sideeffects_TASKS_SUSPEND(uint i);
void nhw_UARTE_regw_sideeffects_TASKS_FLUSHRX(uint i);

void nhw_UARTE_regw_sideeffects_INTENSET(uint i);
void nhw_UARTE_regw_sideeffects_INTENCLR(uint i);

/* Side-effecting function when any event register is written: */
void nhw_UARTE_regw_sideeffects_EVENTS_all(uint t);

void nhw_UARTE_regw_sideeffects_SUBSCRIBE_STARTRX(uint i);
void nhw_UARTE_regw_sideeffects_SUBSCRIBE_STOPRX(uint i);
void nhw_UARTE_regw_sideeffects_SUBSCRIBE_STARTTX(uint i);
void nhw_UARTE_regw_sideeffects_SUBSCRIBE_STOPTX(uint i);
void nhw_UARTE_regw_sideeffects_SUBSCRIBE_FLUSHRX(uint i);

uint32_t nhw_UARTE_regr_sideeffects_ERRORSRC(unsigned int inst);
void nhw_UARTE_regw_sideeffects_ERRORSRC(unsigned int inst);

uint32_t nhw_UARTE_regr_sideeffects_RXD(unsigned int inst);
void nhw_UARTE_regw_sideeffects_TXD(unsigned int inst);

void nhw_UARTE_regw_sideeffects_ENABLE(unsigned int inst);
void nhw_UARTE_regw_sideeffects_CONFIG(unsigned int inst);

#if (NHW_HAS_PPI)
void nhw_uarte0_TASKS_STARTRX(void);
void nhw_uarte0_TASKS_STOPRX(void);
void nhw_uarte0_TASKS_STARTTX(void);
void nhw_uarte0_TASKS_STOPTX(void);
void nhw_uarte0_TASKS_SUSPEND(void);
void nhw_uarte0_TASKS_FLUSHRX(void);

void nhw_uarte1_TASKS_STARTRX(void);
void nhw_uarte1_TASKS_STOPRX(void);
void nhw_uarte1_TASKS_STARTTX(void);
void nhw_uarte1_TASKS_STOPTX(void);
void nhw_uarte1_TASKS_SUSPEND(void);
void nhw_uarte1_TASKS_FLUSHRX(void);
#endif /* (NHW_HAS_PPI) */

extern NRF_UARTE_Type NRF_UARTE_regs[];
#if (NHW_UARTE_HAS_UART)
extern NRF_UART_Type *NRF_UART_regs[];
#endif

typedef void (*uart_rtxb_cb_f)(uint inst, uint8_t *data);
/*
 * (Test interface) Register a callback which will be called
 * each time a byte is transmitted or received.
 * The callback may replace that byte with something else which will
 * be {stored in the UART Rx FIFO, sent over the line} instead of
 * the original byte.
 *
 * If the callback does not want to modify the data being received,
 * it should not modify the content of *data.
 *
 * This function returns the pointer to a possible previously registered
 * {Rx,Tx} callback (NULL if none was registered)
 *
 * <inst> is the UART instance for which we are registering the callback.
 *        UARTs are indexed globally for the whole SOC, as shown in NHW_config.h
 *        and --uart_list
 * <Rx_NotTx> should be set 1 to set the Rx callback, 0 to set the Tx callback
 */
uart_rtxb_cb_f nhw_uarte_register_callback(int inst, uart_rtxb_cb_f cb, bool Rx_NotTx);


#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_UART_H */
