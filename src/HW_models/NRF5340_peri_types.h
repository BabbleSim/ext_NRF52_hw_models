/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * Note: Most of these file content is taken directly or with minor
 * modifications from the Nordic nrfx MDK files, which are
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Therefore this file overall has that same license.
 *
 * HW peripherals SW regiters interface definitions
 * For an nRF5340 SOC
 *
 */

#ifndef _NRF5340_PERI_TYPES_H
#define _NRF5340_PERI_TYPES_H

#include <stdint.h>

#ifndef __IM
  #define __IM
#endif
#ifndef __OM
  #define __OM
#endif
#ifndef __IOM
  #define __IOM
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief FICR_INFO [INFO] (Device info)
  */
typedef struct {
  __IM  uint32_t  CONFIGID;                     /*!< (@ 0x00000000) Configuration identifier                                   */
  __IM  uint32_t  DEVICEID[2];                  /*!< (@ 0x00000004) Description collection: Device identifier                  */
  __IM  uint32_t  PART;                         /*!< (@ 0x0000000C) Part code                                                  */
  __IM  uint32_t  VARIANT;                      /*!< (@ 0x00000010) Part Variant, Hardware version and Production
                                                                    configuration                                              */
  __IM  uint32_t  PACKAGE;                      /*!< (@ 0x00000014) Package option                                             */
  __IM  uint32_t  RAM;                          /*!< (@ 0x00000018) RAM variant                                                */
  __IM  uint32_t  FLASH;                        /*!< (@ 0x0000001C) Flash variant                                              */
  __IM  uint32_t  CODEPAGESIZE;                 /*!< (@ 0x00000020) Code memory page size in bytes                             */
  __IM  uint32_t  CODESIZE;                     /*!< (@ 0x00000024) Code memory size                                           */
  __IM  uint32_t  DEVICETYPE;                   /*!< (@ 0x00000028) Device type                                                */
} FICR_INFO_Type;                               /*!< Size = 44 (0x2c)                                                          */


/**
  * @brief FICR_TRIMCNF [TRIMCNF] (Unspecified)
  */
typedef struct {
  __IM  uint32_t  ADDR;                         /*!< (@ 0x00000000) Description cluster: Address                               */
  __IM  uint32_t  DATA;                         /*!< (@ 0x00000004) Description cluster: Data                                  */
} FICR_TRIMCNF_Type;                            /*!< Size = 8 (0x8)                                                            */

/**
  * @brief DPPIC_TASKS_CHG [TASKS_CHG] (Channel group tasks)
  */
typedef struct {
  __OM  uint32_t  EN;                           /*!< (@ 0x00000000) Description cluster: Enable channel group n                */
  __OM  uint32_t  DIS;                          /*!< (@ 0x00000004) Description cluster: Disable channel group n               */
} DPPIC_TASKS_CHG_Type;                         /*!< Size = 8 (0x8)                                                            */


/**
  * @brief DPPIC_SUBSCRIBE_CHG [SUBSCRIBE_CHG] (Subscribe configuration for tasks)
  */
typedef struct {
  __IOM uint32_t  EN;                           /*!< (@ 0x00000000) Description cluster: Subscribe configuration
                                                                    for task CHG[n].EN                                         */
  __IOM uint32_t  DIS;                          /*!< (@ 0x00000004) Description cluster: Subscribe configuration
                                                                    for task CHG[n].DIS                                        */
} DPPIC_SUBSCRIBE_CHG_Type;                     /*!< Size = 8 (0x8)                                                            */


/* =========================================================================================================================== */
/* ================                                          EGU                                              ================ */
/* =========================================================================================================================== */


/**
  * @brief Event generator unit 0 (EGU0_NS)
  */

