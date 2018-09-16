Models of some of the HW present in a NRF52xxx.
Where relevant differences exist, these models try to align with a NRF52382.

These models are accurate enough to allow the current Zephyr to run and
its BLE stack to function. For more details please see the notes on the source
files for each peripheral model.

These models are based only on the public SOC specifications:
http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fchips%2Fnrf52832.html&cp=2_1

They have been developed without any other knowledge and probably contain
inacuracies, and defects.


About the structure of the HW models:
#####################################

The actual HW models of the SOC peripherals are split in one file per peripheral.
The files are named NRF_<PERIPHERAL>.{c|h}.

Mostly all these models have the following functions:

* Interface: * 

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

* Internal/static: *

signal_<event register name>() : Signal that <event> has just happened

The tasks, registers and event names should match the register interface
specified in the linked documentation.
