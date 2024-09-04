/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * AAR - Accelerated address resolver
 * ECB - AES electronic codebook mode encryption
 * CCM - AES CCM mode encryption
 *
 * Notes:
 *  * [AAR1][CCM1] The AAR and CCM peripherals share register addressing, but the layout is not
 *    coherent between both.
 *    For the spec, it is quite unclear how the event and interrupt logic is glued/shared.
 *    (Note that, for ex., END and ERROR do not match in the INTEN bit, and have different, overlapping,
 *     PUBLISH registers in AAR and CCM mode. Therefore, for ex., there can't be just a single
 *     signal for both END events from the RTL logic towards the event/interrupt generation)
 *    In real HW, it is understood that if INTEN or PUBLISH_* is left with a configuration and the other
 *    mode is enabled spurious interrupts/events can be generated.
 *
 *    The model keeps a common register set, but separate interrupt and event logic.
 *    The spec tells that SW should ensure the configuration is cleared in one side before using
 *    the other. So SW which does this should not see a difference.
 *
 *  * [AAR2]: It is unclear what the real AAR peripheral will do if TASK_START is triggered while the
 *    AAR is already running. The model will do nothing but warn.
 *
 *  * [AAR3][CCM3]: It is unclear how the AAR & CCM peripherals will behave if ENABLE is changed while
 *    either is operating. The model will just warn.
 *
 *  * [AAR4]: For EVENTS_ERROR, the spec says "Operation aborted because of a STOP task or due to an error"
 *    But it is unclear if a STOP will always cause the EVENT, even if the block was not running.
 *    The model does only generate a warning in this later case, and only generated a EVENTS_ERROR if the
 *    block was indeed stopped with a STOP task.
 *
 *  * [ECB1]: How long t_ECB is, is just a guess at this point (1 micros)
 *
 *  * [ECB2]: About TASK_START the spec states that
 *    "If a crypto operation is already running in the AES core, the START task will
 *    not start a new encryption and an ERROR event will be triggered"
 *    It is unclear if ERRORSTATUS is changed though, and if there is a difference
 *    between, say, it being busy by another ECB operation or an AAR/CCM operation.
 *    The model does not modify ERRORSTATUS if another ECB was running,
 *    but sets it to 3(Aborted) if the AAR or CCM were.
 *
 *  * [ECB3]: About TASK_STOP: The spec states:
 *    "If a running ECB operation is aborted by STOP, the ERROR event is triggered."
 *    It is unclear if ERRORSTATUS is changed though. The model does not.
 *    And similarly to [AAR4], the event ERROR EVENT is only generated if it was running and stopped by the task.
 *
 *  * [CCM1]: The CCM model (unlike the real HW) is instantaneous.
 *    During encryption the RADIO model needs the whole packet at the preamble start,
 *    so we cannot have the model be "as slow" as the real HW.
 *    During decryption the HW is always running in Fast mode, so being a bit faster
 *    in the model should ideally not matter
 *
 *  * [CCM3]: TASK_RATEOVERRIDE & RATEOVERRIDE & DATARATE do not have any real function (see CCM1)
 *  * [CCM3b]: ERRORSTATUS EncryptionTooSlow cannot happen
 *
 *  * [CCM4]: Unlike the 52/53 models, the decryption is triggered by the START task, and not
 *    by the radio models (The decryption is meant to be started by the radio PAYLOAD EVENT,
 *    and run fast, unlike in the 52/3 where it was meant to run slowly as the bytes were
 *    received)
 *
 *  * [CCM5]: At this point only BLE protocol is supported
 *    (and therefore an AAD length/l_a of 1 byte, and MIC length of 4 bytes)
 *
 *  * [CCM6]: It is unclear if for the real HW, during decryption, when l_c < MACLEN (or < 5 for BLE)
 *    if apart from an ERROR event being generated something else happens (for ex. if ERRORSTATUS
 *    is changed)
 *
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "bs_types.h"
#include "NHW_config.h"
#include "NHW_templates.h"
#include "NHW_peri_types.h"
#include "NHW_54_AAR_CCM_ECB.h"
#include "NHW_DPPI.h"
#include "NHW_EVDMA.h"
#include "HW_utils.h"
#include "nsi_hw_scheduler.h"
#include "irq_ctrl.h"
#include "bs_tracing.h"
#include "BLECrypt_if.h"
#include "nsi_tasks.h"
#include "nsi_hws_models_if.h"

static bs_time_t Timer_AAR_CCM_ECB = TIME_NEVER;

union NRF_AARCCM_Type NRF_AARCCM_regs[NHW_AARCCMECB_TOTAL_INST];

NRF_AAR_Type *NRF_AAR_regs[NHW_AARCCMECB_TOTAL_INST];
NRF_CCM_Type *NRF_CCM_regs[NHW_AARCCMECB_TOTAL_INST];
NRF_ECB_Type NRF_ECB_regs[NHW_AARCCMECB_TOTAL_INST];

