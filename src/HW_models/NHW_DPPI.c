/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DPPI - Distributed programmable peripheral interconnect
 * https://infocenter.nordicsemi.com/topic/ps_nrf5340/dppi.html?cp=4_0_0_6_8
 *
 * Notes:
 *  * Unlike in real HW, if a set of SUBSCRIBE_CHG[n].EN and SUBSCRIBE_CHG[n].DIS
 *    would be triggered by the same event, and affect the same channels,
 *    the enables would not have priority over the disables.
 *    Instead the channels would be enabled and disabled in order of their group
 *    tasks subscriptions.
 *    (Changing this behavior is possible with some extra implementation effort)
 *
 *  * No SPU controlled security checks are yet implemented:
 *    * Any peripheral (secure or not) may subscribe and publish to any channel
 *      (and their events will not be gated irrespectively of the SPU configuration)
 *    * Channel groups are not bundled as secure/non-secure based
 *      on the individual channels security
 *    * Both secure and non-secure bus accesses can read & modify all configuration
 *      registers (and therefore affect all channels and channel groups)
 *
 *  * At this point, if multiple events are sent to the same DPPI at the same "time"
 *    (same delta cycle) they are not merged. (Changing this behavior is possible:
 *    Future impl note: A list of pended callbacks could be created,
 *    and one delta cycle later it could be gone thru)
 *
 * Implementation spec:
 *   This file provides the implementation of the DPPI peripherals,
 *   and instantiates N of them (and initializes them at startup and frees them on exit),
 *   as described in the configuration (NHW_config.h)
 *
 *   Each DPPI has a configurable number of channels and of channel groups.
 *   Each DPPI is also connected to a DPPI (for task subscription) as per the configuration.
 *
 *   Each DPPI keeps two main structures as its status:
 *    Its register layout NRF_DPPIC_regs[inst]
 *    and its status structure nhw_dppi_st[inst]
 *
 *   To speed up execution and registration simplicity,
 *   peripherals task/subscription ports are *not* permanently "plugged" to the DPPI.
 *   Instead, when a peripheral subscribe register is written, if the subscription is
 *   enabled, the peripheral is dynamically registered into that DPPI channel.
 *   This is done by keeping a list of subscribed callbacks in nhw_dppi_st.registry[channel]
 *   This list will grow dynamically as needed (if many peripherals subscribe to the same channel)
 *
 *   When a peripheral publishes an event to a channel, the DPPI will go thru
 *   all currently subscribed peripherals and call their respective callbacks
 *   (the respective task handlers)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"
#include "NHW_common_types.h"
#include "NHW_config.h"
#include "NHW_DPPI.h"
#include "NHW_peri_types.h"

#define DPPI_ALLOC_CHUNK_SIZE 4
#define SUBSCRIBE_EN_MASK (0x1UL << 31)
#define SUBSCRIBE_CHIDX_MASK (0xFFU)

struct dppi_registry_el {
  dppi_callback_t callback;
  void *param;
};

typedef void (*dppi_callback_noparam_t)(void);

struct dppi_status {
  NRF_DPPIC_Type *NRF_DPPIC_regs;
  uint n_ch;  /* Number of channels configured in this DPPI instance */
  uint n_chg; /* Number of channels' groups configured in this DPPI instance */

  /* Registry of subscribed peripherals to this DPPI channels*/
  struct dppi_registry_el **registry; //[n_ch][reg_size]
  uint *reg_size; //[n_ch] Total allocated size of registry[n_ch]
  uint *reg_used; //[n_ch] Number of used entries in registry[n_ch]

  /* DPPI interface as a "normal peripheral" to a DPPI: */
  uint dppi_map; //To which DPPI instance are this DPPI subscription ports connected to
  //Which of the subscriptions ports are currently connected, and to which channel:
  struct nhw_subsc_mem *CHG_EN_subscribed; //[n_chg]
  struct nhw_subsc_mem *CHG_DIS_subscribed; //[n_chg]

  uint32_t *shadow_CHG; /*[n_chg] Shadowed/internal version of the CHG register to allow
                                  ignoring/reverting disabled writes to CHG */
};

static struct dppi_status nhw_dppi_st[NHW_DPPI_TOTAL_INST];
NRF_DPPIC_Type NRF_DPPIC_regs[NHW_DPPI_TOTAL_INST];

