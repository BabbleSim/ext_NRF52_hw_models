For general information about these models refer to [README.md](README.md)

You will only need to continue reading if you are curious about how these
models are built, or if you want to use them for some other purpose than
with the nrf52_bsim.

## Requirements

The purpose of these models is to provide a good enoug HW model for BLE
protocol simulations, which will focus on the SW, to be able to debug it,
and run long simulations fast.<br>
We are not interested in modelling the particularities of the HW when
they are not relevant for the SW execution.
Therefore many details can be simplified or omited all together.

The focus of these models is on the Zephyr BLE stack, and therefore
only the peripherals which are necessary for its proper function have
been modelled so far.

Regarding the time accuracy, these models will focus mostly on the radio
activity accuracy. With other peripherals being overall more rough. Note that
as all models are based on public specifications, all peripherals timings
could, anyhow, not be better than what is described in those.<br>
Overall these models have a time granularity of 1us.

## Implementation specification

Overall these models are "event driven". There is several ways
of building these kind of models, but for simplicity, speed, and to not need to
rely on any kind of library or 3rd party engine, these models are built with a
very nimble engine provided partly by the models themselves, and partly by the
nrf52_bsim "time machine".<br>
The reason for this division is due to the nrf52_bsim being
the overall scheduler, which initializes both models and "CPU", and is in
charge of when each should run.<br>

### About the event scheduling

In reality any action performed by a HW peripheral will take some amount of
time.<br>
For our purposes any HW process which takes too short for the SW to
realize, will be modelled as being instantaneous in simulation time.
Such processes will just be implemented as a C function (or a set of them),
which will change the models status as needed.

Other processes do take a considerable amount of time, like for example sending
a radio packet, or generating a random number.<br>
Such processes will be modelled in a bit more complex way:

  * The process model may use one or several "events"/timers.
  * When needed these timers will be set at a point in the future where some
    action needs to be performed.
  * Whenever that time is reached, an scheduler will call a function in that
    model tasked with continuing executing that task/process.

In this model, all of these events|timers types and their callbacks are known
in design/compile time.
Meaning, there is no dynamic registration of events types.<br>
Normally each peripheral model will have 1 of such event timers, and it will
be up to the peripheral model to schedule several subevents using only that
one timer and callback if needed.

The HW models top level, `NRF_HW_model_top`, collects all these events and
exposes only 1 timer (with 1 callback) to the overall scheduler provided by
the nrf52_bsim.

Whenever a HW model updates its timer, it will call a function in the top level
to update that top level timer if needed.

The overall scheduler provided by the nrf52_bsim, will advance simulated time
when needed, and call into the top level HW models "event|task runner"
(`nrf_hw_some_timer_reached()`) whenever its timer time is reached.
This will in turn call whichever HW submodule task function was scheduled for
this moment.

Note that several HW submodules may be scheduled to run in the same µs.
In this case, they will be handled in different "delta cycles" in that same µs.
, that is, in different consecutive calls to `nrf_hw_some_timer_reached()`.
Each timer|event has a given priority, and therefore will always be called
in the same order relative to other HW events which may be schedule in the
same µs.<br>
Note also, that any HW submodule may schedule a new event to be called in the
same µs in which it is running. This can be done for any purpose,
like for example to deffer a sideeffect of writing to a register from a SW
thread into the HW models thread.

### The SW registers IF

Each perihperal model which has HW registers accessible by SW, presents
a structure which matches those registers' layout.
This structure will be allocated somewhere in the process memory, but certainly
not in the same address as in the real HW.
Therefore any access to the real registers must be, in someway, corrected
to access this structure instead.
In Zephyr's nrf52_bsim case, this is achieved by providing a version of the
macros which, in real point to the peripherals base addresses, which points
to these structures.

Writing to this structure in itself will only cause that memory location to be
changed. For many registers this is perfectly fine, as that is just the same
that happens in the real HW (a register is changed, which later may be
read by the actual HW).<br>
But for some registers, accesing them (writing and/or reading them) causes some
sideeffect, that is, something else to happen in the HW.
For example, in real HW, writting a `1` to
`NRF_RNG->TASKS_START` will start the random number generation.

