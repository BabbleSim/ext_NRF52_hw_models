/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx.h"
#if defined(NRF5340_XXAA_NETWORK)
#include "hal/nrf_aar.h"
#include "hal/nrf_ccm.h"
#include "hal/nrf_ecb.h"
#include "hal/nrf_radio.h"
#include "hal/nrf_rng.h"
#include "hal/nrf_temp.h"
#elif defined(NRF5340_XXAA_APPLICATION)
#endif
#include "hal/nrf_clock.h"
#include "hal/nrf_dppi.h"
#include "hal/nrf_egu.h"
#include "hal/nrf_gpiote.h"
#include "hal/nrf_ipc.h"
#include "hal/nrf_rtc.h"
#include "hal/nrf_timer.h"
#include "bs_tracing.h"

#define PERIPHERAL_REG_BASE(per, nbr, post) \
    (void*)NRF_##per##nbr##post##_BASE

#define IS_PERIPHERAL_REG(p, per, nbr, post) \
    (p >= PERIPHERAL_REG_BASE(per, nbr, post)) && \
    ((intptr_t)p < (intptr_t)PERIPHERAL_REG_BASE(per, nbr, post) + sizeof(NRF_##per##_Type))

typedef void (*subscribe_set_f)(void *, int , uint8_t);
typedef void (*subscribe_clear_f)(void *, int);

/*
 * Given a peripheral task/event (task_event) value (A value of a nrf_<peri>_task_t or nrf_<peri>_event_t)
 * return true if it is a task, or false if it is an event
 */
static bool nrf_dppi_hack_is_task(int task_event, void *p_reg) {
  /*
   * In the NRF peripherals:
   *  [0x000..0x080) are tasks
   *  [0x080..0x100) are subscribe registers
   *  [0x100..0x180) are events
   *  [0x180..0x200) are publish registers
   */
  if (task_event < 0x100) {
    return true;
  } else {
    return false;
  }
}

/*
 * Given a pointer to a task or event register in an unknown peripheral
 * Get
 *  **p_reg: A pointer to the register base address
 *  *set_f/clear_f: pointers to the HAL subscribe set and clear functions that correspond to that peripheral
 * The task/event *offset* corresponding to that subscribe register (input to most nrf HAL functions)
 */
static void nrf_dppi_hack_get_task_from_ptr(void *task_reg,
                                            void **p_reg,
                                           subscribe_set_f *set_f, subscribe_clear_f *clear_f,
                                           int *task) {

#define IF_PER(per, nbr, post, lname)                            \
  if (IS_PERIPHERAL_REG(task_reg, per, nbr, post)) {             \
    *p_reg = PERIPHERAL_REG_BASE(per, nbr, post);                \
    *task = (intptr_t)task_reg - (intptr_t)*p_reg;               \
    *set_f = (subscribe_set_f)nrf_##lname##_subscribe_set;       \
    *clear_f = (subscribe_clear_f)nrf_##lname##_subscribe_clear; \
    return;

#if defined(NRF5340_XXAA_NETWORK)
  /*IF_PER(POWER, , _NS, power)
  } else*/ IF_PER(CLOCK, , _NS, clock)
  } else IF_PER(RADIO, , _NS, radio)
  } else IF_PER(RNG, , _NS, rng)
  } else IF_PER(GPIOTE, , _NS, gpiote)
  /* 11 WDT */
  } else IF_PER(TIMER, 0, _NS, timer)
  } else IF_PER(ECB, , _NS, ecb)
  } else IF_PER(AAR, , _NS, aar)
  } else IF_PER(CCM, , _NS, ccm)
  } else IF_PER(DPPIC, , _NS, dppi)
  /*} else IF_PER(TEMP, , _NS, temp)*/ //Lacking definition in official nrf HAL
  } else IF_PER(RTC, 0, _NS, rtc)
  } else IF_PER(IPC, , _NS, ipc)
  /* 19 SERIAL0 */
  } else IF_PER(EGU, 0, _NS, egu)
  } else IF_PER(RTC, 1, _NS, rtc)
  } else IF_PER(TIMER, 1, _NS, timer)
  } else IF_PER(TIMER, 2, _NS, timer)
  } else {
    bs_trace_error_time_line("Tried to look for a task register not known to these HW models\n");
    return; /* unreachable */
  }