/*
 * Initialize all DPPI instances for this SOC
 */
static void nhw_dppi_init(void) {
  static uint nhw_dppi_dppi_map[NHW_DPPI_TOTAL_INST] = NHW_DPPI_DPPI_MAP;
  static uint nhw_dppi_n_ch[NHW_DPPI_TOTAL_INST] = NHW_DPPI_N_CH;
  static uint nhw_dppi_n_chg[NHW_DPPI_TOTAL_INST] = NHW_DPPI_N_CHG;

  memset(NRF_DPPIC_regs, 0x0, sizeof(NRF_DPPIC_regs));

  for (int i = 0; i < NHW_DPPI_TOTAL_INST; i ++) {
    struct dppi_status *el = &nhw_dppi_st[i];

    el->NRF_DPPIC_regs = &NRF_DPPIC_regs[i];
    el->n_ch = nhw_dppi_n_ch[i];
    el->n_chg = nhw_dppi_n_chg[i];

    int n_ch = nhw_dppi_n_ch[i];

    el->registry = (struct dppi_registry_el **)bs_calloc(n_ch, sizeof(void*));
    el->reg_size = (uint*)bs_calloc(n_ch, sizeof(uint));
    el->reg_used = (uint*)bs_calloc(n_ch, sizeof(uint));
    /*
     * Note that each element of registry starts as NULL,
     * and reg_size as 0, the first time somebody tries to subscribe
     * to a given channel, the registry will be allocated
     */

    int n_chg = nhw_dppi_n_chg[i];

    el->dppi_map = nhw_dppi_dppi_map[i];
    el->CHG_EN_subscribed = (struct nhw_subsc_mem*)bs_calloc(n_chg, sizeof(struct nhw_subsc_mem));
    el->CHG_DIS_subscribed = (struct nhw_subsc_mem*)bs_calloc(n_chg, sizeof(struct nhw_subsc_mem));

    el->shadow_CHG = (uint32_t*)bs_calloc(n_chg, sizeof(uint32_t));
  }
}

NSI_TASK(nhw_dppi_init, HW_INIT, 10);

/*
 * Free all DPPI instances resources before program exit
 */
static void nhw_dppi_free(void)
{
  for (int i = 0; i < NHW_DPPI_TOTAL_INST; i ++) {
    if (nhw_dppi_st[i].registry != NULL) { /* LCOV_EXCL_BR_LINE */
      int n_ch = nhw_dppi_st[i].n_ch;
      for (int n = 0; n < n_ch; n++) {
        if (nhw_dppi_st[i].registry[n] != NULL) { /* LCOV_EXCL_BR_LINE */
          free(nhw_dppi_st[i].registry[n]);
        }
      }
      free(nhw_dppi_st[i].registry);
      nhw_dppi_st[i].registry = NULL;
    }
    free(nhw_dppi_st[i].reg_size);
    nhw_dppi_st[i].reg_size = NULL;

    free(nhw_dppi_st[i].reg_used);
    nhw_dppi_st[i].reg_used = NULL;

    free(nhw_dppi_st[i].CHG_EN_subscribed);
    nhw_dppi_st[i].CHG_EN_subscribed = NULL;

    free(nhw_dppi_st[i].CHG_DIS_subscribed);
    nhw_dppi_st[i].CHG_DIS_subscribed = NULL;

    free(nhw_dppi_st[i].shadow_CHG);
    nhw_dppi_st[i].shadow_CHG = NULL;
  }
}

NSI_TASK(nhw_dppi_free, ON_EXIT_PRE, 100);

static void nhw_dppi_check_inst_valid(unsigned int dppi_inst,
                                      const char* type)
{
  if (dppi_inst >= NHW_DPPI_TOTAL_INST) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Attempted to %s non existent DPPI device (%i>=%i)\n",
                                  __func__, type, dppi_inst, NHW_DPPI_TOTAL_INST);
  } /* LCOV_EXCL_STOP */
}

static void nhw_dppi_check_ch_valid(unsigned int dppi_inst,
                                    unsigned int ch_n,
                                    const char* type)
{
  nhw_dppi_check_inst_valid(dppi_inst, type);

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  if (this->registry == NULL) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Programming error: Attempted to %s DDPI%i "
                             "which is not initialized or already cleaned up\n",
                              __func__, type, dppi_inst);
  }
  if (ch_n >= this->n_ch) {
    bs_trace_error_time_line("%s: Attempted to %s non existent channel (%i>=%i) "
                             "in DDPI%i\n",
                              __func__, type, ch_n, this->n_ch, dppi_inst);
  } /* LCOV_EXCL_STOP */
}