typedef struct {                                /*!< (@ 0x4001B000) EGU0_NS Structure                                          */
  __OM  uint32_t  TASKS_TRIGGER[16];            /*!< (@ 0x00000000) Description collection: Trigger n for triggering
                                                                    the corresponding TRIGGERED[n] event                       */
  __IM  uint32_t  RESERVED[16];
  __IOM uint32_t  SUBSCRIBE_TRIGGER[16];        /*!< (@ 0x00000080) Description collection: Subscribe configuration
                                                                    for task TRIGGER[n]                                        */
  __IM  uint32_t  RESERVED1[16];
  __IOM uint32_t  EVENTS_TRIGGERED[16];         /*!< (@ 0x00000100) Description collection: Event number n generated
                                                                    by triggering the corresponding TRIGGER[n]
                                                                    task                                                       */
  __IM  uint32_t  RESERVED2[16];
  __IOM uint32_t  PUBLISH_TRIGGERED[16];        /*!< (@ 0x00000180) Description collection: Publish configuration
                                                                    for event TRIGGERED[n]                                     */
  __IM  uint32_t  RESERVED3[80];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
} NRF_EGU_Type;                                 /*!< Size = 780 (0x30c)                                                        */

/* Peripheral: EGU */
/* Description: Event generator unit */

/* Register: EGU_TASKS_TRIGGER */
/* Description: Description collection: Trigger n for triggering the corresponding TRIGGERED[n] event */

/* Bit 0 : Trigger n for triggering the corresponding TRIGGERED[n] event */
#define EGU_TASKS_TRIGGER_TASKS_TRIGGER_Pos (0UL) /*!< Position of TASKS_TRIGGER field. */
#define EGU_TASKS_TRIGGER_TASKS_TRIGGER_Msk (0x1UL << EGU_TASKS_TRIGGER_TASKS_TRIGGER_Pos) /*!< Bit mask of TASKS_TRIGGER field. */
#define EGU_TASKS_TRIGGER_TASKS_TRIGGER_Trigger (1UL) /*!< Trigger task */

/* Register: EGU_SUBSCRIBE_TRIGGER */
/* Description: Description collection: Subscribe configuration for task TRIGGER[n] */

/* Bit 31 :   */
#define EGU_SUBSCRIBE_TRIGGER_EN_Pos (31UL) /*!< Position of EN field. */
#define EGU_SUBSCRIBE_TRIGGER_EN_Msk (0x1UL << EGU_SUBSCRIBE_TRIGGER_EN_Pos) /*!< Bit mask of EN field. */
#define EGU_SUBSCRIBE_TRIGGER_EN_Disabled (0UL) /*!< Disable subscription */
#define EGU_SUBSCRIBE_TRIGGER_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task TRIGGER[n] will subscribe to */
#define EGU_SUBSCRIBE_TRIGGER_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define EGU_SUBSCRIBE_TRIGGER_CHIDX_Msk (0xFFUL << EGU_SUBSCRIBE_TRIGGER_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: EGU_EVENTS_TRIGGERED */
/* Description: Description collection: Event number n generated by triggering the corresponding TRIGGER[n] task */

/* Bit 0 : Event number n generated by triggering the corresponding TRIGGER[n] task */
#define EGU_EVENTS_TRIGGERED_EVENTS_TRIGGERED_Pos (0UL) /*!< Position of EVENTS_TRIGGERED field. */
#define EGU_EVENTS_TRIGGERED_EVENTS_TRIGGERED_Msk (0x1UL << EGU_EVENTS_TRIGGERED_EVENTS_TRIGGERED_Pos) /*!< Bit mask of EVENTS_TRIGGERED field. */
#define EGU_EVENTS_TRIGGERED_EVENTS_TRIGGERED_NotGenerated (0UL) /*!< Event not generated */
#define EGU_EVENTS_TRIGGERED_EVENTS_TRIGGERED_Generated (1UL) /*!< Event generated */

/* Register: EGU_PUBLISH_TRIGGERED */
/* Description: Description collection: Publish configuration for event TRIGGERED[n] */

