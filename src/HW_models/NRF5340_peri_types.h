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


/**
  * @brief CLOCK_HFCLKAUDIO [HFCLKAUDIO] (Unspecified)
  */
typedef struct {
  __IOM uint32_t  FREQUENCY;                    /*!< (@ 0x00000000) Audio PLL frequency in 11.176 MHz - 11.402 MHz
                                                                    or 12.165 MHz - 12.411 MHz frequency bands                 */
} CLOCK_HFCLKAUDIO_Type;                        /*!< Size = 4 (0x4)                                                            */


/**
  * @brief RESET_NETWORK [NETWORK] (ULP network core control)
  */
typedef struct {
  __IM  uint32_t  RESERVED;
  __IOM uint32_t  FORCEOFF;                     /*!< (@ 0x00000004) Force network core off                                     */
} RESET_NETWORK_Type;                           /*!< Size = 8 (0x8)                                                            */


/* =========================================================================================================================== */
/* ================                                         CLOCK                                          ================ */
/* =========================================================================================================================== */


/**
  * @brief Clock management (CLOCK)
  */

typedef struct {                                /*!< (@ 0x40005000) CLOCK_NS Structure                                         */
  __OM  uint32_t  TASKS_HFCLKSTART;             /*!< (@ 0x00000000) Start HFCLK128M/HFCLK64M source as selected in
                                                                    HFCLKSRC                                                   */
  __OM  uint32_t  TASKS_HFCLKSTOP;              /*!< (@ 0x00000004) Stop HFCLK128M/HFCLK64M source                             */
  __OM  uint32_t  TASKS_LFCLKSTART;             /*!< (@ 0x00000008) Start LFCLK source as selected in LFCLKSRC                 */
  __OM  uint32_t  TASKS_LFCLKSTOP;              /*!< (@ 0x0000000C) Stop LFCLK source                                          */
  __OM  uint32_t  TASKS_CAL;                    /*!< (@ 0x00000010) Start calibration of LFRC oscillator                       */
  __IM  uint32_t  RESERVED;
  __OM  uint32_t  TASKS_HFCLKAUDIOSTART;        /*!< (@ 0x00000018) Start HFCLKAUDIO source                                    */
  __OM  uint32_t  TASKS_HFCLKAUDIOSTOP;         /*!< (@ 0x0000001C) Stop HFCLKAUDIO source                                     */
  __OM  uint32_t  TASKS_HFCLK192MSTART;         /*!< (@ 0x00000020) Start HFCLK192M source as selected in HFCLK192MSRC         */
  __OM  uint32_t  TASKS_HFCLK192MSTOP;          /*!< (@ 0x00000024) Stop HFCLK192M source                                      */
  __IM  uint32_t  RESERVED1[22];
  __IOM uint32_t  SUBSCRIBE_HFCLKSTART;         /*!< (@ 0x00000080) Subscribe configuration for task HFCLKSTART                */
  __IOM uint32_t  SUBSCRIBE_HFCLKSTOP;          /*!< (@ 0x00000084) Subscribe configuration for task HFCLKSTOP                 */
  __IOM uint32_t  SUBSCRIBE_LFCLKSTART;         /*!< (@ 0x00000088) Subscribe configuration for task LFCLKSTART                */
  __IOM uint32_t  SUBSCRIBE_LFCLKSTOP;          /*!< (@ 0x0000008C) Subscribe configuration for task LFCLKSTOP                 */
  __IOM uint32_t  SUBSCRIBE_CAL;                /*!< (@ 0x00000090) Subscribe configuration for task CAL                       */
  __IM  uint32_t  RESERVED2;
  __IOM uint32_t  SUBSCRIBE_HFCLKAUDIOSTART;    /*!< (@ 0x00000098) Subscribe configuration for task HFCLKAUDIOSTART           */
  __IOM uint32_t  SUBSCRIBE_HFCLKAUDIOSTOP;     /*!< (@ 0x0000009C) Subscribe configuration for task HFCLKAUDIOSTOP            */
  __IOM uint32_t  SUBSCRIBE_HFCLK192MSTART;     /*!< (@ 0x000000A0) Subscribe configuration for task HFCLK192MSTART            */
  __IOM uint32_t  SUBSCRIBE_HFCLK192MSTOP;      /*!< (@ 0x000000A4) Subscribe configuration for task HFCLK192MSTOP             */
  __IM  uint32_t  RESERVED3[22];
  __IOM uint32_t  EVENTS_HFCLKSTARTED;          /*!< (@ 0x00000100) HFCLK128M/HFCLK64M source started                          */
  __IOM uint32_t  EVENTS_LFCLKSTARTED;          /*!< (@ 0x00000104) LFCLK source started                                       */
  __IM  uint32_t  RESERVED4[5];
  __IOM uint32_t  EVENTS_DONE;                  /*!< (@ 0x0000011C) Calibration of LFRC oscillator complete event              */
  __IOM uint32_t  EVENTS_HFCLKAUDIOSTARTED;     /*!< (@ 0x00000120) HFCLKAUDIO source started                                  */
  __IOM uint32_t  EVENTS_HFCLK192MSTARTED;      /*!< (@ 0x00000124) HFCLK192M source started                                   */
  __IM  uint32_t  RESERVED5[22];
  __IOM uint32_t  PUBLISH_HFCLKSTARTED;         /*!< (@ 0x00000180) Publish configuration for event HFCLKSTARTED               */
  __IOM uint32_t  PUBLISH_LFCLKSTARTED;         /*!< (@ 0x00000184) Publish configuration for event LFCLKSTARTED               */
  __IM  uint32_t  RESERVED6[5];
  __IOM uint32_t  PUBLISH_DONE;                 /*!< (@ 0x0000019C) Publish configuration for event DONE                       */
  __IOM uint32_t  PUBLISH_HFCLKAUDIOSTARTED;    /*!< (@ 0x000001A0) Publish configuration for event HFCLKAUDIOSTARTED          */
  __IOM uint32_t  PUBLISH_HFCLK192MSTARTED;     /*!< (@ 0x000001A4) Publish configuration for event HFCLK192MSTARTED           */
  __IM  uint32_t  RESERVED7[86];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  INTPEND;                      /*!< (@ 0x0000030C) Pending interrupts                                         */
  __IM  uint32_t  RESERVED8[62];
  __IM  uint32_t  HFCLKRUN;                     /*!< (@ 0x00000408) Status indicating that HFCLKSTART task has been
                                                                    triggered                                                  */
  __IM  uint32_t  HFCLKSTAT;                    /*!< (@ 0x0000040C) Status indicating which HFCLK128M/HFCLK64M source
                                                                    is running This register value in any CLOCK
                                                                    instance reflects status only due to configurations/action
                                                                    in that CLOCK instance.                                    */
  __IM  uint32_t  RESERVED9;
  __IM  uint32_t  LFCLKRUN;                     /*!< (@ 0x00000414) Status indicating that LFCLKSTART task has been
                                                                    triggered                                                  */
  __IM  uint32_t  LFCLKSTAT;                    /*!< (@ 0x00000418) Status indicating which LFCLK source is running
                                                                    This register value in any CLOCK instance
                                                                    reflects status only due to configurations/actions
                                                                    in that CLOCK instance.                                    */
  __IM  uint32_t  LFCLKSRCCOPY;                 /*!< (@ 0x0000041C) Copy of LFCLKSRC register, set when LFCLKSTART
                                                                    task was triggered                                         */
  __IM  uint32_t  RESERVED10[12];
  __IM  uint32_t  HFCLKAUDIORUN;                /*!< (@ 0x00000450) Status indicating that HFCLKAUDIOSTART task has
                                                                    been triggered                                             */
  __IM  uint32_t  HFCLKAUDIOSTAT;               /*!< (@ 0x00000454) Status indicating which HFCLKAUDIO source is
                                                                    running                                                    */
  __IM  uint32_t  HFCLK192MRUN;                 /*!< (@ 0x00000458) Status indicating that HFCLK192MSTART task has
                                                                    been triggered                                             */
  __IM  uint32_t  HFCLK192MSTAT;                /*!< (@ 0x0000045C) Status indicating which HFCLK192M source is running        */
  __IM  uint32_t  RESERVED11[45];
  __IOM uint32_t  HFCLKSRC;                     /*!< (@ 0x00000514) Clock source for HFCLK128M/HFCLK64M                        */
  __IOM uint32_t  LFCLKSRC;                     /*!< (@ 0x00000518) Clock source for LFCLK                                     */
  __IM  uint32_t  RESERVED12[15];
  __IOM uint32_t  HFCLKCTRL;                    /*!< (@ 0x00000558) HFCLK128M frequency configuration                          */
  __IOM CLOCK_HFCLKAUDIO_Type HFCLKAUDIO;       /*!< (@ 0x0000055C) Unspecified                                                */
  __IM  uint32_t  RESERVED13[4];
  __IOM uint32_t  HFCLKALWAYSRUN;               /*!< (@ 0x00000570) Automatic or manual control of HFCLK128M/HFCLK64M          */
  __IOM uint32_t  LFCLKALWAYSRUN;               /*!< (@ 0x00000574) Automatic or manual control of LFCLK                       */
  __IM  uint32_t  RESERVED14;
  __IOM uint32_t  HFCLKAUDIOALWAYSRUN;          /*!< (@ 0x0000057C) Automatic or manual control of HFCLKAUDIO                  */
  __IOM uint32_t  HFCLK192MSRC;                 /*!< (@ 0x00000580) Clock source for HFCLK192M                                 */
  __IOM uint32_t  HFCLK192MALWAYSRUN;           /*!< (@ 0x00000584) Automatic or manual control of HFCLK192M                   */
  __IM  uint32_t  RESERVED15[12];
  __IOM uint32_t  HFCLK192MCTRL;                /*!< (@ 0x000005B8) HFCLK192M frequency configuration                          */
} NRF_CLOCK_Type;                               /*!< Size = 1468 (0x5bc)                                                       */


/* Peripheral: CLOCK */
/* Description: Clock management 0 */

/* Register: CLOCK_TASKS_HFCLKSTART */
/* Description: Start HFCLK128M/HFCLK64M source as selected in HFCLKSRC */

/* Bit 0 : Start HFCLK128M/HFCLK64M source as selected in HFCLKSRC */
#define CLOCK_TASKS_HFCLKSTART_TASKS_HFCLKSTART_Pos (0UL) /*!< Position of TASKS_HFCLKSTART field. */
#define CLOCK_TASKS_HFCLKSTART_TASKS_HFCLKSTART_Msk (0x1UL << CLOCK_TASKS_HFCLKSTART_TASKS_HFCLKSTART_Pos) /*!< Bit mask of TASKS_HFCLKSTART field. */
#define CLOCK_TASKS_HFCLKSTART_TASKS_HFCLKSTART_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_HFCLKSTOP */
/* Description: Stop HFCLK128M/HFCLK64M source */

/* Bit 0 : Stop HFCLK128M/HFCLK64M source */
#define CLOCK_TASKS_HFCLKSTOP_TASKS_HFCLKSTOP_Pos (0UL) /*!< Position of TASKS_HFCLKSTOP field. */
#define CLOCK_TASKS_HFCLKSTOP_TASKS_HFCLKSTOP_Msk (0x1UL << CLOCK_TASKS_HFCLKSTOP_TASKS_HFCLKSTOP_Pos) /*!< Bit mask of TASKS_HFCLKSTOP field. */
#define CLOCK_TASKS_HFCLKSTOP_TASKS_HFCLKSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_LFCLKSTART */
/* Description: Start LFCLK source as selected in LFCLKSRC */

/* Bit 0 : Start LFCLK source as selected in LFCLKSRC */
#define CLOCK_TASKS_LFCLKSTART_TASKS_LFCLKSTART_Pos (0UL) /*!< Position of TASKS_LFCLKSTART field. */
#define CLOCK_TASKS_LFCLKSTART_TASKS_LFCLKSTART_Msk (0x1UL << CLOCK_TASKS_LFCLKSTART_TASKS_LFCLKSTART_Pos) /*!< Bit mask of TASKS_LFCLKSTART field. */
#define CLOCK_TASKS_LFCLKSTART_TASKS_LFCLKSTART_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_LFCLKSTOP */
/* Description: Stop LFCLK source */