static void nhw_dppi_check_chg_valid(unsigned int dppi_inst,
                                     unsigned int chg_n,
                                     const char* type)
{
  nhw_dppi_check_inst_valid(dppi_inst, type);

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];

  if (this->registry == NULL) { /* LCOV_EXCL_START */
    bs_trace_error_time_line("%s: Programming error: Attempted to %s DDPI%i "
                             "which is not initialized or already cleaned up\n",
                              __func__, type, dppi_inst);
  }
  if (chg_n >= this->n_chg) {
    bs_trace_error_time_line("%s: Attempted to %s non existent channel group "
                             " (%i>=%i) in DDPI%i\n",
                              __func__, type, chg_n, this->n_chg, dppi_inst);
  } /* LCOV_EXCL_STOP */
}

/*
 * Subscribe a peripheral to a DPPI channel
 *
 * dppi_inst   DPPI instance
 * channel_nbr The channel in that DPPI
 * callback    The function which will be called when the DPPI event is received
 * param       An arbitrary void pointer, which will be passed to the callback
 *
 * Note if param is DPPI_CB_NO_PARAM, the callback will be called with the signature
 * void (*dppi_callback_t)(void)
 *
 * Note: The pair {callback, param} is used to identify a registration
 *       Therefore 2 registrations may never use the same pair (it is an error to do so)
 */
void nhw_dppi_channel_subscribe(unsigned int dppi_inst,
                                unsigned int ch_n,
                                dppi_callback_t callback,
                                void *param)
{
  nhw_dppi_check_ch_valid(dppi_inst, ch_n, "subscribe to");

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  struct dppi_registry_el *ch_reg = this->registry[ch_n];

  for (int i = 0; i < this->reg_used[ch_n]; i++) {
    if ((ch_reg[i].callback == callback) /* LCOV_EXCL_START */
        && (ch_reg[i].param == param)) {
      bs_trace_error_time_line("%s: Programming error: Attempted to subscribe "
                               "twice to DDPI%i ch %i\n",
                                __func__, dppi_inst, ch_n);
    } /* LCOV_EXCL_STOP */
  }

  if (this->reg_used[ch_n] >= this->reg_size[ch_n]) {
    this->reg_size[ch_n] += DPPI_ALLOC_CHUNK_SIZE;
    this->registry[ch_n] = bs_realloc(this->registry[ch_n],
                                      this->reg_size[ch_n] * sizeof(struct dppi_registry_el));
  }
  int n = this->reg_used[ch_n];
  this->registry[ch_n][n].callback = callback;
  this->registry[ch_n][n].param = param;
  this->reg_used[ch_n]++;
}

static inline void nhw_dppi_shift_registration(struct dppi_registry_el *ch_reg,
                                               int off,
                                               int n)
{
  for (int i = off+1; i < n; i++) {
    ch_reg[i-1].callback = ch_reg[i].callback;
    ch_reg[i-1].param = ch_reg[i].param;
  }
  ch_reg[n-1].callback = NULL;
  ch_reg[n-1].param = NULL;
}

/*
 * Un-subscribe a peripheral from a DPPI channel
 *
 * dppi_inst           DPPI instance
 * channel_nbr         The channel in that DPPI into which we unsubscribe
 * callback and param: The parameters which were used during the registration
 *
 * Note: The pair {callback, param} are used to identify a registration
 *       Therefore 2 registrations may never use the same pair (it is an error to do so)
 */
void nhw_dppi_channel_unsubscribe(unsigned int dppi_inst,
                                  unsigned int ch_n,
                                  dppi_callback_t callback,
                                  void *param)
{
  nhw_dppi_check_ch_valid(dppi_inst, ch_n, "unsubscribe from");

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  struct dppi_registry_el *ch_reg = this->registry[ch_n];

  for (int i = 0; i < this->reg_used[ch_n]; i++) {
    if ((ch_reg[i].callback == callback)
        && (ch_reg[i].param == param)) {
      nhw_dppi_shift_registration(ch_reg, i, this->reg_used[ch_n]);
      this->reg_used[ch_n]--;
      return;
    }
  }
  bs_trace_error_time_line("%s: Programming error: Attempted to unsubscribe but "
                           "previous subscription not found in DDPI%i ch%i\n",
                           __func__, dppi_inst, ch_n);
}

