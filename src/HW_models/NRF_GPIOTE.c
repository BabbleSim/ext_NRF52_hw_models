/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * GPIOTE - GPIO tasks and events
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/gpiote.html?cp=5_1_0_5_8
 *
 * This model has the following limitations:
 *  * INTENCLR will always read as 0
 *  * Unlike in real HW, tasks cannot occur simultaneously, they always happen in some sequence
 *    so task priority is not accounted for
 */

#include <stdint.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NRF_GPIOTE.h"
#include "NRF_GPIO.h"
#include "NRF_PPI.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "nsi_tasks.h"

NRF_GPIOTE_Type NRF_GPIOTE_regs = {0};
/* Mapping of peripheral instance to {int controller instance, int number} */
static struct nhw_irq_mapping nhw_gpiote_irq_map[NHW_GPIOTE_TOTAL_INST] = NHW_GPIOTE_INT_MAP;

static uint32_t GPIOTE_ITEN;
static bool gpiote_int_line; /* Is the GPIOTE currently driving its interrupt line high */

/* For each GPIO channel, its status */
static struct gpiote_ch_status_t {
	uint8_t mode;
	uint8_t port;  /* GPIO instance */
	uint8_t pin;   /* GPIO pin in that instance (psel) */
	uint8_t polarity; /* Content of the CONFIG[].polarity field */
	/* Level at which the GPIOTE has been driving this pin,
	 * or which it has been getting from the GPIO
	 * true = high, false = low */
	bool level;
} gpiote_ch_status[N_GPIOTE_CHANNELS];

/**
 * Initialize the GPIOs model
 */
static void nrf_gpiote_init(void) {
	memset(&NRF_GPIOTE_regs, 0, sizeof(NRF_GPIOTE_regs));
}

NSI_TASK(nrf_gpiote_init, HW_INIT, 100);

static void nrf_gpiote_events_port(void);

/*
 * API to the GPIO components, in which they can signal that their DETECT output signal
 * has been raised.
 *
 * We do not keep track in the GPIOTE about the signal being lowered, as the GPIOTE
 * only reacts to raising edges.
 * Therefore it is the responsibility of the GPIO models to call this if and only
 * if there is a raising edge on their DETECT output signal.
 */
void nrf_gpiote_port_event_raise(unsigned int port) {
	(void)port; /* unused */
	nrf_gpiote_events_port();
}

void nrf_gpiote_TASKS_OUT(unsigned int n) {
	struct gpiote_ch_status_t *sc = &gpiote_ch_status[n];
	if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
		return;
	}
	switch (sc->polarity) {
	case GPIOTE_CONFIG_POLARITY_None:
		return;
	case GPIOTE_CONFIG_POLARITY_LoToHi:
		sc->level = true;
		break;
	case GPIOTE_CONFIG_POLARITY_HiToLo:
		sc->level = false;
		break;
	case GPIOTE_CONFIG_POLARITY_Toggle:
		sc->level = !sc->level;
		break;
	default:	/* LCOV_EXCL_START */
		bs_trace_error_time_line("%s: Missconfigured CONFIG.CONFIG[%i]\n", n);
		break;
	}		/* LCOV_EXCL_STOP */
	/* We may be calling the GPIO even if we haven't changed it, but that is fine */
	nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

void nrf_gpiote_TASKS_SET(unsigned int n) {
	struct gpiote_ch_status_t *sc = &gpiote_ch_status[n];
	if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
		return;
	}
	sc->level = true;
	/* We may be calling the GPIO even if we haven't changed it, but that is fine */
	nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

void nrf_gpiote_TASKS_CLR(unsigned int n) {
	struct gpiote_ch_status_t *sc = &gpiote_ch_status[n];
	if (sc->mode != GPIOTE_CONFIG_MODE_Task) {
		return;
	}
	sc->level = false;
	/* We may be calling the GPIO even if we haven't changed it, but that is fine */
	nrf_gpio_peri_change_output(sc->port, sc->pin, sc->level);
}

static void nrf_gpiote_eval_interrupt(void) {
	bool new_int_line = false;
	int mask;

	for (int i = 0; i < N_GPIOTE_CHANNELS; i++) {
		mask = (GPIOTE_ITEN >> i) & 0x1;
		if (NRF_GPIOTE_regs.EVENTS_IN[i] && mask) {
			new_int_line = true;
			break; /* No need to check more */
		}
	}
	mask = (GPIOTE_ITEN & GPIOTE_INTENCLR_PORT_Msk) >> GPIOTE_INTENCLR_PORT_Pos;
	if (NRF_GPIOTE_regs.EVENTS_PORT && mask) {
		new_int_line = true;
	}

	int inst = 0;
	if (gpiote_int_line == false && new_int_line == true) {
		gpiote_int_line = true;
		hw_irq_ctrl_raise_level_irq_line(nhw_gpiote_irq_map[inst].cntl_inst,
				nhw_gpiote_irq_map[inst].int_nbr);
	} else if (gpiote_int_line == true && new_int_line == false) {
		gpiote_int_line = false;
		hw_irq_ctrl_lower_level_irq_line(nhw_gpiote_irq_map[inst].cntl_inst,
				nhw_gpiote_irq_map[inst].int_nbr);
	}
}