/* Bit 0 : Stop LFCLK source */
#define CLOCK_TASKS_LFCLKSTOP_TASKS_LFCLKSTOP_Pos (0UL) /*!< Position of TASKS_LFCLKSTOP field. */
#define CLOCK_TASKS_LFCLKSTOP_TASKS_LFCLKSTOP_Msk (0x1UL << CLOCK_TASKS_LFCLKSTOP_TASKS_LFCLKSTOP_Pos) /*!< Bit mask of TASKS_LFCLKSTOP field. */
#define CLOCK_TASKS_LFCLKSTOP_TASKS_LFCLKSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_CAL */
/* Description: Start calibration of LFRC oscillator */

/* Bit 0 : Start calibration of LFRC oscillator */
#define CLOCK_TASKS_CAL_TASKS_CAL_Pos (0UL) /*!< Position of TASKS_CAL field. */
#define CLOCK_TASKS_CAL_TASKS_CAL_Msk (0x1UL << CLOCK_TASKS_CAL_TASKS_CAL_Pos) /*!< Bit mask of TASKS_CAL field. */
#define CLOCK_TASKS_CAL_TASKS_CAL_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_HFCLKAUDIOSTART */
/* Description: Start HFCLKAUDIO source */

/* Bit 0 : Start HFCLKAUDIO source */
#define CLOCK_TASKS_HFCLKAUDIOSTART_TASKS_HFCLKAUDIOSTART_Pos (0UL) /*!< Position of TASKS_HFCLKAUDIOSTART field. */
#define CLOCK_TASKS_HFCLKAUDIOSTART_TASKS_HFCLKAUDIOSTART_Msk (0x1UL << CLOCK_TASKS_HFCLKAUDIOSTART_TASKS_HFCLKAUDIOSTART_Pos) /*!< Bit mask of TASKS_HFCLKAUDIOSTART field. */
#define CLOCK_TASKS_HFCLKAUDIOSTART_TASKS_HFCLKAUDIOSTART_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_HFCLKAUDIOSTOP */
/* Description: Stop HFCLKAUDIO source */

/* Bit 0 : Stop HFCLKAUDIO source */
#define CLOCK_TASKS_HFCLKAUDIOSTOP_TASKS_HFCLKAUDIOSTOP_Pos (0UL) /*!< Position of TASKS_HFCLKAUDIOSTOP field. */
#define CLOCK_TASKS_HFCLKAUDIOSTOP_TASKS_HFCLKAUDIOSTOP_Msk (0x1UL << CLOCK_TASKS_HFCLKAUDIOSTOP_TASKS_HFCLKAUDIOSTOP_Pos) /*!< Bit mask of TASKS_HFCLKAUDIOSTOP field. */
#define CLOCK_TASKS_HFCLKAUDIOSTOP_TASKS_HFCLKAUDIOSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_HFCLK192MSTART */
/* Description: Start HFCLK192M source as selected in HFCLK192MSRC */

/* Bit 0 : Start HFCLK192M source as selected in HFCLK192MSRC */
#define CLOCK_TASKS_HFCLK192MSTART_TASKS_HFCLK192MSTART_Pos (0UL) /*!< Position of TASKS_HFCLK192MSTART field. */
#define CLOCK_TASKS_HFCLK192MSTART_TASKS_HFCLK192MSTART_Msk (0x1UL << CLOCK_TASKS_HFCLK192MSTART_TASKS_HFCLK192MSTART_Pos) /*!< Bit mask of TASKS_HFCLK192MSTART field. */
#define CLOCK_TASKS_HFCLK192MSTART_TASKS_HFCLK192MSTART_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_TASKS_HFCLK192MSTOP */
/* Description: Stop HFCLK192M source */

/* Bit 0 : Stop HFCLK192M source */
#define CLOCK_TASKS_HFCLK192MSTOP_TASKS_HFCLK192MSTOP_Pos (0UL) /*!< Position of TASKS_HFCLK192MSTOP field. */
#define CLOCK_TASKS_HFCLK192MSTOP_TASKS_HFCLK192MSTOP_Msk (0x1UL << CLOCK_TASKS_HFCLK192MSTOP_TASKS_HFCLK192MSTOP_Pos) /*!< Bit mask of TASKS_HFCLK192MSTOP field. */
#define CLOCK_TASKS_HFCLK192MSTOP_TASKS_HFCLK192MSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: CLOCK_SUBSCRIBE_HFCLKSTART */
/* Description: Subscribe configuration for task HFCLKSTART */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLKSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKSTART_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLKSTART_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLKSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLKSTART will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLKSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLKSTART_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLKSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_HFCLKSTOP */
/* Description: Subscribe configuration for task HFCLKSTOP */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLKSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLKSTOP will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLKSTOP_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLKSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_LFCLKSTART */
/* Description: Subscribe configuration for task LFCLKSTART */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_LFCLKSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_LFCLKSTART_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_LFCLKSTART_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_LFCLKSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_LFCLKSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task LFCLKSTART will subscribe to */
#define CLOCK_SUBSCRIBE_LFCLKSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_LFCLKSTART_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_LFCLKSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_LFCLKSTOP */
/* Description: Subscribe configuration for task LFCLKSTOP */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_LFCLKSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task LFCLKSTOP will subscribe to */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_LFCLKSTOP_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_LFCLKSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_CAL */
/* Description: Subscribe configuration for task CAL */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_CAL_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_CAL_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_CAL_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_CAL_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_CAL_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CAL will subscribe to */
#define CLOCK_SUBSCRIBE_CAL_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_CAL_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_CAL_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_HFCLKAUDIOSTART */
/* Description: Subscribe configuration for task HFCLKAUDIOSTART */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLKAUDIOSTART will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLKAUDIOSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP */
/* Description: Subscribe configuration for task HFCLKAUDIOSTOP */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLKAUDIOSTOP will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLKAUDIOSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_HFCLK192MSTART */
/* Description: Subscribe configuration for task HFCLK192MSTART */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLK192MSTART_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLK192MSTART will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTART_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLK192MSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_SUBSCRIBE_HFCLK192MSTOP */
/* Description: Subscribe configuration for task HFCLK192MSTOP */

