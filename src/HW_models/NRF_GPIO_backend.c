/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * "Backends" for the GPIO pins.
 * Currently there is 3 parts to this:
 *   * Outputs changes can be recorded in a file
 *   * Inputs can be driver from a file
 *   * Outputs can be short-circuited to inputs thru a configuration file
 *
 * Check docs/GPIO.md for more info.
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NRF_GPIO.h"
#include "bs_types.h"
#include "nsi_hw_scheduler.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "bs_cmd_line.h"
#include "bs_dynargs.h"
#include "nsi_hws_models_if.h"
#include "nsi_tasks.h"

static bs_time_t Timer_GPIO_input = TIME_NEVER;

static char *gpio_in_file_path = NULL; /* Possible file for input stimuli */
static char *gpio_out_file_path = NULL; /* Possible file for dumping output toggles */
static char *gpio_conf_file_path = NULL; /* Possible file for configuration (short-circuits) */

#define MAXLINESIZE 2048
#define MAX_SHORTS 8

/* Table keeping all configured short-circuits */
static struct {
	uint8_t port;
	uint8_t pin;
} shorts[NRF_GPIOS][NRF_GPIO_MAX_PINS_PER_PORT][MAX_SHORTS];

static FILE *output_file_ptr; /* File pointer for gpio_out_file_path */

/* GPIO input status */
static struct {
	FILE *input_file_ptr; /* File pointer for gpio_out_file_path */
	/* Next event port.pin & level: */
	unsigned int port;
	unsigned int pin;
	bool level;           /* true: high; false: low*/
} gpio_input_file_st;

static void nrf_gpio_load_config(void);
static void nrf_gpio_init_output_file(void);
static void nrf_gpio_init_input_file(void);

/*
 * Initialize the GPIO backends
 */
void nrf_gpio_backend_init(void)
{
	memset(shorts, UINT8_MAX, sizeof(shorts));

	nrf_gpio_load_config();
	nrf_gpio_init_output_file();
	nrf_gpio_init_input_file();
}

/*
 * Cleanup before exit
 */
static void nrf_gpio_backend_cleaup(void)
{
	if (output_file_ptr != NULL) {
		fclose(output_file_ptr);
		output_file_ptr = NULL;
	}

	if (gpio_input_file_st.input_file_ptr != NULL) {
		fclose(gpio_input_file_st.input_file_ptr);
		gpio_input_file_st.input_file_ptr = NULL;
	}
}

NSI_TASK(nrf_gpio_backend_cleaup, ON_EXIT_PRE, 100);


static void nrf_gpio_register_cmd_args(void){

  static bs_args_struct_t args_struct_toadd[] = {
    {
      .option="gpio_in_file",
      .name="path",
      .type='s',
      .dest=(void *)&gpio_in_file_path,
      .descript="Optional path to a file containing GPIOs inputs activity",
    },
    {
      .option="gpio_out_file",
      .name="path",
      .type='s',
      .dest=(void *)&gpio_out_file_path,
      .descript="Optional path to a file where GPIOs output activity will be saved",
    },
    {
      .option="gpio_conf_file",
      .name="path",
      .type='s',
      .dest=(void *)&gpio_conf_file_path,
      .descript="Optional path to a file where the GPIOs configuration will be found.",
    },
    ARG_TABLE_ENDMARKER
  };

  bs_add_extra_dynargs(args_struct_toadd);
}

NSI_TASK(nrf_gpio_register_cmd_args, PRE_BOOT_1, 100);

/*
 * Propagate an output change thru its external short-circuits
 */
void nrf_gpio_backend_short_propagate(unsigned int port, unsigned int n, bool value)
{
	int i;
	for (i = 0 ; i < MAX_SHORTS; i++){
		if (shorts[port][n][i].port == UINT8_MAX) {
			break;
		}
		nrf_gpio_eval_input(shorts[port][n][i].port, shorts[port][n][i].pin, value);
	}
}

/*
 * Initialize the GPIO output activity file
 * (open and write csv header)
 */
static void nrf_gpio_init_output_file(void)
{
	if (gpio_out_file_path == NULL) {
		return;
	}

	bs_create_folders_in_path(gpio_out_file_path);
	output_file_ptr = bs_fopen(gpio_out_file_path, "w");
	fprintf(output_file_ptr, "time(microsecond),port,pin,level\n");
}

/*
 * Register an output pin change in the gpio output file
 */
