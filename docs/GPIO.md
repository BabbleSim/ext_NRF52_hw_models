## GPIO models

A model of the GPIO peripherals (both P0 and P1 ports) is included with these models.

You can find information about their limitations and approximations in
[the source files](../src/HW_models/NRF_GPIO.c).

Note that as of today these models:
* Consider the output drivers to have 0 impedance: The drive is instantaneous.
* Do not allow outputs drivers to be in a tristate mode (an output can be disconnected, but not
 not-driven)
* Do not allow several outputs to be shortcircuited together to form logical OR's or AND's
* Do not model input pulls.

That is, inputs just sample the input pin, and outputs just deliver a logical level: high or low.

### Driving inputs:

Inputs can be driven in 3 different ways:

* From an input stimuli file (`-gpio_in_file=<path>`). Which follows the stimuli format
  described below
* From test code, calling `nrf_gpio_test_change_pin_level()`
* By shortcuiting the input and output, either set from a configuration file
  (`-gpio_conf_file=<path>`, see below), or
  programmatically, by calling `nrf_gpio_backend_register_short()`

### Output files:

It is possible to have the GPIOs output activity dumped into a file.
For this just call the excutable with `-gpio_out_file=<path>`.
Any toggle in any pin configured as an output will be dumped to that file, following the
stimuli file format described below.

### Monitor inputs/outputs from test code:

Embedded test code specific for simulation can monitor the inputs and outputs changes
by registering a callback with `nrf_gpio_test_register_in_callback()` and
`nrf_gpio_test_register_out_callback()`.
With the first one, the callback will be called each time an *input gpio pin register* is modified.
With the second one, each time the output pin itself changes. That is, both will be called only
if the pin is connected/driven in that direction.

### Stimuli file format

This is a comma separated file (.csv), with 4 columns: time,port,pin,level. Where:
* Time is a 64 bit unsigned value, representing time in microseconds since boot.
* Port is the GPIO port number starting from 0 (for a nrf52833: 0 or 1).
* Pin is the pin number in that port (for a nrf52833: 0..31 for port 0, and 0..9 for port 1)
* Level is either 0 (for low) or 1 (for high)

For example:

```
time(microsecond),port,pin,level
0,0,0,1
200,0,0,0
600,0,0,1
800,0,0,0
1000,0,0,1
1000,0,0,0
101624,0,0,1
```

Where pin 0 in port 0, is toggled at boot, 200microseconds, 600microseconds, 800microseconds, 1ms
(up and immediately down), and 101.624ms.

### Configuration file format

The configuration file can define output->input short-circuits.

Note that these shorts only work one way: output to input.
It is not possible to short 2 outputs together.
It is possible to short one output to several (up to 8) inputs.

The configuration file is made of lines, each with the following format, either:

`short out_port.out_pin in_port.in_pin`<br>
`s out_port.out_pin in_port.in_pin`

The first word must always be either "short" or "s"
And it must always be followed by 4 unsigned integers in 2 pairs.
Each pair separated by a dot. The first value of each pair being the port number,
the second value the pin in that port.
The first pair indicating the output pin that will be shorted from, and the 2nd pair
the input pin that will be shorted to.

For example:

```
short 0.1 1.2
short 0.1 1.3
```
To short pin 1 from port 0, to both pins 2 and 3 from port 1,<br>

```
short 1.0 0.2
```
to short pin 0 from port 1 to pin 2 from port 2.

Note that both ports and pins are numbered from 0.

### Comments in input files

For both stimuli input and configuration files, '#' is treated as a comment delimiter: That
character and anything after until the end of the line will be ignored.

Empty lines, are also ignored.