/* Bit 31 :   */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_EN_Msk (0x1UL << CLOCK_SUBSCRIBE_HFCLK192MSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task HFCLK192MSTOP will subscribe to */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_SUBSCRIBE_HFCLK192MSTOP_CHIDX_Msk (0xFFUL << CLOCK_SUBSCRIBE_HFCLK192MSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_EVENTS_HFCLKSTARTED */
/* Description: HFCLK128M/HFCLK64M source started */

/* Bit 0 : HFCLK128M/HFCLK64M source started */
#define CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED_Pos (0UL) /*!< Position of EVENTS_HFCLKSTARTED field. */
#define CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED_Msk (0x1UL << CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED_Pos) /*!< Bit mask of EVENTS_HFCLKSTARTED field. */
#define CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED_NotGenerated (0UL) /*!< Event not generated */
#define CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED_Generated (1UL) /*!< Event generated */

/* Register: CLOCK_EVENTS_LFCLKSTARTED */
/* Description: LFCLK source started */

/* Bit 0 : LFCLK source started */
#define CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED_Pos (0UL) /*!< Position of EVENTS_LFCLKSTARTED field. */
#define CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED_Msk (0x1UL << CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED_Pos) /*!< Bit mask of EVENTS_LFCLKSTARTED field. */
#define CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED_NotGenerated (0UL) /*!< Event not generated */
#define CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED_Generated (1UL) /*!< Event generated */

/* Register: CLOCK_EVENTS_DONE */
/* Description: Calibration of LFRC oscillator complete event */

/* Bit 0 : Calibration of LFRC oscillator complete event */
#define CLOCK_EVENTS_DONE_EVENTS_DONE_Pos (0UL) /*!< Position of EVENTS_DONE field. */
#define CLOCK_EVENTS_DONE_EVENTS_DONE_Msk (0x1UL << CLOCK_EVENTS_DONE_EVENTS_DONE_Pos) /*!< Bit mask of EVENTS_DONE field. */
#define CLOCK_EVENTS_DONE_EVENTS_DONE_NotGenerated (0UL) /*!< Event not generated */
#define CLOCK_EVENTS_DONE_EVENTS_DONE_Generated (1UL) /*!< Event generated */

/* Register: CLOCK_EVENTS_HFCLKAUDIOSTARTED */
/* Description: HFCLKAUDIO source started */

/* Bit 0 : HFCLKAUDIO source started */
#define CLOCK_EVENTS_HFCLKAUDIOSTARTED_EVENTS_HFCLKAUDIOSTARTED_Pos (0UL) /*!< Position of EVENTS_HFCLKAUDIOSTARTED field. */
#define CLOCK_EVENTS_HFCLKAUDIOSTARTED_EVENTS_HFCLKAUDIOSTARTED_Msk (0x1UL << CLOCK_EVENTS_HFCLKAUDIOSTARTED_EVENTS_HFCLKAUDIOSTARTED_Pos) /*!< Bit mask of EVENTS_HFCLKAUDIOSTARTED field. */
#define CLOCK_EVENTS_HFCLKAUDIOSTARTED_EVENTS_HFCLKAUDIOSTARTED_NotGenerated (0UL) /*!< Event not generated */
#define CLOCK_EVENTS_HFCLKAUDIOSTARTED_EVENTS_HFCLKAUDIOSTARTED_Generated (1UL) /*!< Event generated */

/* Register: CLOCK_EVENTS_HFCLK192MSTARTED */
/* Description: HFCLK192M source started */

/* Bit 0 : HFCLK192M source started */
#define CLOCK_EVENTS_HFCLK192MSTARTED_EVENTS_HFCLK192MSTARTED_Pos (0UL) /*!< Position of EVENTS_HFCLK192MSTARTED field. */
#define CLOCK_EVENTS_HFCLK192MSTARTED_EVENTS_HFCLK192MSTARTED_Msk (0x1UL << CLOCK_EVENTS_HFCLK192MSTARTED_EVENTS_HFCLK192MSTARTED_Pos) /*!< Bit mask of EVENTS_HFCLK192MSTARTED field. */
#define CLOCK_EVENTS_HFCLK192MSTARTED_EVENTS_HFCLK192MSTARTED_NotGenerated (0UL) /*!< Event not generated */
#define CLOCK_EVENTS_HFCLK192MSTARTED_EVENTS_HFCLK192MSTARTED_Generated (1UL) /*!< Event generated */

/* Register: CLOCK_PUBLISH_HFCLKSTARTED */
/* Description: Publish configuration for event HFCLKSTARTED */

/* Bit 31 :   */
#define CLOCK_PUBLISH_HFCLKSTARTED_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_PUBLISH_HFCLKSTARTED_EN_Msk (0x1UL << CLOCK_PUBLISH_HFCLKSTARTED_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_PUBLISH_HFCLKSTARTED_EN_Disabled (0UL) /*!< Disable publishing */
#define CLOCK_PUBLISH_HFCLKSTARTED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event HFCLKSTARTED will publish to. */
#define CLOCK_PUBLISH_HFCLKSTARTED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_PUBLISH_HFCLKSTARTED_CHIDX_Msk (0xFFUL << CLOCK_PUBLISH_HFCLKSTARTED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_PUBLISH_LFCLKSTARTED */
/* Description: Publish configuration for event LFCLKSTARTED */

/* Bit 31 :   */
#define CLOCK_PUBLISH_LFCLKSTARTED_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_PUBLISH_LFCLKSTARTED_EN_Msk (0x1UL << CLOCK_PUBLISH_LFCLKSTARTED_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_PUBLISH_LFCLKSTARTED_EN_Disabled (0UL) /*!< Disable publishing */
#define CLOCK_PUBLISH_LFCLKSTARTED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event LFCLKSTARTED will publish to. */
#define CLOCK_PUBLISH_LFCLKSTARTED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_PUBLISH_LFCLKSTARTED_CHIDX_Msk (0xFFUL << CLOCK_PUBLISH_LFCLKSTARTED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_PUBLISH_DONE */
/* Description: Publish configuration for event DONE */

/* Bit 31 :   */
#define CLOCK_PUBLISH_DONE_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_PUBLISH_DONE_EN_Msk (0x1UL << CLOCK_PUBLISH_DONE_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_PUBLISH_DONE_EN_Disabled (0UL) /*!< Disable publishing */
#define CLOCK_PUBLISH_DONE_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event DONE will publish to. */
#define CLOCK_PUBLISH_DONE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_PUBLISH_DONE_CHIDX_Msk (0xFFUL << CLOCK_PUBLISH_DONE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_PUBLISH_HFCLKAUDIOSTARTED */
/* Description: Publish configuration for event HFCLKAUDIOSTARTED */

/* Bit 31 :   */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_EN_Msk (0x1UL << CLOCK_PUBLISH_HFCLKAUDIOSTARTED_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_EN_Disabled (0UL) /*!< Disable publishing */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event HFCLKAUDIOSTARTED will publish to. */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_PUBLISH_HFCLKAUDIOSTARTED_CHIDX_Msk (0xFFUL << CLOCK_PUBLISH_HFCLKAUDIOSTARTED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_PUBLISH_HFCLK192MSTARTED */
/* Description: Publish configuration for event HFCLK192MSTARTED */

/* Bit 31 :   */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_EN_Pos (31UL) /*!< Position of EN field. */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_EN_Msk (0x1UL << CLOCK_PUBLISH_HFCLK192MSTARTED_EN_Pos) /*!< Bit mask of EN field. */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_EN_Disabled (0UL) /*!< Disable publishing */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event HFCLK192MSTARTED will publish to. */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CLOCK_PUBLISH_HFCLK192MSTARTED_CHIDX_Msk (0xFFUL << CLOCK_PUBLISH_HFCLK192MSTARTED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CLOCK_INTEN */
/* Description: Enable or disable interrupt */

/* Bit 9 : Enable or disable interrupt for event HFCLK192MSTARTED */
#define CLOCK_INTEN_HFCLK192MSTARTED_Pos (9UL) /*!< Position of HFCLK192MSTARTED field. */
#define CLOCK_INTEN_HFCLK192MSTARTED_Msk (0x1UL << CLOCK_INTEN_HFCLK192MSTARTED_Pos) /*!< Bit mask of HFCLK192MSTARTED field. */
#define CLOCK_INTEN_HFCLK192MSTARTED_Disabled (0UL) /*!< Disable */
#define CLOCK_INTEN_HFCLK192MSTARTED_Enabled (1UL) /*!< Enable */

/* Bit 8 : Enable or disable interrupt for event HFCLKAUDIOSTARTED */
#define CLOCK_INTEN_HFCLKAUDIOSTARTED_Pos (8UL) /*!< Position of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTEN_HFCLKAUDIOSTARTED_Msk (0x1UL << CLOCK_INTEN_HFCLKAUDIOSTARTED_Pos) /*!< Bit mask of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTEN_HFCLKAUDIOSTARTED_Disabled (0UL) /*!< Disable */
#define CLOCK_INTEN_HFCLKAUDIOSTARTED_Enabled (1UL) /*!< Enable */

/* Bit 7 : Enable or disable interrupt for event DONE */
#define CLOCK_INTEN_DONE_Pos (7UL) /*!< Position of DONE field. */
#define CLOCK_INTEN_DONE_Msk (0x1UL << CLOCK_INTEN_DONE_Pos) /*!< Bit mask of DONE field. */
#define CLOCK_INTEN_DONE_Disabled (0UL) /*!< Disable */
#define CLOCK_INTEN_DONE_Enabled (1UL) /*!< Enable */

/* Bit 1 : Enable or disable interrupt for event LFCLKSTARTED */
#define CLOCK_INTEN_LFCLKSTARTED_Pos (1UL) /*!< Position of LFCLKSTARTED field. */
#define CLOCK_INTEN_LFCLKSTARTED_Msk (0x1UL << CLOCK_INTEN_LFCLKSTARTED_Pos) /*!< Bit mask of LFCLKSTARTED field. */
#define CLOCK_INTEN_LFCLKSTARTED_Disabled (0UL) /*!< Disable */
#define CLOCK_INTEN_LFCLKSTARTED_Enabled (1UL) /*!< Enable */

/* Bit 0 : Enable or disable interrupt for event HFCLKSTARTED */
#define CLOCK_INTEN_HFCLKSTARTED_Pos (0UL) /*!< Position of HFCLKSTARTED field. */
#define CLOCK_INTEN_HFCLKSTARTED_Msk (0x1UL << CLOCK_INTEN_HFCLKSTARTED_Pos) /*!< Bit mask of HFCLKSTARTED field. */
#define CLOCK_INTEN_HFCLKSTARTED_Disabled (0UL) /*!< Disable */
#define CLOCK_INTEN_HFCLKSTARTED_Enabled (1UL) /*!< Enable */

/* Register: CLOCK_INTENSET */
/* Description: Enable interrupt */

/* Bit 9 : Write '1' to enable interrupt for event HFCLK192MSTARTED */
#define CLOCK_INTENSET_HFCLK192MSTARTED_Pos (9UL) /*!< Position of HFCLK192MSTARTED field. */
#define CLOCK_INTENSET_HFCLK192MSTARTED_Msk (0x1UL << CLOCK_INTENSET_HFCLK192MSTARTED_Pos) /*!< Bit mask of HFCLK192MSTARTED field. */
#define CLOCK_INTENSET_HFCLK192MSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENSET_HFCLK192MSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENSET_HFCLK192MSTARTED_Set (1UL) /*!< Enable */

/* Bit 8 : Write '1' to enable interrupt for event HFCLKAUDIOSTARTED */
#define CLOCK_INTENSET_HFCLKAUDIOSTARTED_Pos (8UL) /*!< Position of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTENSET_HFCLKAUDIOSTARTED_Msk (0x1UL << CLOCK_INTENSET_HFCLKAUDIOSTARTED_Pos) /*!< Bit mask of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTENSET_HFCLKAUDIOSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENSET_HFCLKAUDIOSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENSET_HFCLKAUDIOSTARTED_Set (1UL) /*!< Enable */

/* Bit 7 : Write '1' to enable interrupt for event DONE */
#define CLOCK_INTENSET_DONE_Pos (7UL) /*!< Position of DONE field. */
#define CLOCK_INTENSET_DONE_Msk (0x1UL << CLOCK_INTENSET_DONE_Pos) /*!< Bit mask of DONE field. */
#define CLOCK_INTENSET_DONE_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENSET_DONE_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENSET_DONE_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event LFCLKSTARTED */
#define CLOCK_INTENSET_LFCLKSTARTED_Pos (1UL) /*!< Position of LFCLKSTARTED field. */
#define CLOCK_INTENSET_LFCLKSTARTED_Msk (0x1UL << CLOCK_INTENSET_LFCLKSTARTED_Pos) /*!< Bit mask of LFCLKSTARTED field. */
#define CLOCK_INTENSET_LFCLKSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENSET_LFCLKSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENSET_LFCLKSTARTED_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event HFCLKSTARTED */
#define CLOCK_INTENSET_HFCLKSTARTED_Pos (0UL) /*!< Position of HFCLKSTARTED field. */
#define CLOCK_INTENSET_HFCLKSTARTED_Msk (0x1UL << CLOCK_INTENSET_HFCLKSTARTED_Pos) /*!< Bit mask of HFCLKSTARTED field. */
#define CLOCK_INTENSET_HFCLKSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENSET_HFCLKSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENSET_HFCLKSTARTED_Set (1UL) /*!< Enable */

/* Register: CLOCK_INTENCLR */
/* Description: Disable interrupt */

/* Bit 9 : Write '1' to disable interrupt for event HFCLK192MSTARTED */
#define CLOCK_INTENCLR_HFCLK192MSTARTED_Pos (9UL) /*!< Position of HFCLK192MSTARTED field. */
#define CLOCK_INTENCLR_HFCLK192MSTARTED_Msk (0x1UL << CLOCK_INTENCLR_HFCLK192MSTARTED_Pos) /*!< Bit mask of HFCLK192MSTARTED field. */
#define CLOCK_INTENCLR_HFCLK192MSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENCLR_HFCLK192MSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENCLR_HFCLK192MSTARTED_Clear (1UL) /*!< Disable */

/* Bit 8 : Write '1' to disable interrupt for event HFCLKAUDIOSTARTED */
#define CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Pos (8UL) /*!< Position of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Msk (0x1UL << CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Pos) /*!< Bit mask of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENCLR_HFCLKAUDIOSTARTED_Clear (1UL) /*!< Disable */

/* Bit 7 : Write '1' to disable interrupt for event DONE */
#define CLOCK_INTENCLR_DONE_Pos (7UL) /*!< Position of DONE field. */
#define CLOCK_INTENCLR_DONE_Msk (0x1UL << CLOCK_INTENCLR_DONE_Pos) /*!< Bit mask of DONE field. */
#define CLOCK_INTENCLR_DONE_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENCLR_DONE_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENCLR_DONE_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable interrupt for event LFCLKSTARTED */
#define CLOCK_INTENCLR_LFCLKSTARTED_Pos (1UL) /*!< Position of LFCLKSTARTED field. */
#define CLOCK_INTENCLR_LFCLKSTARTED_Msk (0x1UL << CLOCK_INTENCLR_LFCLKSTARTED_Pos) /*!< Bit mask of LFCLKSTARTED field. */
#define CLOCK_INTENCLR_LFCLKSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENCLR_LFCLKSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENCLR_LFCLKSTARTED_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event HFCLKSTARTED */
#define CLOCK_INTENCLR_HFCLKSTARTED_Pos (0UL) /*!< Position of HFCLKSTARTED field. */
#define CLOCK_INTENCLR_HFCLKSTARTED_Msk (0x1UL << CLOCK_INTENCLR_HFCLKSTARTED_Pos) /*!< Bit mask of HFCLKSTARTED field. */
#define CLOCK_INTENCLR_HFCLKSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define CLOCK_INTENCLR_HFCLKSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define CLOCK_INTENCLR_HFCLKSTARTED_Clear (1UL) /*!< Disable */

/* Register: CLOCK_INTPEND */
/* Description: Pending interrupts */

/* Bit 9 : Read pending status of interrupt for event HFCLK192MSTARTED */
#define CLOCK_INTPEND_HFCLK192MSTARTED_Pos (9UL) /*!< Position of HFCLK192MSTARTED field. */
#define CLOCK_INTPEND_HFCLK192MSTARTED_Msk (0x1UL << CLOCK_INTPEND_HFCLK192MSTARTED_Pos) /*!< Bit mask of HFCLK192MSTARTED field. */
#define CLOCK_INTPEND_HFCLK192MSTARTED_NotPending (0UL) /*!< Read: Not pending */
#define CLOCK_INTPEND_HFCLK192MSTARTED_Pending (1UL) /*!< Read: Pending */

/* Bit 8 : Read pending status of interrupt for event HFCLKAUDIOSTARTED */
#define CLOCK_INTPEND_HFCLKAUDIOSTARTED_Pos (8UL) /*!< Position of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTPEND_HFCLKAUDIOSTARTED_Msk (0x1UL << CLOCK_INTPEND_HFCLKAUDIOSTARTED_Pos) /*!< Bit mask of HFCLKAUDIOSTARTED field. */
#define CLOCK_INTPEND_HFCLKAUDIOSTARTED_NotPending (0UL) /*!< Read: Not pending */
#define CLOCK_INTPEND_HFCLKAUDIOSTARTED_Pending (1UL) /*!< Read: Pending */

/* Bit 7 : Read pending status of interrupt for event DONE */
#define CLOCK_INTPEND_DONE_Pos (7UL) /*!< Position of DONE field. */
#define CLOCK_INTPEND_DONE_Msk (0x1UL << CLOCK_INTPEND_DONE_Pos) /*!< Bit mask of DONE field. */
#define CLOCK_INTPEND_DONE_NotPending (0UL) /*!< Read: Not pending */
#define CLOCK_INTPEND_DONE_Pending (1UL) /*!< Read: Pending */

/* Bit 1 : Read pending status of interrupt for event LFCLKSTARTED */
#define CLOCK_INTPEND_LFCLKSTARTED_Pos (1UL) /*!< Position of LFCLKSTARTED field. */
#define CLOCK_INTPEND_LFCLKSTARTED_Msk (0x1UL << CLOCK_INTPEND_LFCLKSTARTED_Pos) /*!< Bit mask of LFCLKSTARTED field. */
#define CLOCK_INTPEND_LFCLKSTARTED_NotPending (0UL) /*!< Read: Not pending */
#define CLOCK_INTPEND_LFCLKSTARTED_Pending (1UL) /*!< Read: Pending */

/* Bit 0 : Read pending status of interrupt for event HFCLKSTARTED */
#define CLOCK_INTPEND_HFCLKSTARTED_Pos (0UL) /*!< Position of HFCLKSTARTED field. */
#define CLOCK_INTPEND_HFCLKSTARTED_Msk (0x1UL << CLOCK_INTPEND_HFCLKSTARTED_Pos) /*!< Bit mask of HFCLKSTARTED field. */
#define CLOCK_INTPEND_HFCLKSTARTED_NotPending (0UL) /*!< Read: Not pending */
#define CLOCK_INTPEND_HFCLKSTARTED_Pending (1UL) /*!< Read: Pending */

/* Register: CLOCK_HFCLKRUN */
/* Description: Status indicating that HFCLKSTART task has been triggered */

/* Bit 0 : HFCLKSTART task triggered or not */
#define CLOCK_HFCLKRUN_STATUS_Pos (0UL) /*!< Position of STATUS field. */
#define CLOCK_HFCLKRUN_STATUS_Msk (0x1UL << CLOCK_HFCLKRUN_STATUS_Pos) /*!< Bit mask of STATUS field. */
#define CLOCK_HFCLKRUN_STATUS_NotTriggered (0UL) /*!< Task not triggered */
#define CLOCK_HFCLKRUN_STATUS_Triggered (1UL) /*!< Task triggered */

/* Register: CLOCK_HFCLKSTAT */
/* Description: Status indicating which HFCLK128M/HFCLK64M source is running This register value in any CLOCK instance reflects status only due to configurations/actions in that CLOCK instance. */

/* Bit 16 : HFCLK state */
#define CLOCK_HFCLKSTAT_STATE_Pos (16UL) /*!< Position of STATE field. */
#define CLOCK_HFCLKSTAT_STATE_Msk (0x1UL << CLOCK_HFCLKSTAT_STATE_Pos) /*!< Bit mask of STATE field. */
#define CLOCK_HFCLKSTAT_STATE_NotRunning (0UL) /*!< HFCLK not running */
#define CLOCK_HFCLKSTAT_STATE_Running (1UL) /*!< HFCLK running */

/* Bit 4 : ALWAYSRUN activated */
#define CLOCK_HFCLKSTAT_ALWAYSRUNNING_Pos (4UL) /*!< Position of ALWAYSRUNNING field. */
#define CLOCK_HFCLKSTAT_ALWAYSRUNNING_Msk (0x1UL << CLOCK_HFCLKSTAT_ALWAYSRUNNING_Pos) /*!< Bit mask of ALWAYSRUNNING field. */
#define CLOCK_HFCLKSTAT_ALWAYSRUNNING_NotRunning (0UL) /*!< Automatic clock control enabled */
#define CLOCK_HFCLKSTAT_ALWAYSRUNNING_Running (1UL) /*!< Oscillator is always running */

/* Bit 0 : Active clock source */
#define CLOCK_HFCLKSTAT_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_HFCLKSTAT_SRC_Msk (0x1UL << CLOCK_HFCLKSTAT_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_HFCLKSTAT_SRC_HFINT (0UL) /*!< Clock source: HFINT - 128 MHz on-chip oscillator */
#define CLOCK_HFCLKSTAT_SRC_HFXO (1UL) /*!< Clock source: HFXO - 128 MHz clock derived from external 32 MHz crystal oscillator */

/* Register: CLOCK_LFCLKRUN */
/* Description: Status indicating that LFCLKSTART task has been triggered */

/* Bit 0 : LFCLKSTART task triggered or not */
#define CLOCK_LFCLKRUN_STATUS_Pos (0UL) /*!< Position of STATUS field. */
#define CLOCK_LFCLKRUN_STATUS_Msk (0x1UL << CLOCK_LFCLKRUN_STATUS_Pos) /*!< Bit mask of STATUS field. */
#define CLOCK_LFCLKRUN_STATUS_NotTriggered (0UL) /*!< Task not triggered */
#define CLOCK_LFCLKRUN_STATUS_Triggered (1UL) /*!< Task triggered */

/* Register: CLOCK_LFCLKSTAT */
/* Description: Status indicating which LFCLK source is running This register value in any CLOCK instance reflects status only due to configurations/actions in that CLOCK instance. */

/* Bit 16 : LFCLK state */
#define CLOCK_LFCLKSTAT_STATE_Pos (16UL) /*!< Position of STATE field. */
#define CLOCK_LFCLKSTAT_STATE_Msk (0x1UL << CLOCK_LFCLKSTAT_STATE_Pos) /*!< Bit mask of STATE field. */
#define CLOCK_LFCLKSTAT_STATE_NotRunning (0UL) /*!< LFCLK not running */
#define CLOCK_LFCLKSTAT_STATE_Running (1UL) /*!< LFCLK running */

/* Bit 4 : ALWAYSRUN activated */
#define CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos (4UL) /*!< Position of ALWAYSRUNNING field. */
#define CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk (0x1UL << CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos) /*!< Bit mask of ALWAYSRUNNING field. */
#define CLOCK_LFCLKSTAT_ALWAYSRUNNING_NotRunning (0UL) /*!< Automatic clock control enabled */
#define CLOCK_LFCLKSTAT_ALWAYSRUNNING_Running (1UL) /*!< Oscillator is always running */

/* Bits 1..0 : Active clock source */
#define CLOCK_LFCLKSTAT_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_LFCLKSTAT_SRC_Msk (0x3UL << CLOCK_LFCLKSTAT_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_LFCLKSTAT_SRC_LFRC (1UL) /*!< 32.768 kHz RC oscillator */
#define CLOCK_LFCLKSTAT_SRC_LFXO (2UL) /*!< 32.768 kHz crystal oscillator */
#define CLOCK_LFCLKSTAT_SRC_LFSYNT (3UL) /*!< 32.768 kHz synthesized from HFCLK */

/* Register: CLOCK_LFCLKSRCCOPY */
/* Description: Copy of LFCLKSRC register, set when LFCLKSTART task was triggered */

/* Bits 1..0 : Clock source */
#define CLOCK_LFCLKSRCCOPY_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_LFCLKSRCCOPY_SRC_Msk (0x3UL << CLOCK_LFCLKSRCCOPY_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_LFCLKSRCCOPY_SRC_LFRC (1UL) /*!< 32.768 kHz RC oscillator */
#define CLOCK_LFCLKSRCCOPY_SRC_LFXO (2UL) /*!< 32.768 kHz crystal oscillator */
#define CLOCK_LFCLKSRCCOPY_SRC_LFSYNT (3UL) /*!< 32.768 kHz synthesized from HFCLK */

/* Register: CLOCK_HFCLKAUDIORUN */
/* Description: Status indicating that HFCLKAUDIOSTART task has been triggered */

/* Bit 0 : HFCLKAUDIOSTART task triggered or not */
#define CLOCK_HFCLKAUDIORUN_STATUS_Pos (0UL) /*!< Position of STATUS field. */
#define CLOCK_HFCLKAUDIORUN_STATUS_Msk (0x1UL << CLOCK_HFCLKAUDIORUN_STATUS_Pos) /*!< Bit mask of STATUS field. */
#define CLOCK_HFCLKAUDIORUN_STATUS_NotTriggered (0UL) /*!< Task not triggered */
#define CLOCK_HFCLKAUDIORUN_STATUS_Triggered (1UL) /*!< Task triggered */

/* Register: CLOCK_HFCLKAUDIOSTAT */
/* Description: Status indicating which HFCLKAUDIO source is running */

/* Bit 16 : HFCLKAUDIO state */
#define CLOCK_HFCLKAUDIOSTAT_STATE_Pos (16UL) /*!< Position of STATE field. */
#define CLOCK_HFCLKAUDIOSTAT_STATE_Msk (0x1UL << CLOCK_HFCLKAUDIOSTAT_STATE_Pos) /*!< Bit mask of STATE field. */
#define CLOCK_HFCLKAUDIOSTAT_STATE_NotRunning (0UL) /*!< HFCLKAUDIO not running */
#define CLOCK_HFCLKAUDIOSTAT_STATE_Running (1UL) /*!< HFCLKAUDIO running */

/* Bit 4 : ALWAYSRUN activated */
#define CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Pos (4UL) /*!< Position of ALWAYSRUNNING field. */
#define CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Msk (0x1UL << CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Pos) /*!< Bit mask of ALWAYSRUNNING field. */
#define CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_NotRunning (0UL) /*!< Automatic clock control enabled */
#define CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Running (1UL) /*!< Oscillator is always running */

/* Register: CLOCK_HFCLK192MRUN */
/* Description: Status indicating that HFCLK192MSTART task has been triggered */

/* Bit 0 : HFCLK192MSTART task triggered or not */
#define CLOCK_HFCLK192MRUN_STATUS_Pos (0UL) /*!< Position of STATUS field. */
#define CLOCK_HFCLK192MRUN_STATUS_Msk (0x1UL << CLOCK_HFCLK192MRUN_STATUS_Pos) /*!< Bit mask of STATUS field. */
#define CLOCK_HFCLK192MRUN_STATUS_NotTriggered (0UL) /*!< Task not triggered */
#define CLOCK_HFCLK192MRUN_STATUS_Triggered (1UL) /*!< Task triggered */

/* Register: CLOCK_HFCLK192MSTAT */
/* Description: Status indicating which HFCLK192M source is running */

/* Bit 16 : HFCLK192M state */
#define CLOCK_HFCLK192MSTAT_STATE_Pos (16UL) /*!< Position of STATE field. */
#define CLOCK_HFCLK192MSTAT_STATE_Msk (0x1UL << CLOCK_HFCLK192MSTAT_STATE_Pos) /*!< Bit mask of STATE field. */
#define CLOCK_HFCLK192MSTAT_STATE_NotRunning (0UL) /*!< HFCLK192M not running */
#define CLOCK_HFCLK192MSTAT_STATE_Running (1UL) /*!< HFCLK192M running */

/* Bit 4 : ALWAYSRUN activated */
#define CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Pos (4UL) /*!< Position of ALWAYSRUNNING field. */
#define CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Msk (0x1UL << CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Pos) /*!< Bit mask of ALWAYSRUNNING field. */
#define CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_NotRunning (0UL) /*!< Automatic clock control enabled */
#define CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Running (1UL) /*!< Oscillator is always running */

/* Bit 0 : Active clock source */
#define CLOCK_HFCLK192MSTAT_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_HFCLK192MSTAT_SRC_Msk (0x1UL << CLOCK_HFCLK192MSTAT_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_HFCLK192MSTAT_SRC_HFINT (0UL) /*!< Clock source: HFINT - on-chip oscillator */
#define CLOCK_HFCLK192MSTAT_SRC_HFXO (1UL) /*!< Clock source: HFXO - derived from external 32 MHz crystal oscillator */

/* Register: CLOCK_HFCLKSRC */
/* Description: Clock source for HFCLK128M/HFCLK64M */

/* Bit 0 : Select which HFCLK source is started by the HFCLKSTART task */
#define CLOCK_HFCLKSRC_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_HFCLKSRC_SRC_Msk (0x1UL << CLOCK_HFCLKSRC_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_HFCLKSRC_SRC_HFINT (0UL) /*!< HFCLKSTART task starts HFINT oscillator */
#define CLOCK_HFCLKSRC_SRC_HFXO (1UL) /*!< HFCLKSTART task starts HFXO oscillator */

/* Register: CLOCK_LFCLKSRC */
/* Description: Clock source for LFCLK */

/* Bits 1..0 : Select which LFCLK source is started by the LFCLKSTART task */
#define CLOCK_LFCLKSRC_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_LFCLKSRC_SRC_Msk (0x3UL << CLOCK_LFCLKSRC_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_LFCLKSRC_SRC_LFRC (1UL) /*!< 32.768 kHz RC oscillator */
#define CLOCK_LFCLKSRC_SRC_LFXO (2UL) /*!< 32.768 kHz crystal oscillator */
#define CLOCK_LFCLKSRC_SRC_LFSYNT (3UL) /*!< 32.768 kHz synthesized from HFCLK */

/* Register: CLOCK_HFCLKCTRL */
/* Description: HFCLK128M frequency configuration */

/* Bits 1..0 : High frequency clock HCLK */
#define CLOCK_HFCLKCTRL_HCLK_Pos (0UL) /*!< Position of HCLK field. */
#define CLOCK_HFCLKCTRL_HCLK_Msk (0x3UL << CLOCK_HFCLKCTRL_HCLK_Pos) /*!< Bit mask of HCLK field. */
#define CLOCK_HFCLKCTRL_HCLK_Div1 (0UL) /*!< Divide HFCLK by 1 */
#define CLOCK_HFCLKCTRL_HCLK_Div2 (1UL) /*!< Divide HFCLK by 2 */

/* Register: CLOCK_HFCLKAUDIO_FREQUENCY */
/* Description: Audio PLL frequency in 11.176 MHz - 11.402 MHz or 12.165 MHz - 12.411 MHz frequency bands */

/* Bits 15..0 : Frequency 0: 10.666 MHz 65535: 13.333 MHz */
#define CLOCK_HFCLKAUDIO_FREQUENCY_FREQUENCY_Pos (0UL) /*!< Position of FREQUENCY field. */
#define CLOCK_HFCLKAUDIO_FREQUENCY_FREQUENCY_Msk (0xFFFFUL << CLOCK_HFCLKAUDIO_FREQUENCY_FREQUENCY_Pos) /*!< Bit mask of FREQUENCY field. */

/* Register: CLOCK_HFCLKALWAYSRUN */
/* Description: Automatic or manual control of HFCLK128M/HFCLK64M */

/* Bit 0 : Ensure clock is always running */
#define CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Pos (0UL) /*!< Position of ALWAYSRUN field. */
#define CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Msk (0x1UL << CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Pos) /*!< Bit mask of ALWAYSRUN field. */
#define CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Automatic (0UL) /*!< Use automatic clock control */
#define CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_AlwaysRun (1UL) /*!< Ensure clock is always running */

/* Register: CLOCK_LFCLKALWAYSRUN */
/* Description: Automatic or manual control of LFCLK */

/* Bit 0 : Ensure clock is always running */
#define CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos (0UL) /*!< Position of ALWAYSRUN field. */
#define CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk (0x1UL << CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos) /*!< Bit mask of ALWAYSRUN field. */
#define CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Automatic (0UL) /*!< Use automatic clock control */
#define CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_AlwaysRun (1UL) /*!< Ensure clock is always running */

/* Register: CLOCK_HFCLKAUDIOALWAYSRUN */
/* Description: Automatic or manual control of HFCLKAUDIO */

/* Bit 0 : Ensure clock is always running */
#define CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Pos (0UL) /*!< Position of ALWAYSRUN field. */
#define CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Msk (0x1UL << CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Pos) /*!< Bit mask of ALWAYSRUN field. */
#define CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Automatic (0UL) /*!< Use automatic clock control */
#define CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_AlwaysRun (1UL) /*!< Ensure clock is always running */

/* Register: CLOCK_HFCLK192MSRC */
/* Description: Clock source for HFCLK192M */

/* Bit 0 : Select which HFCLK192M source is started by the HFCLK192MSTART task */
#define CLOCK_HFCLK192MSRC_SRC_Pos (0UL) /*!< Position of SRC field. */
#define CLOCK_HFCLK192MSRC_SRC_Msk (0x1UL << CLOCK_HFCLK192MSRC_SRC_Pos) /*!< Bit mask of SRC field. */
#define CLOCK_HFCLK192MSRC_SRC_HFINT (0UL) /*!< HFCLK192MSTART task starts HFINT oscillator */
#define CLOCK_HFCLK192MSRC_SRC_HFXO (1UL) /*!< HFCLK192MSTART task starts HFXO oscillator */

/* Register: CLOCK_HFCLK192MALWAYSRUN */
/* Description: Automatic or manual control of HFCLK192M */

/* Bit 0 : Ensure clock is always running */
#define CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Pos (0UL) /*!< Position of ALWAYSRUN field. */
#define CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Msk (0x1UL << CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Pos) /*!< Bit mask of ALWAYSRUN field. */
#define CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Automatic (0UL) /*!< Use automatic clock control */
#define CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_AlwaysRun (1UL) /*!< Ensure clock is always running */

/* Register: CLOCK_HFCLK192MCTRL */
/* Description: HFCLK192M frequency configuration */

/* Bits 1..0 : High frequency clock HCLK192M */
#define CLOCK_HFCLK192MCTRL_HCLK192M_Pos (0UL) /*!< Position of HCLK192M field. */
#define CLOCK_HFCLK192MCTRL_HCLK192M_Msk (0x3UL << CLOCK_HFCLK192MCTRL_HCLK192M_Pos) /*!< Bit mask of HCLK192M field. */
#define CLOCK_HFCLK192MCTRL_HCLK192M_Div1 (0UL) /*!< Divide HFCLK192M by 1 */
#define CLOCK_HFCLK192MCTRL_HCLK192M_Div2 (1UL) /*!< Divide HFCLK192M by 2 */
#define CLOCK_HFCLK192MCTRL_HCLK192M_Div4 (2UL) /*!< Divide HFCLK192M by 4 */


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
/* ================                                         RESET                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief Reset control (RESET)
  */

typedef struct {                                /*!< (@ 0x40005000) RESET_NS Structure                                         */
  __IM  uint32_t  RESERVED[256];
  __IOM uint32_t  RESETREAS;                    /*!< (@ 0x00000400) Reset reason                                               */
  __IM  uint32_t  RESERVED1[131];
  __IOM RESET_NETWORK_Type NETWORK;             /*!< (@ 0x00000610) ULP network core control                                   */
} NRF_RESET_Type;                               /*!< Size = 1560 (0x618)                                                       */


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

/* =========================================================================================================================== */
/* ================                                          RTC                                              ================ */
/* =========================================================================================================================== */


/**
  * @brief Real-time counter 0 (RTC0_NS)
  */

typedef struct {                                /*!< (@ 0x41011000) RTC0_NS Structure                                          */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Start RTC counter                                          */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000004) Stop RTC counter                                           */
  __OM  uint32_t  TASKS_CLEAR;                  /*!< (@ 0x00000008) Clear RTC counter                                          */
  __OM  uint32_t  TASKS_TRIGOVRFLW;             /*!< (@ 0x0000000C) Set counter to 0xFFFFF0                                    */
  __IM  uint32_t  RESERVED[12];
  __OM  uint32_t  TASKS_CAPTURE[4];             /*!< (@ 0x00000040) Description collection: Capture RTC counter to
                                                                    CC[n] register                                             */
  __IM  uint32_t  RESERVED1[12];
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000080) Subscribe configuration for task START                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000084) Subscribe configuration for task STOP                      */
  __IOM uint32_t  SUBSCRIBE_CLEAR;              /*!< (@ 0x00000088) Subscribe configuration for task CLEAR                     */
  __IOM uint32_t  SUBSCRIBE_TRIGOVRFLW;         /*!< (@ 0x0000008C) Subscribe configuration for task TRIGOVRFLW                */
  __IM  uint32_t  RESERVED2[12];
  __IOM uint32_t  SUBSCRIBE_CAPTURE[4];         /*!< (@ 0x000000C0) Description collection: Subscribe configuration
                                                                    for task CAPTURE[n]                                        */
  __IM  uint32_t  RESERVED3[12];
  __IOM uint32_t  EVENTS_TICK;                  /*!< (@ 0x00000100) Event on counter increment                                 */
  __IOM uint32_t  EVENTS_OVRFLW;                /*!< (@ 0x00000104) Event on counter overflow                                  */
  __IM  uint32_t  RESERVED4[14];
  __IOM uint32_t  EVENTS_COMPARE[4];            /*!< (@ 0x00000140) Description collection: Compare event on CC[n]
                                                                    match                                                      */
  __IM  uint32_t  RESERVED5[12];
  __IOM uint32_t  PUBLISH_TICK;                 /*!< (@ 0x00000180) Publish configuration for event TICK                       */
  __IOM uint32_t  PUBLISH_OVRFLW;               /*!< (@ 0x00000184) Publish configuration for event OVRFLW                     */
  __IM  uint32_t  RESERVED6[14];
  __IOM uint32_t  PUBLISH_COMPARE[4];           /*!< (@ 0x000001C0) Description collection: Publish configuration
                                                                    for event COMPARE[n]                                       */
  __IM  uint32_t  RESERVED7[12];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED8[64];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED9[13];
  __IOM uint32_t  EVTEN;                        /*!< (@ 0x00000340) Enable or disable event routing                            */
  __IOM uint32_t  EVTENSET;                     /*!< (@ 0x00000344) Enable event routing                                       */
  __IOM uint32_t  EVTENCLR;                     /*!< (@ 0x00000348) Disable event routing                                      */
  __IM  uint32_t  RESERVED10[110];
  __IM  uint32_t  COUNTER;                      /*!< (@ 0x00000504) Current counter value                                      */
  __IOM uint32_t  PRESCALER;                    /*!< (@ 0x00000508) 12-bit prescaler for counter frequency (32768
                                                                    / (PRESCALER + 1)). Must be written when
                                                                    RTC is stopped.                                            */
  __IM  uint32_t  RESERVED11[13];
  __IOM uint32_t  CC[4];                        /*!< (@ 0x00000540) Description collection: Compare register n                 */
} NRF_RTC_Type;                                 /*!< Size = 1360 (0x550)                                                       */


/* Peripheral: RTC */
/* Description: Real-time counter 0 */

/* Register: RTC_TASKS_START */
/* Description: Start RTC counter */

/* Bit 0 : Start RTC counter */
#define RTC_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define RTC_TASKS_START_TASKS_START_Msk (0x1UL << RTC_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define RTC_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: RTC_TASKS_STOP */
/* Description: Stop RTC counter */

/* Bit 0 : Stop RTC counter */
#define RTC_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define RTC_TASKS_STOP_TASKS_STOP_Msk (0x1UL << RTC_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define RTC_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: RTC_TASKS_CLEAR */
/* Description: Clear RTC counter */

/* Bit 0 : Clear RTC counter */
#define RTC_TASKS_CLEAR_TASKS_CLEAR_Pos (0UL) /*!< Position of TASKS_CLEAR field. */
#define RTC_TASKS_CLEAR_TASKS_CLEAR_Msk (0x1UL << RTC_TASKS_CLEAR_TASKS_CLEAR_Pos) /*!< Bit mask of TASKS_CLEAR field. */
#define RTC_TASKS_CLEAR_TASKS_CLEAR_Trigger (1UL) /*!< Trigger task */

/* Register: RTC_TASKS_TRIGOVRFLW */
/* Description: Set counter to 0xFFFFF0 */

/* Bit 0 : Set counter to 0xFFFFF0 */
#define RTC_TASKS_TRIGOVRFLW_TASKS_TRIGOVRFLW_Pos (0UL) /*!< Position of TASKS_TRIGOVRFLW field. */
#define RTC_TASKS_TRIGOVRFLW_TASKS_TRIGOVRFLW_Msk (0x1UL << RTC_TASKS_TRIGOVRFLW_TASKS_TRIGOVRFLW_Pos) /*!< Bit mask of TASKS_TRIGOVRFLW field. */
#define RTC_TASKS_TRIGOVRFLW_TASKS_TRIGOVRFLW_Trigger (1UL) /*!< Trigger task */

/* Register: RTC_TASKS_CAPTURE */
/* Description: Description collection: Capture RTC counter to CC[n] register */

/* Bit 0 : Capture RTC counter to CC[n] register */
#define RTC_TASKS_CAPTURE_TASKS_CAPTURE_Pos (0UL) /*!< Position of TASKS_CAPTURE field. */
#define RTC_TASKS_CAPTURE_TASKS_CAPTURE_Msk (0x1UL << RTC_TASKS_CAPTURE_TASKS_CAPTURE_Pos) /*!< Bit mask of TASKS_CAPTURE field. */
#define RTC_TASKS_CAPTURE_TASKS_CAPTURE_Trigger (1UL) /*!< Trigger task */

/* Register: RTC_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define RTC_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_SUBSCRIBE_START_EN_Msk (0x1UL << RTC_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define RTC_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define RTC_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << RTC_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define RTC_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_SUBSCRIBE_STOP_EN_Msk (0x1UL << RTC_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RTC_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define RTC_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << RTC_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_SUBSCRIBE_CLEAR */
/* Description: Subscribe configuration for task CLEAR */

/* Bit 31 :   */
#define RTC_SUBSCRIBE_CLEAR_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_SUBSCRIBE_CLEAR_EN_Msk (0x1UL << RTC_SUBSCRIBE_CLEAR_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_SUBSCRIBE_CLEAR_EN_Disabled (0UL) /*!< Disable subscription */
#define RTC_SUBSCRIBE_CLEAR_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CLEAR will subscribe to */
#define RTC_SUBSCRIBE_CLEAR_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_SUBSCRIBE_CLEAR_CHIDX_Msk (0xFFUL << RTC_SUBSCRIBE_CLEAR_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_SUBSCRIBE_TRIGOVRFLW */
/* Description: Subscribe configuration for task TRIGOVRFLW */

/* Bit 31 :   */
#define RTC_SUBSCRIBE_TRIGOVRFLW_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_SUBSCRIBE_TRIGOVRFLW_EN_Msk (0x1UL << RTC_SUBSCRIBE_TRIGOVRFLW_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_SUBSCRIBE_TRIGOVRFLW_EN_Disabled (0UL) /*!< Disable subscription */
#define RTC_SUBSCRIBE_TRIGOVRFLW_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task TRIGOVRFLW will subscribe to */
#define RTC_SUBSCRIBE_TRIGOVRFLW_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_SUBSCRIBE_TRIGOVRFLW_CHIDX_Msk (0xFFUL << RTC_SUBSCRIBE_TRIGOVRFLW_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_SUBSCRIBE_CAPTURE */
/* Description: Description collection: Subscribe configuration for task CAPTURE[n] */

/* Bit 31 :   */
#define RTC_SUBSCRIBE_CAPTURE_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_SUBSCRIBE_CAPTURE_EN_Msk (0x1UL << RTC_SUBSCRIBE_CAPTURE_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_SUBSCRIBE_CAPTURE_EN_Disabled (0UL) /*!< Disable subscription */
#define RTC_SUBSCRIBE_CAPTURE_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CAPTURE[n] will subscribe to */
#define RTC_SUBSCRIBE_CAPTURE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_SUBSCRIBE_CAPTURE_CHIDX_Msk (0xFFUL << RTC_SUBSCRIBE_CAPTURE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_EVENTS_TICK */
/* Description: Event on counter increment */

/* Bit 0 : Event on counter increment */
#define RTC_EVENTS_TICK_EVENTS_TICK_Pos (0UL) /*!< Position of EVENTS_TICK field. */
#define RTC_EVENTS_TICK_EVENTS_TICK_Msk (0x1UL << RTC_EVENTS_TICK_EVENTS_TICK_Pos) /*!< Bit mask of EVENTS_TICK field. */
#define RTC_EVENTS_TICK_EVENTS_TICK_NotGenerated (0UL) /*!< Event not generated */
#define RTC_EVENTS_TICK_EVENTS_TICK_Generated (1UL) /*!< Event generated */

/* Register: RTC_EVENTS_OVRFLW */
/* Description: Event on counter overflow */

/* Bit 0 : Event on counter overflow */
#define RTC_EVENTS_OVRFLW_EVENTS_OVRFLW_Pos (0UL) /*!< Position of EVENTS_OVRFLW field. */
#define RTC_EVENTS_OVRFLW_EVENTS_OVRFLW_Msk (0x1UL << RTC_EVENTS_OVRFLW_EVENTS_OVRFLW_Pos) /*!< Bit mask of EVENTS_OVRFLW field. */
#define RTC_EVENTS_OVRFLW_EVENTS_OVRFLW_NotGenerated (0UL) /*!< Event not generated */
#define RTC_EVENTS_OVRFLW_EVENTS_OVRFLW_Generated (1UL) /*!< Event generated */

/* Register: RTC_EVENTS_COMPARE */
/* Description: Description collection: Compare event on CC[n] match */

/* Bit 0 : Compare event on CC[n] match */
#define RTC_EVENTS_COMPARE_EVENTS_COMPARE_Pos (0UL) /*!< Position of EVENTS_COMPARE field. */
#define RTC_EVENTS_COMPARE_EVENTS_COMPARE_Msk (0x1UL << RTC_EVENTS_COMPARE_EVENTS_COMPARE_Pos) /*!< Bit mask of EVENTS_COMPARE field. */
#define RTC_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated (0UL) /*!< Event not generated */
#define RTC_EVENTS_COMPARE_EVENTS_COMPARE_Generated (1UL) /*!< Event generated */

/* Register: RTC_PUBLISH_TICK */
/* Description: Publish configuration for event TICK */

/* Bit 31 :   */
#define RTC_PUBLISH_TICK_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_PUBLISH_TICK_EN_Msk (0x1UL << RTC_PUBLISH_TICK_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_PUBLISH_TICK_EN_Disabled (0UL) /*!< Disable publishing */
#define RTC_PUBLISH_TICK_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event TICK will publish to. */
#define RTC_PUBLISH_TICK_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_PUBLISH_TICK_CHIDX_Msk (0xFFUL << RTC_PUBLISH_TICK_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_PUBLISH_OVRFLW */
/* Description: Publish configuration for event OVRFLW */

/* Bit 31 :   */
#define RTC_PUBLISH_OVRFLW_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_PUBLISH_OVRFLW_EN_Msk (0x1UL << RTC_PUBLISH_OVRFLW_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_PUBLISH_OVRFLW_EN_Disabled (0UL) /*!< Disable publishing */
#define RTC_PUBLISH_OVRFLW_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event OVRFLW will publish to. */
#define RTC_PUBLISH_OVRFLW_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_PUBLISH_OVRFLW_CHIDX_Msk (0xFFUL << RTC_PUBLISH_OVRFLW_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_PUBLISH_COMPARE */
/* Description: Description collection: Publish configuration for event COMPARE[n] */

/* Bit 31 :   */
#define RTC_PUBLISH_COMPARE_EN_Pos (31UL) /*!< Position of EN field. */
#define RTC_PUBLISH_COMPARE_EN_Msk (0x1UL << RTC_PUBLISH_COMPARE_EN_Pos) /*!< Bit mask of EN field. */
#define RTC_PUBLISH_COMPARE_EN_Disabled (0UL) /*!< Disable publishing */
#define RTC_PUBLISH_COMPARE_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event COMPARE[n] will publish to. */
#define RTC_PUBLISH_COMPARE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RTC_PUBLISH_COMPARE_CHIDX_Msk (0xFFUL << RTC_PUBLISH_COMPARE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RTC_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 3 : Shortcut between event COMPARE[3] and task CLEAR */
#define RTC_SHORTS_COMPARE3_CLEAR_Pos (3UL) /*!< Position of COMPARE3_CLEAR field. */
#define RTC_SHORTS_COMPARE3_CLEAR_Msk (0x1UL << RTC_SHORTS_COMPARE3_CLEAR_Pos) /*!< Bit mask of COMPARE3_CLEAR field. */
#define RTC_SHORTS_COMPARE3_CLEAR_Disabled (0UL) /*!< Disable shortcut */
#define RTC_SHORTS_COMPARE3_CLEAR_Enabled (1UL) /*!< Enable shortcut */

/* Bit 0 : Shortcut between event COMPARE[0] and task CLEAR */
#define RTC_SHORTS_COMPARE0_CLEAR_Pos (0UL) /*!< Position of COMPARE0_CLEAR field. */
#define RTC_SHORTS_COMPARE0_CLEAR_Msk (0x1UL << RTC_SHORTS_COMPARE0_CLEAR_Pos) /*!< Bit mask of COMPARE0_CLEAR field. */
#define RTC_SHORTS_COMPARE0_CLEAR_Disabled (0UL) /*!< Disable shortcut */
#define RTC_SHORTS_COMPARE0_CLEAR_Enabled (1UL) /*!< Enable shortcut */

/* Register: RTC_INTENSET */
/* Description: Enable interrupt */

/* Bit 19 : Write '1' to enable interrupt for event COMPARE[3] */
#define RTC_INTENSET_COMPARE3_Pos (19UL) /*!< Position of COMPARE3 field. */
#define RTC_INTENSET_COMPARE3_Msk (0x1UL << RTC_INTENSET_COMPARE3_Pos) /*!< Bit mask of COMPARE3 field. */
#define RTC_INTENSET_COMPARE3_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENSET_COMPARE3_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENSET_COMPARE3_Set (1UL) /*!< Enable */

/* Bit 16 : Write '1' to enable interrupt for event COMPARE[0] */
#define RTC_INTENSET_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define RTC_INTENSET_COMPARE0_Msk (0x1UL << RTC_INTENSET_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define RTC_INTENSET_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENSET_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENSET_COMPARE0_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event OVRFLW */
#define RTC_INTENSET_OVRFLW_Pos (1UL) /*!< Position of OVRFLW field. */
#define RTC_INTENSET_OVRFLW_Msk (0x1UL << RTC_INTENSET_OVRFLW_Pos) /*!< Bit mask of OVRFLW field. */
#define RTC_INTENSET_OVRFLW_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENSET_OVRFLW_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENSET_OVRFLW_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event TICK */
#define RTC_INTENSET_TICK_Pos (0UL) /*!< Position of TICK field. */
#define RTC_INTENSET_TICK_Msk (0x1UL << RTC_INTENSET_TICK_Pos) /*!< Bit mask of TICK field. */
#define RTC_INTENSET_TICK_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENSET_TICK_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENSET_TICK_Set (1UL) /*!< Enable */

/* Register: RTC_INTENCLR */
/* Description: Disable interrupt */

/* Bit 19 : Write '1' to disable interrupt for event COMPARE[3] */
#define RTC_INTENCLR_COMPARE3_Pos (19UL) /*!< Position of COMPARE3 field. */
#define RTC_INTENCLR_COMPARE3_Msk (0x1UL << RTC_INTENCLR_COMPARE3_Pos) /*!< Bit mask of COMPARE3 field. */
#define RTC_INTENCLR_COMPARE3_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENCLR_COMPARE3_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENCLR_COMPARE3_Clear (1UL) /*!< Disable */

/* Bit 16 : Write '1' to disable interrupt for event COMPARE[0] */
#define RTC_INTENCLR_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define RTC_INTENCLR_COMPARE0_Msk (0x1UL << RTC_INTENCLR_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define RTC_INTENCLR_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENCLR_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENCLR_COMPARE0_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable interrupt for event OVRFLW */
#define RTC_INTENCLR_OVRFLW_Pos (1UL) /*!< Position of OVRFLW field. */
#define RTC_INTENCLR_OVRFLW_Msk (0x1UL << RTC_INTENCLR_OVRFLW_Pos) /*!< Bit mask of OVRFLW field. */
#define RTC_INTENCLR_OVRFLW_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENCLR_OVRFLW_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENCLR_OVRFLW_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event TICK */
#define RTC_INTENCLR_TICK_Pos (0UL) /*!< Position of TICK field. */
#define RTC_INTENCLR_TICK_Msk (0x1UL << RTC_INTENCLR_TICK_Pos) /*!< Bit mask of TICK field. */
#define RTC_INTENCLR_TICK_Disabled (0UL) /*!< Read: Disabled */
#define RTC_INTENCLR_TICK_Enabled (1UL) /*!< Read: Enabled */
#define RTC_INTENCLR_TICK_Clear (1UL) /*!< Disable */

/* Register: RTC_EVTEN */
/* Description: Enable or disable event routing */

/* Bit 19 : Enable or disable event routing for event COMPARE[3] */
#define RTC_EVTEN_COMPARE3_Pos (19UL) /*!< Position of COMPARE3 field. */
#define RTC_EVTEN_COMPARE3_Msk (0x1UL << RTC_EVTEN_COMPARE3_Pos) /*!< Bit mask of COMPARE3 field. */
#define RTC_EVTEN_COMPARE3_Disabled (0UL) /*!< Disable */
#define RTC_EVTEN_COMPARE3_Enabled (1UL) /*!< Enable */

/* Bit 16 : Enable or disable event routing for event COMPARE[0] */
#define RTC_EVTEN_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define RTC_EVTEN_COMPARE0_Msk (0x1UL << RTC_EVTEN_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define RTC_EVTEN_COMPARE0_Disabled (0UL) /*!< Disable */
#define RTC_EVTEN_COMPARE0_Enabled (1UL) /*!< Enable */

/* Bit 1 : Enable or disable event routing for event OVRFLW */
#define RTC_EVTEN_OVRFLW_Pos (1UL) /*!< Position of OVRFLW field. */
#define RTC_EVTEN_OVRFLW_Msk (0x1UL << RTC_EVTEN_OVRFLW_Pos) /*!< Bit mask of OVRFLW field. */
#define RTC_EVTEN_OVRFLW_Disabled (0UL) /*!< Disable */
#define RTC_EVTEN_OVRFLW_Enabled (1UL) /*!< Enable */

/* Bit 0 : Enable or disable event routing for event TICK */
#define RTC_EVTEN_TICK_Pos (0UL) /*!< Position of TICK field. */
#define RTC_EVTEN_TICK_Msk (0x1UL << RTC_EVTEN_TICK_Pos) /*!< Bit mask of TICK field. */
#define RTC_EVTEN_TICK_Disabled (0UL) /*!< Disable */
#define RTC_EVTEN_TICK_Enabled (1UL) /*!< Enable */

/* Register: RTC_EVTENSET */
/* Description: Enable event routing */

/* Bit 19 : Write '1' to enable event routing for event COMPARE[3] */
#define RTC_EVTENSET_COMPARE3_Pos (19UL) /*!< Position of COMPARE3 field. */
#define RTC_EVTENSET_COMPARE3_Msk (0x1UL << RTC_EVTENSET_COMPARE3_Pos) /*!< Bit mask of COMPARE3 field. */
#define RTC_EVTENSET_COMPARE3_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENSET_COMPARE3_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENSET_COMPARE3_Set (1UL) /*!< Enable */

/* Bit 16 : Write '1' to enable event routing for event COMPARE[0] */
#define RTC_EVTENSET_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define RTC_EVTENSET_COMPARE0_Msk (0x1UL << RTC_EVTENSET_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define RTC_EVTENSET_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENSET_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENSET_COMPARE0_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable event routing for event OVRFLW */
#define RTC_EVTENSET_OVRFLW_Pos (1UL) /*!< Position of OVRFLW field. */
#define RTC_EVTENSET_OVRFLW_Msk (0x1UL << RTC_EVTENSET_OVRFLW_Pos) /*!< Bit mask of OVRFLW field. */
#define RTC_EVTENSET_OVRFLW_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENSET_OVRFLW_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENSET_OVRFLW_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable event routing for event TICK */
#define RTC_EVTENSET_TICK_Pos (0UL) /*!< Position of TICK field. */
#define RTC_EVTENSET_TICK_Msk (0x1UL << RTC_EVTENSET_TICK_Pos) /*!< Bit mask of TICK field. */
#define RTC_EVTENSET_TICK_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENSET_TICK_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENSET_TICK_Set (1UL) /*!< Enable */

/* Register: RTC_EVTENCLR */
/* Description: Disable event routing */

/* Bit 19 : Write '1' to disable event routing for event COMPARE[3] */
#define RTC_EVTENCLR_COMPARE3_Pos (19UL) /*!< Position of COMPARE3 field. */
#define RTC_EVTENCLR_COMPARE3_Msk (0x1UL << RTC_EVTENCLR_COMPARE3_Pos) /*!< Bit mask of COMPARE3 field. */
#define RTC_EVTENCLR_COMPARE3_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENCLR_COMPARE3_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENCLR_COMPARE3_Clear (1UL) /*!< Disable */

/* Bit 16 : Write '1' to disable event routing for event COMPARE[0] */
#define RTC_EVTENCLR_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define RTC_EVTENCLR_COMPARE0_Msk (0x1UL << RTC_EVTENCLR_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define RTC_EVTENCLR_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENCLR_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENCLR_COMPARE0_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable event routing for event OVRFLW */
#define RTC_EVTENCLR_OVRFLW_Pos (1UL) /*!< Position of OVRFLW field. */
#define RTC_EVTENCLR_OVRFLW_Msk (0x1UL << RTC_EVTENCLR_OVRFLW_Pos) /*!< Bit mask of OVRFLW field. */
#define RTC_EVTENCLR_OVRFLW_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENCLR_OVRFLW_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENCLR_OVRFLW_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable event routing for event TICK */
#define RTC_EVTENCLR_TICK_Pos (0UL) /*!< Position of TICK field. */
#define RTC_EVTENCLR_TICK_Msk (0x1UL << RTC_EVTENCLR_TICK_Pos) /*!< Bit mask of TICK field. */
#define RTC_EVTENCLR_TICK_Disabled (0UL) /*!< Read: Disabled */
#define RTC_EVTENCLR_TICK_Enabled (1UL) /*!< Read: Enabled */
#define RTC_EVTENCLR_TICK_Clear (1UL) /*!< Disable */

/* Register: RTC_COUNTER */
/* Description: Current counter value */

/* Bits 23..0 : Counter value */
#define RTC_COUNTER_COUNTER_Pos (0UL) /*!< Position of COUNTER field. */
#define RTC_COUNTER_COUNTER_Msk (0xFFFFFFUL << RTC_COUNTER_COUNTER_Pos) /*!< Bit mask of COUNTER field. */

/* Register: RTC_PRESCALER */
/* Description: 12-bit prescaler for counter frequency (32768 / (PRESCALER + 1)). Must be written when RTC is stopped. */

/* Bits 11..0 : Prescaler value */
#define RTC_PRESCALER_PRESCALER_Pos (0UL) /*!< Position of PRESCALER field. */
#define RTC_PRESCALER_PRESCALER_Msk (0xFFFUL << RTC_PRESCALER_PRESCALER_Pos) /*!< Bit mask of PRESCALER field. */

/* Register: RTC_CC */
/* Description: Description collection: Compare register n */

/* Bits 23..0 : Compare value */
#define RTC_CC_COMPARE_Pos (0UL) /*!< Position of COMPARE field. */
#define RTC_CC_COMPARE_Msk (0xFFFFFFUL << RTC_CC_COMPARE_Pos) /*!< Bit mask of COMPARE field. */



/* =========================================================================================================================== */
/* ================                                         TIMER                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief Timer/Counter 0 (TIMER0_NS)
  */

typedef struct {                                /*!< (@ 0x4100C000) TIMER0_NS Structure                                        */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Start Timer                                                */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000004) Stop Timer                                                 */
  __OM  uint32_t  TASKS_COUNT;                  /*!< (@ 0x00000008) Increment Timer (Counter mode only)                        */
  __OM  uint32_t  TASKS_CLEAR;                  /*!< (@ 0x0000000C) Clear time                                                 */
  __OM  uint32_t  TASKS_SHUTDOWN;               /*!< (@ 0x00000010) Deprecated register - Shut down timer                      */
  __IM  uint32_t  RESERVED[11];
  __OM  uint32_t  TASKS_CAPTURE[8];             /*!< (@ 0x00000040) Description collection: Capture Timer value to
                                                                    CC[n] register                                             */
  __IM  uint32_t  RESERVED1[8];
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000080) Subscribe configuration for task START                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000084) Subscribe configuration for task STOP                      */
  __IOM uint32_t  SUBSCRIBE_COUNT;              /*!< (@ 0x00000088) Subscribe configuration for task COUNT                     */
  __IOM uint32_t  SUBSCRIBE_CLEAR;              /*!< (@ 0x0000008C) Subscribe configuration for task CLEAR                     */
  __IOM uint32_t  SUBSCRIBE_SHUTDOWN;           /*!< (@ 0x00000090) Deprecated register - Subscribe configuration
                                                                    for task SHUTDOWN                                          */
  __IM  uint32_t  RESERVED2[11];
  __IOM uint32_t  SUBSCRIBE_CAPTURE[8];         /*!< (@ 0x000000C0) Description collection: Subscribe configuration
                                                                    for task CAPTURE[n]                                        */
  __IM  uint32_t  RESERVED3[24];
  __IOM uint32_t  EVENTS_COMPARE[8];            /*!< (@ 0x00000140) Description collection: Compare event on CC[n]
                                                                    match                                                      */
  __IM  uint32_t  RESERVED4[24];
  __IOM uint32_t  PUBLISH_COMPARE[8];           /*!< (@ 0x000001C0) Description collection: Publish configuration
                                                                    for event COMPARE[n]                                       */
  __IM  uint32_t  RESERVED5[8];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED6[63];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED7[126];
  __IOM uint32_t  MODE;                         /*!< (@ 0x00000504) Timer mode selection                                       */
  __IOM uint32_t  BITMODE;                      /*!< (@ 0x00000508) Configure the number of bits used by the TIMER             */
  __IM  uint32_t  RESERVED8;
  __IOM uint32_t  PRESCALER;                    /*!< (@ 0x00000510) Timer prescaler register                                   */
  __IM  uint32_t  RESERVED9[11];
  __IOM uint32_t  CC[8];                        /*!< (@ 0x00000540) Description collection: Capture/Compare register
                                                                    n                                                          */
  __IM  uint32_t  RESERVED10[8];
  __IOM uint32_t  ONESHOTEN[8];                 /*!< (@ 0x00000580) Description collection: Enable one-shot operation
                                                                    for Capture/Compare channel n                              */
} NRF_TIMER_Type;                               /*!< Size = 1440 (0x5a0)                                                       */