struct aar_status {
  NRF_AAR_Type *NRF_AAR_regs;
  uint32_t INTEN;

  bool Running;
  bs_time_t Timer;
  uint clockMHz;
};
struct ccm_status {
  NRF_CCM_Type *NRF_CCM_regs;
  uint32_t INTEN;
  bool Running;
};
struct ecb_status {
  NRF_ECB_Type *NRF_ECB_regs;
  uint32_t INTEN;
  bool Running;
  bs_time_t Timer;
  uint t_ECB;
};

static struct aar_status nhw_aar_st[NHW_AARCCMECB_TOTAL_INST];
static struct ccm_status nhw_ccm_st[NHW_AARCCMECB_TOTAL_INST];
static struct ecb_status nhw_ecb_st[NHW_AARCCMECB_TOTAL_INST];
/* Mapping of peripheral instance to DPPI instance */
#define nhw_AAR_dppi_map nhw_AARCCMECB_dppi_map
#define nhw_CCM_dppi_map nhw_AARCCMECB_dppi_map
#define nhw_ECB_dppi_map nhw_AARCCMECB_dppi_map
static uint nhw_AARCCMECB_dppi_map[NHW_AARCCMECB_TOTAL_INST] = NHW_AARCCMECB_DPPI_MAP;

void nhw_aes_ecb_cheat_reset_t_ecb(void);
static bool nhw_ECB_possible_abort(uint inst);
static void nhw_AAR_TASK_START(uint inst);
static void nhw_AAR_TASK_STOP(uint inst);
static void nhw_CCM_TASK_START(uint inst);
static void nhw_CCM_TASK_STOP(uint inst);

static void nhw_AARCCMECB_init(void) {
  memset(&NRF_AARCCM_regs, 0, sizeof(NRF_AARCCM_regs));
  memset(&NRF_ECB_regs, 0, sizeof(NRF_ECB_regs));
  uint clockMHz[NHW_AARCCMECB_TOTAL_INST] = NHW_AARCCMECB_CLOCK;

  for (int i = 0; i < NHW_AARCCMECB_TOTAL_INST; i++) {
    NRF_AAR_regs[i] = (NRF_AAR_Type *)&NRF_AARCCM_regs[i];
    NRF_CCM_regs[i] = (NRF_CCM_Type *)&NRF_AARCCM_regs[i];

    nhw_aar_st[i].NRF_AAR_regs = NRF_AAR_regs[i];
    nhw_aar_st[i].INTEN = 0;
    nhw_aar_st[i].Timer = TIME_NEVER;
    nhw_aar_st[i].Running = false;
    nhw_aar_st[i].clockMHz = clockMHz[i];

    nhw_ccm_st[i].NRF_CCM_regs = NRF_CCM_regs[i];
    nhw_ccm_st[i].INTEN = 0;
    nhw_ccm_st[i].Running = false;

    nhw_ecb_st[i].NRF_ECB_regs = &NRF_ECB_regs[i];
    nhw_ecb_st[i].INTEN = 0;
    nhw_ecb_st[i].Running = false;
  }
  nhw_aes_ecb_cheat_reset_t_ecb();
  Timer_AAR_CCM_ECB = TIME_NEVER;
}

NSI_TASK(nhw_AARCCMECB_init, HW_INIT, 100);

static void update_master_timer(void) {
  Timer_AAR_CCM_ECB = TIME_NEVER;
  for (int i = 0; i < NHW_AARCCMECB_TOTAL_INST; i++) {
    if (nhw_aar_st[i].Running) {
      if (nhw_aar_st[i].Timer < Timer_AAR_CCM_ECB) {
        Timer_AAR_CCM_ECB = nhw_aar_st[i].Timer;
      }
    }
    if (nhw_ecb_st[i].Running) {
      if (nhw_ecb_st[i].Timer < Timer_AAR_CCM_ECB) {
        Timer_AAR_CCM_ECB = nhw_ecb_st[i].Timer;
      }
    }
  }
  nsi_hws_find_next_event();
}

static bool aarccm_int_line[NHW_AARCCMECB_TOTAL_INST]; /* Is the AAR/CCM currently driving its interrupt line high */
static struct nhw_irq_mapping nhw_aarccm_irq_map[NHW_AARCCMECB_TOTAL_INST] = NHW_AARCCM_INTMAP;