/*
 * Signal an event to a channel from a peripheral
 * This will cause all peripherals which subscribed to that channel
 * to get their callbacks called.
 *
 * dppi_inst DPPI instance
 * ch_n      The channel in which the event is being published
 */
void nhw_dppi_event_signal(uint dppi_inst, uint ch_n)
{
  nhw_dppi_check_ch_valid(dppi_inst, ch_n, "send event to");
  struct dppi_status *this = &nhw_dppi_st[dppi_inst];

  if ((this->NRF_DPPIC_regs->CHEN & ((uint32_t)0x1 << ch_n)) == 0) {
    return;
  }

  struct dppi_registry_el *ch_reg = this->registry[ch_n];

  for (int i = 0; i < this->reg_used[ch_n]; i++) {
    if (ch_reg[i].callback) { /* LCOV_EXCL_BR_LINE */
      if (ch_reg[i].param != (void*)DPPI_CB_NO_PARAM) {
        ch_reg[i].callback(ch_reg[i].param);
      } else {
        ((dppi_callback_noparam_t)ch_reg[i].callback)();
      }
    } else { /* LCOV_EXCL_START */
      bs_trace_error_time_line("%s: Programming error: Hole in "
                               " DPPI %i registry, ch%i i=%i\n",
                               __func__, dppi_inst, ch_n, i);
    } /* LCOV_EXCL_STOP */
  }
}

static void nhw_dppi_task_chg_en(uint dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "triggered TASK_CHGn_EN in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  regs->CHEN |= regs->CHG[n];
  regs->CHENSET = regs->CHEN;
}

static void nhw_dppi_task_chg_dis(uint dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "triggered TASK_CHGn_EN in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  regs->CHEN &= ~regs->CHG[n];
  regs->CHENSET = regs->CHEN;
}

void nhw_dppi_regw_sideeffects_TASK_CHGn_EN(uint dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "write to TASK_CHGn_EN register in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  if (regs->TASKS_CHG[n].EN) { /* LCOV_EXCL_BR_LINE */
    regs->TASKS_CHG[n].EN = 0;
    nhw_dppi_task_chg_en(dppi_inst, n);
  }
}

void nhw_dppi_regw_sideeffects_TASK_CHGn_DIS(uint dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "write to TASK_CHGn_DIS register in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  if (regs->TASKS_CHG[n].DIS) { /* LCOV_EXCL_BR_LINE */
    regs->TASKS_CHG[n].DIS = 0;
    nhw_dppi_task_chg_dis(dppi_inst, n);
  }
}

static void nhw_dppi_taskwrap_chg_en(void* param) {
  unsigned int dppi_inst = (uintptr_t)param >> 16;
  uint n = (uintptr_t)param & 0xFFFF;
  nhw_dppi_task_chg_en(dppi_inst, n);
}

static void nhw_dppi_taskwrap_chg_dis(void* param) {
  unsigned int dppi_inst = (uintptr_t)param >> 16;
  uint n = (uintptr_t)param & 0xFFFF;
  nhw_dppi_task_chg_dis(dppi_inst, n);
}

void nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_EN(unsigned int dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "write to SUBSCRIBE_CHG_EN register in");

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       regs->SUBSCRIBE_CHG[n].EN,
                                       &this->CHG_EN_subscribed[n],
                                       nhw_dppi_taskwrap_chg_en,
                                       (void*)((dppi_inst << 16) + n));
}

void nhw_dppi_regw_sideeffects_SUBSCRIBE_CHG_DIS(unsigned int dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "write to SUBSCRIBE_CHG_DIS register in");

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  nhw_dppi_common_subscribe_sideeffect(this->dppi_map,
                                       regs->SUBSCRIBE_CHG[n].DIS,
                                       &this->CHG_DIS_subscribed[n],
                                       nhw_dppi_taskwrap_chg_dis,
                                       (void*)((dppi_inst << 16) + n));
}

/**
 * Update DPPI CHEN mask after a write to CHENSET
 */
