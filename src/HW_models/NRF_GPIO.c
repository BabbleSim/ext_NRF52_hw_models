/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * GPIO - General purpose input/output
 * https://infocenter.nordicsemi.com/topic/ps_nrf52833/gpio.html?cp=5_1_0_5_7
 *
 * This model has the following limitations:
 *  * Note a.1:
 *    PIN_CNF.PULL is ignored. If a pin input is disconnected and not driven externally it will
 *    just remain at zero/low.
 *  * Note a.2:
 *    PIN_CNF.DRIVE / drive strength is ignored
 *    It is not possible to connect two outputs together building a logical AND or OR
 *  * Note a.3:
 *    Reading OUTCLR & DIRCLR reads always 0
 *  * Note a.4: There is no modeling of system idle/off, and as such a DETECT raising edge from the
 *    GPIO will not cause the system to exit those states.
 *
 * This model has the following approximations:
 *  * Note b.1:
 *    If a DIR bit is cleared for a pin which was being driven as a high output,
 *    that pin will be lowered immediately (like if it was driven low)
 *  * Note b.2:
 *    All drives are instantaneous.
 *  * Note b.3:
 *    During pin reconfigurations, there can be spurious transitions in the pins outputs
 *    which may not correspond to how the real HW would behave
 *  * In real HW, after a write to LATCH, the DETECT event output to the GPIOTE is kept low for
 *    a few clocks, before being raised again (if it needs to), in the model the new pulse/raise
 *    is sent instantaneously to the GPIOTE
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_peri_types.h"
#include "NRF_GPIO.h"
#include "NRF_GPIOTE.h"
#include "bs_tracing.h"
#include "nsi_tasks.h"

NRF_GPIO_Type NRF_GPIO_regs[NRF_GPIOS];

/* Number of pins per port: */
static int GPIO_n_ports_pins[NRF_GPIOS] = NRF_GPIO_PORTS_PINS;

static uint32_t IO_level[NRF_GPIOS]; /* Actual level in the pin */
static uint32_t DETECT[NRF_GPIOS];   /* Sense output / unlatched/non-sticky detect */
static uint32_t LDETECT[NRF_GPIOS];  /* Latched sense output */
static bool DETECT_signal[NRF_GPIOS]; /* Individual detect signal to the GPIOTE */

static uint32_t INPUT_mask[NRF_GPIOS]; /* As a 32bit mask, PIN_CNF[*].INPUT (0: enabled; 1: disabled)*/
static uint32_t SENSE_mask[NRF_GPIOS]; /* As a 32bit mask, PIN_CNF[*].SENSE.en (1: enabled; 0: disabled)*/
static uint32_t SENSE_inv[NRF_GPIOS];  /* As a 32bit mask, PIN_CNF[*].SENSE.inv (1: inverted;0: not inverted) */

/*
 * Is the output driven by another peripheral (1) or the GPIO directly (0).
 * Note that we don't keep track of who "owns" a pin, only that somebody else does
 */
static uint32_t out_override[NRF_GPIOS];
/* Out value provided by other peripherals */
static uint32_t external_OUT[NRF_GPIOS];

/* Is the pin input controlled by a peripheral(1) or the GPIO(0) */
static uint32_t input_override[NRF_GPIOS];
/* If input_override, is the peripheral configuring the input buffer as connected (1) or disconnected (0) */
static uint32_t input_override_connected[NRF_GPIOS];

/* Is "dir" controlled by a peripheral(1) or the GPIO(0) */
static uint32_t dir_override[NRF_GPIOS];
/* If dir_override is set, is the peripheral configuring the output as connected (1) or disconnected (0) */
static uint32_t dir_override_set[NRF_GPIOS];

/* Callbacks for peripherals to be informed of input changes */
static nrf_gpio_input_callback_t per_intoggle_callbacks[NRF_GPIOS][NRF_GPIO_MAX_PINS_PER_PORT];
/* Callbacks for test code to be informed of input/output changes: */
static nrf_gpio_input_callback_t test_intoggle_callback;
static nrf_gpio_input_callback_t test_outtoggle_callback;

/*
 * Initialize the GPIOs model
 */
static void nrf_gpio_init(void) {
	memset(NRF_GPIO_regs, 0, sizeof(NRF_GPIO_regs));

	for (int p = 0; p < NRF_GPIOS; p ++) {
		for (int n = 0; n < GPIO_n_ports_pins[p]; n++) {
			NRF_GPIO_regs[p].PIN_CNF[n] = 0x2; /* Disconnected out of reset */
		}
		INPUT_mask[p] = UINT32_MAX; /* All disconnected out of reset */
	}

	nrf_gpio_backend_init();
}