/* Bit 31 :   */
#define EGU_PUBLISH_TRIGGERED_EN_Pos (31UL) /*!< Position of EN field. */
#define EGU_PUBLISH_TRIGGERED_EN_Msk (0x1UL << EGU_PUBLISH_TRIGGERED_EN_Pos) /*!< Bit mask of EN field. */
#define EGU_PUBLISH_TRIGGERED_EN_Disabled (0UL) /*!< Disable publishing */
#define EGU_PUBLISH_TRIGGERED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event TRIGGERED[n] will publish to. */
#define EGU_PUBLISH_TRIGGERED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define EGU_PUBLISH_TRIGGERED_CHIDX_Msk (0xFFUL << EGU_PUBLISH_TRIGGERED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: EGU_INTEN */
/* Description: Enable or disable interrupt */

/* Bit 15 : Enable or disable interrupt for event TRIGGERED[15] */
#define EGU_INTEN_TRIGGERED15_Pos (15UL) /*!< Position of TRIGGERED15 field. */
#define EGU_INTEN_TRIGGERED15_Msk (0x1UL << EGU_INTEN_TRIGGERED15_Pos) /*!< Bit mask of TRIGGERED15 field. */
#define EGU_INTEN_TRIGGERED15_Disabled (0UL) /*!< Disable */
#define EGU_INTEN_TRIGGERED15_Enabled (1UL) /*!< Enable */

/* Bit 0 : Enable or disable interrupt for event TRIGGERED[0] */
#define EGU_INTEN_TRIGGERED0_Pos (0UL) /*!< Position of TRIGGERED0 field. */
#define EGU_INTEN_TRIGGERED0_Msk (0x1UL << EGU_INTEN_TRIGGERED0_Pos) /*!< Bit mask of TRIGGERED0 field. */
#define EGU_INTEN_TRIGGERED0_Disabled (0UL) /*!< Disable */
#define EGU_INTEN_TRIGGERED0_Enabled (1UL) /*!< Enable */

/* Register: EGU_INTENSET */
/* Description: Enable interrupt */

/* Bit 15 : Write '1' to enable interrupt for event TRIGGERED[15] */
#define EGU_INTENSET_TRIGGERED15_Pos (15UL) /*!< Position of TRIGGERED15 field. */
#define EGU_INTENSET_TRIGGERED15_Msk (0x1UL << EGU_INTENSET_TRIGGERED15_Pos) /*!< Bit mask of TRIGGERED15 field. */
#define EGU_INTENSET_TRIGGERED15_Disabled (0UL) /*!< Read: Disabled */
#define EGU_INTENSET_TRIGGERED15_Enabled (1UL) /*!< Read: Enabled */
#define EGU_INTENSET_TRIGGERED15_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event TRIGGERED[0] */
#define EGU_INTENSET_TRIGGERED0_Pos (0UL) /*!< Position of TRIGGERED0 field. */
#define EGU_INTENSET_TRIGGERED0_Msk (0x1UL << EGU_INTENSET_TRIGGERED0_Pos) /*!< Bit mask of TRIGGERED0 field. */
#define EGU_INTENSET_TRIGGERED0_Disabled (0UL) /*!< Read: Disabled */
#define EGU_INTENSET_TRIGGERED0_Enabled (1UL) /*!< Read: Enabled */
#define EGU_INTENSET_TRIGGERED0_Set (1UL) /*!< Enable */

/* Register: EGU_INTENCLR */
/* Description: Disable interrupt */

/* Bit 15 : Write '1' to disable interrupt for event TRIGGERED[15] */
#define EGU_INTENCLR_TRIGGERED15_Pos (15UL) /*!< Position of TRIGGERED15 field. */
#define EGU_INTENCLR_TRIGGERED15_Msk (0x1UL << EGU_INTENCLR_TRIGGERED15_Pos) /*!< Bit mask of TRIGGERED15 field. */
#define EGU_INTENCLR_TRIGGERED15_Disabled (0UL) /*!< Read: Disabled */
#define EGU_INTENCLR_TRIGGERED15_Enabled (1UL) /*!< Read: Enabled */
#define EGU_INTENCLR_TRIGGERED15_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event TRIGGERED[0] */
#define EGU_INTENCLR_TRIGGERED0_Pos (0UL) /*!< Position of TRIGGERED0 field. */
#define EGU_INTENCLR_TRIGGERED0_Msk (0x1UL << EGU_INTENCLR_TRIGGERED0_Pos) /*!< Bit mask of TRIGGERED0 field. */
#define EGU_INTENCLR_TRIGGERED0_Disabled (0UL) /*!< Read: Disabled */
#define EGU_INTENCLR_TRIGGERED0_Enabled (1UL) /*!< Read: Enabled */
#define EGU_INTENCLR_TRIGGERED0_Clear (1UL) /*!< Disable */




