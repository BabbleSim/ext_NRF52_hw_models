## UART(E) models

A model of the UART and UART-E peripherals is included with these models.

You can find information about their limitations and approximations in
[the source files](../src/HW_models/NHW_UART.c).

The UART models are divided in 2 parts:
* A peripheral model, which emulates both the nRF UART and UART-E.
* A backend which can send and receive the data somewhere.

When using the peripheral you select which backend is used for each UART instance
with command line parameters.
If no backend is enabled, that UART instance is disconnected: whatever is sent is dropped, and
nothing is received.

### Logging to files

Each UART can be configured to log to a file the Tx and/or Rx bytes.
Enabling this and selecting the file is done with command line options.
When enabled, the model will dump one line in this file per byte, with two columns:
The first column is a timestamp, in microseconds, when the frame *ended*;
The second column is the byte itself.

### Test API

The UART also has a test API which allows embedded code to register callbacks to be
called whenever a byte is transmitted or received. This callback can replace the byte before
the UART peripheral processes it further. Check
[the UART header file](../src/HW_models/NHW_UART.h) for more info.
It is also possible for test code to call `nhw_UARTE_digest_Rx_byte()` to inject
bytes into the UART peripheral like if they arrived thru the Rx line.

### Backends:

Today there are 2 backends to choose from:
 * The loopback backend
 * The FIFO backend

#### The Loopback backend

This backend just connects an instance Tx to its Rx, including the RTS and CTS signals.
While having minimal overhead.

#### The FIFO backend

This backend can connect two instances of these UART models to each other.
For this it uses one Linux FIFO (named pipe) for Tx and another for Rx.
In these pipes it will carry both the data, information about the CTS/RTS pin toggles,
bit rate, frame configuration, and other control messages.
Therefore you cannot connect this backend to a console, or real devices.

You enable this backend for each instance using command line parameters, by specifying
the path to each FIFO.

The backend will automatically create and delete the FIFOs on its own.

Remember to use unique paths for each simulation you run in parallel.

The backend will also realize about a possible abrupt disconnect from the other side.
In that case, it will terminate the simulation.
In case of a graceful disconnect from the other side, the backend will, by default, also
terminate the simulation; But you can change this with a command line option, so that it will
just disconnect the backend and continue running.

For performance reasons, the backend does not react immediately to a CTS/RTS pin toggle from the
other side. Instead up to 1 frame time (1 byte time) will elapse between the pin toggle
and the UART peripheral model being notified. Note that the UART peripheral has a 6 byte Rx FIFO,
and that it toggles RTS while there are still 4 spaces left. So even though this will, in some cases,
cause up to 1 extra byte to be sent to the other side, it should not cause any transmission losses.

If there is a miss-configuration between the devices (bit rate, parity or number of stop bits),
the backend will print a warning for each received byte, but the ERRORSRC register in the UART
peripheral won't be set due to fame, or parity errors, or break conditions.

#### Loopback

It is possible to connect a UART instance Tx directly to its Rx (or to another instance Rx),
and have the RTR propagated to the CTS.
To do this, just configure the same FIFO file name for both the Rx and Tx, for example like:
`-uart0_fifob_rxfile=looped_back -uart0_fifob_txfile=looped_back`
Note that you can also use the loopback backend when connecting a single instance in loopback,
and have the same result with lower overhead, and no files created on disk.

**IMPOTANT**:
  Do not connect both devices which are connected thru the UART FIFO backend to the Physical layer
  simulation simultaneously. Connect only the one which has the BLE/15.4 controller.
  Otherwise, with the current implementation the simulation will deadlock with very high
  likelihood, and if it does not deadlock it will slow down the simulation considerably.
  You can still provide the sim_id and an unused device number to the other device, but
  in that case, launch it with the `-nosim` option.