NSI_TASK(nrf_gpio_init, HW_INIT, 100);

unsigned int nrf_gpio_get_number_pins_in_port(int port) {
	return GPIO_n_ports_pins[port];
}

static void nrf_gpio_eval_outputs(unsigned int port);
static void nrf_gpio_eval_inputs(unsigned int port);
void nrf_gpio_eval_input(unsigned int port, unsigned int n, bool value);

/*
 * Register a test callback to be called whenever a pin IN register changes
 */
void nrf_gpio_test_register_in_callback(nrf_gpio_input_callback_t fptr) {
	test_intoggle_callback = fptr;
}

/*
 * Register a test callback to be called whenever an *output* pin changes
 */
void nrf_gpio_test_register_out_callback(nrf_gpio_input_callback_t fptr) {
	test_outtoggle_callback = fptr;
}

/*
 * Change a pin input value
 *
 * Note: The pin must not be currently driven by the SOC, or you will get an error
 *
 * Inputs:
 *  * port: Which GPIO port
 *  * n: which pin in that GPIO port
 *  * value: true: high, false: low
 */
void nrf_gpio_test_change_pin_level(unsigned int port, unsigned int n, bool value) {
	nrf_gpio_eval_input(port, n, value);
}

/*
 * Get a pin level
 *
 * Inputs:
 *  * port: Which GPIO port
 *  * n: which pin in that GPIO port
 * Return: true (high), false (low)
 */
bool nrf_gpio_get_pin_level(unsigned int port, unsigned int n) {
	return (IO_level[port] >> n) & 0x1;
}

#define CHECK_PIN_EXISTS(port, n, dir) \
	if (port >= NRF_GPIOS || n >= GPIO_n_ports_pins[port]) { \
		bs_trace_error_time_line("%s: Error, attempted to toggle "dir" for nonexistent " \
					"GPIO port %i, pin %i\n", \
					__func__, port, n); \
	}

static inline uint32_t get_enabled_inputs(unsigned int port){
	return (~input_override[port] & ~INPUT_mask[port])
		| (input_override[port] & input_override_connected[port]);
}

static inline uint32_t get_dir(unsigned int port){
	return (~dir_override[port] & NRF_GPIO_regs[port].DIR)
		| (dir_override[port] & dir_override_set[port]);
}

/*
 * Function with which another peripheral can claim configuration control of a pin.
 *
 * Inputs:
 *	* port: Which GPIO port
 *	* n : which pin in that GPIO port
 *	* override_output:
 *		* -1 : Don't change
 *		*  0 : Leave for GPIO control (GPIO OUT register sets the output value)
 *		*  1 : Take external control of pin output value (peripheral sets the output value
 *		       with nrf_gpio_peri_change_output() )
 *	* override_input:
 *		* -1 : Don't change
 *		*  0 : Leave input to be controlled by the GPIO module
 *		*  2 : Take external control of input, and disconnect
 *		*  3 : Take external control of input, and connect
 *	* override_dir:
 *		* -1 : Don't change
 *		*  0 : Leave DIR to be controlled by the GPIO module
 *		*  2 : Take external control of DIR, and disconnect
 *		*  3 : Take external control of DIR, and connect
 *      * fptr: Function to be called whenever that input toggles (if enabled).
 *              Set to NULL if not needed.
 *      * new_level:
 *              * -1: Don't change
 *              *  0: low
 *              *  1: high
 */