#elif defined(NRF5340_XXAA_APPLICATION)
  /*IF_PER(DCNF, , _NS, dcnf)
  } else IF_PER(FPU, , _NS, fpu)
  } else IF_PER(CACHE, , _NS, cache)
  } else IF_PER(SPU, , _NS, spu)
  } else IF_PER(OSCILLARTORS, , _NS, oscillators)
  } else IF_PER(REGULATORS, , _NS, regulators)
  } else */ IF_PER(CLOCK, , _NS, clock)
  /*} else IF_PER(POWER, , _NS, power)
  } else IF_PER(RESET, , _NS, reset)
  } else IF_PER(CTRLAP, , _NS, ctrlap)
  } else IF_PER(SPIM, 0, _NS, spi)
  } else IF_PER(SPIM, 1, _NS, spi)
  } else IF_PER(SPIM, 4, _NS, spi)
  } else IF_PER(SPIM, 2, _NS, spi)
  } else IF_PER(SPIM, 3, _NS, spi) */
  } else IF_PER(GPIOTE, 0, _S, gpiote)
  /*} else IF_PER(SAADC, , _NS, saadc)*/
  } else IF_PER(TIMER, 0, _NS, timer)
  } else IF_PER(TIMER, 1, _NS, timer)
  } else IF_PER(TIMER, 2, _NS, timer)
  } else IF_PER(RTC, 0, _NS, rtc)
  } else IF_PER(RTC, 1, _NS, rtc)
  } else IF_PER(DPPIC, , _NS, dppi)
  /*} else IF_PER(WDT0, , _NS, dppi)
  } else IF_PER(WDT1, , _NS, dppi)
  } else IF_PER(COMP, , _NS, dppi)*/
  } else IF_PER(EGU, 0, _NS, egu)
  } else IF_PER(EGU, 1, _NS, egu)
  } else IF_PER(EGU, 2, _NS, egu)
  } else IF_PER(EGU, 3, _NS, egu)
  } else IF_PER(EGU, 4, _NS, egu)
  } else IF_PER(EGU, 5, _NS, egu)
  /*} else IF_PER(PWM0, , _NS, dppi)
  } else IF_PER(PWM1, , _NS, dppi)
  } else IF_PER(PWM2, , _NS, dppi)
  } else IF_PER(PWM3, , _NS, dppi)
  } else IF_PER(PDM0, , _NS, dppi)
  } else IF_PER(I2S0, , _NS, dppi)*/
  } else IF_PER(IPC, , _NS, ipc)
  /* QSPI, NFCT */
  } else {
    bs_trace_error_time_line("Tried to look for a task register not known to these HW models\n");
    return; /* unreachable */
  }
#endif

#undef IF_PER
}

void nrf_dppi_hack_subscribe_set(void *sub_reg, unsigned int channel)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;

  intptr_t task_reg_pr = (intptr_t)sub_reg - NRF_SUBSCRIBE_PUBLISH_OFFSET((intptr_t)sub_reg);

  nrf_dppi_hack_get_task_from_ptr((void *)task_reg_pr, &p_reg, &set_f, &clear_f, &task);
  if (nrf_dppi_hack_is_task(task, p_reg)) {
    set_f(p_reg, task, channel);
  }
}

void nrf_dppi_hack_subscribe_clear(void *sub_reg)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;

  intptr_t task_reg_pr = (intptr_t)sub_reg - NRF_SUBSCRIBE_PUBLISH_OFFSET((intptr_t)sub_reg);

  nrf_dppi_hack_get_task_from_ptr((void *)task_reg_pr, &p_reg, &set_f, &clear_f, &task);
  if (nrf_dppi_hack_is_task(task, p_reg)) {
    clear_f(p_reg, task);
  }
}