void nrf_gpio_backend_write_output_change(unsigned int port, unsigned int n, bool value)
{
	if (output_file_ptr != NULL){
		fprintf(output_file_ptr, "%"PRItime",%u,%u,%u\n",
			nsi_hws_get_time(), port, n, value);
	}
}

/*
 * Read a line from a file into a buffer (s), while
 * skipping duplicate spaces (unless they are quoted), comments (#...),
 * and empty lines
 * The string will be null terminated (even if nothing is copied in)
 *
 * Return: The number of characters copied into s (apart from the termination 0 byte)
 */
static int readline(char *s, int size, FILE *stream)
{
	int c = 0, i=0;
	bool was_a_space = true;
	bool in_a_string = false;

	while ((i == 0) && (c != EOF)) {
		while ((i < size - 1) && ((c=getc(stream)) != EOF) && c!='\n') {
			if (isspace(c) && (!in_a_string)) {
				if (was_a_space) {
					continue;
				}
				was_a_space = true;
			} else {
				was_a_space = false;
			}
			if (c=='"') {
				in_a_string = !in_a_string;
			}
			if (c == '#') {
				bs_skipline(stream);
				break;
			}
			s[i++] =c;
		}
	}
	s[i] = 0;

	if (i >= size - 1) {
		bs_trace_warning_line("Truncated line while reading from file after %i chars\n",
				      size-1);
	}
	return i;
}

/*
 * Register an external GPIO output -> input short-circuit
 * Normally this is automatically called when a gpio configuration file
 * defines a short-circuit, but it can also be called from test code.
 */
void nrf_gpio_backend_register_short(uint8_t Port_out, uint8_t Pin_out,
				     uint8_t Port_in, uint8_t Pin_in)
{
	int i;
	unsigned int max_pins;

	for (i = 0 ; i < MAX_SHORTS; i++) {
		if (shorts[Port_out][Pin_out][i].port == UINT8_MAX)
			break;
	}
	if (i == MAX_SHORTS) {
		bs_trace_error_line("%s: Number of supported shorts per output (%i) exceeded\n",
				__func__, MAX_SHORTS);
	}
	if (Port_out >= NRF_GPIOS) {
		bs_trace_error_time_line("%s: GPIO configuration file attempted to set short from "
				"non existing GPIO port (%u>=%u)\n",
				__func__, Port_out, NRF_GPIOS);
	}
	if (Port_in >= NRF_GPIOS) {
		bs_trace_error_time_line("%s: GPIO configuration file attempted to set short to "
				"non existing GPIO port (%u>=%u)\n",
				__func__, Port_in, NRF_GPIOS);
	}
	max_pins = nrf_gpio_get_number_pins_in_port(Port_out);
	if (Pin_out >= max_pins) {
		bs_trace_error_time_line("%s: GPIO configuration file attempted to set short from "
				"non existing GPIO pin in port %i (%u>=%u)\n",
				__func__, Port_out, Pin_out, max_pins);
	}
	max_pins = nrf_gpio_get_number_pins_in_port(Port_in);
	if (Pin_in >= max_pins) {
		bs_trace_error_time_line("%s: GPIO configuration file attempted to set short to "
				"non existing GPIO pin in port %i (%u>=%u)\n",
				__func__, Port_in, Pin_in, max_pins);
	}
	shorts[Port_out][Pin_out][i].port = Port_in;
	shorts[Port_out][Pin_out][i].pin  = Pin_in;
}

static int process_config_line(char *s)
{
	unsigned long X,x,Y,y;
	char *endp;
	char *buf = s;
	const char error_msg[] = "%s: Corrupted GPIO configuration file, the valid format is "
			"\"shortcut X.x Y.y\"\nLine was:%s\n";

	if (strncmp(s, "short", 5) == 0){
		buf += 5;
	} else if (strncmp(s, "s", 1) == 0){
		buf += 1;
	} else {
		bs_trace_error_line("%s: Only the command short (or \"s\") is understood at this "
				    "point, Line read \"%s\" instead\n", __func__, s);
	}
	X = strtoul(buf, &endp, 0);
	if ((endp == buf) || (*endp!='.')) {
		bs_trace_error_line(error_msg, __func__, s);
	}
	buf = endp + 1;
	x = strtoul(buf, &endp, 0);
	if ((endp == buf) || (*endp!=' ')){
		bs_trace_error_line(error_msg, __func__, s);
	}
	buf = endp + 1;
	Y = strtoul(buf, &endp, 0);
	if ((endp == buf) || (*endp!='.')) {
		bs_trace_error_line(error_msg, __func__, s);
	}
	buf = endp + 1;
	y = strtoul(buf, &endp, 0);
	if (endp == buf) {
		bs_trace_error_line(error_msg, __func__, s);
	}
	bs_trace_info_time(4, "Short-circuiting GPIO port %li pin %li to GPIO port %li pin %li\n",
			X,x,Y,y);
	nrf_gpio_backend_register_short(X, x, Y, y);
	return 0;
}