/* =========================================================================================================================== */
/* ================                                         DPPIC_NS                                          ================ */
/* =========================================================================================================================== */


/**
  * @brief Distributed programmable peripheral interconnect controller (DPPIC_NS)
  */

typedef struct {                                /*!< (@ 0x4100F000) DPPIC_NS Structure                                         */
  __OM  DPPIC_TASKS_CHG_Type TASKS_CHG[6];      /*!< (@ 0x00000000) Channel group tasks                                        */
  __IM  uint32_t  RESERVED[20];
  __IOM DPPIC_SUBSCRIBE_CHG_Type SUBSCRIBE_CHG[6];/*!< (@ 0x00000080) Subscribe configuration for tasks                        */
  __IM  uint32_t  RESERVED1[276];
  __IOM uint32_t  CHEN;                         /*!< (@ 0x00000500) Channel enable register                                    */
  __IOM uint32_t  CHENSET;                      /*!< (@ 0x00000504) Channel enable set register                                */
  __IOM uint32_t  CHENCLR;                      /*!< (@ 0x00000508) Channel enable clear register                              */
  __IM  uint32_t  RESERVED2[189];
  __IOM uint32_t  CHG[6];                       /*!< (@ 0x00000800) Description collection: Channel group n Note:
                                                                    Writes to this register are ignored if either
                                                                    SUBSCRIBE_CHG[n].EN or SUBSCRIBE_CHG[n].DIS
                                                                    is enabled                                                 */
} NRF_DPPIC_Type;                               /*!< Size = 2072 (0x818)                                                       */



/* Peripheral: DPPIC */
/* Description: Distributed programmable peripheral interconnect controller */

/* Register: DPPIC_TASKS_CHG_EN */
/* Description: Description cluster: Enable channel group n */

/* Bit 0 : Enable channel group n */
#define DPPIC_TASKS_CHG_EN_EN_Pos (0UL) /*!< Position of EN field. */
#define DPPIC_TASKS_CHG_EN_EN_Msk (0x1UL << DPPIC_TASKS_CHG_EN_EN_Pos) /*!< Bit mask of EN field. */
#define DPPIC_TASKS_CHG_EN_EN_Trigger (1UL) /*!< Trigger task */

/* Register: DPPIC_TASKS_CHG_DIS */
/* Description: Description cluster: Disable channel group n */

/* Bit 0 : Disable channel group n */
#define DPPIC_TASKS_CHG_DIS_DIS_Pos (0UL) /*!< Position of DIS field. */
#define DPPIC_TASKS_CHG_DIS_DIS_Msk (0x1UL << DPPIC_TASKS_CHG_DIS_DIS_Pos) /*!< Bit mask of DIS field. */
#define DPPIC_TASKS_CHG_DIS_DIS_Trigger (1UL) /*!< Trigger task */

/* Register: DPPIC_SUBSCRIBE_CHG_EN */
/* Description: Description cluster: Subscribe configuration for task CHG[n].EN */