For these type of registers with sideeffects, the HW models must be triggered,
this is achieved by calling `nrf_<periperal>_regw_sideeffects_<register name>()`
after the write itself.
In the nrf52_bsim case, this is done in the replacement nRFx HAL function.

### HW interrupts

For a HW model to raise an interrupt all it will need to do is call into the
interrupt controller model function `hw_irq_ctrl_set_irq(<irq_nbr>)`.

The interrupt controller will update its status, and if the interrupt was not
masked, one delta cycle later, awake the CPU by calling
`posix_interrupt_raised()`.

In the nrf52_bsim `posix_interrupt_raised()` is provided by the Zephyr
POSIX arch `inf_clock`.

### Structure of the HW models:

The actual HW models of the SOC peripherals are split in one file per peripheral.
The files are named `NRF_<PERIPHERAL>.{c|h}`.

Mostly all these models have the following functions:

#### Interface:
```
nrf_<periperal>_init()            : Initialize the model
nrf_<periperal>_cleanup()         : Free any memory used by the model
nrf_<periperal>_<TASK name>()     : Perform the actions triggered by <TASK>
nrf_<periperal>_regw_sideeffects_<register name>()
                                  : Trigger any possible sideeffect from writing
                                    to that regiter
Timer_<peripheral> &
nrf_<periperal>_timer_triggered() : Models which take time to perform their work
                                    Use a registered timer. When that timer is
                                    reached, this function is called to perform
                                    any neccessary step, including update that
                                    timer.
```
#### Internal/static:
```
signal_<event register name>() : Signal that <event> has just happened

```
The tasks, registers and event names should match the register interface
specified in the linked documentation.


## Integrating these models in another system

This subsection provides information you would need if you try to use
these models without the nrf52_bsim wrapping logic.

### Models interface towards a simulation scheduler

As described before, overall the models are "event driven":
They rely on an overall scheduler triggering (calling) them in
the appropriate times.

In the case of Zephyr's nrf52_bsim, this overall scheduler is provided by
the nrf52_bsim "time machine". The interface the models expect from this
is described in [`time_machine_if.h`](../src/HW_models/time_machine_if.h)

The top level [`NRF_HW_model_top.c`](../src/HW_models/NRF_HW_model_top.c)
exposes one event timer: `nrf_hw_next_timer_to_trigger`.
This timer represents, in microseconds, when the models need to perform
the next action.

It is the responsability of that overall scheduler to ensure the HW models
top level scheduling function (`nrf_hw_some_timer_reached()`) is called
whenever that time is reached.

This timer may be changed after each execution of the models, or whenever
a HW register is written.
When the models change this timer, they will call
`tm_find_next_timer_to_trigger()` to notify that overall scheduler of the
change.

The models are initalized by calling first `nrf_hw_pre_init()`.
Later `nrf_hw_initialize()` shall be called with the command line selected
options.<br>
No other HW model function can be called before these two.<br>
`nrf_hw_models_free_all()` shall be called to clean up after the simulation
is done: to deallocate and close any resource the models may be using.

### Models interface towards a CPU model:

For details about the SW register IF please see check the
"The SW registers IF" section above.
For details about how interrupts are raised see "HW interrupts" above.

### Thread safety

This HW models API is NOT thread safe: This means a call to one of the
HW models functions cannot be done if another thread of the same process
is currently also calling another of these functions.
Meaning, only one function may be called at a time.<br>
This is not going to be a problem if only one thread calls into the HW models.
It won't be a problem either if by any other synchronization mechanism it is
ensured only one thread calls into these HW models at a time.
(this second case is how it is done in the nrf52_bsim)

### Command line intercace arguments

These models command line arguments/options are described in
[`NRF_hw_args.h`](../src/HW_models/NRF_hw_args.h)

The way to describe the command line arguments follows Babblesim's
`libUtilv1` command line parsing convention.

You can check the nrf52_bsim wrapping code for an insight on how
you can set them.

