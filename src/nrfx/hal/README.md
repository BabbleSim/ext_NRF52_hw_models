## nRFx HAL

The HAL source files provided with these models implement
some modifications to the HAL interface [real nRFx HAL](https://github.com/NordicSemiconductor/nrfx/)
needed due to the HW models peculiarities.
Its main purpose is to allow unmodified Zephyr code to run directly
using this repository HW models.

There is two main differences in this HAL compared to the real one:

* The peripherals base addresses point to the HW models register IF
  instead of to hardcoded HW addresses.<br>
  For example `NRF_PPI_BASE` is defined as `(&NRF_PPI_regs)`, where
  `NRF_PPI_regs` is the HW models structure containing the PPI model
  registers

* When a HW register with sideeffects is written, the appropriate
  HW model function is called to trigger that sideeffect.<br><br>
  A sideeffect should be understood as some activity beyond the modification
  of the register itself. For example, in real HW, writting a `1` to
  `NRF_RNG->TASKS_START` will start the random number generation.
  When using the HW models, doing this write in itself would only change
  that memory location content. To trigger the actual sideeffect the
  function `nrf_rng_regw_sideeffects_TASK_START()` needs to be called.
  This replacement HAL functions take care of that.

The original implementation of the HAL APIs from the real nrfx is included in `nrf_hal_originals.c`.
All these functions are labelled as weak, so they can be replaced as needed with new definition in `src/nrfx/hal/`
This removes the need for duplicating code.

Note that this HAL version should be, wherever possible, a copy of the
official nRFx HAL, with only minor differences due to what is described above.
And normally most differences between the official version and this one will be
simply due to this not having been updated yet.

Note that, so far, this replacement HAL only provides a small subset of the
whole nRFx API: Mostly the functions and types used by the current Zephyr
BLE controller and necessary drivers, and only for the peripherals which are
part of these models.
It is certainly possible to expand it to cover the whole API.