/* Peripheral: TIMER */
/* Description: Timer/Counter 0 */

/* Register: TIMER_TASKS_START */
/* Description: Start Timer */

/* Bit 0 : Start Timer */
#define TIMER_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define TIMER_TASKS_START_TASKS_START_Msk (0x1UL << TIMER_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define TIMER_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_TASKS_STOP */
/* Description: Stop Timer */

/* Bit 0 : Stop Timer */
#define TIMER_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define TIMER_TASKS_STOP_TASKS_STOP_Msk (0x1UL << TIMER_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define TIMER_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_TASKS_COUNT */
/* Description: Increment Timer (Counter mode only) */

/* Bit 0 : Increment Timer (Counter mode only) */
#define TIMER_TASKS_COUNT_TASKS_COUNT_Pos (0UL) /*!< Position of TASKS_COUNT field. */
#define TIMER_TASKS_COUNT_TASKS_COUNT_Msk (0x1UL << TIMER_TASKS_COUNT_TASKS_COUNT_Pos) /*!< Bit mask of TASKS_COUNT field. */
#define TIMER_TASKS_COUNT_TASKS_COUNT_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_TASKS_CLEAR */
/* Description: Clear time */

/* Bit 0 : Clear time */
#define TIMER_TASKS_CLEAR_TASKS_CLEAR_Pos (0UL) /*!< Position of TASKS_CLEAR field. */
#define TIMER_TASKS_CLEAR_TASKS_CLEAR_Msk (0x1UL << TIMER_TASKS_CLEAR_TASKS_CLEAR_Pos) /*!< Bit mask of TASKS_CLEAR field. */
#define TIMER_TASKS_CLEAR_TASKS_CLEAR_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_TASKS_SHUTDOWN */
/* Description: Deprecated register - Shut down timer */