static void nrf_gpiote_events_in(unsigned int n) {
	NRF_GPIOTE_regs.EVENTS_IN[n] = 1;
	nrf_gpiote_eval_interrupt();
	nrf_ppi_event(GPIOTE_EVENTS_IN_0 + n);
}

static void nrf_gpiote_events_port(void) {
	NRF_GPIOTE_regs.EVENTS_PORT = 1;
	nrf_gpiote_eval_interrupt();
	nrf_ppi_event(GPIOTE_EVENTS_PORT);
}

/*
 * Function to be called (by the GPIO model) when a pin changes
 * for which an EVENTS_IN is registered
 */
static void nrf_gpiote_input_change_ntf(unsigned int port, unsigned int n, bool value)
{
	int i;
	struct gpiote_ch_status_t *sc;
	bool generate_event = false;

	/* Find event we are connecting this pin to */
	for (i = 0; i < N_GPIOTE_CHANNELS; i++) {
		sc = &gpiote_ch_status[i];
		if (sc->port == port && sc->pin == n) {
			break;
		}
	}
	if (i == N_GPIOTE_CHANNELS) { /* LCOV_EXCL_START */
		bs_trace_error_time_line("%s: Programming error, received notification from not "
					 "connected GPIO port.pin:%i.%i\n", port, n);
	} /* LCOV_EXCL_STOP */

	switch (sc->polarity) {
	case GPIOTE_CONFIG_POLARITY_None:
		return;
	case GPIOTE_CONFIG_POLARITY_LoToHi:
		if ((sc->level == false) && (value == true)) {
			generate_event = true;
		}
		break;
	case GPIOTE_CONFIG_POLARITY_HiToLo:
		if ((sc->level == true) && (value == false)) {
			generate_event = true;
		}
		break;
	case GPIOTE_CONFIG_POLARITY_Toggle:
		if (sc->level != value) {
			generate_event = true;
		}
		break;
	default:	/* LCOV_EXCL_START */
		bs_trace_error_time_line("%s: Missconfigured CONFIG.CONFIG[%i]\n", n);
		break;
	}		/* LCOV_EXCL_STOP */
	sc->level = value;

	if (generate_event) {
		nrf_gpiote_events_in(i);
	}
}

/*
 * Register write side-effecting functions
 */

void nrf_gpiote_regw_sideeffects_TASKS_OUT(unsigned int n) {
	if (NRF_GPIOTE_regs.TASKS_OUT[n]) {
		NRF_GPIOTE_regs.TASKS_OUT[n] = 0;
		nrf_gpiote_TASKS_OUT(n);
	}
}

void nrf_gpiote_regw_sideeffects_TASKS_SET(unsigned int n) {
	if (NRF_GPIOTE_regs.TASKS_SET[n]) {
		NRF_GPIOTE_regs.TASKS_SET[n] = 0;
		nrf_gpiote_TASKS_SET(n);
	}
}

void nrf_gpiote_regw_sideeffects_TASKS_CLR(unsigned int n) {
	if (NRF_GPIOTE_regs.TASKS_CLR[n]) {
		NRF_GPIOTE_regs.TASKS_CLR[n] = 0;
		nrf_gpiote_TASKS_CLR(n);
	}
}

void nrf_gpiote_regw_sideeffects_EVENTS_IN(unsigned int n) {
	nrf_gpiote_eval_interrupt();
}

void nrf_gpiote_regw_sideeffects_EVENTS_PORT(void) {
	nrf_gpiote_eval_interrupt();

}

void nrf_gpiote_regw_sideeffects_INTENSET(void) {
	if (NRF_GPIOTE_regs.INTENSET) {
		GPIOTE_ITEN |= NRF_GPIOTE_regs.INTENSET;
		NRF_GPIOTE_regs.INTENSET = GPIOTE_ITEN;
		nrf_gpiote_eval_interrupt();
	}
}

void nrf_gpiote_regw_sideeffects_INTENCLR(void) {
	if (NRF_GPIOTE_regs.INTENCLR) {
		GPIOTE_ITEN &= ~NRF_GPIOTE_regs.INTENCLR;
		NRF_GPIOTE_regs.INTENCLR = 0;
		nrf_gpiote_eval_interrupt();
	}
}