static void nhw_AAR_eval_interrupt(uint inst) {
  bool new_int_line = false;

  uint32_t INTEN = nhw_aar_st[inst].INTEN;

  //Side note: Bit mapping in the INTEN register differs depending on mode
  if (NRF_AAR_regs[inst]->ENABLE == AAR_ENABLE_ENABLE_Enabled) {
    NHW_CHECK_INTERRUPT(AAR, NRF_AAR_regs[inst]->, END,         INTEN)
    NHW_CHECK_INTERRUPT(AAR, NRF_AAR_regs[inst]->, RESOLVED,    INTEN)
    NHW_CHECK_INTERRUPT(AAR, NRF_AAR_regs[inst]->, NOTRESOLVED, INTEN)
    NHW_CHECK_INTERRUPT(AAR, NRF_AAR_regs[inst]->, ERROR,       INTEN)
  } else {
    bs_trace_info_time_line(3,"%s called with AAR not enabled (%i)\n", __func__, NRF_AAR_regs[inst]->ENABLE);
  }

  hw_irq_ctrl_toggle_level_irq_line_if(&aarccm_int_line[inst],
                                       new_int_line,
                                       &nhw_aarccm_irq_map[inst]);
}

static void nhw_CCM_eval_interrupt(uint inst) {
  bool new_int_line = false;

  uint32_t INTEN = nhw_ccm_st[inst].INTEN;

  if (NRF_CCM_regs[inst]->ENABLE == CCM_ENABLE_ENABLE_Enabled) {
    NHW_CHECK_INTERRUPT(CCM, NRF_CCM_regs[inst]->, END,   INTEN)
    NHW_CHECK_INTERRUPT(CCM, NRF_CCM_regs[inst]->, ERROR, INTEN)
  } else {
    bs_trace_info_time_line(3,"%s called with CCM not enabled (%i)\n", __func__, NRF_CCM_regs[inst]->ENABLE);
  }

  hw_irq_ctrl_toggle_level_irq_line_if(&aarccm_int_line[inst],
                                       new_int_line,
                                       &nhw_aarccm_irq_map[inst]);
}

static void nhw_ECB_eval_interrupt(uint inst) {
  static struct nhw_irq_mapping nhw_ecb_irq_map[NHW_AARCCMECB_TOTAL_INST] = NHW_ECB_INTMAP;
  static bool ecb_int_line[NHW_AARCCMECB_TOTAL_INST]; /* Is the ECB currently driving its interrupt line high */
  bool new_int_line = false;

  uint32_t INTEN = nhw_ecb_st[inst].INTEN;

  NHW_CHECK_INTERRUPT(ECB, NRF_ECB_regs[inst]., END,   INTEN)
  NHW_CHECK_INTERRUPT(ECB, NRF_ECB_regs[inst]., ERROR, INTEN)

  hw_irq_ctrl_toggle_level_irq_line_if(&ecb_int_line[inst],
                                       new_int_line,
                                       &nhw_ecb_irq_map[inst]);
}


#define NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE_WRAP(TASK_N, PERI)                  \
  static void nhw_##PERI##_task##TASK_N##_wrap(void* param)                         \
  {                                                                                 \
    nhw_##PERI##_TASK_##TASK_N((int) param);                                        \
  }

#define NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N, PERI, peri)                 \
  void nhw_##PERI##_regw_sideeffects_SUBSCRIBE_##TASK_N(uint inst)                  \
  {                                                                                 \
     struct peri##_status *st = &nhw_##peri##_st[inst];                             \
     static struct nhw_subsc_mem task##_subscribed[NHW_AARCCMECB_TOTAL_INST];       \
                                                                                    \
     nhw_dppi_common_subscribe_sideeffect(nhw_AARCCMECB_dppi_map[inst],             \
                                          st->NRF_##PERI##_regs->SUBSCRIBE_##TASK_N,\
                                          &task##_subscribed[inst],                 \
                                          nhw_##PERI##_task##TASK_N##_wrap,         \
                                          (void*) inst);                            \
  }


static NHW_SIGNAL_EVENT(AAR, NRF_AAR_regs[inst]->, END)
static NHW_SIGNAL_EVENT(AAR, NRF_AAR_regs[inst]->, RESOLVED)
static NHW_SIGNAL_EVENT(AAR, NRF_AAR_regs[inst]->, NOTRESOLVED)
static NHW_SIGNAL_EVENT(AAR, NRF_AAR_regs[inst]->, ERROR)

static void nhw_AAR_stop(uint inst) {
  struct aar_status *st = &nhw_aar_st[inst];
  st->Running = false;
  st->Timer = TIME_NEVER;
  update_master_timer();
}