void nrf_gpio_peri_pin_control(unsigned int port, unsigned int n,
			      int override_output, int override_input, int override_dir,
			      nrf_gpio_input_callback_t fptr, int new_level) {

	if (port >= NRF_GPIOS || n >= GPIO_n_ports_pins[port]) { /* LCOV_EXCL_BR_LINE */
		bs_trace_error_time_line("Programming error\n"); /* LCOV_EXCL_LINE */
	}

	uint32_t mask = 1<<n;
	bool need_output_eval = false;
	bool need_input_eval = false;

	if (override_output >= 0) {
		out_override[port] &= ~mask;
		out_override[port] |= (uint32_t)(override_output?1:0) << n;
		need_output_eval = true;
	}
	if (override_input >= 0) {
		input_override[port] &= ~mask;
		input_override[port] |= (uint32_t)(override_input?1:0) << n;

		input_override_connected[port] &= ~mask;
		input_override_connected[port] |= (uint32_t)(override_input==3?1:0) << n;

		need_input_eval = true;
	}
	if (override_dir >= 0) {
		dir_override[port] &= ~mask;
		dir_override[port] |= (uint32_t)(override_dir?1:0) << n;

		dir_override_set[port] &= ~mask;
		dir_override_set[port] |= (uint32_t)(override_dir==3?1:0) << n;

		need_output_eval = true;
	}
	per_intoggle_callbacks[port][n] = fptr;
	if (new_level >= 0) {
		external_OUT[port] &= ~((uint32_t)1 << n);
		external_OUT[port] |= (uint32_t)(new_level?1:0) << n;
		need_output_eval = true;
	}

	if (need_output_eval) {
		nrf_gpio_eval_outputs(port);
	}
	if (need_input_eval) {
		nrf_gpio_eval_inputs(port);
	}
}

/*
 * A peripheral wants to toggle a GPIO output to a new value <value>.
 * Note: The value may be the same as it was, in which case nothing will happen.
 *
 * Inputs:
 *  * port is the GPIO port
 *  * n is the pin number inside that GPIO port (0..31)
 *  * value is the new output value high (true) or low (false)
 */
void nrf_gpio_peri_change_output(unsigned int port, unsigned int n, bool value)
{
	CHECK_PIN_EXISTS(port, n, "output"); /* LCOV_EXCL_BR_LINE */

	if (((out_override[port] >> n) & 0x1) != 1) { /* LCOV_EXCL_START */
		bs_trace_error_time_line("%s: Programming error, a peripheral is trying to toggle "
					"a GPIO output it does not own, GPIO port %i, pin %i\n",
					__func__, port, n);
	}

	if (((get_dir(port) >> n) & 0x1) != 1) {
		bs_trace_warning_time_line("%s: A peripheral is trying to toggle "
					   "a GPIO output but the output is disabled, "
					   "GPIO port %i, pin %i\n",
					   __func__, port, n);
	} /* LCOV_EXCL_STOP */

	external_OUT[port] &= ~((uint32_t)1 << n);
	external_OUT[port] |= (uint32_t)value << n;
	nrf_gpio_eval_outputs(port);
}

static void nrf_gpio_update_detect_signal(unsigned int port) {
	if (NRF_GPIO_regs[port].DETECTMODE == 0){ //gpio.detect signal from not latched detect
		DETECT_signal[port] = (DETECT[port] != 0);
	} else {//gpio.detect signal from latched detect
		DETECT_signal[port] = (LDETECT[port] != 0);
	}
}

/*
 * Evaluate sense output (after a change of input or configuration)
 */
static void nrf_gpio_eval_sense(unsigned int port){
	/* Note SENSE_dir inverts the output */
	DETECT[port] = (NRF_GPIO_regs[port].IN ^ SENSE_inv[port]) & SENSE_mask[port];
	LDETECT[port] |= DETECT[port];
	NRF_GPIO_regs[port].LATCH = LDETECT[port];

	bool old_DETECT_signal = DETECT_signal[port];

	nrf_gpio_update_detect_signal(port);

	if ((DETECT_signal[port] == true) && (old_DETECT_signal==false)) {
		nrf_gpiote_port_event_raise(port);
	}
}

/*
 * Return the level of the DETECT output signal for a GPIO instance
 *
 * input: port: The GPIO instance number
 */
bool nrf_gpio_get_detect_level(unsigned int port){
	return DETECT_signal[port];
}

/*
 * The input has changed and the driver is connected,
 * notify as necessary
 */
static void nrf_gpio_input_change_sideeffects(unsigned int port,unsigned int n)
{
	if (per_intoggle_callbacks[port][n] != NULL) {
		per_intoggle_callbacks[port][n](port, n, (NRF_GPIO_regs[port].IN >> n) & 0x1);
	}
	if (test_intoggle_callback != NULL) {
		test_intoggle_callback(port, n, (NRF_GPIO_regs[port].IN >> n) & 0x1);
	}
}

/*
 * Get the level of the IN signal for GPIO <port> pin <n>
 */
bool nrf_gpio_get_IN(unsigned int port, unsigned int n) {
	return (NRF_GPIO_regs[port].IN >> n) & 0x1;
}