void nrf_gpiote_regw_sideeffects_CONFIG(unsigned int n) {
	struct gpiote_ch_status_t *sc = &gpiote_ch_status[n];
	unsigned int mode = NRF_GPIOTE_regs.CONFIG[n] & GPIOTE_CONFIG_MODE_Msk;
	unsigned int pin = (NRF_GPIOTE_regs.CONFIG[n] & GPIOTE_CONFIG_PSEL_Msk)
				>>GPIOTE_CONFIG_PSEL_Pos;
	unsigned int port = (NRF_GPIOTE_regs.CONFIG[n] & GPIOTE_CONFIG_PORT_Msk)
				>>GPIOTE_CONFIG_PORT_Pos;
	unsigned int polarity = (NRF_GPIOTE_regs.CONFIG[n] & GPIOTE_CONFIG_POLARITY_Msk)
				>>GPIOTE_CONFIG_POLARITY_Pos;
	unsigned int outinit = (NRF_GPIOTE_regs.CONFIG[n] & GPIOTE_CONFIG_OUTINIT_Msk)
				>>GPIOTE_CONFIG_OUTINIT_Pos;

	if ((port != sc->port) || (pin != sc->pin)
	    || (mode == GPIOTE_CONFIG_MODE_Disabled  && sc->mode != GPIOTE_CONFIG_MODE_Disabled)) {
		/* Disconnect the old GPIO pin from the GPIOTE */
		nrf_gpio_peri_pin_control(sc->port, sc->pin, 0, 0, 0, NULL, -1);
	}

	sc->mode = mode;
	sc->pin = pin;
	sc->port = port;
	sc->polarity = polarity;

	if (mode == GPIOTE_CONFIG_MODE_Event) {
		sc->level = nrf_gpio_get_pin_level(port, pin);
		nrf_gpio_peri_pin_control(port, pin, 1, 3, 2, nrf_gpiote_input_change_ntf, -1);
	} else if (mode == GPIOTE_CONFIG_MODE_Task) {
		sc->level = outinit;
		nrf_gpio_peri_pin_control(port, pin, 1, 2, 3, NULL, outinit);
	} else if (mode != GPIOTE_CONFIG_MODE_Disabled) { /* LCOV_EXCL_START */
		bs_trace_error_time_line("%s: GPIOTE.CONFIG[%u].mode configured to an illegal "
					 "value(%u)\n", __func__, n, mode);
	} /* LCOV_EXCL_STOP */
}

/*
 * Trampolines to automatically call from the PPI
 */
/* Generated with:
#! /usr/bin/env python3
#GPIOTE.c
for task in {"OUT","SET","CLR"}:
    for i in range(0,8):
       print("void nrf_gpiote_TASKS_%s_%i(void){ nrf_gpiote_TASKS_%s(%i); }"%(task,i,task,i))
#GPIOTE.h
for task in {"OUT","SET","CLR"}:
    for i in range(0,8):
       print("void nrf_gpiote_TASKS_%s_%i(void);"%(task,i))
*/
void nrf_gpiote_TASKS_SET_0(void){ nrf_gpiote_TASKS_SET(0); }
void nrf_gpiote_TASKS_SET_1(void){ nrf_gpiote_TASKS_SET(1); }
void nrf_gpiote_TASKS_SET_2(void){ nrf_gpiote_TASKS_SET(2); }
void nrf_gpiote_TASKS_SET_3(void){ nrf_gpiote_TASKS_SET(3); }
void nrf_gpiote_TASKS_SET_4(void){ nrf_gpiote_TASKS_SET(4); }
void nrf_gpiote_TASKS_SET_5(void){ nrf_gpiote_TASKS_SET(5); }
void nrf_gpiote_TASKS_SET_6(void){ nrf_gpiote_TASKS_SET(6); }
void nrf_gpiote_TASKS_SET_7(void){ nrf_gpiote_TASKS_SET(7); }
void nrf_gpiote_TASKS_CLR_0(void){ nrf_gpiote_TASKS_CLR(0); }
void nrf_gpiote_TASKS_CLR_1(void){ nrf_gpiote_TASKS_CLR(1); }
void nrf_gpiote_TASKS_CLR_2(void){ nrf_gpiote_TASKS_CLR(2); }
void nrf_gpiote_TASKS_CLR_3(void){ nrf_gpiote_TASKS_CLR(3); }
void nrf_gpiote_TASKS_CLR_4(void){ nrf_gpiote_TASKS_CLR(4); }
void nrf_gpiote_TASKS_CLR_5(void){ nrf_gpiote_TASKS_CLR(5); }
void nrf_gpiote_TASKS_CLR_6(void){ nrf_gpiote_TASKS_CLR(6); }
void nrf_gpiote_TASKS_CLR_7(void){ nrf_gpiote_TASKS_CLR(7); }
void nrf_gpiote_TASKS_OUT_0(void){ nrf_gpiote_TASKS_OUT(0); }
void nrf_gpiote_TASKS_OUT_1(void){ nrf_gpiote_TASKS_OUT(1); }
void nrf_gpiote_TASKS_OUT_2(void){ nrf_gpiote_TASKS_OUT(2); }
void nrf_gpiote_TASKS_OUT_3(void){ nrf_gpiote_TASKS_OUT(3); }
void nrf_gpiote_TASKS_OUT_4(void){ nrf_gpiote_TASKS_OUT(4); }
void nrf_gpiote_TASKS_OUT_5(void){ nrf_gpiote_TASKS_OUT(5); }
void nrf_gpiote_TASKS_OUT_6(void){ nrf_gpiote_TASKS_OUT(6); }
void nrf_gpiote_TASKS_OUT_7(void){ nrf_gpiote_TASKS_OUT(7); }