void nhw_dppi_regw_sideeffects_CHENSET(uint dppi_inst)
{
  nhw_dppi_check_inst_valid(dppi_inst, "access CHENSET register in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  if (regs->CHENSET != 0) { /* LCOV_EXCL_BR_LINE */
    regs->CHEN |= regs->CHENSET;
    regs->CHENSET = regs->CHEN;
  }
}

/**
 * Update DPPI CHEN mask after a write to CHENCLR
 */
void nhw_dppi_regw_sideeffects_CHENCLR(uint dppi_inst)
{
  nhw_dppi_check_inst_valid(dppi_inst, "access CHENCLR register in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  if (regs->CHENCLR != 0) { /* LCOV_EXCL_BR_LINE */
    regs->CHEN &= ~regs->CHENCLR;
    regs->CHENCLR = 0;
  }
}

void nhw_dppi_regw_sideeffects_CHEN(uint dppi_inst)
{
  nhw_dppi_check_inst_valid(dppi_inst, "access CHENSET register in");

  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;
  regs->CHENSET = regs->CHEN;
}

void nhw_dppi_regw_sideeffects_CHGn(uint dppi_inst, uint n)
{
  nhw_dppi_check_chg_valid(dppi_inst, n, "access CHENSET register in");

  struct dppi_status *this = &nhw_dppi_st[dppi_inst];
  NRF_DPPIC_Type *regs = nhw_dppi_st[dppi_inst].NRF_DPPIC_regs;

  /*
   * As per the spec:
   * Writes to this register are ignored if either SUBSCRIBE_CHG[n].EN
   * or SUBSCRIBE_CHG[n].DIS is enabled
   */
  if ((regs->SUBSCRIBE_CHG[n].EN & SUBSCRIBE_EN_MASK)
      || (regs->SUBSCRIBE_CHG[n].DIS & SUBSCRIBE_EN_MASK)) {
    bs_trace_warning_time_line("%s: Ignoring write to DPPI%i CHG%i while one of "
                               "its subscriptions is enabled (as per the spec)\n",
                               __func__, dppi_inst, n);
    regs->CHG[n] = this->shadow_CHG[n];
    return;
  }

  this->shadow_CHG[n] = regs->CHG[n];
}

/*
 * NOTE: This is not a DPPI function per se, but a common function
 * for all peripherals to handle an event publishing
 *
 * If the publish_register is enabled,
 * call nhw_dppi_event_signal() for the configured CHIDX
 */
void nhw_dppi_event_signal_if(uint dppi_inst, uint32_t publish_reg) {
  uint chidx;

  if ((publish_reg & SUBSCRIBE_EN_MASK) == 0){
    return;
  }

  chidx = publish_reg & SUBSCRIBE_CHIDX_MASK;

  nhw_dppi_event_signal(dppi_inst, chidx);
}

/*
 * NOTE: This is not a DPPI function per se, but a common function
 * for all peripherals to handle the side-effects of any write to a SUBSCRIBE register
 *
 * dppi_inst      Which DPPI the peripheral is connected to
 * SUBSCRIBE_reg  Value of the SUBSCRIBE_<EVENT> register written by SW
 * last           Pointer to an static storage in the peripheral, which keeps
 *                the status of the subscription (is/wast it subscribed,
 *                and to which channel)
 *
 * callback & param: Parameters for nhw_dppi_channel_subscribe()
 *
 */
void nhw_dppi_common_subscribe_sideeffect(unsigned int dppi_inst,
                                          uint32_t SUBSCRIBE_reg,
                                          struct nhw_subsc_mem *last,
                                          dppi_callback_t callback,
                                          void *param)
{
  bool new_is_subs = SUBSCRIBE_reg & SUBSCRIBE_EN_MASK;
  unsigned int new_channel = SUBSCRIBE_reg & SUBSCRIBE_CHIDX_MASK;

  if ((last->is_subscribed == new_is_subs)
    && (last->subscribed_ch == new_channel)) {
    //Nothing has changed
    return;
  }

  if (last->is_subscribed == true) {
    nhw_dppi_channel_unsubscribe(dppi_inst,
                                 last->subscribed_ch,
                                 callback,
                                 param);
  }
  last->is_subscribed = new_is_subs;
  last->subscribed_ch = new_channel;
  if (new_is_subs) {
    nhw_dppi_channel_subscribe(dppi_inst,
                               new_channel,
                               callback,
                               param);
  }
}