static void nhw_AAR_resolve_logic(uint inst) {
  NRF_AAR_regs[inst]->ERRORSTATUS = 0;
  NRF_AAR_regs[inst]->OUT.AMOUNT = 0;

  EVDMA_status_t in_evdma, out_evdma;
  int ret;
  size_t n_access;
  uint32_t hash = 0, prand = 0, hash_check;
  uint8_t irk[16];
  uint n_resolved = 0;
  uint n_iter = 0;
  uint8_t prand_buf[16], hash_check_buf[16];

#define MAX_IRKS 4095

#define IF_NOT_READ_ERROR(ret) \
  if (ret < 0) { \
    NRF_AAR_regs[inst]->ERRORSTATUS = AAR_ERRORSTATUS_ERRORSTATUS_PrematureInptrEnd; \
    nhw_AAR_stop(inst); \
    nhw_AAR_signal_EVENTS_ERROR(inst); \
    return; \
  }

  ret = nhw_EVDMA_start(&in_evdma, (void *)NRF_AAR_regs[inst]->IN.PTR);
  if (ret) {
    bs_trace_error_time_line("AAR IN.PTR is NULL during start, that would have hard faulted\n");
  }
  ret = nhw_EVDMA_start(&out_evdma, (void *)NRF_AAR_regs[inst]->OUT.PTR);
  if (ret) {
    bs_trace_error_time_line("AAR OUT.PTR is NULL, that would have hard faulted\n");
  }

  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         (uint8_t *)&hash, 3,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);

  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         (uint8_t *)&prand, 3,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);

  bs_trace_raw_time(9,"HW AAR address from packet to match %02x:%02x:%02x:%02x:%02x:%02x\n",
      (prand >> 16), (prand >> 8) & 0xFF, prand & 0xFF,
      (hash >> 16), (hash >> 8) & 0xFF, hash & 0xFF);

  if (prand >> 22 != 0x01) {
    /* Not a resolvable private address */
    bs_trace_raw_time(7,"HW AAR the address is not resolvable we proceed anyhow (0x%06X , %x)\n", prand, prand >> 22);
  }

  memset(prand_buf,0,16);
  /* Endiannes reversal to bigendian */
  prand_buf[15] = prand & 0xFF;
  prand_buf[14] = (prand >> 8) & 0xFF;
  prand_buf[13] = (prand >> 16) & 0xFF;

  while ((n_resolved < NRF_AAR_regs[inst]->MAXRESOLVED) && (n_iter <= MAX_IRKS)) {
    ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                           irk, 16,
                           &n_access, (n_iter == 0)?NHW_EVDMA_NEWJOB:NHW_EVDMA_CONTINUEJOB);
    if (ret == -2) { //End of job list => End of IRK list
      break;
    }
    else IF_NOT_READ_ERROR(ret);

    /* this aes_128 function takes and produces big endian results */
    BLECrypt_if_aes_128((const uint8_t *)&irk, prand_buf, hash_check_buf);

    /* Endianess reversal to little endian */
    hash_check = hash_check_buf[15] | (uint32_t)hash_check_buf[14] << 8 | (uint32_t)hash_check_buf[13] << 16;

    bs_trace_raw_time(9, "HW AAR (%i): checking prand = 0x%06X, hash = 0x%06X, hashcheck = 0x%06X\n",
                      n_iter, prand, hash, hash_check);

    if (hash == hash_check) {
      bs_trace_raw_time(7, "HW AAR matched irk %i\n", n_iter);
      n_resolved++;
      NRF_AAR_regs[inst]->OUT.AMOUNT = 2*n_resolved;
      ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                             (uint8_t *)&n_iter, 2,
                             &n_access, NHW_EVDMA_CONTINUEJOB);
      if (ret < 0) {
        NRF_AAR_regs[inst]->ERRORSTATUS = AAR_ERRORSTATUS_ERRORSTATUS_PrematureOutptrEnd;
        nhw_AAR_stop(inst);
        nhw_AAR_signal_EVENTS_ERROR(inst);
        return;
      }
    }
    n_iter++;
  }
#undef IF_NOT_READ_ERROR

  //Very rough approximation of duration ceil( (20 + (8 + 20)*n_iter + 11*N_matches cc)/Clock_in_MHz ) micros
  uint clockMHz = nhw_aar_st[inst].clockMHz;
  bs_time_t t_duration = (20 + (8 + 20)*n_iter + 11*n_resolved + clockMHz - 1)/clockMHz;

  nhw_aar_st[inst].Timer = nsi_hws_get_time() + t_duration;
  update_master_timer();
}

static void nhw_AAR_TASK_START_inner(uint inst) {
  if (nhw_aar_st[inst].Running) {
    bs_trace_warning_time_line("%s called while AAR was already Running enabled => ignored\n", __func__);
    //Note [AAR2]: It is unclear what the real AAR peripheral will do in this case
    return;
  }
  nhw_ECB_possible_abort(inst);

  nhw_aar_st[inst].Running = true;
  nhw_AAR_resolve_logic(inst);
}

static void nhw_AAR_TASK_STOP_inner(uint inst) {
  int was_running = nhw_aar_st[inst].Running;
  nhw_AAR_stop(inst);
  //Note [AAR4]
  if (was_running) {
    nhw_AAR_signal_EVENTS_ERROR(inst);
  } else {
    bs_trace_info_time_line(3, "%s called while AAR was not running\n", __func__);
  }
}