/* Bit 0 : Deprecated field -  Shut down timer */
#define TIMER_TASKS_SHUTDOWN_TASKS_SHUTDOWN_Pos (0UL) /*!< Position of TASKS_SHUTDOWN field. */
#define TIMER_TASKS_SHUTDOWN_TASKS_SHUTDOWN_Msk (0x1UL << TIMER_TASKS_SHUTDOWN_TASKS_SHUTDOWN_Pos) /*!< Bit mask of TASKS_SHUTDOWN field. */
#define TIMER_TASKS_SHUTDOWN_TASKS_SHUTDOWN_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_TASKS_CAPTURE */
/* Description: Description collection: Capture Timer value to CC[n] register */

/* Bit 0 : Capture Timer value to CC[n] register */
#define TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Pos (0UL) /*!< Position of TASKS_CAPTURE field. */
#define TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Msk (0x1UL << TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Pos) /*!< Bit mask of TASKS_CAPTURE field. */
#define TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Trigger (1UL) /*!< Trigger task */

/* Register: TIMER_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_START_EN_Msk (0x1UL << TIMER_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define TIMER_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_STOP_EN_Msk (0x1UL << TIMER_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define TIMER_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SUBSCRIBE_COUNT */
/* Description: Subscribe configuration for task COUNT */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_COUNT_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_COUNT_EN_Msk (0x1UL << TIMER_SUBSCRIBE_COUNT_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_COUNT_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_COUNT_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task COUNT will subscribe to */
#define TIMER_SUBSCRIBE_COUNT_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_COUNT_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_COUNT_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SUBSCRIBE_CLEAR */
/* Description: Subscribe configuration for task CLEAR */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_CLEAR_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_CLEAR_EN_Msk (0x1UL << TIMER_SUBSCRIBE_CLEAR_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_CLEAR_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_CLEAR_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CLEAR will subscribe to */
#define TIMER_SUBSCRIBE_CLEAR_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_CLEAR_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_CLEAR_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SUBSCRIBE_SHUTDOWN */
/* Description: Deprecated register - Subscribe configuration for task SHUTDOWN */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_SHUTDOWN_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_SHUTDOWN_EN_Msk (0x1UL << TIMER_SUBSCRIBE_SHUTDOWN_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_SHUTDOWN_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_SHUTDOWN_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task SHUTDOWN will subscribe to */
#define TIMER_SUBSCRIBE_SHUTDOWN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_SHUTDOWN_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_SHUTDOWN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SUBSCRIBE_CAPTURE */
/* Description: Description collection: Subscribe configuration for task CAPTURE[n] */

/* Bit 31 :   */
#define TIMER_SUBSCRIBE_CAPTURE_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_SUBSCRIBE_CAPTURE_EN_Msk (0x1UL << TIMER_SUBSCRIBE_CAPTURE_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_SUBSCRIBE_CAPTURE_EN_Disabled (0UL) /*!< Disable subscription */
#define TIMER_SUBSCRIBE_CAPTURE_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CAPTURE[n] will subscribe to */
#define TIMER_SUBSCRIBE_CAPTURE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_SUBSCRIBE_CAPTURE_CHIDX_Msk (0xFFUL << TIMER_SUBSCRIBE_CAPTURE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_EVENTS_COMPARE */
/* Description: Description collection: Compare event on CC[n] match */

/* Bit 0 : Compare event on CC[n] match */
#define TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos (0UL) /*!< Position of EVENTS_COMPARE field. */
#define TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Msk (0x1UL << TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos) /*!< Bit mask of EVENTS_COMPARE field. */
#define TIMER_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated (0UL) /*!< Event not generated */
#define TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Generated (1UL) /*!< Event generated */

/* Register: TIMER_PUBLISH_COMPARE */
/* Description: Description collection: Publish configuration for event COMPARE[n] */

/* Bit 31 :   */
#define TIMER_PUBLISH_COMPARE_EN_Pos (31UL) /*!< Position of EN field. */
#define TIMER_PUBLISH_COMPARE_EN_Msk (0x1UL << TIMER_PUBLISH_COMPARE_EN_Pos) /*!< Bit mask of EN field. */
#define TIMER_PUBLISH_COMPARE_EN_Disabled (0UL) /*!< Disable publishing */
#define TIMER_PUBLISH_COMPARE_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event COMPARE[n] will publish to. */
#define TIMER_PUBLISH_COMPARE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TIMER_PUBLISH_COMPARE_CHIDX_Msk (0xFFUL << TIMER_PUBLISH_COMPARE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TIMER_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 23 : Shortcut between event COMPARE[7] and task STOP */
#define TIMER_SHORTS_COMPARE7_STOP_Pos (23UL) /*!< Position of COMPARE7_STOP field. */
#define TIMER_SHORTS_COMPARE7_STOP_Msk (0x1UL << TIMER_SHORTS_COMPARE7_STOP_Pos) /*!< Bit mask of COMPARE7_STOP field. */
#define TIMER_SHORTS_COMPARE7_STOP_Disabled (0UL) /*!< Disable shortcut */
#define TIMER_SHORTS_COMPARE7_STOP_Enabled (1UL) /*!< Enable shortcut */

/* Bit 16 : Shortcut between event COMPARE[0] and task STOP */
#define TIMER_SHORTS_COMPARE0_STOP_Pos (16UL) /*!< Position of COMPARE0_STOP field. */
#define TIMER_SHORTS_COMPARE0_STOP_Msk (0x1UL << TIMER_SHORTS_COMPARE0_STOP_Pos) /*!< Bit mask of COMPARE0_STOP field. */
#define TIMER_SHORTS_COMPARE0_STOP_Disabled (0UL) /*!< Disable shortcut */
#define TIMER_SHORTS_COMPARE0_STOP_Enabled (1UL) /*!< Enable shortcut */

/* Bit 7 : Shortcut between event COMPARE[7] and task CLEAR */
#define TIMER_SHORTS_COMPARE7_CLEAR_Pos (7UL) /*!< Position of COMPARE7_CLEAR field. */
#define TIMER_SHORTS_COMPARE7_CLEAR_Msk (0x1UL << TIMER_SHORTS_COMPARE7_CLEAR_Pos) /*!< Bit mask of COMPARE7_CLEAR field. */
#define TIMER_SHORTS_COMPARE7_CLEAR_Disabled (0UL) /*!< Disable shortcut */
#define TIMER_SHORTS_COMPARE7_CLEAR_Enabled (1UL) /*!< Enable shortcut */

/* Bit 0 : Shortcut between event COMPARE[0] and task CLEAR */
#define TIMER_SHORTS_COMPARE0_CLEAR_Pos (0UL) /*!< Position of COMPARE0_CLEAR field. */
#define TIMER_SHORTS_COMPARE0_CLEAR_Msk (0x1UL << TIMER_SHORTS_COMPARE0_CLEAR_Pos) /*!< Bit mask of COMPARE0_CLEAR field. */
#define TIMER_SHORTS_COMPARE0_CLEAR_Disabled (0UL) /*!< Disable shortcut */
#define TIMER_SHORTS_COMPARE0_CLEAR_Enabled (1UL) /*!< Enable shortcut */

/* Register: TIMER_INTEN */
/* Description: Enable or disable interrupt */

/* Bit 23 : Enable or disable interrupt for event COMPARE[7] */
#define TIMER_INTEN_COMPARE7_Pos (23UL) /*!< Position of COMPARE7 field. */
#define TIMER_INTEN_COMPARE7_Msk (0x1UL << TIMER_INTEN_COMPARE7_Pos) /*!< Bit mask of COMPARE7 field. */
#define TIMER_INTEN_COMPARE7_Disabled (0UL) /*!< Disable */
#define TIMER_INTEN_COMPARE7_Enabled (1UL) /*!< Enable */

/* Bit 16 : Enable or disable interrupt for event COMPARE[0] */
#define TIMER_INTEN_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define TIMER_INTEN_COMPARE0_Msk (0x1UL << TIMER_INTEN_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define TIMER_INTEN_COMPARE0_Disabled (0UL) /*!< Disable */
#define TIMER_INTEN_COMPARE0_Enabled (1UL) /*!< Enable */

/* Register: TIMER_INTENSET */
/* Description: Enable interrupt */

/* Bit 23 : Write '1' to enable interrupt for event COMPARE[7] */
#define TIMER_INTENSET_COMPARE7_Pos (23UL) /*!< Position of COMPARE7 field. */
#define TIMER_INTENSET_COMPARE7_Msk (0x1UL << TIMER_INTENSET_COMPARE7_Pos) /*!< Bit mask of COMPARE7 field. */
#define TIMER_INTENSET_COMPARE7_Disabled (0UL) /*!< Read: Disabled */
#define TIMER_INTENSET_COMPARE7_Enabled (1UL) /*!< Read: Enabled */
#define TIMER_INTENSET_COMPARE7_Set (1UL) /*!< Enable */

/* Bit 16 : Write '1' to enable interrupt for event COMPARE[0] */
#define TIMER_INTENSET_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define TIMER_INTENSET_COMPARE0_Msk (0x1UL << TIMER_INTENSET_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define TIMER_INTENSET_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define TIMER_INTENSET_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define TIMER_INTENSET_COMPARE0_Set (1UL) /*!< Enable */

/* Register: TIMER_INTENCLR */
/* Description: Disable interrupt */

/* Bit 23 : Write '1' to disable interrupt for event COMPARE[7] */
#define TIMER_INTENCLR_COMPARE7_Pos (23UL) /*!< Position of COMPARE7 field. */
#define TIMER_INTENCLR_COMPARE7_Msk (0x1UL << TIMER_INTENCLR_COMPARE7_Pos) /*!< Bit mask of COMPARE7 field. */
#define TIMER_INTENCLR_COMPARE7_Disabled (0UL) /*!< Read: Disabled */
#define TIMER_INTENCLR_COMPARE7_Enabled (1UL) /*!< Read: Enabled */
#define TIMER_INTENCLR_COMPARE7_Clear (1UL) /*!< Disable */

/* Bit 16 : Write '1' to disable interrupt for event COMPARE[0] */
#define TIMER_INTENCLR_COMPARE0_Pos (16UL) /*!< Position of COMPARE0 field. */
#define TIMER_INTENCLR_COMPARE0_Msk (0x1UL << TIMER_INTENCLR_COMPARE0_Pos) /*!< Bit mask of COMPARE0 field. */
#define TIMER_INTENCLR_COMPARE0_Disabled (0UL) /*!< Read: Disabled */
#define TIMER_INTENCLR_COMPARE0_Enabled (1UL) /*!< Read: Enabled */
#define TIMER_INTENCLR_COMPARE0_Clear (1UL) /*!< Disable */

/* Register: TIMER_MODE */
/* Description: Timer mode selection */

/* Bits 1..0 : Timer mode */
#define TIMER_MODE_MODE_Pos (0UL) /*!< Position of MODE field. */
#define TIMER_MODE_MODE_Msk (0x3UL << TIMER_MODE_MODE_Pos) /*!< Bit mask of MODE field. */
#define TIMER_MODE_MODE_Timer (0UL) /*!< Select Timer mode */
#define TIMER_MODE_MODE_Counter (1UL) /*!< Deprecated enumerator -  Select Counter mode */
#define TIMER_MODE_MODE_LowPowerCounter (2UL) /*!< Select Low Power Counter mode */

/* Register: TIMER_BITMODE */
/* Description: Configure the number of bits used by the TIMER */

/* Bits 1..0 : Timer bit width */
#define TIMER_BITMODE_BITMODE_Pos (0UL) /*!< Position of BITMODE field. */
#define TIMER_BITMODE_BITMODE_Msk (0x3UL << TIMER_BITMODE_BITMODE_Pos) /*!< Bit mask of BITMODE field. */
#define TIMER_BITMODE_BITMODE_16Bit (0UL) /*!< 16 bit timer bit width */
#define TIMER_BITMODE_BITMODE_08Bit (1UL) /*!< 8 bit timer bit width */
#define TIMER_BITMODE_BITMODE_24Bit (2UL) /*!< 24 bit timer bit width */
#define TIMER_BITMODE_BITMODE_32Bit (3UL) /*!< 32 bit timer bit width */

/* Register: TIMER_PRESCALER */
/* Description: Timer prescaler register */

/* Bits 3..0 : Prescaler value */
#define TIMER_PRESCALER_PRESCALER_Pos (0UL) /*!< Position of PRESCALER field. */
#define TIMER_PRESCALER_PRESCALER_Msk (0xFUL << TIMER_PRESCALER_PRESCALER_Pos) /*!< Bit mask of PRESCALER field. */

/* Register: TIMER_CC */
/* Description: Description collection: Capture/Compare register n */

/* Bits 31..0 : Capture/Compare value */
#define TIMER_CC_CC_Pos (0UL) /*!< Position of CC field. */
#define TIMER_CC_CC_Msk (0xFFFFFFFFUL << TIMER_CC_CC_Pos) /*!< Bit mask of CC field. */

/* Register: TIMER_ONESHOTEN */
/* Description: Description collection: Enable one-shot operation for Capture/Compare channel n */

/* Bit 0 : Enable one-shot operation */
#define TIMER_ONESHOTEN_ONESHOTEN_Pos (0UL) /*!< Position of ONESHOTEN field. */
#define TIMER_ONESHOTEN_ONESHOTEN_Msk (0x1UL << TIMER_ONESHOTEN_ONESHOTEN_Pos) /*!< Bit mask of ONESHOTEN field. */
#define TIMER_ONESHOTEN_ONESHOTEN_Disable (0UL) /*!< Disable one-shot operation */
#define TIMER_ONESHOTEN_ONESHOTEN_Enable (1UL) /*!< Enable one-shot operation */



#ifdef __cplusplus
}
#endif

#endif /* _NRF5340_PERI_TYPES_H */