/* Bit 31 :   */
#define DPPIC_SUBSCRIBE_CHG_EN_EN_Pos (31UL) /*!< Position of EN field. */
#define DPPIC_SUBSCRIBE_CHG_EN_EN_Msk (0x1UL << DPPIC_SUBSCRIBE_CHG_EN_EN_Pos) /*!< Bit mask of EN field. */
#define DPPIC_SUBSCRIBE_CHG_EN_EN_Disabled (0UL) /*!< Disable subscription */
#define DPPIC_SUBSCRIBE_CHG_EN_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CHG[n].EN will subscribe to */
#define DPPIC_SUBSCRIBE_CHG_EN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define DPPIC_SUBSCRIBE_CHG_EN_CHIDX_Msk (0xFFUL << DPPIC_SUBSCRIBE_CHG_EN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: DPPIC_SUBSCRIBE_CHG_DIS */
/* Description: Description cluster: Subscribe configuration for task CHG[n].DIS */

/* Bit 31 :   */
#define DPPIC_SUBSCRIBE_CHG_DIS_EN_Pos (31UL) /*!< Position of EN field. */
#define DPPIC_SUBSCRIBE_CHG_DIS_EN_Msk (0x1UL << DPPIC_SUBSCRIBE_CHG_DIS_EN_Pos) /*!< Bit mask of EN field. */
#define DPPIC_SUBSCRIBE_CHG_DIS_EN_Disabled (0UL) /*!< Disable subscription */
#define DPPIC_SUBSCRIBE_CHG_DIS_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CHG[n].DIS will subscribe to */
#define DPPIC_SUBSCRIBE_CHG_DIS_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define DPPIC_SUBSCRIBE_CHG_DIS_CHIDX_Msk (0xFFUL << DPPIC_SUBSCRIBE_CHG_DIS_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: DPPIC_CHEN */
/* Description: Channel enable register */

/* Bit 31 : Enable or disable channel 31 */
#define DPPIC_CHEN_CH31_Pos (31UL) /*!< Position of CH31 field. */
#define DPPIC_CHEN_CH31_Msk (0x1UL << DPPIC_CHEN_CH31_Pos) /*!< Bit mask of CH31 field. */
#define DPPIC_CHEN_CH31_Disabled (0UL) /*!< Disable channel */
#define DPPIC_CHEN_CH31_Enabled (1UL) /*!< Enable channel */

/* Bit 0 : Enable or disable channel 0 */
#define DPPIC_CHEN_CH0_Pos (0UL) /*!< Position of CH0 field. */
#define DPPIC_CHEN_CH0_Msk (0x1UL << DPPIC_CHEN_CH0_Pos) /*!< Bit mask of CH0 field. */
#define DPPIC_CHEN_CH0_Disabled (0UL) /*!< Disable channel */
#define DPPIC_CHEN_CH0_Enabled (1UL) /*!< Enable channel */

/* Register: DPPIC_CHENSET */
/* Description: Channel enable set register */

/* Bit 31 : Channel 31 enable set register. Writing 0 has no effect. */
#define DPPIC_CHENSET_CH31_Pos (31UL) /*!< Position of CH31 field. */
#define DPPIC_CHENSET_CH31_Msk (0x1UL << DPPIC_CHENSET_CH31_Pos) /*!< Bit mask of CH31 field. */
#define DPPIC_CHENSET_CH31_Disabled (0UL) /*!< Read: Channel disabled */
#define DPPIC_CHENSET_CH31_Enabled (1UL) /*!< Read: Channel enabled */
#define DPPIC_CHENSET_CH31_Set (1UL) /*!< Write: Enable channel */

/* Bit 0 : Channel 0 enable set register. Writing 0 has no effect. */
#define DPPIC_CHENSET_CH0_Pos (0UL) /*!< Position of CH0 field. */
#define DPPIC_CHENSET_CH0_Msk (0x1UL << DPPIC_CHENSET_CH0_Pos) /*!< Bit mask of CH0 field. */
#define DPPIC_CHENSET_CH0_Disabled (0UL) /*!< Read: Channel disabled */
#define DPPIC_CHENSET_CH0_Enabled (1UL) /*!< Read: Channel enabled */
#define DPPIC_CHENSET_CH0_Set (1UL) /*!< Write: Enable channel */

/* Register: DPPIC_CHENCLR */
/* Description: Channel enable clear register */

