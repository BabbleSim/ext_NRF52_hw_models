For general information about these models refer to [README.md](README.md)

You will only need to continue reading if you are curious about how these
models are built, or if you want to use them for some other purpose than
with Zephyr's nrf5*_bsim.

## Requirements

The purpose of these models is to provide a good enoug HW model for BLE
protocol simulations, which will focus on the SW, to be able to debug it,
and run long simulations fast.<br>
We are not interested in modelling the particularities of the HW when
they are not relevant for the SW execution.
Therefore many details can be simplified or omited all together.

The focus of these models is on the BLE and 15.4 stacks, and therefore
mostly the peripherals which are necessary for its proper function have
been modelled so far.

Regarding the time accuracy, these models will focus mostly on the radio
activity accuracy. With other peripherals being overall more rough. Note that
as all models are based on public specifications, all peripherals timings
could, anyhow, not be better than what is described in those.<br>
Overall these models have a time granularity of 1us.

## Implementation specification

Overall these models are "event driven". There is several ways
of building these kind of models, but for simplicity, these models are built
using a very nimble engine provided by the native simulator, the "hw scheduler".

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
  * Whenever that time is reached, the HW scheduler will call a function in that
    model tasked with continuing executing that task/process.

In this model, all of these events|timers types and their callbacks are known
in compile time.
Meaning, there is no dynamic registration of events types.<br>
Normally each peripheral model will have 1 of such event timers, and it will
be up to the peripheral model to schedule several subevents using only that
one timer and callback if needed.

Whenever a HW model updates its event timer, it will call a function in the HW scheduler.

The overall HW scheduler provided by the native_simulator, will advance simulated time
when needed, and call into the corresponding HW submodule "event|task runner"
whenever its event time is reached.

Note that several HW submodules may be scheduled to run in the same microsecond.
In this case, they will be handled in different "delta cycles" in that same microsecond.
Each timer|event has a given priority, and therefore will always be called
in the same order relative to other HW events which may be schedule in the
same microsecond.<br>
Note also, that any HW submodule may schedule a new event to be called in the
same microsecond in which it is running. This can be done for any purpose,
like for example to deffer a sideeffect of writing to a register from a SW
thread into the HW models thread.
When they do so, their "event|task runner" will be called right after in the
next delta cycle.

### The SW registers IF

Each peripheral model which has HW registers accessible by SW, presents
a structure which matches those registers' layout.
This structure will be allocated somewhere in the process memory, but certainly
not in the same address as in the real HW.
Therefore any access to the real registers must be, in someway, corrected
to access this structure instead.
In Zephyr's nrf5*_bsim case, this is achieved by providing a version of the
macros which, in real HW point to the peripherals base addresses, which points
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
this is achieved by calling `nhw_<periperal>_regw_sideeffects_<register name>()`
after the write itself.
In Zephyr's nrf5*_bsim case, this is done in the replacement nRFx HAL function.

### HW interrupts

For a HW model to raise an interrupt all it will need to do is call into the
interrupt controller model functions
`hw_irq_ctrl_raise/lower_level_irq_line(<cpu_nbr>, <irq_number>)`.

The interrupt controller will update its status, and if the interrupt was not
masked, one delta cycle later, awake the CPU by calling the corresponding
`nsif_cpun_irq_raised(<cpu_nbr>)`.

In Zephyr's nrf5*_bsim `nsif_cpun_irq_raised(<cpu_nbr>)` is provided by the Zephyr
board code.

### Structure of the HW models:

The actual HW models of the SOC peripherals are split in one file per peripheral.
The files are named `NHW_<PERIPHERAL>.{c|h}`.

Mostly all these models have the following functions:

#### Interface:

These models use:

 * The native simulator HW scheduler to register the events timers and callbacks
 * The native simulator "tasks" interface, to register their initialization and cleanup functions
   to be called during the HW initialization and program exit.

Overall, they follow a pattern where each peripheral has these types of functions:

```
nhw_<periperal>_init()            : To initialize the model
nhw_<periperal>_cleanup()         : To free any resources used by the model
nhw_<periperal>_<TASK name>()     : Perform the actions triggered by <TASK>
nhw_<periperal>_regw_sideeffects_<register name>()
                                  : Trigger any possible sideeffect from writing
                                    to that regiter
Timer_<peripheral> &
nhw_<periperal>_timer_triggered() : Models which take time to perform their work
                                    Use a registered event. When that event timer
                                    is reached, this function is called to perform
                                    any neccessary step, including update that
                                    event timer.
```
#### Internal:
```
signal_<event register name>() : Signal that <event> has just happened,
                                 handle shortcuts, and raise interrupts.

```
The tasks, registers and event names should match the register interface
specified in the linked documentation.

## Integrating these models in another system

This subsection provides information you would need if you try to use
these models without Zephyr's nrf5*_bsim wrapping logic.

### Models interface towards a simulation scheduler

As described before, overall the models are "event driven":
They rely on an overall scheduler triggering (calling) them in
the appropriate times.

By default this overall scheduler is provided by the native simulator
HW scheduler.

The models register their events, their timers, callbacks and priorities with
`NSI_HW_EVENT(timer, callback, priority)`.
When two events times coincide, the one with the highest priority should be run first,
and when two have the same priority, it does not matter which is run first, while
they are run in the same order consistently.

The events timers represent, in microseconds, when the models need to perform
the next action.

It is the responsability of that overall scheduler to ensure the HW models
are called whenever their time is reached (and not later).

These event timers may be changed after each execution of the models, or whenever
a HW register is written.
When the models change this timer, they will call `nsi_hws_find_next_event()` to
notify that overall scheduler of the change.

The models are initalized by calling their registered initialization functions
(`NSI_TASK(*, HW_INIT, *);`)
Similarly, on program exit, the models cleanup functions registered
with `NSI_TASK(*, ON_EXIT_*, *);` should be called, to free any system resources.

Some of these models will also require being called very early during the process
execution to register command line arguments.
These are registered with `NSI_TASK(*, PRE_BOOT_1, *);`

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
(this second case is how it is done in Zephyr's nrf5*_bsim)

### Command line interface arguments

These models register their own command line arguments/options using
babblesim's command line argument utilities.
The integration program should support this.

The way to describe the command line arguments follows Babblesim's
`libUtilv1` command line parsing convention.

You can check Zephyr's nrf5*_bsim wrapping code for an insight on how
you can use these component.