void nhw_AARCCM_regw_sideeffects_ENABLE(unsigned int inst) {
  //Note [AAR3][CCM3]
  if ((nhw_aar_st[inst].Running) && (NRF_AAR_regs[inst]->ENABLE != AAR_ENABLE_ENABLE_Enabled)) {
    bs_trace_warning_time_line("AAR/CCM%i ENABLE register modified while AAR was running\n", inst);
  }
  if ((nhw_ccm_st[inst].Running) && (NRF_AAR_regs[inst]->ENABLE != CCM_ENABLE_ENABLE_Enabled)) {
    bs_trace_warning_time_line("AAR/CCM%i ENABLE register modified while CCM was running\n", inst);
  }
}

NHW_SIDEEFFECTS_INTSET(AAR, NRF_AAR_regs[inst]->, nhw_aar_st[inst].INTEN)
NHW_SIDEEFFECTS_INTCLR(AAR, NRF_AAR_regs[inst]->, nhw_aar_st[inst].INTEN)

NHW_SIDEEFFECTS_TASKS(AAR, NRF_AAR_regs[inst]->, START)
NHW_SIDEEFFECTS_TASKS(AAR, NRF_AAR_regs[inst]->, STOP)

#define NHW_AAR_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                   \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE_WRAP(TASK_N, AAR)                         \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N, AAR, aar)

NHW_AAR_REGW_SIDEFFECTS_SUBSCRIBE(START)
NHW_AAR_REGW_SIDEFFECTS_SUBSCRIBE(STOP)

NHW_SIDEEFFECTS_EVENTS(AAR)

static void nhw_AAR_timer_triggered(uint inst) {
  nhw_AAR_stop(inst);

  if (NRF_AAR_regs[inst]->ENABLE != AAR_ENABLE_ENABLE_Enabled) {
    bs_trace_warning_time_line("Programming error\n");
  }

  if (NRF_AAR_regs[inst]->OUT.AMOUNT > 0) {
    nhw_AAR_signal_EVENTS_RESOLVED(inst);
  } else {
    nhw_AAR_signal_EVENTS_NOTRESOLVED(inst);
  }
  nhw_AAR_signal_EVENTS_END(inst);
}

/* CCM */

NHW_SIDEEFFECTS_INTSET(CCM, NRF_CCM_regs[inst]->, nhw_ccm_st[inst].INTEN)
NHW_SIDEEFFECTS_INTCLR(CCM, NRF_CCM_regs[inst]->, nhw_ccm_st[inst].INTEN)

static NHW_SIGNAL_EVENT(CCM, NRF_CCM_regs[inst]-> , END)
static NHW_SIGNAL_EVENT(CCM, NRF_CCM_regs[inst]-> , ERROR)

static int nhw_CCM_MAC_size(uint inst) {
  switch ((NRF_CCM_regs[inst]->MODE & CCM_MODE_MACLEN_Msk) >> CCM_MODE_MACLEN_Pos) {
  case CCM_MODE_MACLEN_M0: return 0;
  case CCM_MODE_MACLEN_M4: return 4;
  case CCM_MODE_MACLEN_M6: return 6;
  case CCM_MODE_MACLEN_M8: return 8;
  case CCM_MODE_MACLEN_M10: return 10;
  case CCM_MODE_MACLEN_M12: return 12;
  case CCM_MODE_MACLEN_M14: return 14;
  case CCM_MODE_MACLEN_M16: return 16;
  default:
    bs_trace_error_time_line("Unsupported MAC size\n");
    return 0;
  }
}