/*
 * Load GPIO configuration file (short-circuits)
 */
static void nrf_gpio_load_config(void)
{
	if (gpio_conf_file_path == NULL) {
		return;
	}

	FILE *fileptr = bs_fopen(gpio_conf_file_path, "r");
	char line_buf[MAXLINESIZE];
	int rc;

	while (true) {
		rc = readline(line_buf, MAXLINESIZE, fileptr);
		if (rc == 0) {
			break;
		}
		rc = process_config_line(line_buf);
		if (rc != 0) {
			break;
		}
	}
	fclose(fileptr);
}

/*
 * Process next (valid) line in the input stimuly file, and program
 * the next input update event
 * (or close down the file if it ended or is corrupted)
 */
static void nrf_gpio_input_process_next_time(char *buf)
{
	bs_time_t time;
	unsigned int port;
	unsigned int pin;
	unsigned int level;
	int n;

	n = sscanf(buf, "%"SCNtime",%u,%u,%u", &time, &port, &pin, &level);
	if (n > 0 && n < 4) {
		bs_trace_warning_time_line("File %s seems corrupted. Ignoring rest of file. "
					   "Expected \""
					   "<uin64_t time>,<uint port>,<uint pin>,<uint level>\". "
					   "Line was:%s\n",
					   gpio_in_file_path, buf);
	}
	if (n < 4) { /* End of file, or corrupted => we are done */
		fclose(gpio_input_file_st.input_file_ptr);
		gpio_input_file_st.input_file_ptr = NULL;
		Timer_GPIO_input = TIME_NEVER;
	} else {
		if (time < nsi_hws_get_time()) {
			bs_trace_error_time_line("%s: GPIO input file went back in time(%s)\n",
						__func__, buf);
		}
		if (port >= NRF_GPIOS) {
			bs_trace_error_time_line("%s: GPIO input file attempted to access not "
						"existing GPIO port (%u>=%u) (%s)\n",
						__func__, port, NRF_GPIOS, buf);
		}
		unsigned int max_pins = nrf_gpio_get_number_pins_in_port(port);
		if (pin >= max_pins) {
			bs_trace_error_time_line("%s: GPIO input file attempted to access not "
						"existing GPIO pin in port %i (%u>=%u) (%s)\n",
						__func__, port, pin, max_pins, buf);
		}
		if (level != 0 && level != 1) {
			bs_trace_error_time_line("%s: level can only be 0 (for low) or 1 (for high)"
						"(%u) (%s)\n",
						__func__, level, buf);
		}
		gpio_input_file_st.level = level;
		gpio_input_file_st.pin = pin;
		gpio_input_file_st.port = port;
		Timer_GPIO_input = time;
	}

	nsi_hws_find_next_event();
}

/*
 * Initialize GPIO input from file, and queue next input event change
 */
static void nrf_gpio_init_input_file(void)
{
	gpio_input_file_st.input_file_ptr = NULL;

	if (gpio_in_file_path == NULL) {
		return;
	}

	char line_buf[MAXLINESIZE];
	int read;

	gpio_input_file_st.input_file_ptr = bs_fopen(gpio_in_file_path, "r");

	read = readline(line_buf, MAXLINESIZE, gpio_input_file_st.input_file_ptr);
	if (strncmp(line_buf,"time",4) == 0) { /* Let's skip a possible csv header line */
		read = readline(line_buf, MAXLINESIZE, gpio_input_file_st.input_file_ptr);
	}
	if (read == 0) {
		bs_trace_warning_line("%s: Input file %s seems empty\n",
				      __func__, gpio_in_file_path);
	}

	nrf_gpio_input_process_next_time(line_buf);
}

/*
 * Event timer handler for the GPIO input
 */
static void nrf_gpio_input_event_triggered(void)
{
	char line_buf[MAXLINESIZE];

	nrf_gpio_eval_input(gpio_input_file_st.port, gpio_input_file_st.pin,
			    gpio_input_file_st.level);

	(void)readline(line_buf, MAXLINESIZE, gpio_input_file_st.input_file_ptr);

	nrf_gpio_input_process_next_time(line_buf);
}

NSI_HW_EVENT(Timer_GPIO_input, nrf_gpio_input_event_triggered, 50);