/* Bit 31 : Channel 31 enable clear register.  Writing 0 has no effect. */
#define DPPIC_CHENCLR_CH31_Pos (31UL) /*!< Position of CH31 field. */
#define DPPIC_CHENCLR_CH31_Msk (0x1UL << DPPIC_CHENCLR_CH31_Pos) /*!< Bit mask of CH31 field. */
#define DPPIC_CHENCLR_CH31_Disabled (0UL) /*!< Read: Channel disabled */
#define DPPIC_CHENCLR_CH31_Enabled (1UL) /*!< Read: Channel enabled */
#define DPPIC_CHENCLR_CH31_Clear (1UL) /*!< Write: Disable channel */

/* Bit 0 : Channel 0 enable clear register.  Writing 0 has no effect. */
#define DPPIC_CHENCLR_CH0_Pos (0UL) /*!< Position of CH0 field. */
#define DPPIC_CHENCLR_CH0_Msk (0x1UL << DPPIC_CHENCLR_CH0_Pos) /*!< Bit mask of CH0 field. */
#define DPPIC_CHENCLR_CH0_Disabled (0UL) /*!< Read: Channel disabled */
#define DPPIC_CHENCLR_CH0_Enabled (1UL) /*!< Read: Channel enabled */
#define DPPIC_CHENCLR_CH0_Clear (1UL) /*!< Write: Disable channel */

/* Register: DPPIC_CHG */
/* Description: Description collection: Channel group n Note: Writes to this register are ignored if either SUBSCRIBE_CHG[n].EN or SUBSCRIBE_CHG[n].DIS is enabled */

/* Bit 31 : Include or exclude channel 31 */
#define DPPIC_CHG_CH31_Pos (31UL) /*!< Position of CH31 field. */
#define DPPIC_CHG_CH31_Msk (0x1UL << DPPIC_CHG_CH31_Pos) /*!< Bit mask of CH31 field. */
#define DPPIC_CHG_CH31_Excluded (0UL) /*!< Exclude */
#define DPPIC_CHG_CH31_Included (1UL) /*!< Include */

/* Bit 0 : Include or exclude channel 0 */
#define DPPIC_CHG_CH0_Pos (0UL) /*!< Position of CH0 field. */
#define DPPIC_CHG_CH0_Msk (0x1UL << DPPIC_CHG_CH0_Pos) /*!< Bit mask of CH0 field. */
#define DPPIC_CHG_CH0_Excluded (0UL) /*!< Exclude */
#define DPPIC_CHG_CH0_Included (1UL) /*!< Include */



/* =========================================================================================================================== */
/* ================                                         POWER_NS                                          ================ */
/* =========================================================================================================================== */


/**
  * @brief Power control 0 (POWER_NS)
  */

typedef struct {                                /*!< (@ 0x40005000) POWER_NS Structure                                         */
  __IM  uint32_t  RESERVED[30];
  __OM  uint32_t  TASKS_CONSTLAT;               /*!< (@ 0x00000078) Enable Constant Latency mode                               */
  __OM  uint32_t  TASKS_LOWPWR;                 /*!< (@ 0x0000007C) Enable Low-Power mode (variable latency)                   */
  __IM  uint32_t  RESERVED1[30];
  __IOM uint32_t  SUBSCRIBE_CONSTLAT;           /*!< (@ 0x000000F8) Subscribe configuration for task CONSTLAT                  */
  __IOM uint32_t  SUBSCRIBE_LOWPWR;             /*!< (@ 0x000000FC) Subscribe configuration for task LOWPWR                    */
  __IM  uint32_t  RESERVED2[2];
  __IOM uint32_t  EVENTS_POFWARN;               /*!< (@ 0x00000108) Power failure warning                                      */
  __IM  uint32_t  RESERVED3[2];
  __IOM uint32_t  EVENTS_SLEEPENTER;            /*!< (@ 0x00000114) CPU entered WFI/WFE sleep                                  */
  __IOM uint32_t  EVENTS_SLEEPEXIT;             /*!< (@ 0x00000118) CPU exited WFI/WFE sleep                                   */
  __IM  uint32_t  RESERVED4[27];
  __IOM uint32_t  PUBLISH_POFWARN;              /*!< (@ 0x00000188) Publish configuration for event POFWARN                    */
  __IM  uint32_t  RESERVED5[2];
  __IOM uint32_t  PUBLISH_SLEEPENTER;           /*!< (@ 0x00000194) Publish configuration for event SLEEPENTER                 */
  __IOM uint32_t  PUBLISH_SLEEPEXIT;            /*!< (@ 0x00000198) Publish configuration for event SLEEPEXIT                  */
  __IM  uint32_t  RESERVED6[89];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED7[132];
  __IOM uint32_t  GPREGRET[2];                  /*!< (@ 0x0000051C) Description collection: General purpose retention
                                                                    register                                                   */
} NRF_POWER_Type;                               /*!< Size = 1316 (0x524)                                                       */