static void nhw_CCM_logic(uint inst) {
  NRF_CCM_regs[inst]->ERRORSTATUS = 0;
  NRF_CCM_regs[inst]->MACSTATUS = CCM_MACSTATUS_MACSTATUS_CheckFailed;

  uint8_t *ccm_nonce = (uint8_t*)NRF_CCM_regs[inst]->NONCE.VALUE;
  uint8_t *key = (uint8_t*)NRF_CCM_regs[inst]->KEY.VALUE;
  uint8_t ccm_nonce_le[13];
  uint8_t key_le[16];
  uint8_t ADATAMASK = NRF_CCM_regs[inst]->ADATAMASK;
  uint mac_size = nhw_CCM_MAC_size(inst);
  uint encrypt_not_decrypt = (NRF_CCM_regs[inst]->MODE & CCM_MODE_MODE_Msk) == CCM_MODE_MODE_Encryption;
  uint protocol = (NRF_CCM_regs[inst]->MODE & CCM_MODE_PROTOCOL_Msk) >> CCM_MODE_PROTOCOL_Pos;
                  //CCM_MODE_PROTOCOL_Ble or CCM_MODE_PROTOCOL_Ieee802154

  hwu_reverse_byte_order(key, key_le, sizeof(key_le));
  hwu_reverse_byte_order(ccm_nonce, ccm_nonce_le, sizeof(ccm_nonce_le));

  EVDMA_status_t in_evdma, out_evdma;
  int ret;
  size_t n_access;
  uint16_t l_a = 0;
  uint16_t l_mc = 0;

#define IF_NOT_READ_ERROR(ret) \
  if (ret < 0) { \
    NRF_CCM_regs[inst]->ERRORSTATUS = CCM_ERRORSTATUS_ERRORSTATUS_PrematureInptrEnd; \
    nhw_CCM_signal_EVENTS_ERROR(inst); \
    return; \
  }
#define IF_NOT_WRITE_ERROR(ret) \
  if (ret < 0) { \
    NRF_CCM_regs[inst]->ERRORSTATUS = CCM_ERRORSTATUS_ERRORSTATUS_PrematureOutptrEnd; \
    nhw_CCM_signal_EVENTS_ERROR(inst); \
    return; \
  }

  ret = nhw_EVDMA_start(&in_evdma, (void *)NRF_CCM_regs[inst]->IN.PTR);
  if (ret) {
    bs_trace_error_time_line("CCM IN.PTR is NULL during start, that would have hard faulted\n");
  }
  ret = nhw_EVDMA_start(&out_evdma, (void *)NRF_CCM_regs[inst]->OUT.PTR);
  if (ret) {
    bs_trace_error_time_line("CCM OUT.PTR is NULL during start, that would have hard faulted\n");
  }

  //Read l_a, which must be 2 bytes always
  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         (uint8_t *)&l_a, 2,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);
  //Write l_a, which must be 2 bytes always
  ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                         (uint8_t *)&l_a, 2,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_WRITE_ERROR(ret);

  //Read l_m/c, which must be 2 bytes always
  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         (uint8_t *)&l_mc, 2,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);

  uint8_t a_in[l_a];
  uint8_t a_copy[l_a];
  uint8_t mc_in[l_mc];
  uint8_t mc_out[l_mc + BS_MAX(mac_size,4)]; //We just make place for the MAC/MIC even if this is decrypting.

  //Read "a" (AAD)
  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         a_in, l_a,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);

  memcpy(a_copy, a_in, l_a);
  a_copy[0] &= ADATAMASK;

  //Read "m" or "c" (the plaintext or encrypted PDU)
  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         mc_in, l_mc,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_READ_ERROR(ret);

  if (!( (l_a == 0 && l_mc == 0) ||
          ((protocol == CCM_MODE_PROTOCOL_Ble) && (l_mc == 0))
       ) ) //With no AAD and payload; or for BLE, with no payload there is nothing to do
   {
    if (encrypt_not_decrypt) {
      BLECrypt_if_encrypt_packet_v3(a_copy,
          l_a,
          l_mc,
          mac_size,
          13,
          mc_in,
          key_le,
          ccm_nonce_le,
          mc_out);
      l_mc += mac_size;

    } else { // Decrypt
      if ((protocol == CCM_MODE_PROTOCOL_Ble) && (l_mc < mac_size + 1)) {
        NRF_CCM_regs[inst]->MACSTATUS = CCM_MACSTATUS_MACSTATUS_CheckFailed;
      } else if ((protocol == CCM_MODE_PROTOCOL_Ieee802154) && (l_mc < mac_size)) {
        NRF_CCM_regs[inst]->MACSTATUS = CCM_MACSTATUS_MACSTATUS_CheckFailed;
        nhw_CCM_signal_EVENTS_ERROR(inst);
      } else {
        int mic_error;
        l_mc = BS_MAX((int)l_mc - mac_size, 0);

        mic_error = !BLECrypt_if_decrypt_packet_v3(a_copy,
            l_a,
            l_mc,
            mac_size,
            13,
            mc_in,
            key_le,
            ccm_nonce_le,
            mac_size == 0,
            mc_out);

        if (!mic_error) {
          NRF_CCM_regs[inst]->MACSTATUS = CCM_MACSTATUS_MACSTATUS_CheckPassed;
        }
      }
    }
  } else{ //l_mc == 0 or..
    if (!encrypt_not_decrypt) {
      nhw_CCM_signal_EVENTS_ERROR(inst);
    }
  }

  //Write l_m, which must be 2 bytes always
  ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                         (uint8_t *)&l_mc, 2,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_WRITE_ERROR(ret);

  //Write a
  ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                         a_in, l_a,
                         &n_access, NHW_EVDMA_NEWJOB);
  IF_NOT_WRITE_ERROR(ret);

  //Write c/m (encrypted/decrypted|plaintext message)
  ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                         mc_out, l_mc,
                         &n_access, NHW_EVDMA_NEWJOB);
  if (ret == -3) { //It seems even not writing any output is fine
    NRF_CCM_regs[inst]->ERRORSTATUS = (0x2UL);
    nhw_CCM_signal_EVENTS_ERROR(inst);
    return;
  }
#undef IF_NOT_READ_ERROR
#undef IF_NOT_WRITE_ERROR

  nhw_CCM_signal_EVENTS_END(inst);
}

static void nhw_CCM_TASK_START_inner(uint inst) {
  nhw_ECB_possible_abort(inst);
  nhw_CCM_logic(inst);
}

