/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Note that the function prototypes are taken from the NRFx HAL
 */
#include "hal/nrf_grtc.h"
#include "bs_tracing.h"
#include "NHW_GRTC.h"

void nrf_grtc_sys_counter_cc_set(NRF_GRTC_Type * p_reg,
                                 uint8_t         cc_channel,
                                 uint64_t        cc_value)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    uint32_t cc_h = (uint32_t)(cc_value >> 32);
    NRFX_ASSERT(cc_h <= NRF_GRTC_SYSCOUNTER_CCH_MASK);

    p_reg->CC[cc_channel].CCL = (uint32_t)cc_value;
    nhw_GRTC_regw_sideeffects_CC_CCL(0, cc_channel);
    p_reg->CC[cc_channel].CCH = cc_h & NRF_GRTC_SYSCOUNTER_CCH_MASK;
    nhw_GRTC_regw_sideeffects_CC_CCH(0, cc_channel);
}

void nrf_grtc_sys_counter_cc_add_set(NRF_GRTC_Type *             p_reg,
                                     uint8_t                     cc_channel,
                                     uint32_t                    value,
                                     nrf_grtc_cc_add_reference_t reference)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    NRFX_ASSERT(value <= NRF_GRTC_SYSCOUNTER_CCADD_MASK);

    p_reg->CC[cc_channel].CCADD = ((uint32_t)reference << GRTC_CC_CCADD_REFERENCE_Pos) |
                               (value & NRF_GRTC_SYSCOUNTER_CCADD_MASK);
    nhw_GRTC_regw_sideeffects_CC_CCADD(0, cc_channel);
}

void nrf_grtc_int_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->GRTC_INTENSET = mask;
    nhw_GRTC_regw_sideeffects_INTENSET(0, GRTC_IRQ_GROUP);
}

void nrf_grtc_int_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->GRTC_INTENCLR = mask;
    nhw_GRTC_regw_sideeffects_INTENCLR(0, GRTC_IRQ_GROUP);
}

void nrf_grtc_subscribe_set(NRF_GRTC_Type * p_reg,
                           nrf_grtc_task_t task,
                           uint8_t        channel)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT((task != NRF_GRTC_TASK_START) &&
                (task != NRF_GRTC_TASK_CLEAR) &&
                (task != NRF_GRTC_TASK_STOP));
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);

    int task_nbr = (task - NRF_GRTC_TASK_CAPTURE_0)/sizeof(uint32_t);

    nhw_GRTC_regw_sideeffects_SUBSCRIBE_CAPTURE(0, task_nbr);
}

void nrf_grtc_subscribe_clear(NRF_GRTC_Type * p_reg,
                             nrf_grtc_task_t task)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT((task != NRF_GRTC_TASK_START) &&
                (task != NRF_GRTC_TASK_CLEAR) &&
                (task != NRF_GRTC_TASK_STOP));
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;

    int task_nbr = (task - NRF_GRTC_TASK_CAPTURE_0)/sizeof(uint32_t);

    nhw_GRTC_regw_sideeffects_SUBSCRIBE_CAPTURE(0, task_nbr);
}

void nrf_grtc_event_clear(NRF_GRTC_Type * p_reg, nrf_grtc_event_t event)
{
  NRFX_ASSERT(event != NRF_GRTC_EVENT_SYSCOUNTERVALID);

  *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;

  nhw_GRTC_regw_sideeffects_EVENTS_all(0);
}


uint32_t nrf_grtc_sys_counter_low_get(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return nhw_GRTC_regr_sideeffects_SYSCOUNTERL(0, NRF_GRTC_DOMAIN_INDEX);
#else
#error "Not supported"
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY

}

uint32_t nrf_grtc_sys_counter_high_get(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return nhw_GRTC_regr_sideeffects_SYSCOUNTERH(0, NRF_GRTC_DOMAIN_INDEX);
#else
#error "Not supported"
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY
}

uint64_t nrf_grtc_sys_counter_get(NRF_GRTC_Type const * p_reg)
{
    nhw_GRTC_regr_sideeffects_SYSCOUNTERL(0, NRF_GRTC_DOMAIN_INDEX);
    nhw_GRTC_regr_sideeffects_SYSCOUNTERH(0, NRF_GRTC_DOMAIN_INDEX);
    return *((const uint64_t volatile *)&p_reg->GRTC_SYSCOUNTER.SYSCOUNTERL);
}

bool nrf_grtc_sys_counter_overflow_check(NRF_GRTC_Type const * p_reg)
{
    nhw_GRTC_regr_sideeffects_SYSCOUNTERH(0, NRF_GRTC_DOMAIN_INDEX);
    return (p_reg->GRTC_SYSCOUNTER.SYSCOUNTERH &
            GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk) ? true : false;
}

void nrf_grtc_task_trigger(NRF_GRTC_Type * p_reg, nrf_grtc_task_t grtc_task)
{
    uint inst = 0;
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)grtc_task)) = 0x1UL;

    if ((grtc_task >= NRF_GRTC_TASK_CAPTURE_0) && (grtc_task < NRF_GRTC_TASK_START)) {
      int task_nbr = (grtc_task - NRF_GRTC_TASK_CAPTURE_0)/sizeof(uint32_t);
      nhw_GRTC_regw_sideeffects_TASKS_CAPTURE(inst, task_nbr);
    } else if (grtc_task == NRF_GRTC_TASK_START) {
      nhw_GRTC_regw_sideeffects_TASKS_START(inst);
    } else if (grtc_task == NRF_GRTC_TASK_STOP) {
      nhw_GRTC_regw_sideeffects_TASKS_STOP(inst);
    } else if (grtc_task == NRF_GRTC_TASK_CLEAR) {
      nhw_GRTC_regw_sideeffects_TASKS_CLEAR(inst);
#if NRF_GRTC_HAS_PWM
    } else if (grtc_task == NRF_GRTC_TASK_PWM_START) {
       nhw_GRTC_regw_sideeffects_TASKS_PWMSTART(inst);
    } else if (grtc_task == NRF_GRTC_TASK_PWM_STOP) {
       nhw_GRTC_regw_sideeffects_TASKS_PWMSTOP(inst);
#endif
    } else {
      bs_trace_error_time_line("GRTC task %i not known\n", grtc_task);
    }
}

void nrf_grtc_sys_counter_compare_event_enable(NRF_GRTC_Type * p_reg,
                                                                 uint8_t         cc_channel)
{

#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    p_reg->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Enable;
    nhw_GRTC_regw_sideeffects_CC_CCEN(0, cc_channel);
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_compare_event_disable(NRF_GRTC_Type * p_reg,
                                                                  uint8_t         cc_channel)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    p_reg->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
    nhw_GRTC_regw_sideeffects_CC_CCEN(0, cc_channel);
}