/* =========================================================================================================================== */
/* ================                                          RNG_NS                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief Random Number Generator (RNG_NS)
  */

typedef struct {                                /*!< (@ 0x41009000) RNG_NS Structure                                           */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Task starting the random number generator                  */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000004) Task stopping the random number generator                  */
  __IM  uint32_t  RESERVED[30];
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000080) Subscribe configuration for task START                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000084) Subscribe configuration for task STOP                      */
  __IM  uint32_t  RESERVED1[30];
  __IOM uint32_t  EVENTS_VALRDY;                /*!< (@ 0x00000100) Event being generated for every new random number
                                                                    written to the VALUE register                              */
  __IM  uint32_t  RESERVED2[31];
  __IOM uint32_t  PUBLISH_VALRDY;               /*!< (@ 0x00000180) Publish configuration for event VALRDY                     */
  __IM  uint32_t  RESERVED3[31];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED4[64];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED5[126];
  __IOM uint32_t  CONFIG;                       /*!< (@ 0x00000504) Configuration register                                     */
  __IM  uint32_t  VALUE;                        /*!< (@ 0x00000508) Output random number                                       */
} NRF_RNG_Type;                                 /*!< Size = 1292 (0x50c)                                                       */


/* Peripheral: RNG */
/* Description: Random Number Generator */

/* Register: RNG_TASKS_START */
/* Description: Task starting the random number generator */

/* Bit 0 : Task starting the random number generator */
#define RNG_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define RNG_TASKS_START_TASKS_START_Msk (0x1UL << RNG_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define RNG_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: RNG_TASKS_STOP */
/* Description: Task stopping the random number generator */

/* Bit 0 : Task stopping the random number generator */
#define RNG_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define RNG_TASKS_STOP_TASKS_STOP_Msk (0x1UL << RNG_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define RNG_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: RNG_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define RNG_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define RNG_SUBSCRIBE_START_EN_Msk (0x1UL << RNG_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define RNG_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define RNG_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define RNG_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RNG_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << RNG_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RNG_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define RNG_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RNG_SUBSCRIBE_STOP_EN_Msk (0x1UL << RNG_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define RNG_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RNG_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define RNG_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RNG_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << RNG_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RNG_EVENTS_VALRDY */
/* Description: Event being generated for every new random number written to the VALUE register */

/* Bit 0 : Event being generated for every new random number written to the VALUE register */
#define RNG_EVENTS_VALRDY_EVENTS_VALRDY_Pos (0UL) /*!< Position of EVENTS_VALRDY field. */
#define RNG_EVENTS_VALRDY_EVENTS_VALRDY_Msk (0x1UL << RNG_EVENTS_VALRDY_EVENTS_VALRDY_Pos) /*!< Bit mask of EVENTS_VALRDY field. */
#define RNG_EVENTS_VALRDY_EVENTS_VALRDY_NotGenerated (0UL) /*!< Event not generated */
#define RNG_EVENTS_VALRDY_EVENTS_VALRDY_Generated (1UL) /*!< Event generated */