static void nhw_CCM_TASK_STOP_inner(uint inst) {
  /* Note [CCM1]. The encryption is always instantaneous in the model,
   * so FW should not catch it running */
}

static void nhw_CCM_TASK_RATEOVERRIDE(uint inst) {
  /* Note [CCM3]. The encryption is always instantaneous in the model,
   * so we do not need to do anything here */
}

static void nhw_AARCCM_TASK_START(uint inst) {
  if (NRF_CCM_regs[inst]->ENABLE == AAR_ENABLE_ENABLE_Enabled) {
    nhw_AAR_TASK_START_inner(inst);
    return;
  } else if (NRF_CCM_regs[inst]->ENABLE == CCM_ENABLE_ENABLE_Enabled) {
    nhw_CCM_TASK_START_inner(inst);
  } else {
    bs_trace_warning_time_line("%s called with neither AAR or CCM enabled (%i) => ignored\n", __func__, NRF_CCM_regs[inst]->ENABLE);
        return;
  }
}

static void nhw_AARCCM_TASK_STOP(uint inst) {
  if (NRF_CCM_regs[inst]->ENABLE == AAR_ENABLE_ENABLE_Enabled) {
    nhw_AAR_TASK_STOP_inner(inst);
    return;
  } else if (NRF_CCM_regs[inst]->ENABLE == CCM_ENABLE_ENABLE_Enabled) {
    nhw_CCM_TASK_STOP_inner(inst);
  } else {
    bs_trace_warning_time_line("%s called with neither AAR or CCM enabled (%i) => ignored\n", __func__, NRF_CCM_regs[inst]->ENABLE);
        return;
  }
}

static void nhw_AAR_TASK_START(uint inst) {
  nhw_AARCCM_TASK_START(inst);
}

static void nhw_CCM_TASK_START(uint inst) {
  nhw_AARCCM_TASK_START(inst);
}

static void nhw_AAR_TASK_STOP(uint inst) {
  nhw_AARCCM_TASK_STOP(inst);
}

static void nhw_CCM_TASK_STOP(uint inst) {
  nhw_AARCCM_TASK_STOP(inst);
}

NHW_SIDEEFFECTS_TASKS(CCM, NRF_CCM_regs[inst]->, START)
NHW_SIDEEFFECTS_TASKS(CCM, NRF_CCM_regs[inst]->, STOP)
NHW_SIDEEFFECTS_TASKS(CCM, NRF_CCM_regs[inst]->, RATEOVERRIDE)

#define NHW_CCM_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                   \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE_WRAP(TASK_N, CCM)                         \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N, CCM, ccm)

NHW_CCM_REGW_SIDEFFECTS_SUBSCRIBE(START)
NHW_CCM_REGW_SIDEFFECTS_SUBSCRIBE(STOP)
NHW_CCM_REGW_SIDEFFECTS_SUBSCRIBE(RATEOVERRIDE)

NHW_SIDEEFFECTS_EVENTS(CCM)

/* ECB */

/*
 * Cheat interface to adjust the time in microseconds it takes
 * for a 16byte AES ECB block to be computed.
 * (For all ECB peripheral instances)
 */
void nhw_aes_ecb_cheat_set_t_ecb(unsigned int new_t){
  for (int i = 0; i < NHW_AARCCMECB_TOTAL_INST; i++) {
    nhw_ecb_st[i].t_ECB = new_t;
  }
}

/*
 * Cheat interface to reset the time it takes
 * for a 16byte AES ECB block to be computed
 * to the value specified in the infocenter spec.
 * (For all ECB peripheral instances)
 */
void nhw_aes_ecb_cheat_reset_t_ecb(void){
  uint t_ECB[NHW_AARCCMECB_TOTAL_INST] = NHW_ECB_t_ECB;
  for (int i = 0; i < NHW_AARCCMECB_TOTAL_INST; i++) {
    nhw_ecb_st[i].t_ECB = t_ECB[i];
  }
}

NHW_SIDEEFFECTS_INTSET(ECB, NRF_ECB_regs[inst]., nhw_ecb_st[inst].INTEN)
NHW_SIDEEFFECTS_INTCLR(ECB, NRF_ECB_regs[inst]., nhw_ecb_st[inst].INTEN)

static NHW_SIGNAL_EVENT(ECB, NRF_ECB_regs[inst]. , END)
static NHW_SIGNAL_EVENT(ECB, NRF_ECB_regs[inst]. , ERROR)

static void nhw_ECB_stop(uint inst) {
  struct ecb_status *st = &nhw_ecb_st[inst];
  st->Running = false;
  st->Timer = TIME_NEVER;
  update_master_timer();
}

static bool nhw_ECB_possible_abort(uint inst) {
  if ((nhw_aar_st[inst].Running) || (nhw_ccm_st[inst].Running)) {
    nhw_ECB_stop(inst);
    NRF_ECB_regs[inst].ERRORSTATUS = 3; /* Aborted (Once the MDK is updated use the name) */
    nhw_ECB_signal_EVENTS_ERROR(inst);
    return true;
  }
  return false;
}