/*
 * An input pin has toggled or the input configuration has changed,
 * propagate it
 */
static void nrf_gpio_eval_inputs(unsigned int port)
{
	uint32_t new_IN = IO_level[port] & get_enabled_inputs(port);

	uint32_t diff = new_IN ^ NRF_GPIO_regs[port].IN;

	NRF_GPIO_regs[port].IN = new_IN;

	for (int n = __builtin_ffs(diff) - 1; n >= 0; n = __builtin_ffs(diff) - 1) {
		nrf_gpio_input_change_sideeffects(port, n);
		diff &= ~(1 << n);
	}

	nrf_gpio_eval_sense(port);
}

/*
 * An input *may* be changing to a new value <value>.
 * Note: The value may be the same as it was, in which case nothing will happen.
 *
 * This function is meant to be called from something which drives the input
 * **externally**
 *
 * Inputs:
 *  * port is the GPIO port
 *  * n is the pin number inside that GPIO port (0..31)
 *  * value: is the input high (true) or low (false)
 */
void nrf_gpio_eval_input(unsigned int port, unsigned int n, bool value)
{
	CHECK_PIN_EXISTS(port, n, "input"); /* LCOV_EXCL_BR_LINE */

	uint32_t dir = get_dir(port);

	if ((dir >> n) & 0x1) { /* LCOV_EXCL_START */
		bs_trace_warning_time_line("%s: Attempted to drive externally a pin which is "
					   "currently being driven by the SOC. It will be ignored."
					   "GPIO port %i, pin %i\n",
					   __func__, port, n);
		return;
	}			/* LCOV_EXCL_STOP */

	int diff = ((IO_level[port] >> n) & 0x1) ^ (uint32_t)value;

	if (diff == 0) {
		/* No toggle */
		return;
	}

	IO_level[port] ^= (uint32_t)1 << n;

	nrf_gpio_eval_inputs(port);
}

/*
 * The output is being changed, propagate it as necessary and/or record it.
 */
static void nrf_gpio_output_change_sideeffects(unsigned int port,unsigned  int n, bool value)
{
	nrf_gpio_backend_write_output_change(port, n, value);
	if (test_outtoggle_callback != NULL) {
		test_outtoggle_callback(port, n, value);
	}
	nrf_gpio_backend_short_propagate(port, n, value);
}

/*
 * Reevaluate outputs after a configuration or OUT/external_OUT change
 */
static void nrf_gpio_eval_outputs(unsigned int port)
{
	/* Actual level in the pin, but only of the bits driven by output: */
	static uint32_t O_level[NRF_GPIOS];

	uint32_t dir = get_dir(port); /* Which pins are driven by output */

	uint32_t out = (~out_override[port] & NRF_GPIO_regs[port].OUT)
			| (out_override[port] & external_OUT[port]);

	uint32_t new_output = dir & out;

	uint32_t diff = new_output ^ O_level[port];

	if (diff == 0) {
		return;
	}

	O_level[port] = new_output;

	IO_level[port] &= ~dir;
	IO_level[port] |= O_level[port];

	for (int n = __builtin_ffs(diff) - 1; n >= 0; n = __builtin_ffs(diff) - 1) {
		nrf_gpio_output_change_sideeffects(port, n, (new_output >> n) & 0x1);
		diff &= ~(1 << n);
	}

	/* Inputs may be connected to pins driven by outputs, let's check */
	nrf_gpio_eval_inputs(port);
}


/*
 * Register write side-effecting functions:
 */

void nrf_gpio_regw_sideeffects_OUT(unsigned int port) {
	nrf_gpio_eval_outputs(port);
}

void nrf_gpio_regw_sideeffects_OUTSET(unsigned int port) {
	if (NRF_GPIO_regs[port].OUTSET) {
		NRF_GPIO_regs[port].OUT |= NRF_GPIO_regs[port].OUTSET;
		nrf_gpio_eval_outputs(port);
	}
	NRF_GPIO_regs[port].OUTSET = NRF_GPIO_regs[port].OUT;
}

void nrf_gpio_regw_sideeffects_OUTCLR(unsigned int port) {
	if (NRF_GPIO_regs[port].OUTCLR) {
		NRF_GPIO_regs[port].OUT &= ~NRF_GPIO_regs[port].OUTCLR;
		NRF_GPIO_regs[port].OUTCLR = 0;
		nrf_gpio_eval_outputs(port);
	}
}

