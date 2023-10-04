/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "nrfx.h"
#include "hal/nrf_aar.h"
#include "hal/nrf_clock.h"
#include "hal/nrf_ccm.h"
#include "hal/nrf_ecb.h"
#include "hal/nrf_egu.h"
#include "hal/nrf_gpiote.h"
#include "hal/nrf_radio.h"
#include "hal/nrf_rng.h"
#include "hal/nrf_rtc.h"
#include "hal/nrf_temp.h"
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
 * Given a pointer to a subscribe register in an unknown peripheral
 * get
 * A pointer to the HAL subscribe set and clear functions that correspond to it
 * The task offset corresponding to that subscribe register
 */
static void nrf_dppi_hack_get_subscribe_from_ptr(void *sub_reg, void **p_reg,
                                           subscribe_set_f *set_f, subscribe_clear_f *clear_f,
                                           int *task) {

#define IF_PER(per, nbr, post, lname)               \
  if (IS_PERIPHERAL_REG(sub_reg, per, nbr, post)) { \
    *p_reg = PERIPHERAL_REG_BASE(per, nbr, post);   \
    *task = (intptr_t)sub_reg - (intptr_t)*p_reg - 0x80/*Offset between tasks and subscribe registers*/; \
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
  /*} else IF_PER(TEMP, , _NS, temp)*/ //Lacking definition in official nrf HAL
  } else IF_PER(RTC, 0, _NS, rtc)
  /* 18 IPC */
  /* 19 SERIAL0 */
  } else IF_PER(EGU, 0, _NS, egu)
  } else IF_PER(RTC, 1, _NS, rtc)
  } else IF_PER(TIMER, 1, _NS, timer)
  } else IF_PER(TIMER, 2, _NS, timer)
  } else {
    bs_trace_error_time_line("Tried to look for a subscribe register not known to these HW models\n");
    return; /* unreachable */
  }
#elif defined(NRF5340_XXAA_APPLICATION)
  bs_trace_error_time_line("%s not yet implemented for NRF5340_XXAA_APPLICATION\n", __func__);
#endif

#undef IF_PER
}

void nrf_dppi_hack_subscribe_set(void *sub_reg, unsigned int channel)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;

  nrf_dppi_hack_get_subscribe_from_ptr(sub_reg, &p_reg, &set_f, &clear_f, &task);
  set_f(p_reg, task, channel);
}

void nrf_dppi_hack_subscribe_clear(void *sub_reg)
{
  void *p_reg;
  int task;
  subscribe_set_f set_f;
  subscribe_clear_f clear_f;

  nrf_dppi_hack_get_subscribe_from_ptr(sub_reg, &p_reg, &set_f, &clear_f, &task);
  clear_f(p_reg, task);
}