/* Register: RNG_PUBLISH_VALRDY */
/* Description: Publish configuration for event VALRDY */

/* Bit 31 :   */
#define RNG_PUBLISH_VALRDY_EN_Pos (31UL) /*!< Position of EN field. */
#define RNG_PUBLISH_VALRDY_EN_Msk (0x1UL << RNG_PUBLISH_VALRDY_EN_Pos) /*!< Bit mask of EN field. */
#define RNG_PUBLISH_VALRDY_EN_Disabled (0UL) /*!< Disable publishing */
#define RNG_PUBLISH_VALRDY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event VALRDY will publish to. */
#define RNG_PUBLISH_VALRDY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RNG_PUBLISH_VALRDY_CHIDX_Msk (0xFFUL << RNG_PUBLISH_VALRDY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RNG_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 0 : Shortcut between event VALRDY and task STOP */
#define RNG_SHORTS_VALRDY_STOP_Pos (0UL) /*!< Position of VALRDY_STOP field. */
#define RNG_SHORTS_VALRDY_STOP_Msk (0x1UL << RNG_SHORTS_VALRDY_STOP_Pos) /*!< Bit mask of VALRDY_STOP field. */
#define RNG_SHORTS_VALRDY_STOP_Disabled (0UL) /*!< Disable shortcut */
#define RNG_SHORTS_VALRDY_STOP_Enabled (1UL) /*!< Enable shortcut */

/* Register: RNG_INTENSET */
/* Description: Enable interrupt */

/* Bit 0 : Write '1' to enable interrupt for event VALRDY */
#define RNG_INTENSET_VALRDY_Pos (0UL) /*!< Position of VALRDY field. */
#define RNG_INTENSET_VALRDY_Msk (0x1UL << RNG_INTENSET_VALRDY_Pos) /*!< Bit mask of VALRDY field. */
#define RNG_INTENSET_VALRDY_Disabled (0UL) /*!< Read: Disabled */
#define RNG_INTENSET_VALRDY_Enabled (1UL) /*!< Read: Enabled */
#define RNG_INTENSET_VALRDY_Set (1UL) /*!< Enable */

/* Register: RNG_INTENCLR */
/* Description: Disable interrupt */

/* Bit 0 : Write '1' to disable interrupt for event VALRDY */
#define RNG_INTENCLR_VALRDY_Pos (0UL) /*!< Position of VALRDY field. */
#define RNG_INTENCLR_VALRDY_Msk (0x1UL << RNG_INTENCLR_VALRDY_Pos) /*!< Bit mask of VALRDY field. */
#define RNG_INTENCLR_VALRDY_Disabled (0UL) /*!< Read: Disabled */
#define RNG_INTENCLR_VALRDY_Enabled (1UL) /*!< Read: Enabled */
#define RNG_INTENCLR_VALRDY_Clear (1UL) /*!< Disable */

/* Register: RNG_CONFIG */
/* Description: Configuration register */

/* Bit 0 : Bias correction */
#define RNG_CONFIG_DERCEN_Pos (0UL) /*!< Position of DERCEN field. */
#define RNG_CONFIG_DERCEN_Msk (0x1UL << RNG_CONFIG_DERCEN_Pos) /*!< Bit mask of DERCEN field. */
#define RNG_CONFIG_DERCEN_Disabled (0UL) /*!< Disabled */
#define RNG_CONFIG_DERCEN_Enabled (1UL) /*!< Enabled */

/* Register: RNG_VALUE */
/* Description: Output random number */

/* Bits 7..0 : Generated random number */
#define RNG_VALUE_VALUE_Pos (0UL) /*!< Position of VALUE field. */
#define RNG_VALUE_VALUE_Msk (0xFFUL << RNG_VALUE_VALUE_Pos) /*!< Bit mask of VALUE field. */



#ifdef __cplusplus
}
#endif

#endif /* _NRF5340_PERI_TYPES_H */