void nrf_gpio_regw_sideeffects_DIR(unsigned int port) {
	/* Mirror change into PIN_CNF[*].DIR */
	for (int n = 0; n < GPIO_n_ports_pins[port]; n++ ) {
		NRF_GPIO_regs[port].PIN_CNF[n] &= ~GPIO_PIN_CNF_DIR_Msk;
		NRF_GPIO_regs[port].PIN_CNF[n] |= (NRF_GPIO_regs[port].DIR >> n) & 0x1;
	}

	nrf_gpio_eval_outputs(port);
}

void nrf_gpio_regw_sideeffects_DIRSET(unsigned int port) {
	if (NRF_GPIO_regs[port].DIRSET) {
		NRF_GPIO_regs[port].DIR |= NRF_GPIO_regs[port].DIRSET;
		nrf_gpio_regw_sideeffects_DIR(port);
	}
	NRF_GPIO_regs[port].DIRSET = NRF_GPIO_regs[port].DIR;
}

void nrf_gpio_regw_sideeffects_DIRCLR(unsigned int port) {
	if (NRF_GPIO_regs[port].DIRCLR) {
		NRF_GPIO_regs[port].DIR &= ~NRF_GPIO_regs[port].DIRCLR;
		NRF_GPIO_regs[port].DIRCLR = 0;
		nrf_gpio_regw_sideeffects_DIR(port);
	}
}

void nrf_gpio_regw_sideeffects_LATCH(unsigned int port) {

	/* LATCH contains what SW wrote: */
	uint32_t sw_input = NRF_GPIO_regs[port].LATCH;

	/* Whatever bits SW set to 1, it is trying to clear: */
	LDETECT[port] &= ~sw_input;

	/* But where the sense output is high, the bits are kept high: */
	LDETECT[port] |= DETECT[port];

	NRF_GPIO_regs[port].LATCH = LDETECT[port];

	nrf_gpio_update_detect_signal(port);

	/*
	 * Note the text from the spec:
	 *   If one or more bits in the LATCH register are '1' after the CPU has
	 *   performed a clear operation on the LATCH registers, a rising edge will be generated
	 *   on the LDETECT signal.
	 * "the CPU has performed a clear operation" == after writing LATCH with any bit to 1
	 */
	if (sw_input != 0 && LDETECT[port] != 0 && NRF_GPIO_regs[port].DETECTMODE == 1) {
		nrf_gpiote_port_event_raise(port);
	}
}

void nrf_gpio_regw_sideeffects_DETECTMODE(unsigned int port) {
	nrf_gpio_eval_sense(port);
}

void nrf_gpio_regw_sideeffects_PIN_CNF(unsigned int port,unsigned  int n) {

	bool need_output_eval = false;
	bool need_input_eval = false;
	bool need_sense_eval = false;

	int dir = NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_DIR_Msk;

	if (dir != ((NRF_GPIO_regs[port].DIR >> n) & 0x1)) {
		NRF_GPIO_regs[port].DIR ^= 1 << n;
		need_output_eval = true;
	}

	/* Note: DRIVE and PULL are not yet used in this model
	int pull = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_PULL_Msk)
					>> GPIO_PIN_CNF_PULL_Pos;

	int drive = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_DRIVE_Msk)
					>> GPIO_PIN_CNF_DRIVE_Pos;
	*/

	int input = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_INPUT_Msk)
			>> GPIO_PIN_CNF_INPUT_Pos;
	if (input != ((INPUT_mask[port] >> n) & 0x1)) {
		INPUT_mask[port] ^= 1 << n;
		need_input_eval = true;
	}

	int sense = (NRF_GPIO_regs[port].PIN_CNF[n] & GPIO_PIN_CNF_SENSE_Msk)
			>> GPIO_PIN_CNF_SENSE_Pos;
	if (((sense >> 1) & 0x1) != ((SENSE_mask[port] >> n) & 0x1)) {
		SENSE_mask[port] ^= 1 << n;
		need_sense_eval = true;
	}
	if ((sense & 0x1) != ((SENSE_inv[port] >> n) & 0x1)) {
		SENSE_inv[port] ^= 1 << n;
		need_sense_eval = true;
	}

	if (need_output_eval) {
		nrf_gpio_eval_outputs(port);
	}
	if (need_input_eval) {
		nrf_gpio_eval_inputs(port);
	}
	if (need_sense_eval) {
		nrf_gpio_eval_sense(port);
	}
}