static void nhw_ECB_logic(uint inst) {
  uint8_t input[16];
  uint8_t output[16];
  EVDMA_status_t in_evdma, out_evdma;
  size_t n_access;
  int ret;

  NRF_ECB_regs[inst].ERRORSTATUS = 0;

  memset(input, 0, sizeof(input));

  ret = nhw_EVDMA_start(&in_evdma, (void *)NRF_ECB_regs[inst].IN.PTR);
  if (ret) {
    bs_trace_error_time_line("ECB IN.PTR is NULL during start, that would have hard faulted\n");
  }
  ret = nhw_EVDMA_access(&in_evdma, NHW_EVDMA_READ,
                         input, 16,
                         &n_access, NHW_EVDMA_NEWJOB);
  if (n_access ==  0) { //It is ok to have a job for less than 16 bytes, just not to have none.
    NRF_ECB_regs[inst].ERRORSTATUS = ECB_ERRORSTATUS_ERRORSTATUS_PrematureInptrEnd;
    nhw_ECB_stop(inst);
    nhw_ECB_signal_EVENTS_ERROR(inst);
    return;
  }

  BLECrypt_if_aes_128((const uint8_t *)&NRF_ECB_regs[inst].KEY, input, output);

  ret = nhw_EVDMA_start(&out_evdma, (void *)NRF_ECB_regs[inst].OUT.PTR);
  if (ret) {
    bs_trace_error_time_line("ECB OUT.PTR is NULL during start, that would have hard faulted\n");
  }
  ret = nhw_EVDMA_access(&out_evdma, NHW_EVDMA_WRITE,
                         output, 16,
                         &n_access, NHW_EVDMA_NEWJOB);
  if (n_access ==  0) { //It is ok to have a job for less than 16 bytes, just not to have none.
    NRF_ECB_regs[inst].ERRORSTATUS = ECB_ERRORSTATUS_ERRORSTATUS_PrematureOutptrEnd;
    nhw_ECB_stop(inst);
    nhw_ECB_signal_EVENTS_ERROR(inst);
    return;
  }

  //Note [ECB1]
  nhw_ecb_st[inst].Timer = nsi_hws_get_time() + nhw_ecb_st[inst].t_ECB;
  update_master_timer();
}

static void nhw_ECB_TASK_START(uint inst) {
  //Note [ECB2]
  if (nhw_ecb_st[inst].Running) {
    bs_trace_warning_time_line("%s called while ECB was already running => error event\n", __func__);
    nhw_ECB_signal_EVENTS_ERROR(inst);
    return;
  }
  if (nhw_ECB_possible_abort(inst)) {
    bs_trace_warning_time_line("%s called while AAR or CCM was running => error event\n", __func__);
    return;
  }
  nhw_ecb_st[inst].Running = true;
  nhw_ECB_logic(inst);
}

static void nhw_ECB_TASK_STOP(uint inst) {
  int was_running = nhw_ecb_st[inst].Running;
  nhw_ECB_stop(inst);
  if (was_running) {
    nhw_ECB_signal_EVENTS_ERROR(inst);
    //Note [ECB3]
  } else {
    bs_trace_info_time_line(3, "%s called while ECB was not running\n", __func__);
  }
}

NHW_SIDEEFFECTS_TASKS(ECB, NRF_ECB_regs[inst]., START)
NHW_SIDEEFFECTS_TASKS(ECB, NRF_ECB_regs[inst]., STOP)

static void nhw_ECB_timer_triggered(uint inst) {
  nhw_ECB_stop(inst);
  nhw_ECB_signal_EVENTS_END(inst);
}

#define NHW_ECB_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N)                                   \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE_WRAP(TASK_N, ECB)                         \
  NHW_AARECBCCM_REGW_SIDEFFECTS_SUBSCRIBE(TASK_N, ECB, ecb)

NHW_ECB_REGW_SIDEFFECTS_SUBSCRIBE(START)
NHW_ECB_REGW_SIDEFFECTS_SUBSCRIBE(STOP)

NHW_SIDEEFFECTS_EVENTS(ECB)

/***/

static void nhw_AARCCMECB_timer_triggered(void) {
  for (int i = 0; i < NHW_AARCCMECB_TOTAL_INST; i++) {
    if (Timer_AAR_CCM_ECB == nhw_aar_st[i].Timer) {
      nhw_AAR_timer_triggered(i);
    } else if (Timer_AAR_CCM_ECB == nhw_ecb_st[i].Timer) {
      nhw_ECB_timer_triggered(i);
    } else {
      bs_trace_error_time_line("Programming error\n");
    }
  }
  nsi_hws_find_next_event();
}

NSI_HW_EVENT(Timer_AAR_CCM_ECB, nhw_AARCCMECB_timer_triggered, 50);
