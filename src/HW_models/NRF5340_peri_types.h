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
  * @brief FICR_NFC [NFC] (Unspecified)
  */
typedef struct {
  __IM  uint32_t  TAGHEADER0;                   /*!< (@ 0x00000000) Default header for NFC Tag. Software can read
                                                                    these values to populate NFCID1_3RD_LAST,
                                                                    NFCID1_2ND_LAST and NFCID1_LAST.                           */
  __IM  uint32_t  TAGHEADER1;                   /*!< (@ 0x00000004) Default header for NFC Tag. Software can read
                                                                    these values to populate NFCID1_3RD_LAST,
                                                                    NFCID1_2ND_LAST and NFCID1_LAST.                           */
  __IM  uint32_t  TAGHEADER2;                   /*!< (@ 0x00000008) Default header for NFC Tag. Software can read
                                                                    these values to populate NFCID1_3RD_LAST,
                                                                    NFCID1_2ND_LAST and NFCID1_LAST.                           */
  __IM  uint32_t  TAGHEADER3;                   /*!< (@ 0x0000000C) Default header for NFC Tag. Software can read
                                                                    these values to populate NFCID1_3RD_LAST,
                                                                    NFCID1_2ND_LAST and NFCID1_LAST.                           */
} FICR_NFC_Type;                                /*!< Size = 16 (0x10)                                                          */


/**
  * @brief FICR_TRNG90B [TRNG90B] (NIST800-90B RNG calibration data)
  */
typedef struct {
  __IM  uint32_t  BYTES;                        /*!< (@ 0x00000000) Amount of bytes for the required entropy bits              */
  __IM  uint32_t  RCCUTOFF;                     /*!< (@ 0x00000004) Repetition counter cutoff                                  */
  __IM  uint32_t  APCUTOFF;                     /*!< (@ 0x00000008) Adaptive proportion cutoff                                 */
  __IM  uint32_t  STARTUP;                      /*!< (@ 0x0000000C) Amount of bytes for the startup tests                      */
  __IM  uint32_t  ROSC1;                        /*!< (@ 0x00000010) Sample count for ring oscillator 1                         */
  __IM  uint32_t  ROSC2;                        /*!< (@ 0x00000014) Sample count for ring oscillator 2                         */
  __IM  uint32_t  ROSC3;                        /*!< (@ 0x00000018) Sample count for ring oscillator 3                         */
  __IM  uint32_t  ROSC4;                        /*!< (@ 0x0000001C) Sample count for ring oscillator 4                         */
} FICR_TRNG90B_Type;                            /*!< Size = 32 (0x20)                                                          */


/**
  * @brief RADIO_PSEL [PSEL] (Unspecified)
  */
typedef struct {
  __IOM uint32_t  DFEGPIO[8];                   /*!< (@ 0x00000000) Description collection: Pin select for DFE pin
                                                                    n                                                          */
} RADIO_PSEL_Type;                              /*!< Size = 32 (0x20)                                                          */


/**
  * @brief RADIO_DFEPACKET [DFEPACKET] (DFE packet EasyDMA channel)
  */
typedef struct {
  __IOM uint32_t  PTR;                          /*!< (@ 0x00000000) Data pointer                                               */
  __IOM uint32_t  MAXCNT;                       /*!< (@ 0x00000004) Maximum number of buffer words to transfer                 */
  __IM  uint32_t  AMOUNT;                       /*!< (@ 0x00000008) Number of samples transferred in the last transaction      */
} RADIO_DFEPACKET_Type;                         /*!< Size = 12 (0xc)                                                           */

/**
  * @brief DPPIC_TASKS_CHG [TASKS_CHG] (Channel group tasks)
  */
typedef struct {
  __OM  uint32_t  EN;                           /*!< (@ 0x00000000) Description cluster: Enable channel group n                */
  __OM  uint32_t  DIS;                          /*!< (@ 0x00000004) Description cluster: Disable channel group n               */
} NRF_DPPIC_TASKS_CHG_Type;                         /*!< Size = 8 (0x8)                                                            */


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


/**
  * @brief UART_PSEL [PSEL] (Unspecified)
  */
typedef struct {
  __IOM uint32_t  RTS;                          /*!< (@ 0x00000000) Pin select for RTS                                         */
  __IOM uint32_t  TXD;                          /*!< (@ 0x00000004) Pin select for TXD                                         */
  __IOM uint32_t  CTS;                          /*!< (@ 0x00000008) Pin select for CTS                                         */
  __IOM uint32_t  RXD;                          /*!< (@ 0x0000000C) Pin select for RXD                                         */
} UART_PSEL_Type;                               /*!< Size = 16 (0x10)                                                          */


/**
  * @brief UARTE_PSEL [PSEL] (Unspecified)
  */
typedef struct {
  __IOM uint32_t  RTS;                          /*!< (@ 0x00000000) Pin select for RTS signal                                  */
  __IOM uint32_t  TXD;                          /*!< (@ 0x00000004) Pin select for TXD signal                                  */
  __IOM uint32_t  CTS;                          /*!< (@ 0x00000008) Pin select for CTS signal                                  */
  __IOM uint32_t  RXD;                          /*!< (@ 0x0000000C) Pin select for RXD signal                                  */
} UARTE_PSEL_Type;                              /*!< Size = 16 (0x10)                                                          */


/**
  * @brief UARTE_RXD [RXD] (RXD EasyDMA channel)
  */
typedef struct {
  __IOM uint32_t  PTR;                          /*!< (@ 0x00000000) Data pointer                                               */
  __IOM uint32_t  MAXCNT;                       /*!< (@ 0x00000004) Maximum number of bytes in receive buffer                  */
  __IM  uint32_t  AMOUNT;                       /*!< (@ 0x00000008) Number of bytes transferred in the last transaction        */
} UARTE_RXD_Type;                               /*!< Size = 12 (0xc)                                                           */


/**
  * @brief UARTE_TXD [TXD] (TXD EasyDMA channel)
  */
typedef struct {
  __IOM uint32_t  PTR;                          /*!< (@ 0x00000000) Data pointer                                               */
  __IOM uint32_t  MAXCNT;                       /*!< (@ 0x00000004) Maximum number of bytes in transmit buffer                 */
  __IM  uint32_t  AMOUNT;                       /*!< (@ 0x00000008) Number of bytes transferred in the last transaction        */
} UARTE_TXD_Type;                               /*!< Size = 12 (0xc)                                                           */


/**
  * @brief VREQCTRL_VREGRADIO [VREGRADIO] (Unspecified)
  */
typedef struct {
  __IOM uint32_t  VREQH;                        /*!< (@ 0x00000000) Request high voltage on RADIO After requesting
                                                                    high voltage, the user must wait until VREQHREADY
                                                                    is set to Ready                                            */
  __IM  uint32_t  RESERVED;
  __IM  uint32_t  VREQHREADY;                   /*!< (@ 0x00000008) High voltage on RADIO is ready                             */
} VREQCTRL_VREGRADIO_Type;                      /*!< Size = 12 (0xc)                                                           */

/* =========================================================================================================================== */
/* ================                                          AAR                                              ================ */
/* =========================================================================================================================== */


/**
  * @brief Accelerated Address Resolver (AAR)
  */

typedef struct {                                /*!< (@ 0x4100E000) AAR_NS Structure                                           */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Start resolving addresses based on IRKs specified
                                                                    in the IRK data structure                                  */
  __IM  uint32_t  RESERVED;
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000008) Stop resolving addresses                                   */
  __IM  uint32_t  RESERVED1[29];
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000080) Subscribe configuration for task START                     */
  __IM  uint32_t  RESERVED2;
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000088) Subscribe configuration for task STOP                      */
  __IM  uint32_t  RESERVED3[29];
  __IOM uint32_t  EVENTS_END;                   /*!< (@ 0x00000100) Address resolution procedure complete                      */
  __IOM uint32_t  EVENTS_RESOLVED;              /*!< (@ 0x00000104) Address resolved                                           */
  __IOM uint32_t  EVENTS_NOTRESOLVED;           /*!< (@ 0x00000108) Address not resolved                                       */
  __IM  uint32_t  RESERVED4[29];
  __IOM uint32_t  PUBLISH_END;                  /*!< (@ 0x00000180) Publish configuration for event END                        */
  __IOM uint32_t  PUBLISH_RESOLVED;             /*!< (@ 0x00000184) Publish configuration for event RESOLVED                   */
  __IOM uint32_t  PUBLISH_NOTRESOLVED;          /*!< (@ 0x00000188) Publish configuration for event NOTRESOLVED                */
  __IM  uint32_t  RESERVED5[94];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED6[61];
  __IM  uint32_t  STATUS;                       /*!< (@ 0x00000400) Resolution status                                          */
  __IM  uint32_t  RESERVED7[63];
  __IOM uint32_t  ENABLE;                       /*!< (@ 0x00000500) Enable AAR                                                 */
  __IOM uint32_t  NIRK;                         /*!< (@ 0x00000504) Number of IRKs                                             */
  __IOM uint32_t  IRKPTR;                       /*!< (@ 0x00000508) Pointer to IRK data structure                              */
  __IM  uint32_t  RESERVED8;
  __IOM uint32_t  ADDRPTR;                      /*!< (@ 0x00000510) Pointer to the resolvable address                          */
  __IOM uint32_t  SCRATCHPTR;                   /*!< (@ 0x00000514) Pointer to data area used for temporary storage            */
} NRF_AAR_Type;                                 /*!< Size = 1304 (0x518)                                                       */

/* Peripheral: AAR */
/* Description: Accelerated Address Resolver */

/* Register: AAR_TASKS_START */
/* Description: Start resolving addresses based on IRKs specified in the IRK data structure */

/* Bit 0 : Start resolving addresses based on IRKs specified in the IRK data structure */
#define AAR_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define AAR_TASKS_START_TASKS_START_Msk (0x1UL << AAR_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define AAR_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: AAR_TASKS_STOP */
/* Description: Stop resolving addresses */

/* Bit 0 : Stop resolving addresses */
#define AAR_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define AAR_TASKS_STOP_TASKS_STOP_Msk (0x1UL << AAR_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define AAR_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: AAR_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define AAR_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define AAR_SUBSCRIBE_START_EN_Msk (0x1UL << AAR_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define AAR_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define AAR_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define AAR_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define AAR_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << AAR_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: AAR_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define AAR_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define AAR_SUBSCRIBE_STOP_EN_Msk (0x1UL << AAR_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define AAR_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define AAR_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define AAR_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define AAR_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << AAR_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: AAR_EVENTS_END */
/* Description: Address resolution procedure complete */

/* Bit 0 : Address resolution procedure complete */
#define AAR_EVENTS_END_EVENTS_END_Pos (0UL) /*!< Position of EVENTS_END field. */
#define AAR_EVENTS_END_EVENTS_END_Msk (0x1UL << AAR_EVENTS_END_EVENTS_END_Pos) /*!< Bit mask of EVENTS_END field. */
#define AAR_EVENTS_END_EVENTS_END_NotGenerated (0UL) /*!< Event not generated */
#define AAR_EVENTS_END_EVENTS_END_Generated (1UL) /*!< Event generated */

/* Register: AAR_EVENTS_RESOLVED */
/* Description: Address resolved */

/* Bit 0 : Address resolved */
#define AAR_EVENTS_RESOLVED_EVENTS_RESOLVED_Pos (0UL) /*!< Position of EVENTS_RESOLVED field. */
#define AAR_EVENTS_RESOLVED_EVENTS_RESOLVED_Msk (0x1UL << AAR_EVENTS_RESOLVED_EVENTS_RESOLVED_Pos) /*!< Bit mask of EVENTS_RESOLVED field. */
#define AAR_EVENTS_RESOLVED_EVENTS_RESOLVED_NotGenerated (0UL) /*!< Event not generated */
#define AAR_EVENTS_RESOLVED_EVENTS_RESOLVED_Generated (1UL) /*!< Event generated */

/* Register: AAR_EVENTS_NOTRESOLVED */
/* Description: Address not resolved */

/* Bit 0 : Address not resolved */
#define AAR_EVENTS_NOTRESOLVED_EVENTS_NOTRESOLVED_Pos (0UL) /*!< Position of EVENTS_NOTRESOLVED field. */
#define AAR_EVENTS_NOTRESOLVED_EVENTS_NOTRESOLVED_Msk (0x1UL << AAR_EVENTS_NOTRESOLVED_EVENTS_NOTRESOLVED_Pos) /*!< Bit mask of EVENTS_NOTRESOLVED field. */
#define AAR_EVENTS_NOTRESOLVED_EVENTS_NOTRESOLVED_NotGenerated (0UL) /*!< Event not generated */
#define AAR_EVENTS_NOTRESOLVED_EVENTS_NOTRESOLVED_Generated (1UL) /*!< Event generated */

/* Register: AAR_PUBLISH_END */
/* Description: Publish configuration for event END */

/* Bit 31 :   */
#define AAR_PUBLISH_END_EN_Pos (31UL) /*!< Position of EN field. */
#define AAR_PUBLISH_END_EN_Msk (0x1UL << AAR_PUBLISH_END_EN_Pos) /*!< Bit mask of EN field. */
#define AAR_PUBLISH_END_EN_Disabled (0UL) /*!< Disable publishing */
#define AAR_PUBLISH_END_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event END will publish to. */
#define AAR_PUBLISH_END_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define AAR_PUBLISH_END_CHIDX_Msk (0xFFUL << AAR_PUBLISH_END_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: AAR_PUBLISH_RESOLVED */
/* Description: Publish configuration for event RESOLVED */

/* Bit 31 :   */
#define AAR_PUBLISH_RESOLVED_EN_Pos (31UL) /*!< Position of EN field. */
#define AAR_PUBLISH_RESOLVED_EN_Msk (0x1UL << AAR_PUBLISH_RESOLVED_EN_Pos) /*!< Bit mask of EN field. */
#define AAR_PUBLISH_RESOLVED_EN_Disabled (0UL) /*!< Disable publishing */
#define AAR_PUBLISH_RESOLVED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event RESOLVED will publish to. */
#define AAR_PUBLISH_RESOLVED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define AAR_PUBLISH_RESOLVED_CHIDX_Msk (0xFFUL << AAR_PUBLISH_RESOLVED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: AAR_PUBLISH_NOTRESOLVED */
/* Description: Publish configuration for event NOTRESOLVED */

/* Bit 31 :   */
#define AAR_PUBLISH_NOTRESOLVED_EN_Pos (31UL) /*!< Position of EN field. */
#define AAR_PUBLISH_NOTRESOLVED_EN_Msk (0x1UL << AAR_PUBLISH_NOTRESOLVED_EN_Pos) /*!< Bit mask of EN field. */
#define AAR_PUBLISH_NOTRESOLVED_EN_Disabled (0UL) /*!< Disable publishing */
#define AAR_PUBLISH_NOTRESOLVED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event NOTRESOLVED will publish to. */
#define AAR_PUBLISH_NOTRESOLVED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define AAR_PUBLISH_NOTRESOLVED_CHIDX_Msk (0xFFUL << AAR_PUBLISH_NOTRESOLVED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: AAR_INTENSET */
/* Description: Enable interrupt */

/* Bit 2 : Write '1' to enable interrupt for event NOTRESOLVED */
#define AAR_INTENSET_NOTRESOLVED_Pos (2UL) /*!< Position of NOTRESOLVED field. */
#define AAR_INTENSET_NOTRESOLVED_Msk (0x1UL << AAR_INTENSET_NOTRESOLVED_Pos) /*!< Bit mask of NOTRESOLVED field. */
#define AAR_INTENSET_NOTRESOLVED_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENSET_NOTRESOLVED_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENSET_NOTRESOLVED_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event RESOLVED */
#define AAR_INTENSET_RESOLVED_Pos (1UL) /*!< Position of RESOLVED field. */
#define AAR_INTENSET_RESOLVED_Msk (0x1UL << AAR_INTENSET_RESOLVED_Pos) /*!< Bit mask of RESOLVED field. */
#define AAR_INTENSET_RESOLVED_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENSET_RESOLVED_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENSET_RESOLVED_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event END */
#define AAR_INTENSET_END_Pos (0UL) /*!< Position of END field. */
#define AAR_INTENSET_END_Msk (0x1UL << AAR_INTENSET_END_Pos) /*!< Bit mask of END field. */
#define AAR_INTENSET_END_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENSET_END_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENSET_END_Set (1UL) /*!< Enable */

/* Register: AAR_INTENCLR */
/* Description: Disable interrupt */

/* Bit 2 : Write '1' to disable interrupt for event NOTRESOLVED */
#define AAR_INTENCLR_NOTRESOLVED_Pos (2UL) /*!< Position of NOTRESOLVED field. */
#define AAR_INTENCLR_NOTRESOLVED_Msk (0x1UL << AAR_INTENCLR_NOTRESOLVED_Pos) /*!< Bit mask of NOTRESOLVED field. */
#define AAR_INTENCLR_NOTRESOLVED_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENCLR_NOTRESOLVED_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENCLR_NOTRESOLVED_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable interrupt for event RESOLVED */
#define AAR_INTENCLR_RESOLVED_Pos (1UL) /*!< Position of RESOLVED field. */
#define AAR_INTENCLR_RESOLVED_Msk (0x1UL << AAR_INTENCLR_RESOLVED_Pos) /*!< Bit mask of RESOLVED field. */
#define AAR_INTENCLR_RESOLVED_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENCLR_RESOLVED_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENCLR_RESOLVED_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event END */
#define AAR_INTENCLR_END_Pos (0UL) /*!< Position of END field. */
#define AAR_INTENCLR_END_Msk (0x1UL << AAR_INTENCLR_END_Pos) /*!< Bit mask of END field. */
#define AAR_INTENCLR_END_Disabled (0UL) /*!< Read: Disabled */
#define AAR_INTENCLR_END_Enabled (1UL) /*!< Read: Enabled */
#define AAR_INTENCLR_END_Clear (1UL) /*!< Disable */

/* Register: AAR_STATUS */
/* Description: Resolution status */

/* Bits 3..0 : The IRK that was used last time an address was resolved */
#define AAR_STATUS_STATUS_Pos (0UL) /*!< Position of STATUS field. */
#define AAR_STATUS_STATUS_Msk (0xFUL << AAR_STATUS_STATUS_Pos) /*!< Bit mask of STATUS field. */

/* Register: AAR_ENABLE */
/* Description: Enable AAR */

/* Bits 1..0 : Enable or disable AAR */
#define AAR_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
#define AAR_ENABLE_ENABLE_Msk (0x3UL << AAR_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
#define AAR_ENABLE_ENABLE_Disabled (0UL) /*!< Disable */
#define AAR_ENABLE_ENABLE_Enabled (3UL) /*!< Enable */

/* Register: AAR_NIRK */
/* Description: Number of IRKs */

/* Bits 4..0 : Number of Identity Root Keys available in the IRK data structure */
#define AAR_NIRK_NIRK_Pos (0UL) /*!< Position of NIRK field. */
#define AAR_NIRK_NIRK_Msk (0x1FUL << AAR_NIRK_NIRK_Pos) /*!< Bit mask of NIRK field. */

/* Register: AAR_IRKPTR */
/* Description: Pointer to IRK data structure */

/* Bits 31..0 : Pointer to the IRK data structure */
#define AAR_IRKPTR_IRKPTR_Pos (0UL) /*!< Position of IRKPTR field. */
#define AAR_IRKPTR_IRKPTR_Msk (0xFFFFFFFFUL << AAR_IRKPTR_IRKPTR_Pos) /*!< Bit mask of IRKPTR field. */

/* Register: AAR_ADDRPTR */
/* Description: Pointer to the resolvable address */

/* Bits 31..0 : Pointer to the resolvable address (6-bytes) */
#define AAR_ADDRPTR_ADDRPTR_Pos (0UL) /*!< Position of ADDRPTR field. */
#define AAR_ADDRPTR_ADDRPTR_Msk (0xFFFFFFFFUL << AAR_ADDRPTR_ADDRPTR_Pos) /*!< Bit mask of ADDRPTR field. */

/* Register: AAR_SCRATCHPTR */
/* Description: Pointer to data area used for temporary storage */

/* Bits 31..0 : Pointer to a scratch data area used for temporary storage during resolution. A space of minimum 3 bytes must be reserved. */
#define AAR_SCRATCHPTR_SCRATCHPTR_Pos (0UL) /*!< Position of SCRATCHPTR field. */
#define AAR_SCRATCHPTR_SCRATCHPTR_Msk (0xFFFFFFFFUL << AAR_SCRATCHPTR_SCRATCHPTR_Pos) /*!< Bit mask of SCRATCHPTR field. */



/* =========================================================================================================================== */
/* ================                                          CCM                                              ================ */
/* =========================================================================================================================== */


/**
  * @brief AES CCM mode encryption (CCM)
  */

typedef struct {                                /*!< (@ 0x4100E000) CCM_NS Structure                                           */
  __OM  uint32_t  TASKS_KSGEN;                  /*!< (@ 0x00000000) Start generation of keystream. This operation
                                                                    will stop by itself when completed.                        */
  __OM  uint32_t  TASKS_CRYPT;                  /*!< (@ 0x00000004) Start encryption/decryption. This operation will
                                                                    stop by itself when completed.                             */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000008) Stop encryption/decryption                                 */
  __OM  uint32_t  TASKS_RATEOVERRIDE;           /*!< (@ 0x0000000C) Override DATARATE setting in MODE register with
                                                                    the contents of the RATEOVERRIDE register
                                                                    for any ongoing encryption/decryption                      */
  __IM  uint32_t  RESERVED[28];
  __IOM uint32_t  SUBSCRIBE_KSGEN;              /*!< (@ 0x00000080) Subscribe configuration for task KSGEN                     */
  __IOM uint32_t  SUBSCRIBE_CRYPT;              /*!< (@ 0x00000084) Subscribe configuration for task CRYPT                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000088) Subscribe configuration for task STOP                      */
  __IOM uint32_t  SUBSCRIBE_RATEOVERRIDE;       /*!< (@ 0x0000008C) Subscribe configuration for task RATEOVERRIDE              */
  __IM  uint32_t  RESERVED1[28];
  __IOM uint32_t  EVENTS_ENDKSGEN;              /*!< (@ 0x00000100) Keystream generation complete                              */
  __IOM uint32_t  EVENTS_ENDCRYPT;              /*!< (@ 0x00000104) Encrypt/decrypt complete                                   */
  __IOM uint32_t  EVENTS_ERROR;                 /*!< (@ 0x00000108) Deprecated register - CCM error event                      */
  __IM  uint32_t  RESERVED2[29];
  __IOM uint32_t  PUBLISH_ENDKSGEN;             /*!< (@ 0x00000180) Publish configuration for event ENDKSGEN                   */
  __IOM uint32_t  PUBLISH_ENDCRYPT;             /*!< (@ 0x00000184) Publish configuration for event ENDCRYPT                   */
  __IOM uint32_t  PUBLISH_ERROR;                /*!< (@ 0x00000188) Deprecated register - Publish configuration for
                                                                    event ERROR                                                */
  __IM  uint32_t  RESERVED3[29];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED4[64];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED5[61];
  __IM  uint32_t  MICSTATUS;                    /*!< (@ 0x00000400) MIC check result                                           */
  __IM  uint32_t  RESERVED6[63];
  __IOM uint32_t  ENABLE;                       /*!< (@ 0x00000500) Enable                                                     */
  __IOM uint32_t  MODE;                         /*!< (@ 0x00000504) Operation mode                                             */
  __IOM uint32_t  CNFPTR;                       /*!< (@ 0x00000508) Pointer to data structure holding the AES key
                                                                    and the NONCE vector                                       */
  __IOM uint32_t  INPTR;                        /*!< (@ 0x0000050C) Input pointer                                              */
  __IOM uint32_t  OUTPTR;                       /*!< (@ 0x00000510) Output pointer                                             */
  __IOM uint32_t  SCRATCHPTR;                   /*!< (@ 0x00000514) Pointer to data area used for temporary storage            */
  __IOM uint32_t  MAXPACKETSIZE;                /*!< (@ 0x00000518) Length of keystream generated when MODE.LENGTH
                                                                    = Extended                                                 */
  __IOM uint32_t  RATEOVERRIDE;                 /*!< (@ 0x0000051C) Data rate override setting.                                */
  __IOM uint32_t  HEADERMASK;                   /*!< (@ 0x00000520) Header (S0) mask.                                          */
} NRF_CCM_Type;                                 /*!< Size = 1316 (0x524)                                                       */


/* Peripheral: CCM */
/* Description: AES CCM mode encryption */

/* Register: CCM_TASKS_KSGEN */
/* Description: Start generation of keystream. This operation will stop by itself when completed. */

/* Bit 0 : Start generation of keystream. This operation will stop by itself when completed. */
#define CCM_TASKS_KSGEN_TASKS_KSGEN_Pos (0UL) /*!< Position of TASKS_KSGEN field. */
#define CCM_TASKS_KSGEN_TASKS_KSGEN_Msk (0x1UL << CCM_TASKS_KSGEN_TASKS_KSGEN_Pos) /*!< Bit mask of TASKS_KSGEN field. */
#define CCM_TASKS_KSGEN_TASKS_KSGEN_Trigger (1UL) /*!< Trigger task */

/* Register: CCM_TASKS_CRYPT */
/* Description: Start encryption/decryption. This operation will stop by itself when completed. */

/* Bit 0 : Start encryption/decryption. This operation will stop by itself when completed. */
#define CCM_TASKS_CRYPT_TASKS_CRYPT_Pos (0UL) /*!< Position of TASKS_CRYPT field. */
#define CCM_TASKS_CRYPT_TASKS_CRYPT_Msk (0x1UL << CCM_TASKS_CRYPT_TASKS_CRYPT_Pos) /*!< Bit mask of TASKS_CRYPT field. */
#define CCM_TASKS_CRYPT_TASKS_CRYPT_Trigger (1UL) /*!< Trigger task */

/* Register: CCM_TASKS_STOP */
/* Description: Stop encryption/decryption */

/* Bit 0 : Stop encryption/decryption */
#define CCM_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define CCM_TASKS_STOP_TASKS_STOP_Msk (0x1UL << CCM_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define CCM_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: CCM_TASKS_RATEOVERRIDE */
/* Description: Override DATARATE setting in MODE register with the contents of the RATEOVERRIDE register for any ongoing encryption/decryption */

/* Bit 0 : Override DATARATE setting in MODE register with the contents of the RATEOVERRIDE register for any ongoing encryption/decryption */
#define CCM_TASKS_RATEOVERRIDE_TASKS_RATEOVERRIDE_Pos (0UL) /*!< Position of TASKS_RATEOVERRIDE field. */
#define CCM_TASKS_RATEOVERRIDE_TASKS_RATEOVERRIDE_Msk (0x1UL << CCM_TASKS_RATEOVERRIDE_TASKS_RATEOVERRIDE_Pos) /*!< Bit mask of TASKS_RATEOVERRIDE field. */
#define CCM_TASKS_RATEOVERRIDE_TASKS_RATEOVERRIDE_Trigger (1UL) /*!< Trigger task */

/* Register: CCM_SUBSCRIBE_KSGEN */
/* Description: Subscribe configuration for task KSGEN */

/* Bit 31 :   */
#define CCM_SUBSCRIBE_KSGEN_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_SUBSCRIBE_KSGEN_EN_Msk (0x1UL << CCM_SUBSCRIBE_KSGEN_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_SUBSCRIBE_KSGEN_EN_Disabled (0UL) /*!< Disable subscription */
#define CCM_SUBSCRIBE_KSGEN_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task KSGEN will subscribe to */
#define CCM_SUBSCRIBE_KSGEN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_SUBSCRIBE_KSGEN_CHIDX_Msk (0xFFUL << CCM_SUBSCRIBE_KSGEN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_SUBSCRIBE_CRYPT */
/* Description: Subscribe configuration for task CRYPT */

/* Bit 31 :   */
#define CCM_SUBSCRIBE_CRYPT_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_SUBSCRIBE_CRYPT_EN_Msk (0x1UL << CCM_SUBSCRIBE_CRYPT_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_SUBSCRIBE_CRYPT_EN_Disabled (0UL) /*!< Disable subscription */
#define CCM_SUBSCRIBE_CRYPT_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CRYPT will subscribe to */
#define CCM_SUBSCRIBE_CRYPT_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_SUBSCRIBE_CRYPT_CHIDX_Msk (0xFFUL << CCM_SUBSCRIBE_CRYPT_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define CCM_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_SUBSCRIBE_STOP_EN_Msk (0x1UL << CCM_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define CCM_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define CCM_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << CCM_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_SUBSCRIBE_RATEOVERRIDE */
/* Description: Subscribe configuration for task RATEOVERRIDE */

/* Bit 31 :   */
#define CCM_SUBSCRIBE_RATEOVERRIDE_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_SUBSCRIBE_RATEOVERRIDE_EN_Msk (0x1UL << CCM_SUBSCRIBE_RATEOVERRIDE_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_SUBSCRIBE_RATEOVERRIDE_EN_Disabled (0UL) /*!< Disable subscription */
#define CCM_SUBSCRIBE_RATEOVERRIDE_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task RATEOVERRIDE will subscribe to */
#define CCM_SUBSCRIBE_RATEOVERRIDE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_SUBSCRIBE_RATEOVERRIDE_CHIDX_Msk (0xFFUL << CCM_SUBSCRIBE_RATEOVERRIDE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_EVENTS_ENDKSGEN */
/* Description: Keystream generation complete */

/* Bit 0 : Keystream generation complete */
#define CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_Pos (0UL) /*!< Position of EVENTS_ENDKSGEN field. */
#define CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_Msk (0x1UL << CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_Pos) /*!< Bit mask of EVENTS_ENDKSGEN field. */
#define CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_NotGenerated (0UL) /*!< Event not generated */
#define CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_Generated (1UL) /*!< Event generated */

/* Register: CCM_EVENTS_ENDCRYPT */
/* Description: Encrypt/decrypt complete */

/* Bit 0 : Encrypt/decrypt complete */
#define CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_Pos (0UL) /*!< Position of EVENTS_ENDCRYPT field. */
#define CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_Msk (0x1UL << CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_Pos) /*!< Bit mask of EVENTS_ENDCRYPT field. */
#define CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_NotGenerated (0UL) /*!< Event not generated */
#define CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_Generated (1UL) /*!< Event generated */

/* Register: CCM_EVENTS_ERROR */
/* Description: Deprecated register - CCM error event */

/* Bit 0 : Deprecated field -  CCM error event */
#define CCM_EVENTS_ERROR_EVENTS_ERROR_Pos (0UL) /*!< Position of EVENTS_ERROR field. */
#define CCM_EVENTS_ERROR_EVENTS_ERROR_Msk (0x1UL << CCM_EVENTS_ERROR_EVENTS_ERROR_Pos) /*!< Bit mask of EVENTS_ERROR field. */
#define CCM_EVENTS_ERROR_EVENTS_ERROR_NotGenerated (0UL) /*!< Event not generated */
#define CCM_EVENTS_ERROR_EVENTS_ERROR_Generated (1UL) /*!< Event generated */

/* Register: CCM_PUBLISH_ENDKSGEN */
/* Description: Publish configuration for event ENDKSGEN */

/* Bit 31 :   */
#define CCM_PUBLISH_ENDKSGEN_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_PUBLISH_ENDKSGEN_EN_Msk (0x1UL << CCM_PUBLISH_ENDKSGEN_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_PUBLISH_ENDKSGEN_EN_Disabled (0UL) /*!< Disable publishing */
#define CCM_PUBLISH_ENDKSGEN_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ENDKSGEN will publish to. */
#define CCM_PUBLISH_ENDKSGEN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_PUBLISH_ENDKSGEN_CHIDX_Msk (0xFFUL << CCM_PUBLISH_ENDKSGEN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_PUBLISH_ENDCRYPT */
/* Description: Publish configuration for event ENDCRYPT */

/* Bit 31 :   */
#define CCM_PUBLISH_ENDCRYPT_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_PUBLISH_ENDCRYPT_EN_Msk (0x1UL << CCM_PUBLISH_ENDCRYPT_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_PUBLISH_ENDCRYPT_EN_Disabled (0UL) /*!< Disable publishing */
#define CCM_PUBLISH_ENDCRYPT_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ENDCRYPT will publish to. */
#define CCM_PUBLISH_ENDCRYPT_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_PUBLISH_ENDCRYPT_CHIDX_Msk (0xFFUL << CCM_PUBLISH_ENDCRYPT_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_PUBLISH_ERROR */
/* Description: Deprecated register - Publish configuration for event ERROR */

/* Bit 31 :   */
#define CCM_PUBLISH_ERROR_EN_Pos (31UL) /*!< Position of EN field. */
#define CCM_PUBLISH_ERROR_EN_Msk (0x1UL << CCM_PUBLISH_ERROR_EN_Pos) /*!< Bit mask of EN field. */
#define CCM_PUBLISH_ERROR_EN_Disabled (0UL) /*!< Disable publishing */
#define CCM_PUBLISH_ERROR_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ERROR will publish to. */
#define CCM_PUBLISH_ERROR_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define CCM_PUBLISH_ERROR_CHIDX_Msk (0xFFUL << CCM_PUBLISH_ERROR_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: CCM_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 0 : Shortcut between event ENDKSGEN and task CRYPT */
#define CCM_SHORTS_ENDKSGEN_CRYPT_Pos (0UL) /*!< Position of ENDKSGEN_CRYPT field. */
#define CCM_SHORTS_ENDKSGEN_CRYPT_Msk (0x1UL << CCM_SHORTS_ENDKSGEN_CRYPT_Pos) /*!< Bit mask of ENDKSGEN_CRYPT field. */
#define CCM_SHORTS_ENDKSGEN_CRYPT_Disabled (0UL) /*!< Disable shortcut */
#define CCM_SHORTS_ENDKSGEN_CRYPT_Enabled (1UL) /*!< Enable shortcut */

/* Register: CCM_INTENSET */
/* Description: Enable interrupt */

/* Bit 2 : Deprecated intsetfield -  Write '1' to enable interrupt for event ERROR */
#define CCM_INTENSET_ERROR_Pos (2UL) /*!< Position of ERROR field. */
#define CCM_INTENSET_ERROR_Msk (0x1UL << CCM_INTENSET_ERROR_Pos) /*!< Bit mask of ERROR field. */
#define CCM_INTENSET_ERROR_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENSET_ERROR_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENSET_ERROR_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event ENDCRYPT */
#define CCM_INTENSET_ENDCRYPT_Pos (1UL) /*!< Position of ENDCRYPT field. */
#define CCM_INTENSET_ENDCRYPT_Msk (0x1UL << CCM_INTENSET_ENDCRYPT_Pos) /*!< Bit mask of ENDCRYPT field. */
#define CCM_INTENSET_ENDCRYPT_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENSET_ENDCRYPT_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENSET_ENDCRYPT_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event ENDKSGEN */
#define CCM_INTENSET_ENDKSGEN_Pos (0UL) /*!< Position of ENDKSGEN field. */
#define CCM_INTENSET_ENDKSGEN_Msk (0x1UL << CCM_INTENSET_ENDKSGEN_Pos) /*!< Bit mask of ENDKSGEN field. */
#define CCM_INTENSET_ENDKSGEN_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENSET_ENDKSGEN_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENSET_ENDKSGEN_Set (1UL) /*!< Enable */

/* Register: CCM_INTENCLR */
/* Description: Disable interrupt */

/* Bit 2 : Deprecated intclrfield -  Write '1' to disable interrupt for event ERROR */
#define CCM_INTENCLR_ERROR_Pos (2UL) /*!< Position of ERROR field. */
#define CCM_INTENCLR_ERROR_Msk (0x1UL << CCM_INTENCLR_ERROR_Pos) /*!< Bit mask of ERROR field. */
#define CCM_INTENCLR_ERROR_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENCLR_ERROR_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENCLR_ERROR_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable interrupt for event ENDCRYPT */
#define CCM_INTENCLR_ENDCRYPT_Pos (1UL) /*!< Position of ENDCRYPT field. */
#define CCM_INTENCLR_ENDCRYPT_Msk (0x1UL << CCM_INTENCLR_ENDCRYPT_Pos) /*!< Bit mask of ENDCRYPT field. */
#define CCM_INTENCLR_ENDCRYPT_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENCLR_ENDCRYPT_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENCLR_ENDCRYPT_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event ENDKSGEN */
#define CCM_INTENCLR_ENDKSGEN_Pos (0UL) /*!< Position of ENDKSGEN field. */
#define CCM_INTENCLR_ENDKSGEN_Msk (0x1UL << CCM_INTENCLR_ENDKSGEN_Pos) /*!< Bit mask of ENDKSGEN field. */
#define CCM_INTENCLR_ENDKSGEN_Disabled (0UL) /*!< Read: Disabled */
#define CCM_INTENCLR_ENDKSGEN_Enabled (1UL) /*!< Read: Enabled */
#define CCM_INTENCLR_ENDKSGEN_Clear (1UL) /*!< Disable */

/* Register: CCM_MICSTATUS */
/* Description: MIC check result */

/* Bit 0 : The result of the MIC check performed during the previous decryption operation */
#define CCM_MICSTATUS_MICSTATUS_Pos (0UL) /*!< Position of MICSTATUS field. */
#define CCM_MICSTATUS_MICSTATUS_Msk (0x1UL << CCM_MICSTATUS_MICSTATUS_Pos) /*!< Bit mask of MICSTATUS field. */
#define CCM_MICSTATUS_MICSTATUS_CheckFailed (0UL) /*!< MIC check failed */
#define CCM_MICSTATUS_MICSTATUS_CheckPassed (1UL) /*!< MIC check passed */

/* Register: CCM_ENABLE */
/* Description: Enable */

/* Bits 1..0 : Enable or disable CCM */
#define CCM_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
#define CCM_ENABLE_ENABLE_Msk (0x3UL << CCM_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
#define CCM_ENABLE_ENABLE_Disabled (0UL) /*!< Disable */
#define CCM_ENABLE_ENABLE_Enabled (2UL) /*!< Enable */

/* Register: CCM_MODE */
/* Description: Operation mode */

/* Bit 24 : Packet length configuration */
#define CCM_MODE_LENGTH_Pos (24UL) /*!< Position of LENGTH field. */
#define CCM_MODE_LENGTH_Msk (0x1UL << CCM_MODE_LENGTH_Pos) /*!< Bit mask of LENGTH field. */
#define CCM_MODE_LENGTH_Default (0UL) /*!< Default length. Effective length of LENGTH field in encrypted/decrypted packet is 5 bits. A keystream for packet payloads up to 27 bytes will be generated. */
#define CCM_MODE_LENGTH_Extended (1UL) /*!< Extended length. Effective length of LENGTH field in encrypted/decrypted packet is 8 bits. A keystream for packet payloads up to MAXPACKETSIZE bytes will be generated. */

/* Bits 17..16 : Radio data rate that the CCM shall run synchronous with */
#define CCM_MODE_DATARATE_Pos (16UL) /*!< Position of DATARATE field. */
#define CCM_MODE_DATARATE_Msk (0x3UL << CCM_MODE_DATARATE_Pos) /*!< Bit mask of DATARATE field. */
#define CCM_MODE_DATARATE_1Mbit (0UL) /*!< 1 Mbps */
#define CCM_MODE_DATARATE_2Mbit (1UL) /*!< 2 Mbps */
#define CCM_MODE_DATARATE_125Kbps (2UL) /*!< 125 kbps */
#define CCM_MODE_DATARATE_500Kbps (3UL) /*!< 500 kbps */

/* Bit 0 : The mode of operation to be used. Settings in this register apply whenever either the KSGEN task or the CRYPT task is triggered. */
#define CCM_MODE_MODE_Pos (0UL) /*!< Position of MODE field. */
#define CCM_MODE_MODE_Msk (0x1UL << CCM_MODE_MODE_Pos) /*!< Bit mask of MODE field. */
#define CCM_MODE_MODE_Encryption (0UL) /*!< AES CCM packet encryption mode */
#define CCM_MODE_MODE_Decryption (1UL) /*!< AES CCM packet decryption mode */

/* Register: CCM_CNFPTR */
/* Description: Pointer to data structure holding the AES key and the NONCE vector */

/* Bits 31..0 : Pointer to the data structure holding the AES key and the CCM NONCE vector (see table CCM data structure overview) */
#define CCM_CNFPTR_CNFPTR_Pos (0UL) /*!< Position of CNFPTR field. */
#define CCM_CNFPTR_CNFPTR_Msk (0xFFFFFFFFUL << CCM_CNFPTR_CNFPTR_Pos) /*!< Bit mask of CNFPTR field. */

/* Register: CCM_INPTR */
/* Description: Input pointer */

/* Bits 31..0 : Input pointer */
#define CCM_INPTR_INPTR_Pos (0UL) /*!< Position of INPTR field. */
#define CCM_INPTR_INPTR_Msk (0xFFFFFFFFUL << CCM_INPTR_INPTR_Pos) /*!< Bit mask of INPTR field. */

/* Register: CCM_OUTPTR */
/* Description: Output pointer */

/* Bits 31..0 : Output pointer */
#define CCM_OUTPTR_OUTPTR_Pos (0UL) /*!< Position of OUTPTR field. */
#define CCM_OUTPTR_OUTPTR_Msk (0xFFFFFFFFUL << CCM_OUTPTR_OUTPTR_Pos) /*!< Bit mask of OUTPTR field. */

/* Register: CCM_SCRATCHPTR */
/* Description: Pointer to data area used for temporary storage */

/* Bits 31..0 : Pointer to a scratch data area used for temporary storage during keystream generation,
        MIC generation and encryption/decryption. */
#define CCM_SCRATCHPTR_SCRATCHPTR_Pos (0UL) /*!< Position of SCRATCHPTR field. */
#define CCM_SCRATCHPTR_SCRATCHPTR_Msk (0xFFFFFFFFUL << CCM_SCRATCHPTR_SCRATCHPTR_Pos) /*!< Bit mask of SCRATCHPTR field. */

/* Register: CCM_MAXPACKETSIZE */
/* Description: Length of keystream generated when MODE.LENGTH = Extended */

/* Bits 7..0 : Length of keystream generated when MODE.LENGTH = Extended. This value must be greater than or equal to the subsequent packet payload to be encrypted/decrypted. */
#define CCM_MAXPACKETSIZE_MAXPACKETSIZE_Pos (0UL) /*!< Position of MAXPACKETSIZE field. */
#define CCM_MAXPACKETSIZE_MAXPACKETSIZE_Msk (0xFFUL << CCM_MAXPACKETSIZE_MAXPACKETSIZE_Pos) /*!< Bit mask of MAXPACKETSIZE field. */

/* Register: CCM_RATEOVERRIDE */
/* Description: Data rate override setting. */

/* Bits 1..0 : Data rate override setting */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_Pos (0UL) /*!< Position of RATEOVERRIDE field. */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_Msk (0x3UL << CCM_RATEOVERRIDE_RATEOVERRIDE_Pos) /*!< Bit mask of RATEOVERRIDE field. */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_1Mbit (0UL) /*!< 1 Mbps */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_2Mbit (1UL) /*!< 2 Mbps */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_125Kbps (2UL) /*!< 125 kbps */
#define CCM_RATEOVERRIDE_RATEOVERRIDE_500Kbps (3UL) /*!< 500 kbps */

/* Register: CCM_HEADERMASK */
/* Description: Header (S0) mask. */

/* Bits 7..0 : Header (S0) mask */
#define CCM_HEADERMASK_HEADERMASK_Pos (0UL) /*!< Position of HEADERMASK field. */
#define CCM_HEADERMASK_HEADERMASK_Msk (0xFFUL << CCM_HEADERMASK_HEADERMASK_Pos) /*!< Bit mask of HEADERMASK field. */



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
/* ================                                          ECB_NS                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief AES ECB Mode Encryption (ECB_NS)
  */

typedef struct {                                /*!< (@ 0x4100D000) ECB_NS Structure                                           */
  __OM  uint32_t  TASKS_STARTECB;               /*!< (@ 0x00000000) Start ECB block encrypt                                    */
  __OM  uint32_t  TASKS_STOPECB;                /*!< (@ 0x00000004) Abort a possible executing ECB operation                   */
  __IM  uint32_t  RESERVED[30];
  __IOM uint32_t  SUBSCRIBE_STARTECB;           /*!< (@ 0x00000080) Subscribe configuration for task STARTECB                  */
  __IOM uint32_t  SUBSCRIBE_STOPECB;            /*!< (@ 0x00000084) Subscribe configuration for task STOPECB                   */
  __IM  uint32_t  RESERVED1[30];
  __IOM uint32_t  EVENTS_ENDECB;                /*!< (@ 0x00000100) ECB block encrypt complete                                 */
  __IOM uint32_t  EVENTS_ERRORECB;              /*!< (@ 0x00000104) ECB block encrypt aborted because of a STOPECB
                                                                    task or due to an error                                    */
  __IM  uint32_t  RESERVED2[30];
  __IOM uint32_t  PUBLISH_ENDECB;               /*!< (@ 0x00000180) Publish configuration for event ENDECB                     */
  __IOM uint32_t  PUBLISH_ERRORECB;             /*!< (@ 0x00000184) Publish configuration for event ERRORECB                   */
  __IM  uint32_t  RESERVED3[95];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED4[126];
  __IOM uint32_t  ECBDATAPTR;                   /*!< (@ 0x00000504) ECB block encrypt memory pointers                          */
} NRF_ECB_Type;                                 /*!< Size = 1288 (0x508)                                                       */


/* Peripheral: ECB */
/* Description: AES ECB Mode Encryption */

/* Register: ECB_TASKS_STARTECB */
/* Description: Start ECB block encrypt */

/* Bit 0 : Start ECB block encrypt */
#define ECB_TASKS_STARTECB_TASKS_STARTECB_Pos (0UL) /*!< Position of TASKS_STARTECB field. */
#define ECB_TASKS_STARTECB_TASKS_STARTECB_Msk (0x1UL << ECB_TASKS_STARTECB_TASKS_STARTECB_Pos) /*!< Bit mask of TASKS_STARTECB field. */
#define ECB_TASKS_STARTECB_TASKS_STARTECB_Trigger (1UL) /*!< Trigger task */

/* Register: ECB_TASKS_STOPECB */
/* Description: Abort a possible executing ECB operation */

/* Bit 0 : Abort a possible executing ECB operation */
#define ECB_TASKS_STOPECB_TASKS_STOPECB_Pos (0UL) /*!< Position of TASKS_STOPECB field. */
#define ECB_TASKS_STOPECB_TASKS_STOPECB_Msk (0x1UL << ECB_TASKS_STOPECB_TASKS_STOPECB_Pos) /*!< Bit mask of TASKS_STOPECB field. */
#define ECB_TASKS_STOPECB_TASKS_STOPECB_Trigger (1UL) /*!< Trigger task */

/* Register: ECB_SUBSCRIBE_STARTECB */
/* Description: Subscribe configuration for task STARTECB */

/* Bit 31 :   */
#define ECB_SUBSCRIBE_STARTECB_EN_Pos (31UL) /*!< Position of EN field. */
#define ECB_SUBSCRIBE_STARTECB_EN_Msk (0x1UL << ECB_SUBSCRIBE_STARTECB_EN_Pos) /*!< Bit mask of EN field. */
#define ECB_SUBSCRIBE_STARTECB_EN_Disabled (0UL) /*!< Disable subscription */
#define ECB_SUBSCRIBE_STARTECB_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STARTECB will subscribe to */
#define ECB_SUBSCRIBE_STARTECB_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define ECB_SUBSCRIBE_STARTECB_CHIDX_Msk (0xFFUL << ECB_SUBSCRIBE_STARTECB_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: ECB_SUBSCRIBE_STOPECB */
/* Description: Subscribe configuration for task STOPECB */

/* Bit 31 :   */
#define ECB_SUBSCRIBE_STOPECB_EN_Pos (31UL) /*!< Position of EN field. */
#define ECB_SUBSCRIBE_STOPECB_EN_Msk (0x1UL << ECB_SUBSCRIBE_STOPECB_EN_Pos) /*!< Bit mask of EN field. */
#define ECB_SUBSCRIBE_STOPECB_EN_Disabled (0UL) /*!< Disable subscription */
#define ECB_SUBSCRIBE_STOPECB_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOPECB will subscribe to */
#define ECB_SUBSCRIBE_STOPECB_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define ECB_SUBSCRIBE_STOPECB_CHIDX_Msk (0xFFUL << ECB_SUBSCRIBE_STOPECB_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: ECB_EVENTS_ENDECB */
/* Description: ECB block encrypt complete */

/* Bit 0 : ECB block encrypt complete */
#define ECB_EVENTS_ENDECB_EVENTS_ENDECB_Pos (0UL) /*!< Position of EVENTS_ENDECB field. */
#define ECB_EVENTS_ENDECB_EVENTS_ENDECB_Msk (0x1UL << ECB_EVENTS_ENDECB_EVENTS_ENDECB_Pos) /*!< Bit mask of EVENTS_ENDECB field. */
#define ECB_EVENTS_ENDECB_EVENTS_ENDECB_NotGenerated (0UL) /*!< Event not generated */
#define ECB_EVENTS_ENDECB_EVENTS_ENDECB_Generated (1UL) /*!< Event generated */

/* Register: ECB_EVENTS_ERRORECB */
/* Description: ECB block encrypt aborted because of a STOPECB task or due to an error */

/* Bit 0 : ECB block encrypt aborted because of a STOPECB task or due to an error */
#define ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Pos (0UL) /*!< Position of EVENTS_ERRORECB field. */
#define ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Msk (0x1UL << ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Pos) /*!< Bit mask of EVENTS_ERRORECB field. */
#define ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_NotGenerated (0UL) /*!< Event not generated */
#define ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Generated (1UL) /*!< Event generated */

/* Register: ECB_PUBLISH_ENDECB */
/* Description: Publish configuration for event ENDECB */

/* Bit 31 :   */
#define ECB_PUBLISH_ENDECB_EN_Pos (31UL) /*!< Position of EN field. */
#define ECB_PUBLISH_ENDECB_EN_Msk (0x1UL << ECB_PUBLISH_ENDECB_EN_Pos) /*!< Bit mask of EN field. */
#define ECB_PUBLISH_ENDECB_EN_Disabled (0UL) /*!< Disable publishing */
#define ECB_PUBLISH_ENDECB_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ENDECB will publish to. */
#define ECB_PUBLISH_ENDECB_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define ECB_PUBLISH_ENDECB_CHIDX_Msk (0xFFUL << ECB_PUBLISH_ENDECB_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: ECB_PUBLISH_ERRORECB */
/* Description: Publish configuration for event ERRORECB */

/* Bit 31 :   */
#define ECB_PUBLISH_ERRORECB_EN_Pos (31UL) /*!< Position of EN field. */
#define ECB_PUBLISH_ERRORECB_EN_Msk (0x1UL << ECB_PUBLISH_ERRORECB_EN_Pos) /*!< Bit mask of EN field. */
#define ECB_PUBLISH_ERRORECB_EN_Disabled (0UL) /*!< Disable publishing */
#define ECB_PUBLISH_ERRORECB_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ERRORECB will publish to. */
#define ECB_PUBLISH_ERRORECB_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define ECB_PUBLISH_ERRORECB_CHIDX_Msk (0xFFUL << ECB_PUBLISH_ERRORECB_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: ECB_INTENSET */
/* Description: Enable interrupt */

/* Bit 1 : Write '1' to enable interrupt for event ERRORECB */
#define ECB_INTENSET_ERRORECB_Pos (1UL) /*!< Position of ERRORECB field. */
#define ECB_INTENSET_ERRORECB_Msk (0x1UL << ECB_INTENSET_ERRORECB_Pos) /*!< Bit mask of ERRORECB field. */
#define ECB_INTENSET_ERRORECB_Disabled (0UL) /*!< Read: Disabled */
#define ECB_INTENSET_ERRORECB_Enabled (1UL) /*!< Read: Enabled */
#define ECB_INTENSET_ERRORECB_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event ENDECB */
#define ECB_INTENSET_ENDECB_Pos (0UL) /*!< Position of ENDECB field. */
#define ECB_INTENSET_ENDECB_Msk (0x1UL << ECB_INTENSET_ENDECB_Pos) /*!< Bit mask of ENDECB field. */
#define ECB_INTENSET_ENDECB_Disabled (0UL) /*!< Read: Disabled */
#define ECB_INTENSET_ENDECB_Enabled (1UL) /*!< Read: Enabled */
#define ECB_INTENSET_ENDECB_Set (1UL) /*!< Enable */

/* Register: ECB_INTENCLR */
/* Description: Disable interrupt */

/* Bit 1 : Write '1' to disable interrupt for event ERRORECB */
#define ECB_INTENCLR_ERRORECB_Pos (1UL) /*!< Position of ERRORECB field. */
#define ECB_INTENCLR_ERRORECB_Msk (0x1UL << ECB_INTENCLR_ERRORECB_Pos) /*!< Bit mask of ERRORECB field. */
#define ECB_INTENCLR_ERRORECB_Disabled (0UL) /*!< Read: Disabled */
#define ECB_INTENCLR_ERRORECB_Enabled (1UL) /*!< Read: Enabled */
#define ECB_INTENCLR_ERRORECB_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event ENDECB */
#define ECB_INTENCLR_ENDECB_Pos (0UL) /*!< Position of ENDECB field. */
#define ECB_INTENCLR_ENDECB_Msk (0x1UL << ECB_INTENCLR_ENDECB_Pos) /*!< Bit mask of ENDECB field. */
#define ECB_INTENCLR_ENDECB_Disabled (0UL) /*!< Read: Disabled */
#define ECB_INTENCLR_ENDECB_Enabled (1UL) /*!< Read: Enabled */
#define ECB_INTENCLR_ENDECB_Clear (1UL) /*!< Disable */

/* Register: ECB_ECBDATAPTR */
/* Description: ECB block encrypt memory pointers */

/* Bits 31..0 : Pointer to the ECB data structure (see Table 1 ECB data structure overview) */
#define ECB_ECBDATAPTR_ECBDATAPTR_Pos (0UL) /*!< Position of ECBDATAPTR field. */
#define ECB_ECBDATAPTR_ECBDATAPTR_Msk (0xFFFFFFFFUL << ECB_ECBDATAPTR_ECBDATAPTR_Pos) /*!< Bit mask of ECBDATAPTR field. */


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
  __OM  NRF_DPPIC_TASKS_CHG_Type TASKS_CHG[6];      /*!< (@ 0x00000000) Channel group tasks                                        */
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
/* ================                                          FICR                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief Factory Information Configuration Registers (FICR)
  */

typedef struct {                                /*!< (@ 0x00FF0000) FICR_S Structure                                           */
  __IM  uint32_t  RESERVED[128];
  __IOM FICR_INFO_Type INFO;                    /*!< (@ 0x00000200) Device info                                                */
  __IM  uint32_t  RESERVED1[53];
  __IOM FICR_TRIMCNF_Type TRIMCNF[32];          /*!< (@ 0x00000300) Unspecified                                                */
  __IM  uint32_t  RESERVED2[20];
  __IOM FICR_NFC_Type NFC;                      /*!< (@ 0x00000450) Unspecified                                                */
  __IM  uint32_t  RESERVED3[488];
  __IOM FICR_TRNG90B_Type TRNG90B;              /*!< (@ 0x00000C00) NIST800-90B RNG calibration data                           */
  __IM  uint32_t  XOSC32MTRIM;                  /*!< (@ 0x00000C20) XOSC32M capacitor selection trim values                    */
} NRF_FICR_APP_Type;                            /*!< Size = 3108 (0xc24)                                                       */

typedef struct {                                /*!< (@ 0x01FF0000) FICR_NS Structure                                          */
  __IM  uint32_t  RESERVED[128];
  __IOM FICR_INFO_Type INFO;                    /*!< (@ 0x00000200) Device info                                                */
  __IM  uint32_t  RESERVED1[21];
  __IM  uint32_t  ER[4];                        /*!< (@ 0x00000280) Description collection: Encryption Root, word
                                                                    n                                                          */
  __IM  uint32_t  IR[4];                        /*!< (@ 0x00000290) Description collection: Identity Root, word n              */
  __IM  uint32_t  DEVICEADDRTYPE;               /*!< (@ 0x000002A0) Device address type                                        */
  __IM  uint32_t  DEVICEADDR[2];                /*!< (@ 0x000002A4) Description collection: Device address n                   */
  __IM  uint32_t  RESERVED2[21];
  __IOM FICR_TRIMCNF_Type TRIMCNF[32];          /*!< (@ 0x00000300) Unspecified                                                */
} NRF_FICR_NET_Type;                            /*!< Size = 1024 (0x400)                                                       */



/* =========================================================================================================================== */
/* ================                                          IPC_NS                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief Interprocessor communication (IPC_NS)
  */

typedef struct {                                /*!< (@ 0x41012000) IPC_NS Structure                                           */
  __OM  uint32_t  TASKS_SEND[16];               /*!< (@ 0x00000000) Description collection: Trigger events on IPC
                                                                    channel enabled in SEND_CNF[n]                             */
  __IM  uint32_t  RESERVED[16];
  __IOM uint32_t  SUBSCRIBE_SEND[16];           /*!< (@ 0x00000080) Description collection: Subscribe configuration
                                                                    for task SEND[n]                                           */
  __IM  uint32_t  RESERVED1[16];
  __IOM uint32_t  EVENTS_RECEIVE[16];           /*!< (@ 0x00000100) Description collection: Event received on one
                                                                    or more of the enabled IPC channels in RECEIVE_CNF[n]      */
  __IM  uint32_t  RESERVED2[16];
  __IOM uint32_t  PUBLISH_RECEIVE[16];          /*!< (@ 0x00000180) Description collection: Publish configuration
                                                                    for event RECEIVE[n]                                       */
  __IM  uint32_t  RESERVED3[80];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  INTPEND;                      /*!< (@ 0x0000030C) Pending interrupts                                         */
  __IM  uint32_t  RESERVED4[128];
  __IOM uint32_t  SEND_CNF[16];                 /*!< (@ 0x00000510) Description collection: Send event configuration
                                                                    for TASKS_SEND[n]                                          */
  __IM  uint32_t  RESERVED5[16];
  __IOM uint32_t  RECEIVE_CNF[16];              /*!< (@ 0x00000590) Description collection: Receive event configuration
                                                                    for EVENTS_RECEIVE[n]                                      */
  __IM  uint32_t  RESERVED6[16];
  __IOM uint32_t  GPMEM[2];                     /*!< (@ 0x00000610) Description collection: General purpose memory             */
} NRF_IPC_Type;                                 /*!< Size = 1560 (0x618)                                                       */


/* =========================================================================================================================== */
/* ================                                        APPMUTEX_NS                                        ================ */
/* =========================================================================================================================== */


/**
  * @brief MUTEX 0 (APPMUTEX_NS)
  */

typedef struct {                                /*!< (@ 0x40030000) APPMUTEX_NS Structure                                      */
  __IM  uint32_t  RESERVED[256];
  __IOM uint32_t  MUTEX[16];                    /*!< (@ 0x00000400) Description collection: Mutex register                     */
} NRF_MUTEX_Type;                               /*!< Size = 1088 (0x440)                                                       */



/* =========================================================================================================================== */
/* ================                                          NVMC                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief Non-volatile memory controller (NVMC)
  *
  * Note: This is the app core register layouts
  * as by now the icache registers are just ignored
  */

typedef struct {                                /*!< (@ 0x40039000) NVMC_NS Structure                                          */
  __IM  uint32_t  RESERVED[256];
  __IM  uint32_t  READY;                        /*!< (@ 0x00000400) Ready flag                                                 */
  __IM  uint32_t  RESERVED1;
  __IM  uint32_t  READYNEXT;                    /*!< (@ 0x00000408) Ready flag                                                 */
  __IM  uint32_t  RESERVED2[62];
  __IOM uint32_t  CONFIG;                       /*!< (@ 0x00000504) Configuration register                                     */
  __IM  uint32_t  RESERVED3;
  __OM  uint32_t  ERASEALL;                     /*!< (@ 0x0000050C) Register for erasing all non-volatile user memory          */
  __IM  uint32_t  RESERVED4[3];
  __IOM uint32_t  ERASEPAGEPARTIALCFG;          /*!< (@ 0x0000051C) Register for partial erase configuration                   */
  __IM  uint32_t  RESERVED5[25];
  __IOM uint32_t  CONFIGNS;                     /*!< (@ 0x00000584) Non-secure configuration register                          */
  __OM  uint32_t  WRITEUICRNS;                  /*!< (@ 0x00000588) Non-secure APPROTECT enable register                       */
} NRF_NVMC_Type;                                /*!< Size = 1420 (0x58c)                                                       */


/* Peripheral: NVMC */
/* Description: Non-volatile memory controller */

/* Register: NVMC_READY */
/* Description: Ready flag */

/* Bit 0 : NVMC is ready or busy */
#define NVMC_READY_READY_Pos (0UL) /*!< Position of READY field. */
#define NVMC_READY_READY_Msk (0x1UL << NVMC_READY_READY_Pos) /*!< Bit mask of READY field. */
#define NVMC_READY_READY_Busy (0UL) /*!< NVMC is busy (ongoing write or erase operation) */
#define NVMC_READY_READY_Ready (1UL) /*!< NVMC is ready */

/* Register: NVMC_READYNEXT */
/* Description: Ready flag */

/* Bit 0 : NVMC can accept a new write operation */
#define NVMC_READYNEXT_READYNEXT_Pos (0UL) /*!< Position of READYNEXT field. */
#define NVMC_READYNEXT_READYNEXT_Msk (0x1UL << NVMC_READYNEXT_READYNEXT_Pos) /*!< Bit mask of READYNEXT field. */
#define NVMC_READYNEXT_READYNEXT_Busy (0UL) /*!< NVMC cannot accept any write operation */
#define NVMC_READYNEXT_READYNEXT_Ready (1UL) /*!< NVMC is ready */

/* Register: NVMC_CONFIG */
/* Description: Configuration register */

/* Bits 2..0 : Program memory access mode. It is strongly recommended to only activate erase and write modes when they are actively used. Enabling write or erase will invalidate the cache and keep it invalidated. */
#define NVMC_CONFIG_WEN_Pos (0UL) /*!< Position of WEN field. */
#define NVMC_CONFIG_WEN_Msk (0x7UL << NVMC_CONFIG_WEN_Pos) /*!< Bit mask of WEN field. */
#define NVMC_CONFIG_WEN_Ren (0UL) /*!< Read only access */
#define NVMC_CONFIG_WEN_Wen (1UL) /*!< Write enabled */
#define NVMC_CONFIG_WEN_Een (2UL) /*!< Erase enabled */
#define NVMC_CONFIG_WEN_PEen (4UL) /*!< Partial erase enabled */

/* Register: NVMC_ERASEALL */
/* Description: Register for erasing all non-volatile user memory */

/* Bit 0 : Erase all non-volatile memory including UICR registers. Before the non-volatile memory can be erased, erasing must be enabled by setting CONFIG.WEN=Een. */
#define NVMC_ERASEALL_ERASEALL_Pos (0UL) /*!< Position of ERASEALL field. */
#define NVMC_ERASEALL_ERASEALL_Msk (0x1UL << NVMC_ERASEALL_ERASEALL_Pos) /*!< Bit mask of ERASEALL field. */
#define NVMC_ERASEALL_ERASEALL_NoOperation (0UL) /*!< No operation */
#define NVMC_ERASEALL_ERASEALL_Erase (1UL) /*!< Start chip erase */

/* Register: NVMC_ERASEPAGEPARTIALCFG */
/* Description: Register for partial erase configuration */

/* Bits 6..0 : Duration of the partial erase in milliseconds */
#define NVMC_ERASEPAGEPARTIALCFG_DURATION_Pos (0UL) /*!< Position of DURATION field. */
#define NVMC_ERASEPAGEPARTIALCFG_DURATION_Msk (0x7FUL << NVMC_ERASEPAGEPARTIALCFG_DURATION_Pos) /*!< Bit mask of DURATION field. */


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
/* ================                                         RADIO_NS                                          ================ */
/* =========================================================================================================================== */


/**
  * @brief 2.4 GHz radio (RADIO_NS)
  */

typedef struct {                                /*!< (@ 0x41008000) RADIO_NS Structure                                         */
  __OM  uint32_t  TASKS_TXEN;                   /*!< (@ 0x00000000) Enable RADIO in TX mode                                    */
  __OM  uint32_t  TASKS_RXEN;                   /*!< (@ 0x00000004) Enable RADIO in RX mode                                    */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000008) Start RADIO                                                */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x0000000C) Stop RADIO                                                 */
  __OM  uint32_t  TASKS_DISABLE;                /*!< (@ 0x00000010) Disable RADIO                                              */
  __OM  uint32_t  TASKS_RSSISTART;              /*!< (@ 0x00000014) Start the RSSI and take one single sample of
                                                                    the receive signal strength                                */
  __OM  uint32_t  TASKS_RSSISTOP;               /*!< (@ 0x00000018) Stop the RSSI measurement                                  */
  __OM  uint32_t  TASKS_BCSTART;                /*!< (@ 0x0000001C) Start the bit counter                                      */
  __OM  uint32_t  TASKS_BCSTOP;                 /*!< (@ 0x00000020) Stop the bit counter                                       */
  __OM  uint32_t  TASKS_EDSTART;                /*!< (@ 0x00000024) Start the energy detect measurement used in IEEE
                                                                    802.15.4 mode                                              */
  __OM  uint32_t  TASKS_EDSTOP;                 /*!< (@ 0x00000028) Stop the energy detect measurement                         */
  __OM  uint32_t  TASKS_CCASTART;               /*!< (@ 0x0000002C) Start the clear channel assessment used in IEEE
                                                                    802.15.4 mode                                              */
  __OM  uint32_t  TASKS_CCASTOP;                /*!< (@ 0x00000030) Stop the clear channel assessment                          */
  __IM  uint32_t  RESERVED[19];
  __IOM uint32_t  SUBSCRIBE_TXEN;               /*!< (@ 0x00000080) Subscribe configuration for task TXEN                      */
  __IOM uint32_t  SUBSCRIBE_RXEN;               /*!< (@ 0x00000084) Subscribe configuration for task RXEN                      */
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000088) Subscribe configuration for task START                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x0000008C) Subscribe configuration for task STOP                      */
  __IOM uint32_t  SUBSCRIBE_DISABLE;            /*!< (@ 0x00000090) Subscribe configuration for task DISABLE                   */
  __IOM uint32_t  SUBSCRIBE_RSSISTART;          /*!< (@ 0x00000094) Subscribe configuration for task RSSISTART                 */
  __IOM uint32_t  SUBSCRIBE_RSSISTOP;           /*!< (@ 0x00000098) Subscribe configuration for task RSSISTOP                  */
  __IOM uint32_t  SUBSCRIBE_BCSTART;            /*!< (@ 0x0000009C) Subscribe configuration for task BCSTART                   */
  __IOM uint32_t  SUBSCRIBE_BCSTOP;             /*!< (@ 0x000000A0) Subscribe configuration for task BCSTOP                    */
  __IOM uint32_t  SUBSCRIBE_EDSTART;            /*!< (@ 0x000000A4) Subscribe configuration for task EDSTART                   */
  __IOM uint32_t  SUBSCRIBE_EDSTOP;             /*!< (@ 0x000000A8) Subscribe configuration for task EDSTOP                    */
  __IOM uint32_t  SUBSCRIBE_CCASTART;           /*!< (@ 0x000000AC) Subscribe configuration for task CCASTART                  */
  __IOM uint32_t  SUBSCRIBE_CCASTOP;            /*!< (@ 0x000000B0) Subscribe configuration for task CCASTOP                   */
  __IM  uint32_t  RESERVED1[19];
  __IOM uint32_t  EVENTS_READY;                 /*!< (@ 0x00000100) RADIO has ramped up and is ready to be started             */
  __IOM uint32_t  EVENTS_ADDRESS;               /*!< (@ 0x00000104) Address sent or received                                   */
  __IOM uint32_t  EVENTS_PAYLOAD;               /*!< (@ 0x00000108) Packet payload sent or received                            */
  __IOM uint32_t  EVENTS_END;                   /*!< (@ 0x0000010C) Packet sent or received                                    */
  __IOM uint32_t  EVENTS_DISABLED;              /*!< (@ 0x00000110) RADIO has been disabled                                    */
  __IOM uint32_t  EVENTS_DEVMATCH;              /*!< (@ 0x00000114) A device address match occurred on the last received
                                                                    packet                                                     */
  __IOM uint32_t  EVENTS_DEVMISS;               /*!< (@ 0x00000118) No device address match occurred on the last
                                                                    received packet                                            */
  __IOM uint32_t  EVENTS_RSSIEND;               /*!< (@ 0x0000011C) Sampling of receive signal strength complete               */
  __IM  uint32_t  RESERVED2[2];
  __IOM uint32_t  EVENTS_BCMATCH;               /*!< (@ 0x00000128) Bit counter reached bit count value                        */
  __IM  uint32_t  RESERVED3;
  __IOM uint32_t  EVENTS_CRCOK;                 /*!< (@ 0x00000130) Packet received with CRC ok                                */
  __IOM uint32_t  EVENTS_CRCERROR;              /*!< (@ 0x00000134) Packet received with CRC error                             */
  __IOM uint32_t  EVENTS_FRAMESTART;            /*!< (@ 0x00000138) IEEE 802.15.4 length field received                        */
  __IOM uint32_t  EVENTS_EDEND;                 /*!< (@ 0x0000013C) Sampling of energy detection complete. A new
                                                                    ED sample is ready for readout from the
                                                                    RADIO.EDSAMPLE register.                                   */
  __IOM uint32_t  EVENTS_EDSTOPPED;             /*!< (@ 0x00000140) The sampling of energy detection has stopped               */
  __IOM uint32_t  EVENTS_CCAIDLE;               /*!< (@ 0x00000144) Wireless medium in idle - clear to send                    */
  __IOM uint32_t  EVENTS_CCABUSY;               /*!< (@ 0x00000148) Wireless medium busy - do not send                         */
  __IOM uint32_t  EVENTS_CCASTOPPED;            /*!< (@ 0x0000014C) The CCA has stopped                                        */
  __IOM uint32_t  EVENTS_RATEBOOST;             /*!< (@ 0x00000150) Ble_LR CI field received, receive mode is changed
                                                                    from Ble_LR125Kbit to Ble_LR500Kbit.                       */
  __IOM uint32_t  EVENTS_TXREADY;               /*!< (@ 0x00000154) RADIO has ramped up and is ready to be started
                                                                    TX path                                                    */
  __IOM uint32_t  EVENTS_RXREADY;               /*!< (@ 0x00000158) RADIO has ramped up and is ready to be started
                                                                    RX path                                                    */
  __IOM uint32_t  EVENTS_MHRMATCH;              /*!< (@ 0x0000015C) MAC header match found                                     */
  __IM  uint32_t  RESERVED4[2];
  __IOM uint32_t  EVENTS_SYNC;                  /*!< (@ 0x00000168) Preamble indicator                                         */
  __IOM uint32_t  EVENTS_PHYEND;                /*!< (@ 0x0000016C) Generated when last bit is sent on air, or received
                                                                    from air                                                   */
  __IOM uint32_t  EVENTS_CTEPRESENT;            /*!< (@ 0x00000170) CTE is present (early warning right after receiving
                                                                    CTEInfo byte)                                              */
  __IM  uint32_t  RESERVED5[3];
  __IOM uint32_t  PUBLISH_READY;                /*!< (@ 0x00000180) Publish configuration for event READY                      */
  __IOM uint32_t  PUBLISH_ADDRESS;              /*!< (@ 0x00000184) Publish configuration for event ADDRESS                    */
  __IOM uint32_t  PUBLISH_PAYLOAD;              /*!< (@ 0x00000188) Publish configuration for event PAYLOAD                    */
  __IOM uint32_t  PUBLISH_END;                  /*!< (@ 0x0000018C) Publish configuration for event END                        */
  __IOM uint32_t  PUBLISH_DISABLED;             /*!< (@ 0x00000190) Publish configuration for event DISABLED                   */
  __IOM uint32_t  PUBLISH_DEVMATCH;             /*!< (@ 0x00000194) Publish configuration for event DEVMATCH                   */
  __IOM uint32_t  PUBLISH_DEVMISS;              /*!< (@ 0x00000198) Publish configuration for event DEVMISS                    */
  __IOM uint32_t  PUBLISH_RSSIEND;              /*!< (@ 0x0000019C) Publish configuration for event RSSIEND                    */
  __IM  uint32_t  RESERVED6[2];
  __IOM uint32_t  PUBLISH_BCMATCH;              /*!< (@ 0x000001A8) Publish configuration for event BCMATCH                    */
  __IM  uint32_t  RESERVED7;
  __IOM uint32_t  PUBLISH_CRCOK;                /*!< (@ 0x000001B0) Publish configuration for event CRCOK                      */
  __IOM uint32_t  PUBLISH_CRCERROR;             /*!< (@ 0x000001B4) Publish configuration for event CRCERROR                   */
  __IOM uint32_t  PUBLISH_FRAMESTART;           /*!< (@ 0x000001B8) Publish configuration for event FRAMESTART                 */
  __IOM uint32_t  PUBLISH_EDEND;                /*!< (@ 0x000001BC) Publish configuration for event EDEND                      */
  __IOM uint32_t  PUBLISH_EDSTOPPED;            /*!< (@ 0x000001C0) Publish configuration for event EDSTOPPED                  */
  __IOM uint32_t  PUBLISH_CCAIDLE;              /*!< (@ 0x000001C4) Publish configuration for event CCAIDLE                    */
  __IOM uint32_t  PUBLISH_CCABUSY;              /*!< (@ 0x000001C8) Publish configuration for event CCABUSY                    */
  __IOM uint32_t  PUBLISH_CCASTOPPED;           /*!< (@ 0x000001CC) Publish configuration for event CCASTOPPED                 */
  __IOM uint32_t  PUBLISH_RATEBOOST;            /*!< (@ 0x000001D0) Publish configuration for event RATEBOOST                  */
  __IOM uint32_t  PUBLISH_TXREADY;              /*!< (@ 0x000001D4) Publish configuration for event TXREADY                    */
  __IOM uint32_t  PUBLISH_RXREADY;              /*!< (@ 0x000001D8) Publish configuration for event RXREADY                    */
  __IOM uint32_t  PUBLISH_MHRMATCH;             /*!< (@ 0x000001DC) Publish configuration for event MHRMATCH                   */
  __IM  uint32_t  RESERVED8[2];
  __IOM uint32_t  PUBLISH_SYNC;                 /*!< (@ 0x000001E8) Publish configuration for event SYNC                       */
  __IOM uint32_t  PUBLISH_PHYEND;               /*!< (@ 0x000001EC) Publish configuration for event PHYEND                     */
  __IOM uint32_t  PUBLISH_CTEPRESENT;           /*!< (@ 0x000001F0) Publish configuration for event CTEPRESENT                 */
  __IM  uint32_t  RESERVED9[3];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED10[64];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED11[61];
  __IM  uint32_t  CRCSTATUS;                    /*!< (@ 0x00000400) CRC status                                                 */
  __IM  uint32_t  RESERVED12;
  __IM  uint32_t  RXMATCH;                      /*!< (@ 0x00000408) Received address                                           */
  __IM  uint32_t  RXCRC;                        /*!< (@ 0x0000040C) CRC field of previously received packet                    */
  __IM  uint32_t  DAI;                          /*!< (@ 0x00000410) Device address match index                                 */
  __IM  uint32_t  PDUSTAT;                      /*!< (@ 0x00000414) Payload status                                             */
  __IM  uint32_t  RESERVED13[13];
  __IM  uint32_t  CTESTATUS;                    /*!< (@ 0x0000044C) CTEInfo parsed from received packet                        */
  __IM  uint32_t  RESERVED14[2];
  __IM  uint32_t  DFESTATUS;                    /*!< (@ 0x00000458) DFE status information                                     */
  __IM  uint32_t  RESERVED15[42];
  __IOM uint32_t  PACKETPTR;                    /*!< (@ 0x00000504) Packet pointer                                             */
  __IOM uint32_t  FREQUENCY;                    /*!< (@ 0x00000508) Frequency                                                  */
  __IOM uint32_t  TXPOWER;                      /*!< (@ 0x0000050C) Output power                                               */
  __IOM uint32_t  MODE;                         /*!< (@ 0x00000510) Data rate and modulation                                   */
  __IOM uint32_t  PCNF0;                        /*!< (@ 0x00000514) Packet configuration register 0                            */
  __IOM uint32_t  PCNF1;                        /*!< (@ 0x00000518) Packet configuration register 1                            */
  __IOM uint32_t  BASE0;                        /*!< (@ 0x0000051C) Base address 0                                             */
  __IOM uint32_t  BASE1;                        /*!< (@ 0x00000520) Base address 1                                             */
  __IOM uint32_t  PREFIX0;                      /*!< (@ 0x00000524) Prefixes bytes for logical addresses 0-3                   */
  __IOM uint32_t  PREFIX1;                      /*!< (@ 0x00000528) Prefixes bytes for logical addresses 4-7                   */
  __IOM uint32_t  TXADDRESS;                    /*!< (@ 0x0000052C) Transmit address select                                    */
  __IOM uint32_t  RXADDRESSES;                  /*!< (@ 0x00000530) Receive address select                                     */
  __IOM uint32_t  CRCCNF;                       /*!< (@ 0x00000534) CRC configuration                                          */
  __IOM uint32_t  CRCPOLY;                      /*!< (@ 0x00000538) CRC polynomial                                             */
  __IOM uint32_t  CRCINIT;                      /*!< (@ 0x0000053C) CRC initial value                                          */
  __IM  uint32_t  RESERVED16;
  __IOM uint32_t  TIFS;                         /*!< (@ 0x00000544) Interframe spacing in us                                   */
  __IM  uint32_t  RSSISAMPLE;                   /*!< (@ 0x00000548) RSSI sample                                                */
  __IM  uint32_t  RESERVED17;
  __IM  uint32_t  STATE;                        /*!< (@ 0x00000550) Current radio state                                        */
  __IOM uint32_t  DATAWHITEIV;                  /*!< (@ 0x00000554) Data whitening initial value                               */
  __IM  uint32_t  RESERVED18[2];
  __IOM uint32_t  BCC;                          /*!< (@ 0x00000560) Bit counter compare                                        */
  __IM  uint32_t  RESERVED19[39];
  __IOM uint32_t  DAB[8];                       /*!< (@ 0x00000600) Description collection: Device address base segment
                                                                    n                                                          */
  __IOM uint32_t  DAP[8];                       /*!< (@ 0x00000620) Description collection: Device address prefix
                                                                    n                                                          */
  __IOM uint32_t  DACNF;                        /*!< (@ 0x00000640) Device address match configuration                         */
  __IOM uint32_t  MHRMATCHCONF;                 /*!< (@ 0x00000644) Search pattern configuration                               */
  __IOM uint32_t  MHRMATCHMAS;                  /*!< (@ 0x00000648) Pattern mask                                               */
  __IM  uint32_t  RESERVED20;
  __IOM uint32_t  MODECNF0;                     /*!< (@ 0x00000650) Radio mode configuration register 0                        */
  __IM  uint32_t  RESERVED21[3];
  __IOM uint32_t  SFD;                          /*!< (@ 0x00000660) IEEE 802.15.4 start of frame delimiter                     */
  __IOM uint32_t  EDCNT;                        /*!< (@ 0x00000664) IEEE 802.15.4 energy detect loop count                     */
  __IM  uint32_t  EDSAMPLE;                     /*!< (@ 0x00000668) IEEE 802.15.4 energy detect level                          */
  __IOM uint32_t  CCACTRL;                      /*!< (@ 0x0000066C) IEEE 802.15.4 clear channel assessment control             */
  __IM  uint32_t  RESERVED22[164];
  __IOM uint32_t  DFEMODE;                      /*!< (@ 0x00000900) Whether to use Angle-of-Arrival (AOA) or Angle-of-Departure
                                                                    (AOD)                                                      */
  __IOM uint32_t  CTEINLINECONF;                /*!< (@ 0x00000904) Configuration for CTE inline mode                          */
  __IM  uint32_t  RESERVED23[2];
  __IOM uint32_t  DFECTRL1;                     /*!< (@ 0x00000910) Various configuration for Direction finding                */
  __IOM uint32_t  DFECTRL2;                     /*!< (@ 0x00000914) Start offset for Direction finding                         */
  __IM  uint32_t  RESERVED24[4];
  __IOM uint32_t  SWITCHPATTERN;                /*!< (@ 0x00000928) GPIO patterns to be used for each antenna                  */
  __IOM uint32_t  CLEARPATTERN;                 /*!< (@ 0x0000092C) Clear the GPIO pattern array for antenna control           */
  __IOM RADIO_PSEL_Type PSEL;                   /*!< (@ 0x00000930) Unspecified                                                */
  __IOM RADIO_DFEPACKET_Type DFEPACKET;         /*!< (@ 0x00000950) DFE packet EasyDMA channel                                 */
  __IM  uint32_t  RESERVED25[424];
  __IOM uint32_t  POWER;                        /*!< (@ 0x00000FFC) Peripheral power control                                   */
} NRF_RADIO_Type;                               /*!< Size = 4096 (0x1000)                                                      */


/* Peripheral: RADIO */
/* Description: 2.4 GHz radio */

/* Register: RADIO_TASKS_TXEN */
/* Description: Enable RADIO in TX mode */

/* Bit 0 : Enable RADIO in TX mode */
#define RADIO_TASKS_TXEN_TASKS_TXEN_Pos (0UL) /*!< Position of TASKS_TXEN field. */
#define RADIO_TASKS_TXEN_TASKS_TXEN_Msk (0x1UL << RADIO_TASKS_TXEN_TASKS_TXEN_Pos) /*!< Bit mask of TASKS_TXEN field. */
#define RADIO_TASKS_TXEN_TASKS_TXEN_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_RXEN */
/* Description: Enable RADIO in RX mode */

/* Bit 0 : Enable RADIO in RX mode */
#define RADIO_TASKS_RXEN_TASKS_RXEN_Pos (0UL) /*!< Position of TASKS_RXEN field. */
#define RADIO_TASKS_RXEN_TASKS_RXEN_Msk (0x1UL << RADIO_TASKS_RXEN_TASKS_RXEN_Pos) /*!< Bit mask of TASKS_RXEN field. */
#define RADIO_TASKS_RXEN_TASKS_RXEN_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_START */
/* Description: Start RADIO */

/* Bit 0 : Start RADIO */
#define RADIO_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define RADIO_TASKS_START_TASKS_START_Msk (0x1UL << RADIO_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define RADIO_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_STOP */
/* Description: Stop RADIO */

/* Bit 0 : Stop RADIO */
#define RADIO_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define RADIO_TASKS_STOP_TASKS_STOP_Msk (0x1UL << RADIO_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define RADIO_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_DISABLE */
/* Description: Disable RADIO */

/* Bit 0 : Disable RADIO */
#define RADIO_TASKS_DISABLE_TASKS_DISABLE_Pos (0UL) /*!< Position of TASKS_DISABLE field. */
#define RADIO_TASKS_DISABLE_TASKS_DISABLE_Msk (0x1UL << RADIO_TASKS_DISABLE_TASKS_DISABLE_Pos) /*!< Bit mask of TASKS_DISABLE field. */
#define RADIO_TASKS_DISABLE_TASKS_DISABLE_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_RSSISTART */
/* Description: Start the RSSI and take one single sample of the receive signal strength */

/* Bit 0 : Start the RSSI and take one single sample of the receive signal strength */
#define RADIO_TASKS_RSSISTART_TASKS_RSSISTART_Pos (0UL) /*!< Position of TASKS_RSSISTART field. */
#define RADIO_TASKS_RSSISTART_TASKS_RSSISTART_Msk (0x1UL << RADIO_TASKS_RSSISTART_TASKS_RSSISTART_Pos) /*!< Bit mask of TASKS_RSSISTART field. */
#define RADIO_TASKS_RSSISTART_TASKS_RSSISTART_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_RSSISTOP */
/* Description: Stop the RSSI measurement */

/* Bit 0 : Stop the RSSI measurement */
#define RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Pos (0UL) /*!< Position of TASKS_RSSISTOP field. */
#define RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Msk (0x1UL << RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Pos) /*!< Bit mask of TASKS_RSSISTOP field. */
#define RADIO_TASKS_RSSISTOP_TASKS_RSSISTOP_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_BCSTART */
/* Description: Start the bit counter */

/* Bit 0 : Start the bit counter */
#define RADIO_TASKS_BCSTART_TASKS_BCSTART_Pos (0UL) /*!< Position of TASKS_BCSTART field. */
#define RADIO_TASKS_BCSTART_TASKS_BCSTART_Msk (0x1UL << RADIO_TASKS_BCSTART_TASKS_BCSTART_Pos) /*!< Bit mask of TASKS_BCSTART field. */
#define RADIO_TASKS_BCSTART_TASKS_BCSTART_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_BCSTOP */
/* Description: Stop the bit counter */

/* Bit 0 : Stop the bit counter */
#define RADIO_TASKS_BCSTOP_TASKS_BCSTOP_Pos (0UL) /*!< Position of TASKS_BCSTOP field. */
#define RADIO_TASKS_BCSTOP_TASKS_BCSTOP_Msk (0x1UL << RADIO_TASKS_BCSTOP_TASKS_BCSTOP_Pos) /*!< Bit mask of TASKS_BCSTOP field. */
#define RADIO_TASKS_BCSTOP_TASKS_BCSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_EDSTART */
/* Description: Start the energy detect measurement used in IEEE 802.15.4 mode */

/* Bit 0 : Start the energy detect measurement used in IEEE 802.15.4 mode */
#define RADIO_TASKS_EDSTART_TASKS_EDSTART_Pos (0UL) /*!< Position of TASKS_EDSTART field. */
#define RADIO_TASKS_EDSTART_TASKS_EDSTART_Msk (0x1UL << RADIO_TASKS_EDSTART_TASKS_EDSTART_Pos) /*!< Bit mask of TASKS_EDSTART field. */
#define RADIO_TASKS_EDSTART_TASKS_EDSTART_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_EDSTOP */
/* Description: Stop the energy detect measurement */

/* Bit 0 : Stop the energy detect measurement */
#define RADIO_TASKS_EDSTOP_TASKS_EDSTOP_Pos (0UL) /*!< Position of TASKS_EDSTOP field. */
#define RADIO_TASKS_EDSTOP_TASKS_EDSTOP_Msk (0x1UL << RADIO_TASKS_EDSTOP_TASKS_EDSTOP_Pos) /*!< Bit mask of TASKS_EDSTOP field. */
#define RADIO_TASKS_EDSTOP_TASKS_EDSTOP_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_CCASTART */
/* Description: Start the clear channel assessment used in IEEE 802.15.4 mode */

/* Bit 0 : Start the clear channel assessment used in IEEE 802.15.4 mode */
#define RADIO_TASKS_CCASTART_TASKS_CCASTART_Pos (0UL) /*!< Position of TASKS_CCASTART field. */
#define RADIO_TASKS_CCASTART_TASKS_CCASTART_Msk (0x1UL << RADIO_TASKS_CCASTART_TASKS_CCASTART_Pos) /*!< Bit mask of TASKS_CCASTART field. */
#define RADIO_TASKS_CCASTART_TASKS_CCASTART_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_TASKS_CCASTOP */
/* Description: Stop the clear channel assessment */

/* Bit 0 : Stop the clear channel assessment */
#define RADIO_TASKS_CCASTOP_TASKS_CCASTOP_Pos (0UL) /*!< Position of TASKS_CCASTOP field. */
#define RADIO_TASKS_CCASTOP_TASKS_CCASTOP_Msk (0x1UL << RADIO_TASKS_CCASTOP_TASKS_CCASTOP_Pos) /*!< Bit mask of TASKS_CCASTOP field. */
#define RADIO_TASKS_CCASTOP_TASKS_CCASTOP_Trigger (1UL) /*!< Trigger task */

/* Register: RADIO_SUBSCRIBE_TXEN */
/* Description: Subscribe configuration for task TXEN */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_TXEN_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_TXEN_EN_Msk (0x1UL << RADIO_SUBSCRIBE_TXEN_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_TXEN_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_TXEN_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task TXEN will subscribe to */
#define RADIO_SUBSCRIBE_TXEN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_TXEN_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_TXEN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_RXEN */
/* Description: Subscribe configuration for task RXEN */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_RXEN_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_RXEN_EN_Msk (0x1UL << RADIO_SUBSCRIBE_RXEN_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_RXEN_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_RXEN_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task RXEN will subscribe to */
#define RADIO_SUBSCRIBE_RXEN_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_RXEN_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_RXEN_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_START_EN_Msk (0x1UL << RADIO_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define RADIO_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_STOP_EN_Msk (0x1UL << RADIO_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define RADIO_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_DISABLE */
/* Description: Subscribe configuration for task DISABLE */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_DISABLE_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_DISABLE_EN_Msk (0x1UL << RADIO_SUBSCRIBE_DISABLE_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_DISABLE_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_DISABLE_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task DISABLE will subscribe to */
#define RADIO_SUBSCRIBE_DISABLE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_DISABLE_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_DISABLE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_RSSISTART */
/* Description: Subscribe configuration for task RSSISTART */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_RSSISTART_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_RSSISTART_EN_Msk (0x1UL << RADIO_SUBSCRIBE_RSSISTART_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_RSSISTART_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_RSSISTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task RSSISTART will subscribe to */
#define RADIO_SUBSCRIBE_RSSISTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_RSSISTART_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_RSSISTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_RSSISTOP */
/* Description: Subscribe configuration for task RSSISTOP */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_RSSISTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_RSSISTOP_EN_Msk (0x1UL << RADIO_SUBSCRIBE_RSSISTOP_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_RSSISTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_RSSISTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task RSSISTOP will subscribe to */
#define RADIO_SUBSCRIBE_RSSISTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_RSSISTOP_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_RSSISTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_BCSTART */
/* Description: Subscribe configuration for task BCSTART */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_BCSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_BCSTART_EN_Msk (0x1UL << RADIO_SUBSCRIBE_BCSTART_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_BCSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_BCSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task BCSTART will subscribe to */
#define RADIO_SUBSCRIBE_BCSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_BCSTART_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_BCSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_BCSTOP */
/* Description: Subscribe configuration for task BCSTOP */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_BCSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_BCSTOP_EN_Msk (0x1UL << RADIO_SUBSCRIBE_BCSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_BCSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_BCSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task BCSTOP will subscribe to */
#define RADIO_SUBSCRIBE_BCSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_BCSTOP_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_BCSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_EDSTART */
/* Description: Subscribe configuration for task EDSTART */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_EDSTART_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_EDSTART_EN_Msk (0x1UL << RADIO_SUBSCRIBE_EDSTART_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_EDSTART_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_EDSTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task EDSTART will subscribe to */
#define RADIO_SUBSCRIBE_EDSTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_EDSTART_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_EDSTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_EDSTOP */
/* Description: Subscribe configuration for task EDSTOP */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_EDSTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_EDSTOP_EN_Msk (0x1UL << RADIO_SUBSCRIBE_EDSTOP_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_EDSTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_EDSTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task EDSTOP will subscribe to */
#define RADIO_SUBSCRIBE_EDSTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_EDSTOP_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_EDSTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_CCASTART */
/* Description: Subscribe configuration for task CCASTART */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_CCASTART_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_CCASTART_EN_Msk (0x1UL << RADIO_SUBSCRIBE_CCASTART_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_CCASTART_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_CCASTART_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CCASTART will subscribe to */
#define RADIO_SUBSCRIBE_CCASTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_CCASTART_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_CCASTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SUBSCRIBE_CCASTOP */
/* Description: Subscribe configuration for task CCASTOP */

/* Bit 31 :   */
#define RADIO_SUBSCRIBE_CCASTOP_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_SUBSCRIBE_CCASTOP_EN_Msk (0x1UL << RADIO_SUBSCRIBE_CCASTOP_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_SUBSCRIBE_CCASTOP_EN_Disabled (0UL) /*!< Disable subscription */
#define RADIO_SUBSCRIBE_CCASTOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task CCASTOP will subscribe to */
#define RADIO_SUBSCRIBE_CCASTOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_SUBSCRIBE_CCASTOP_CHIDX_Msk (0xFFUL << RADIO_SUBSCRIBE_CCASTOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_EVENTS_READY */
/* Description: RADIO has ramped up and is ready to be started */

/* Bit 0 : RADIO has ramped up and is ready to be started */
#define RADIO_EVENTS_READY_EVENTS_READY_Pos (0UL) /*!< Position of EVENTS_READY field. */
#define RADIO_EVENTS_READY_EVENTS_READY_Msk (0x1UL << RADIO_EVENTS_READY_EVENTS_READY_Pos) /*!< Bit mask of EVENTS_READY field. */
#define RADIO_EVENTS_READY_EVENTS_READY_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_READY_EVENTS_READY_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_ADDRESS */
/* Description: Address sent or received */

/* Bit 0 : Address sent or received */
#define RADIO_EVENTS_ADDRESS_EVENTS_ADDRESS_Pos (0UL) /*!< Position of EVENTS_ADDRESS field. */
#define RADIO_EVENTS_ADDRESS_EVENTS_ADDRESS_Msk (0x1UL << RADIO_EVENTS_ADDRESS_EVENTS_ADDRESS_Pos) /*!< Bit mask of EVENTS_ADDRESS field. */
#define RADIO_EVENTS_ADDRESS_EVENTS_ADDRESS_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_ADDRESS_EVENTS_ADDRESS_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_PAYLOAD */
/* Description: Packet payload sent or received */

/* Bit 0 : Packet payload sent or received */
#define RADIO_EVENTS_PAYLOAD_EVENTS_PAYLOAD_Pos (0UL) /*!< Position of EVENTS_PAYLOAD field. */
#define RADIO_EVENTS_PAYLOAD_EVENTS_PAYLOAD_Msk (0x1UL << RADIO_EVENTS_PAYLOAD_EVENTS_PAYLOAD_Pos) /*!< Bit mask of EVENTS_PAYLOAD field. */
#define RADIO_EVENTS_PAYLOAD_EVENTS_PAYLOAD_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_PAYLOAD_EVENTS_PAYLOAD_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_END */
/* Description: Packet sent or received */

/* Bit 0 : Packet sent or received */
#define RADIO_EVENTS_END_EVENTS_END_Pos (0UL) /*!< Position of EVENTS_END field. */
#define RADIO_EVENTS_END_EVENTS_END_Msk (0x1UL << RADIO_EVENTS_END_EVENTS_END_Pos) /*!< Bit mask of EVENTS_END field. */
#define RADIO_EVENTS_END_EVENTS_END_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_END_EVENTS_END_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_DISABLED */
/* Description: RADIO has been disabled */

/* Bit 0 : RADIO has been disabled */
#define RADIO_EVENTS_DISABLED_EVENTS_DISABLED_Pos (0UL) /*!< Position of EVENTS_DISABLED field. */
#define RADIO_EVENTS_DISABLED_EVENTS_DISABLED_Msk (0x1UL << RADIO_EVENTS_DISABLED_EVENTS_DISABLED_Pos) /*!< Bit mask of EVENTS_DISABLED field. */
#define RADIO_EVENTS_DISABLED_EVENTS_DISABLED_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_DISABLED_EVENTS_DISABLED_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_DEVMATCH */
/* Description: A device address match occurred on the last received packet */

/* Bit 0 : A device address match occurred on the last received packet */
#define RADIO_EVENTS_DEVMATCH_EVENTS_DEVMATCH_Pos (0UL) /*!< Position of EVENTS_DEVMATCH field. */
#define RADIO_EVENTS_DEVMATCH_EVENTS_DEVMATCH_Msk (0x1UL << RADIO_EVENTS_DEVMATCH_EVENTS_DEVMATCH_Pos) /*!< Bit mask of EVENTS_DEVMATCH field. */
#define RADIO_EVENTS_DEVMATCH_EVENTS_DEVMATCH_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_DEVMATCH_EVENTS_DEVMATCH_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_DEVMISS */
/* Description: No device address match occurred on the last received packet */

/* Bit 0 : No device address match occurred on the last received packet */
#define RADIO_EVENTS_DEVMISS_EVENTS_DEVMISS_Pos (0UL) /*!< Position of EVENTS_DEVMISS field. */
#define RADIO_EVENTS_DEVMISS_EVENTS_DEVMISS_Msk (0x1UL << RADIO_EVENTS_DEVMISS_EVENTS_DEVMISS_Pos) /*!< Bit mask of EVENTS_DEVMISS field. */
#define RADIO_EVENTS_DEVMISS_EVENTS_DEVMISS_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_DEVMISS_EVENTS_DEVMISS_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_RSSIEND */
/* Description: Sampling of receive signal strength complete */

/* Bit 0 : Sampling of receive signal strength complete */
#define RADIO_EVENTS_RSSIEND_EVENTS_RSSIEND_Pos (0UL) /*!< Position of EVENTS_RSSIEND field. */
#define RADIO_EVENTS_RSSIEND_EVENTS_RSSIEND_Msk (0x1UL << RADIO_EVENTS_RSSIEND_EVENTS_RSSIEND_Pos) /*!< Bit mask of EVENTS_RSSIEND field. */
#define RADIO_EVENTS_RSSIEND_EVENTS_RSSIEND_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_RSSIEND_EVENTS_RSSIEND_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_BCMATCH */
/* Description: Bit counter reached bit count value */

/* Bit 0 : Bit counter reached bit count value */
#define RADIO_EVENTS_BCMATCH_EVENTS_BCMATCH_Pos (0UL) /*!< Position of EVENTS_BCMATCH field. */
#define RADIO_EVENTS_BCMATCH_EVENTS_BCMATCH_Msk (0x1UL << RADIO_EVENTS_BCMATCH_EVENTS_BCMATCH_Pos) /*!< Bit mask of EVENTS_BCMATCH field. */
#define RADIO_EVENTS_BCMATCH_EVENTS_BCMATCH_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_BCMATCH_EVENTS_BCMATCH_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CRCOK */
/* Description: Packet received with CRC ok */

/* Bit 0 : Packet received with CRC ok */
#define RADIO_EVENTS_CRCOK_EVENTS_CRCOK_Pos (0UL) /*!< Position of EVENTS_CRCOK field. */
#define RADIO_EVENTS_CRCOK_EVENTS_CRCOK_Msk (0x1UL << RADIO_EVENTS_CRCOK_EVENTS_CRCOK_Pos) /*!< Bit mask of EVENTS_CRCOK field. */
#define RADIO_EVENTS_CRCOK_EVENTS_CRCOK_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CRCOK_EVENTS_CRCOK_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CRCERROR */
/* Description: Packet received with CRC error */

/* Bit 0 : Packet received with CRC error */
#define RADIO_EVENTS_CRCERROR_EVENTS_CRCERROR_Pos (0UL) /*!< Position of EVENTS_CRCERROR field. */
#define RADIO_EVENTS_CRCERROR_EVENTS_CRCERROR_Msk (0x1UL << RADIO_EVENTS_CRCERROR_EVENTS_CRCERROR_Pos) /*!< Bit mask of EVENTS_CRCERROR field. */
#define RADIO_EVENTS_CRCERROR_EVENTS_CRCERROR_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CRCERROR_EVENTS_CRCERROR_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_FRAMESTART */
/* Description: IEEE 802.15.4 length field received */

/* Bit 0 : IEEE 802.15.4 length field received */
#define RADIO_EVENTS_FRAMESTART_EVENTS_FRAMESTART_Pos (0UL) /*!< Position of EVENTS_FRAMESTART field. */
#define RADIO_EVENTS_FRAMESTART_EVENTS_FRAMESTART_Msk (0x1UL << RADIO_EVENTS_FRAMESTART_EVENTS_FRAMESTART_Pos) /*!< Bit mask of EVENTS_FRAMESTART field. */
#define RADIO_EVENTS_FRAMESTART_EVENTS_FRAMESTART_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_FRAMESTART_EVENTS_FRAMESTART_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_EDEND */
/* Description: Sampling of energy detection complete. A new ED sample is ready for readout from the RADIO.EDSAMPLE register. */

/* Bit 0 : Sampling of energy detection complete. A new ED sample is ready for readout from the RADIO.EDSAMPLE register. */
#define RADIO_EVENTS_EDEND_EVENTS_EDEND_Pos (0UL) /*!< Position of EVENTS_EDEND field. */
#define RADIO_EVENTS_EDEND_EVENTS_EDEND_Msk (0x1UL << RADIO_EVENTS_EDEND_EVENTS_EDEND_Pos) /*!< Bit mask of EVENTS_EDEND field. */
#define RADIO_EVENTS_EDEND_EVENTS_EDEND_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_EDEND_EVENTS_EDEND_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_EDSTOPPED */
/* Description: The sampling of energy detection has stopped */

/* Bit 0 : The sampling of energy detection has stopped */
#define RADIO_EVENTS_EDSTOPPED_EVENTS_EDSTOPPED_Pos (0UL) /*!< Position of EVENTS_EDSTOPPED field. */
#define RADIO_EVENTS_EDSTOPPED_EVENTS_EDSTOPPED_Msk (0x1UL << RADIO_EVENTS_EDSTOPPED_EVENTS_EDSTOPPED_Pos) /*!< Bit mask of EVENTS_EDSTOPPED field. */
#define RADIO_EVENTS_EDSTOPPED_EVENTS_EDSTOPPED_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_EDSTOPPED_EVENTS_EDSTOPPED_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CCAIDLE */
/* Description: Wireless medium in idle - clear to send */

/* Bit 0 : Wireless medium in idle - clear to send */
#define RADIO_EVENTS_CCAIDLE_EVENTS_CCAIDLE_Pos (0UL) /*!< Position of EVENTS_CCAIDLE field. */
#define RADIO_EVENTS_CCAIDLE_EVENTS_CCAIDLE_Msk (0x1UL << RADIO_EVENTS_CCAIDLE_EVENTS_CCAIDLE_Pos) /*!< Bit mask of EVENTS_CCAIDLE field. */
#define RADIO_EVENTS_CCAIDLE_EVENTS_CCAIDLE_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CCAIDLE_EVENTS_CCAIDLE_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CCABUSY */
/* Description: Wireless medium busy - do not send */

/* Bit 0 : Wireless medium busy - do not send */
#define RADIO_EVENTS_CCABUSY_EVENTS_CCABUSY_Pos (0UL) /*!< Position of EVENTS_CCABUSY field. */
#define RADIO_EVENTS_CCABUSY_EVENTS_CCABUSY_Msk (0x1UL << RADIO_EVENTS_CCABUSY_EVENTS_CCABUSY_Pos) /*!< Bit mask of EVENTS_CCABUSY field. */
#define RADIO_EVENTS_CCABUSY_EVENTS_CCABUSY_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CCABUSY_EVENTS_CCABUSY_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CCASTOPPED */
/* Description: The CCA has stopped */

/* Bit 0 : The CCA has stopped */
#define RADIO_EVENTS_CCASTOPPED_EVENTS_CCASTOPPED_Pos (0UL) /*!< Position of EVENTS_CCASTOPPED field. */
#define RADIO_EVENTS_CCASTOPPED_EVENTS_CCASTOPPED_Msk (0x1UL << RADIO_EVENTS_CCASTOPPED_EVENTS_CCASTOPPED_Pos) /*!< Bit mask of EVENTS_CCASTOPPED field. */
#define RADIO_EVENTS_CCASTOPPED_EVENTS_CCASTOPPED_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CCASTOPPED_EVENTS_CCASTOPPED_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_RATEBOOST */
/* Description: Ble_LR CI field received, receive mode is changed from Ble_LR125Kbit to Ble_LR500Kbit. */

/* Bit 0 : Ble_LR CI field received, receive mode is changed from Ble_LR125Kbit to Ble_LR500Kbit. */
#define RADIO_EVENTS_RATEBOOST_EVENTS_RATEBOOST_Pos (0UL) /*!< Position of EVENTS_RATEBOOST field. */
#define RADIO_EVENTS_RATEBOOST_EVENTS_RATEBOOST_Msk (0x1UL << RADIO_EVENTS_RATEBOOST_EVENTS_RATEBOOST_Pos) /*!< Bit mask of EVENTS_RATEBOOST field. */
#define RADIO_EVENTS_RATEBOOST_EVENTS_RATEBOOST_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_RATEBOOST_EVENTS_RATEBOOST_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_TXREADY */
/* Description: RADIO has ramped up and is ready to be started TX path */

/* Bit 0 : RADIO has ramped up and is ready to be started TX path */
#define RADIO_EVENTS_TXREADY_EVENTS_TXREADY_Pos (0UL) /*!< Position of EVENTS_TXREADY field. */
#define RADIO_EVENTS_TXREADY_EVENTS_TXREADY_Msk (0x1UL << RADIO_EVENTS_TXREADY_EVENTS_TXREADY_Pos) /*!< Bit mask of EVENTS_TXREADY field. */
#define RADIO_EVENTS_TXREADY_EVENTS_TXREADY_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_TXREADY_EVENTS_TXREADY_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_RXREADY */
/* Description: RADIO has ramped up and is ready to be started RX path */

/* Bit 0 : RADIO has ramped up and is ready to be started RX path */
#define RADIO_EVENTS_RXREADY_EVENTS_RXREADY_Pos (0UL) /*!< Position of EVENTS_RXREADY field. */
#define RADIO_EVENTS_RXREADY_EVENTS_RXREADY_Msk (0x1UL << RADIO_EVENTS_RXREADY_EVENTS_RXREADY_Pos) /*!< Bit mask of EVENTS_RXREADY field. */
#define RADIO_EVENTS_RXREADY_EVENTS_RXREADY_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_RXREADY_EVENTS_RXREADY_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_MHRMATCH */
/* Description: MAC header match found */

/* Bit 0 : MAC header match found */
#define RADIO_EVENTS_MHRMATCH_EVENTS_MHRMATCH_Pos (0UL) /*!< Position of EVENTS_MHRMATCH field. */
#define RADIO_EVENTS_MHRMATCH_EVENTS_MHRMATCH_Msk (0x1UL << RADIO_EVENTS_MHRMATCH_EVENTS_MHRMATCH_Pos) /*!< Bit mask of EVENTS_MHRMATCH field. */
#define RADIO_EVENTS_MHRMATCH_EVENTS_MHRMATCH_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_MHRMATCH_EVENTS_MHRMATCH_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_SYNC */
/* Description: Preamble indicator */

/* Bit 0 : Preamble indicator */
#define RADIO_EVENTS_SYNC_EVENTS_SYNC_Pos (0UL) /*!< Position of EVENTS_SYNC field. */
#define RADIO_EVENTS_SYNC_EVENTS_SYNC_Msk (0x1UL << RADIO_EVENTS_SYNC_EVENTS_SYNC_Pos) /*!< Bit mask of EVENTS_SYNC field. */
#define RADIO_EVENTS_SYNC_EVENTS_SYNC_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_SYNC_EVENTS_SYNC_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_PHYEND */
/* Description: Generated when last bit is sent on air, or received from air */

/* Bit 0 : Generated when last bit is sent on air, or received from air */
#define RADIO_EVENTS_PHYEND_EVENTS_PHYEND_Pos (0UL) /*!< Position of EVENTS_PHYEND field. */
#define RADIO_EVENTS_PHYEND_EVENTS_PHYEND_Msk (0x1UL << RADIO_EVENTS_PHYEND_EVENTS_PHYEND_Pos) /*!< Bit mask of EVENTS_PHYEND field. */
#define RADIO_EVENTS_PHYEND_EVENTS_PHYEND_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_PHYEND_EVENTS_PHYEND_Generated (1UL) /*!< Event generated */

/* Register: RADIO_EVENTS_CTEPRESENT */
/* Description: CTE is present (early warning right after receiving CTEInfo byte) */

/* Bit 0 : CTE is present (early warning right after receiving CTEInfo byte) */
#define RADIO_EVENTS_CTEPRESENT_EVENTS_CTEPRESENT_Pos (0UL) /*!< Position of EVENTS_CTEPRESENT field. */
#define RADIO_EVENTS_CTEPRESENT_EVENTS_CTEPRESENT_Msk (0x1UL << RADIO_EVENTS_CTEPRESENT_EVENTS_CTEPRESENT_Pos) /*!< Bit mask of EVENTS_CTEPRESENT field. */
#define RADIO_EVENTS_CTEPRESENT_EVENTS_CTEPRESENT_NotGenerated (0UL) /*!< Event not generated */
#define RADIO_EVENTS_CTEPRESENT_EVENTS_CTEPRESENT_Generated (1UL) /*!< Event generated */

/* Register: RADIO_PUBLISH_READY */
/* Description: Publish configuration for event READY */

/* Bit 31 :   */
#define RADIO_PUBLISH_READY_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_READY_EN_Msk (0x1UL << RADIO_PUBLISH_READY_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_READY_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_READY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event READY will publish to. */
#define RADIO_PUBLISH_READY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_READY_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_READY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_ADDRESS */
/* Description: Publish configuration for event ADDRESS */

/* Bit 31 :   */
#define RADIO_PUBLISH_ADDRESS_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_ADDRESS_EN_Msk (0x1UL << RADIO_PUBLISH_ADDRESS_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_ADDRESS_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_ADDRESS_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event ADDRESS will publish to. */
#define RADIO_PUBLISH_ADDRESS_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_ADDRESS_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_ADDRESS_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_PAYLOAD */
/* Description: Publish configuration for event PAYLOAD */

/* Bit 31 :   */
#define RADIO_PUBLISH_PAYLOAD_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_PAYLOAD_EN_Msk (0x1UL << RADIO_PUBLISH_PAYLOAD_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_PAYLOAD_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_PAYLOAD_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event PAYLOAD will publish to. */
#define RADIO_PUBLISH_PAYLOAD_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_PAYLOAD_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_PAYLOAD_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_END */
/* Description: Publish configuration for event END */

/* Bit 31 :   */
#define RADIO_PUBLISH_END_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_END_EN_Msk (0x1UL << RADIO_PUBLISH_END_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_END_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_END_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event END will publish to. */
#define RADIO_PUBLISH_END_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_END_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_END_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_DISABLED */
/* Description: Publish configuration for event DISABLED */

/* Bit 31 :   */
#define RADIO_PUBLISH_DISABLED_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_DISABLED_EN_Msk (0x1UL << RADIO_PUBLISH_DISABLED_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_DISABLED_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_DISABLED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event DISABLED will publish to. */
#define RADIO_PUBLISH_DISABLED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_DISABLED_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_DISABLED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_DEVMATCH */
/* Description: Publish configuration for event DEVMATCH */

/* Bit 31 :   */
#define RADIO_PUBLISH_DEVMATCH_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_DEVMATCH_EN_Msk (0x1UL << RADIO_PUBLISH_DEVMATCH_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_DEVMATCH_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_DEVMATCH_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event DEVMATCH will publish to. */
#define RADIO_PUBLISH_DEVMATCH_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_DEVMATCH_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_DEVMATCH_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_DEVMISS */
/* Description: Publish configuration for event DEVMISS */

/* Bit 31 :   */
#define RADIO_PUBLISH_DEVMISS_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_DEVMISS_EN_Msk (0x1UL << RADIO_PUBLISH_DEVMISS_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_DEVMISS_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_DEVMISS_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event DEVMISS will publish to. */
#define RADIO_PUBLISH_DEVMISS_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_DEVMISS_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_DEVMISS_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_RSSIEND */
/* Description: Publish configuration for event RSSIEND */

/* Bit 31 :   */
#define RADIO_PUBLISH_RSSIEND_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_RSSIEND_EN_Msk (0x1UL << RADIO_PUBLISH_RSSIEND_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_RSSIEND_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_RSSIEND_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event RSSIEND will publish to. */
#define RADIO_PUBLISH_RSSIEND_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_RSSIEND_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_RSSIEND_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_BCMATCH */
/* Description: Publish configuration for event BCMATCH */

/* Bit 31 :   */
#define RADIO_PUBLISH_BCMATCH_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_BCMATCH_EN_Msk (0x1UL << RADIO_PUBLISH_BCMATCH_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_BCMATCH_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_BCMATCH_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event BCMATCH will publish to. */
#define RADIO_PUBLISH_BCMATCH_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_BCMATCH_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_BCMATCH_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CRCOK */
/* Description: Publish configuration for event CRCOK */

/* Bit 31 :   */
#define RADIO_PUBLISH_CRCOK_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CRCOK_EN_Msk (0x1UL << RADIO_PUBLISH_CRCOK_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CRCOK_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CRCOK_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CRCOK will publish to. */
#define RADIO_PUBLISH_CRCOK_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CRCOK_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CRCOK_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CRCERROR */
/* Description: Publish configuration for event CRCERROR */

/* Bit 31 :   */
#define RADIO_PUBLISH_CRCERROR_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CRCERROR_EN_Msk (0x1UL << RADIO_PUBLISH_CRCERROR_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CRCERROR_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CRCERROR_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CRCERROR will publish to. */
#define RADIO_PUBLISH_CRCERROR_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CRCERROR_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CRCERROR_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_FRAMESTART */
/* Description: Publish configuration for event FRAMESTART */

/* Bit 31 :   */
#define RADIO_PUBLISH_FRAMESTART_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_FRAMESTART_EN_Msk (0x1UL << RADIO_PUBLISH_FRAMESTART_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_FRAMESTART_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_FRAMESTART_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event FRAMESTART will publish to. */
#define RADIO_PUBLISH_FRAMESTART_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_FRAMESTART_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_FRAMESTART_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_EDEND */
/* Description: Publish configuration for event EDEND */

/* Bit 31 :   */
#define RADIO_PUBLISH_EDEND_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_EDEND_EN_Msk (0x1UL << RADIO_PUBLISH_EDEND_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_EDEND_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_EDEND_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event EDEND will publish to. */
#define RADIO_PUBLISH_EDEND_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_EDEND_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_EDEND_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_EDSTOPPED */
/* Description: Publish configuration for event EDSTOPPED */

/* Bit 31 :   */
#define RADIO_PUBLISH_EDSTOPPED_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_EDSTOPPED_EN_Msk (0x1UL << RADIO_PUBLISH_EDSTOPPED_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_EDSTOPPED_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_EDSTOPPED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event EDSTOPPED will publish to. */
#define RADIO_PUBLISH_EDSTOPPED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_EDSTOPPED_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_EDSTOPPED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CCAIDLE */
/* Description: Publish configuration for event CCAIDLE */

/* Bit 31 :   */
#define RADIO_PUBLISH_CCAIDLE_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CCAIDLE_EN_Msk (0x1UL << RADIO_PUBLISH_CCAIDLE_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CCAIDLE_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CCAIDLE_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CCAIDLE will publish to. */
#define RADIO_PUBLISH_CCAIDLE_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CCAIDLE_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CCAIDLE_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CCABUSY */
/* Description: Publish configuration for event CCABUSY */

/* Bit 31 :   */
#define RADIO_PUBLISH_CCABUSY_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CCABUSY_EN_Msk (0x1UL << RADIO_PUBLISH_CCABUSY_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CCABUSY_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CCABUSY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CCABUSY will publish to. */
#define RADIO_PUBLISH_CCABUSY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CCABUSY_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CCABUSY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CCASTOPPED */
/* Description: Publish configuration for event CCASTOPPED */

/* Bit 31 :   */
#define RADIO_PUBLISH_CCASTOPPED_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CCASTOPPED_EN_Msk (0x1UL << RADIO_PUBLISH_CCASTOPPED_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CCASTOPPED_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CCASTOPPED_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CCASTOPPED will publish to. */
#define RADIO_PUBLISH_CCASTOPPED_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CCASTOPPED_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CCASTOPPED_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_RATEBOOST */
/* Description: Publish configuration for event RATEBOOST */

/* Bit 31 :   */
#define RADIO_PUBLISH_RATEBOOST_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_RATEBOOST_EN_Msk (0x1UL << RADIO_PUBLISH_RATEBOOST_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_RATEBOOST_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_RATEBOOST_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event RATEBOOST will publish to. */
#define RADIO_PUBLISH_RATEBOOST_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_RATEBOOST_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_RATEBOOST_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_TXREADY */
/* Description: Publish configuration for event TXREADY */

/* Bit 31 :   */
#define RADIO_PUBLISH_TXREADY_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_TXREADY_EN_Msk (0x1UL << RADIO_PUBLISH_TXREADY_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_TXREADY_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_TXREADY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event TXREADY will publish to. */
#define RADIO_PUBLISH_TXREADY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_TXREADY_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_TXREADY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_RXREADY */
/* Description: Publish configuration for event RXREADY */

/* Bit 31 :   */
#define RADIO_PUBLISH_RXREADY_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_RXREADY_EN_Msk (0x1UL << RADIO_PUBLISH_RXREADY_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_RXREADY_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_RXREADY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event RXREADY will publish to. */
#define RADIO_PUBLISH_RXREADY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_RXREADY_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_RXREADY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_MHRMATCH */
/* Description: Publish configuration for event MHRMATCH */

/* Bit 31 :   */
#define RADIO_PUBLISH_MHRMATCH_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_MHRMATCH_EN_Msk (0x1UL << RADIO_PUBLISH_MHRMATCH_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_MHRMATCH_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_MHRMATCH_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event MHRMATCH will publish to. */
#define RADIO_PUBLISH_MHRMATCH_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_MHRMATCH_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_MHRMATCH_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_SYNC */
/* Description: Publish configuration for event SYNC */

/* Bit 31 :   */
#define RADIO_PUBLISH_SYNC_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_SYNC_EN_Msk (0x1UL << RADIO_PUBLISH_SYNC_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_SYNC_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_SYNC_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event SYNC will publish to. */
#define RADIO_PUBLISH_SYNC_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_SYNC_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_SYNC_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_PHYEND */
/* Description: Publish configuration for event PHYEND */

/* Bit 31 :   */
#define RADIO_PUBLISH_PHYEND_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_PHYEND_EN_Msk (0x1UL << RADIO_PUBLISH_PHYEND_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_PHYEND_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_PHYEND_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event PHYEND will publish to. */
#define RADIO_PUBLISH_PHYEND_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_PHYEND_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_PHYEND_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_PUBLISH_CTEPRESENT */
/* Description: Publish configuration for event CTEPRESENT */

/* Bit 31 :   */
#define RADIO_PUBLISH_CTEPRESENT_EN_Pos (31UL) /*!< Position of EN field. */
#define RADIO_PUBLISH_CTEPRESENT_EN_Msk (0x1UL << RADIO_PUBLISH_CTEPRESENT_EN_Pos) /*!< Bit mask of EN field. */
#define RADIO_PUBLISH_CTEPRESENT_EN_Disabled (0UL) /*!< Disable publishing */
#define RADIO_PUBLISH_CTEPRESENT_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event CTEPRESENT will publish to. */
#define RADIO_PUBLISH_CTEPRESENT_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define RADIO_PUBLISH_CTEPRESENT_CHIDX_Msk (0xFFUL << RADIO_PUBLISH_CTEPRESENT_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: RADIO_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 21 : Shortcut between event PHYEND and task START */
#define RADIO_SHORTS_PHYEND_START_Pos (21UL) /*!< Position of PHYEND_START field. */
#define RADIO_SHORTS_PHYEND_START_Msk (0x1UL << RADIO_SHORTS_PHYEND_START_Pos) /*!< Bit mask of PHYEND_START field. */
#define RADIO_SHORTS_PHYEND_START_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_PHYEND_START_Enabled (1UL) /*!< Enable shortcut */

/* Bit 20 : Shortcut between event PHYEND and task DISABLE */
#define RADIO_SHORTS_PHYEND_DISABLE_Pos (20UL) /*!< Position of PHYEND_DISABLE field. */
#define RADIO_SHORTS_PHYEND_DISABLE_Msk (0x1UL << RADIO_SHORTS_PHYEND_DISABLE_Pos) /*!< Bit mask of PHYEND_DISABLE field. */
#define RADIO_SHORTS_PHYEND_DISABLE_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_PHYEND_DISABLE_Enabled (1UL) /*!< Enable shortcut */

/* Bit 19 : Shortcut between event RXREADY and task START */
#define RADIO_SHORTS_RXREADY_START_Pos (19UL) /*!< Position of RXREADY_START field. */
#define RADIO_SHORTS_RXREADY_START_Msk (0x1UL << RADIO_SHORTS_RXREADY_START_Pos) /*!< Bit mask of RXREADY_START field. */
#define RADIO_SHORTS_RXREADY_START_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_RXREADY_START_Enabled (1UL) /*!< Enable shortcut */

/* Bit 18 : Shortcut between event TXREADY and task START */
#define RADIO_SHORTS_TXREADY_START_Pos (18UL) /*!< Position of TXREADY_START field. */
#define RADIO_SHORTS_TXREADY_START_Msk (0x1UL << RADIO_SHORTS_TXREADY_START_Pos) /*!< Bit mask of TXREADY_START field. */
#define RADIO_SHORTS_TXREADY_START_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_TXREADY_START_Enabled (1UL) /*!< Enable shortcut */

/* Bit 17 : Shortcut between event CCAIDLE and task STOP */
#define RADIO_SHORTS_CCAIDLE_STOP_Pos (17UL) /*!< Position of CCAIDLE_STOP field. */
#define RADIO_SHORTS_CCAIDLE_STOP_Msk (0x1UL << RADIO_SHORTS_CCAIDLE_STOP_Pos) /*!< Bit mask of CCAIDLE_STOP field. */
#define RADIO_SHORTS_CCAIDLE_STOP_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_CCAIDLE_STOP_Enabled (1UL) /*!< Enable shortcut */

/* Bit 16 : Shortcut between event EDEND and task DISABLE */
#define RADIO_SHORTS_EDEND_DISABLE_Pos (16UL) /*!< Position of EDEND_DISABLE field. */
#define RADIO_SHORTS_EDEND_DISABLE_Msk (0x1UL << RADIO_SHORTS_EDEND_DISABLE_Pos) /*!< Bit mask of EDEND_DISABLE field. */
#define RADIO_SHORTS_EDEND_DISABLE_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_EDEND_DISABLE_Enabled (1UL) /*!< Enable shortcut */

/* Bit 15 : Shortcut between event READY and task EDSTART */
#define RADIO_SHORTS_READY_EDSTART_Pos (15UL) /*!< Position of READY_EDSTART field. */
#define RADIO_SHORTS_READY_EDSTART_Msk (0x1UL << RADIO_SHORTS_READY_EDSTART_Pos) /*!< Bit mask of READY_EDSTART field. */
#define RADIO_SHORTS_READY_EDSTART_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_READY_EDSTART_Enabled (1UL) /*!< Enable shortcut */

/* Bit 14 : Shortcut between event FRAMESTART and task BCSTART */
#define RADIO_SHORTS_FRAMESTART_BCSTART_Pos (14UL) /*!< Position of FRAMESTART_BCSTART field. */
#define RADIO_SHORTS_FRAMESTART_BCSTART_Msk (0x1UL << RADIO_SHORTS_FRAMESTART_BCSTART_Pos) /*!< Bit mask of FRAMESTART_BCSTART field. */
#define RADIO_SHORTS_FRAMESTART_BCSTART_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_FRAMESTART_BCSTART_Enabled (1UL) /*!< Enable shortcut */

/* Bit 13 : Shortcut between event CCABUSY and task DISABLE */
#define RADIO_SHORTS_CCABUSY_DISABLE_Pos (13UL) /*!< Position of CCABUSY_DISABLE field. */
#define RADIO_SHORTS_CCABUSY_DISABLE_Msk (0x1UL << RADIO_SHORTS_CCABUSY_DISABLE_Pos) /*!< Bit mask of CCABUSY_DISABLE field. */
#define RADIO_SHORTS_CCABUSY_DISABLE_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_CCABUSY_DISABLE_Enabled (1UL) /*!< Enable shortcut */

/* Bit 12 : Shortcut between event CCAIDLE and task TXEN */
#define RADIO_SHORTS_CCAIDLE_TXEN_Pos (12UL) /*!< Position of CCAIDLE_TXEN field. */
#define RADIO_SHORTS_CCAIDLE_TXEN_Msk (0x1UL << RADIO_SHORTS_CCAIDLE_TXEN_Pos) /*!< Bit mask of CCAIDLE_TXEN field. */
#define RADIO_SHORTS_CCAIDLE_TXEN_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_CCAIDLE_TXEN_Enabled (1UL) /*!< Enable shortcut */

/* Bit 11 : Shortcut between event RXREADY and task CCASTART */
#define RADIO_SHORTS_RXREADY_CCASTART_Pos (11UL) /*!< Position of RXREADY_CCASTART field. */
#define RADIO_SHORTS_RXREADY_CCASTART_Msk (0x1UL << RADIO_SHORTS_RXREADY_CCASTART_Pos) /*!< Bit mask of RXREADY_CCASTART field. */
#define RADIO_SHORTS_RXREADY_CCASTART_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_RXREADY_CCASTART_Enabled (1UL) /*!< Enable shortcut */

/* Bit 8 : Shortcut between event DISABLED and task RSSISTOP */
#define RADIO_SHORTS_DISABLED_RSSISTOP_Pos (8UL) /*!< Position of DISABLED_RSSISTOP field. */
#define RADIO_SHORTS_DISABLED_RSSISTOP_Msk (0x1UL << RADIO_SHORTS_DISABLED_RSSISTOP_Pos) /*!< Bit mask of DISABLED_RSSISTOP field. */
#define RADIO_SHORTS_DISABLED_RSSISTOP_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_DISABLED_RSSISTOP_Enabled (1UL) /*!< Enable shortcut */

/* Bit 6 : Shortcut between event ADDRESS and task BCSTART */
#define RADIO_SHORTS_ADDRESS_BCSTART_Pos (6UL) /*!< Position of ADDRESS_BCSTART field. */
#define RADIO_SHORTS_ADDRESS_BCSTART_Msk (0x1UL << RADIO_SHORTS_ADDRESS_BCSTART_Pos) /*!< Bit mask of ADDRESS_BCSTART field. */
#define RADIO_SHORTS_ADDRESS_BCSTART_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_ADDRESS_BCSTART_Enabled (1UL) /*!< Enable shortcut */

/* Bit 5 : Shortcut between event END and task START */
#define RADIO_SHORTS_END_START_Pos (5UL) /*!< Position of END_START field. */
#define RADIO_SHORTS_END_START_Msk (0x1UL << RADIO_SHORTS_END_START_Pos) /*!< Bit mask of END_START field. */
#define RADIO_SHORTS_END_START_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_END_START_Enabled (1UL) /*!< Enable shortcut */

/* Bit 4 : Shortcut between event ADDRESS and task RSSISTART */
#define RADIO_SHORTS_ADDRESS_RSSISTART_Pos (4UL) /*!< Position of ADDRESS_RSSISTART field. */
#define RADIO_SHORTS_ADDRESS_RSSISTART_Msk (0x1UL << RADIO_SHORTS_ADDRESS_RSSISTART_Pos) /*!< Bit mask of ADDRESS_RSSISTART field. */
#define RADIO_SHORTS_ADDRESS_RSSISTART_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_ADDRESS_RSSISTART_Enabled (1UL) /*!< Enable shortcut */

/* Bit 3 : Shortcut between event DISABLED and task RXEN */
#define RADIO_SHORTS_DISABLED_RXEN_Pos (3UL) /*!< Position of DISABLED_RXEN field. */
#define RADIO_SHORTS_DISABLED_RXEN_Msk (0x1UL << RADIO_SHORTS_DISABLED_RXEN_Pos) /*!< Bit mask of DISABLED_RXEN field. */
#define RADIO_SHORTS_DISABLED_RXEN_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_DISABLED_RXEN_Enabled (1UL) /*!< Enable shortcut */

/* Bit 2 : Shortcut between event DISABLED and task TXEN */
#define RADIO_SHORTS_DISABLED_TXEN_Pos (2UL) /*!< Position of DISABLED_TXEN field. */
#define RADIO_SHORTS_DISABLED_TXEN_Msk (0x1UL << RADIO_SHORTS_DISABLED_TXEN_Pos) /*!< Bit mask of DISABLED_TXEN field. */
#define RADIO_SHORTS_DISABLED_TXEN_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_DISABLED_TXEN_Enabled (1UL) /*!< Enable shortcut */

/* Bit 1 : Shortcut between event END and task DISABLE */
#define RADIO_SHORTS_END_DISABLE_Pos (1UL) /*!< Position of END_DISABLE field. */
#define RADIO_SHORTS_END_DISABLE_Msk (0x1UL << RADIO_SHORTS_END_DISABLE_Pos) /*!< Bit mask of END_DISABLE field. */
#define RADIO_SHORTS_END_DISABLE_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_END_DISABLE_Enabled (1UL) /*!< Enable shortcut */

/* Bit 0 : Shortcut between event READY and task START */
#define RADIO_SHORTS_READY_START_Pos (0UL) /*!< Position of READY_START field. */
#define RADIO_SHORTS_READY_START_Msk (0x1UL << RADIO_SHORTS_READY_START_Pos) /*!< Bit mask of READY_START field. */
#define RADIO_SHORTS_READY_START_Disabled (0UL) /*!< Disable shortcut */
#define RADIO_SHORTS_READY_START_Enabled (1UL) /*!< Enable shortcut */

/* Register: RADIO_INTENSET */
/* Description: Enable interrupt */

/* Bit 28 : Write '1' to enable interrupt for event CTEPRESENT */
#define RADIO_INTENSET_CTEPRESENT_Pos (28UL) /*!< Position of CTEPRESENT field. */
#define RADIO_INTENSET_CTEPRESENT_Msk (0x1UL << RADIO_INTENSET_CTEPRESENT_Pos) /*!< Bit mask of CTEPRESENT field. */
#define RADIO_INTENSET_CTEPRESENT_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CTEPRESENT_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CTEPRESENT_Set (1UL) /*!< Enable */

/* Bit 27 : Write '1' to enable interrupt for event PHYEND */
#define RADIO_INTENSET_PHYEND_Pos (27UL) /*!< Position of PHYEND field. */
#define RADIO_INTENSET_PHYEND_Msk (0x1UL << RADIO_INTENSET_PHYEND_Pos) /*!< Bit mask of PHYEND field. */
#define RADIO_INTENSET_PHYEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_PHYEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_PHYEND_Set (1UL) /*!< Enable */

/* Bit 26 : Write '1' to enable interrupt for event SYNC */
#define RADIO_INTENSET_SYNC_Pos (26UL) /*!< Position of SYNC field. */
#define RADIO_INTENSET_SYNC_Msk (0x1UL << RADIO_INTENSET_SYNC_Pos) /*!< Bit mask of SYNC field. */
#define RADIO_INTENSET_SYNC_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_SYNC_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_SYNC_Set (1UL) /*!< Enable */

/* Bit 23 : Write '1' to enable interrupt for event MHRMATCH */
#define RADIO_INTENSET_MHRMATCH_Pos (23UL) /*!< Position of MHRMATCH field. */
#define RADIO_INTENSET_MHRMATCH_Msk (0x1UL << RADIO_INTENSET_MHRMATCH_Pos) /*!< Bit mask of MHRMATCH field. */
#define RADIO_INTENSET_MHRMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_MHRMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_MHRMATCH_Set (1UL) /*!< Enable */

/* Bit 22 : Write '1' to enable interrupt for event RXREADY */
#define RADIO_INTENSET_RXREADY_Pos (22UL) /*!< Position of RXREADY field. */
#define RADIO_INTENSET_RXREADY_Msk (0x1UL << RADIO_INTENSET_RXREADY_Pos) /*!< Bit mask of RXREADY field. */
#define RADIO_INTENSET_RXREADY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_RXREADY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_RXREADY_Set (1UL) /*!< Enable */

/* Bit 21 : Write '1' to enable interrupt for event TXREADY */
#define RADIO_INTENSET_TXREADY_Pos (21UL) /*!< Position of TXREADY field. */
#define RADIO_INTENSET_TXREADY_Msk (0x1UL << RADIO_INTENSET_TXREADY_Pos) /*!< Bit mask of TXREADY field. */
#define RADIO_INTENSET_TXREADY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_TXREADY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_TXREADY_Set (1UL) /*!< Enable */

/* Bit 20 : Write '1' to enable interrupt for event RATEBOOST */
#define RADIO_INTENSET_RATEBOOST_Pos (20UL) /*!< Position of RATEBOOST field. */
#define RADIO_INTENSET_RATEBOOST_Msk (0x1UL << RADIO_INTENSET_RATEBOOST_Pos) /*!< Bit mask of RATEBOOST field. */
#define RADIO_INTENSET_RATEBOOST_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_RATEBOOST_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_RATEBOOST_Set (1UL) /*!< Enable */

/* Bit 19 : Write '1' to enable interrupt for event CCASTOPPED */
#define RADIO_INTENSET_CCASTOPPED_Pos (19UL) /*!< Position of CCASTOPPED field. */
#define RADIO_INTENSET_CCASTOPPED_Msk (0x1UL << RADIO_INTENSET_CCASTOPPED_Pos) /*!< Bit mask of CCASTOPPED field. */
#define RADIO_INTENSET_CCASTOPPED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CCASTOPPED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CCASTOPPED_Set (1UL) /*!< Enable */

/* Bit 18 : Write '1' to enable interrupt for event CCABUSY */
#define RADIO_INTENSET_CCABUSY_Pos (18UL) /*!< Position of CCABUSY field. */
#define RADIO_INTENSET_CCABUSY_Msk (0x1UL << RADIO_INTENSET_CCABUSY_Pos) /*!< Bit mask of CCABUSY field. */
#define RADIO_INTENSET_CCABUSY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CCABUSY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CCABUSY_Set (1UL) /*!< Enable */

/* Bit 17 : Write '1' to enable interrupt for event CCAIDLE */
#define RADIO_INTENSET_CCAIDLE_Pos (17UL) /*!< Position of CCAIDLE field. */
#define RADIO_INTENSET_CCAIDLE_Msk (0x1UL << RADIO_INTENSET_CCAIDLE_Pos) /*!< Bit mask of CCAIDLE field. */
#define RADIO_INTENSET_CCAIDLE_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CCAIDLE_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CCAIDLE_Set (1UL) /*!< Enable */

/* Bit 16 : Write '1' to enable interrupt for event EDSTOPPED */
#define RADIO_INTENSET_EDSTOPPED_Pos (16UL) /*!< Position of EDSTOPPED field. */
#define RADIO_INTENSET_EDSTOPPED_Msk (0x1UL << RADIO_INTENSET_EDSTOPPED_Pos) /*!< Bit mask of EDSTOPPED field. */
#define RADIO_INTENSET_EDSTOPPED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_EDSTOPPED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_EDSTOPPED_Set (1UL) /*!< Enable */

/* Bit 15 : Write '1' to enable interrupt for event EDEND */
#define RADIO_INTENSET_EDEND_Pos (15UL) /*!< Position of EDEND field. */
#define RADIO_INTENSET_EDEND_Msk (0x1UL << RADIO_INTENSET_EDEND_Pos) /*!< Bit mask of EDEND field. */
#define RADIO_INTENSET_EDEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_EDEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_EDEND_Set (1UL) /*!< Enable */

/* Bit 14 : Write '1' to enable interrupt for event FRAMESTART */
#define RADIO_INTENSET_FRAMESTART_Pos (14UL) /*!< Position of FRAMESTART field. */
#define RADIO_INTENSET_FRAMESTART_Msk (0x1UL << RADIO_INTENSET_FRAMESTART_Pos) /*!< Bit mask of FRAMESTART field. */
#define RADIO_INTENSET_FRAMESTART_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_FRAMESTART_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_FRAMESTART_Set (1UL) /*!< Enable */

/* Bit 13 : Write '1' to enable interrupt for event CRCERROR */
#define RADIO_INTENSET_CRCERROR_Pos (13UL) /*!< Position of CRCERROR field. */
#define RADIO_INTENSET_CRCERROR_Msk (0x1UL << RADIO_INTENSET_CRCERROR_Pos) /*!< Bit mask of CRCERROR field. */
#define RADIO_INTENSET_CRCERROR_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CRCERROR_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CRCERROR_Set (1UL) /*!< Enable */

/* Bit 12 : Write '1' to enable interrupt for event CRCOK */
#define RADIO_INTENSET_CRCOK_Pos (12UL) /*!< Position of CRCOK field. */
#define RADIO_INTENSET_CRCOK_Msk (0x1UL << RADIO_INTENSET_CRCOK_Pos) /*!< Bit mask of CRCOK field. */
#define RADIO_INTENSET_CRCOK_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_CRCOK_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_CRCOK_Set (1UL) /*!< Enable */

/* Bit 10 : Write '1' to enable interrupt for event BCMATCH */
#define RADIO_INTENSET_BCMATCH_Pos (10UL) /*!< Position of BCMATCH field. */
#define RADIO_INTENSET_BCMATCH_Msk (0x1UL << RADIO_INTENSET_BCMATCH_Pos) /*!< Bit mask of BCMATCH field. */
#define RADIO_INTENSET_BCMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_BCMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_BCMATCH_Set (1UL) /*!< Enable */

/* Bit 7 : Write '1' to enable interrupt for event RSSIEND */
#define RADIO_INTENSET_RSSIEND_Pos (7UL) /*!< Position of RSSIEND field. */
#define RADIO_INTENSET_RSSIEND_Msk (0x1UL << RADIO_INTENSET_RSSIEND_Pos) /*!< Bit mask of RSSIEND field. */
#define RADIO_INTENSET_RSSIEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_RSSIEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_RSSIEND_Set (1UL) /*!< Enable */

/* Bit 6 : Write '1' to enable interrupt for event DEVMISS */
#define RADIO_INTENSET_DEVMISS_Pos (6UL) /*!< Position of DEVMISS field. */
#define RADIO_INTENSET_DEVMISS_Msk (0x1UL << RADIO_INTENSET_DEVMISS_Pos) /*!< Bit mask of DEVMISS field. */
#define RADIO_INTENSET_DEVMISS_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_DEVMISS_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_DEVMISS_Set (1UL) /*!< Enable */

/* Bit 5 : Write '1' to enable interrupt for event DEVMATCH */
#define RADIO_INTENSET_DEVMATCH_Pos (5UL) /*!< Position of DEVMATCH field. */
#define RADIO_INTENSET_DEVMATCH_Msk (0x1UL << RADIO_INTENSET_DEVMATCH_Pos) /*!< Bit mask of DEVMATCH field. */
#define RADIO_INTENSET_DEVMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_DEVMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_DEVMATCH_Set (1UL) /*!< Enable */

/* Bit 4 : Write '1' to enable interrupt for event DISABLED */
#define RADIO_INTENSET_DISABLED_Pos (4UL) /*!< Position of DISABLED field. */
#define RADIO_INTENSET_DISABLED_Msk (0x1UL << RADIO_INTENSET_DISABLED_Pos) /*!< Bit mask of DISABLED field. */
#define RADIO_INTENSET_DISABLED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_DISABLED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_DISABLED_Set (1UL) /*!< Enable */

/* Bit 3 : Write '1' to enable interrupt for event END */
#define RADIO_INTENSET_END_Pos (3UL) /*!< Position of END field. */
#define RADIO_INTENSET_END_Msk (0x1UL << RADIO_INTENSET_END_Pos) /*!< Bit mask of END field. */
#define RADIO_INTENSET_END_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_END_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_END_Set (1UL) /*!< Enable */

/* Bit 2 : Write '1' to enable interrupt for event PAYLOAD */
#define RADIO_INTENSET_PAYLOAD_Pos (2UL) /*!< Position of PAYLOAD field. */
#define RADIO_INTENSET_PAYLOAD_Msk (0x1UL << RADIO_INTENSET_PAYLOAD_Pos) /*!< Bit mask of PAYLOAD field. */
#define RADIO_INTENSET_PAYLOAD_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_PAYLOAD_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_PAYLOAD_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event ADDRESS */
#define RADIO_INTENSET_ADDRESS_Pos (1UL) /*!< Position of ADDRESS field. */
#define RADIO_INTENSET_ADDRESS_Msk (0x1UL << RADIO_INTENSET_ADDRESS_Pos) /*!< Bit mask of ADDRESS field. */
#define RADIO_INTENSET_ADDRESS_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_ADDRESS_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_ADDRESS_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event READY */
#define RADIO_INTENSET_READY_Pos (0UL) /*!< Position of READY field. */
#define RADIO_INTENSET_READY_Msk (0x1UL << RADIO_INTENSET_READY_Pos) /*!< Bit mask of READY field. */
#define RADIO_INTENSET_READY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENSET_READY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENSET_READY_Set (1UL) /*!< Enable */

/* Register: RADIO_INTENCLR */
/* Description: Disable interrupt */

/* Bit 28 : Write '1' to disable interrupt for event CTEPRESENT */
#define RADIO_INTENCLR_CTEPRESENT_Pos (28UL) /*!< Position of CTEPRESENT field. */
#define RADIO_INTENCLR_CTEPRESENT_Msk (0x1UL << RADIO_INTENCLR_CTEPRESENT_Pos) /*!< Bit mask of CTEPRESENT field. */
#define RADIO_INTENCLR_CTEPRESENT_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CTEPRESENT_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CTEPRESENT_Clear (1UL) /*!< Disable */

/* Bit 27 : Write '1' to disable interrupt for event PHYEND */
#define RADIO_INTENCLR_PHYEND_Pos (27UL) /*!< Position of PHYEND field. */
#define RADIO_INTENCLR_PHYEND_Msk (0x1UL << RADIO_INTENCLR_PHYEND_Pos) /*!< Bit mask of PHYEND field. */
#define RADIO_INTENCLR_PHYEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_PHYEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_PHYEND_Clear (1UL) /*!< Disable */

/* Bit 26 : Write '1' to disable interrupt for event SYNC */
#define RADIO_INTENCLR_SYNC_Pos (26UL) /*!< Position of SYNC field. */
#define RADIO_INTENCLR_SYNC_Msk (0x1UL << RADIO_INTENCLR_SYNC_Pos) /*!< Bit mask of SYNC field. */
#define RADIO_INTENCLR_SYNC_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_SYNC_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_SYNC_Clear (1UL) /*!< Disable */

/* Bit 23 : Write '1' to disable interrupt for event MHRMATCH */
#define RADIO_INTENCLR_MHRMATCH_Pos (23UL) /*!< Position of MHRMATCH field. */
#define RADIO_INTENCLR_MHRMATCH_Msk (0x1UL << RADIO_INTENCLR_MHRMATCH_Pos) /*!< Bit mask of MHRMATCH field. */
#define RADIO_INTENCLR_MHRMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_MHRMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_MHRMATCH_Clear (1UL) /*!< Disable */

/* Bit 22 : Write '1' to disable interrupt for event RXREADY */
#define RADIO_INTENCLR_RXREADY_Pos (22UL) /*!< Position of RXREADY field. */
#define RADIO_INTENCLR_RXREADY_Msk (0x1UL << RADIO_INTENCLR_RXREADY_Pos) /*!< Bit mask of RXREADY field. */
#define RADIO_INTENCLR_RXREADY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_RXREADY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_RXREADY_Clear (1UL) /*!< Disable */

/* Bit 21 : Write '1' to disable interrupt for event TXREADY */
#define RADIO_INTENCLR_TXREADY_Pos (21UL) /*!< Position of TXREADY field. */
#define RADIO_INTENCLR_TXREADY_Msk (0x1UL << RADIO_INTENCLR_TXREADY_Pos) /*!< Bit mask of TXREADY field. */
#define RADIO_INTENCLR_TXREADY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_TXREADY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_TXREADY_Clear (1UL) /*!< Disable */

/* Bit 20 : Write '1' to disable interrupt for event RATEBOOST */
#define RADIO_INTENCLR_RATEBOOST_Pos (20UL) /*!< Position of RATEBOOST field. */
#define RADIO_INTENCLR_RATEBOOST_Msk (0x1UL << RADIO_INTENCLR_RATEBOOST_Pos) /*!< Bit mask of RATEBOOST field. */
#define RADIO_INTENCLR_RATEBOOST_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_RATEBOOST_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_RATEBOOST_Clear (1UL) /*!< Disable */

/* Bit 19 : Write '1' to disable interrupt for event CCASTOPPED */
#define RADIO_INTENCLR_CCASTOPPED_Pos (19UL) /*!< Position of CCASTOPPED field. */
#define RADIO_INTENCLR_CCASTOPPED_Msk (0x1UL << RADIO_INTENCLR_CCASTOPPED_Pos) /*!< Bit mask of CCASTOPPED field. */
#define RADIO_INTENCLR_CCASTOPPED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CCASTOPPED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CCASTOPPED_Clear (1UL) /*!< Disable */

/* Bit 18 : Write '1' to disable interrupt for event CCABUSY */
#define RADIO_INTENCLR_CCABUSY_Pos (18UL) /*!< Position of CCABUSY field. */
#define RADIO_INTENCLR_CCABUSY_Msk (0x1UL << RADIO_INTENCLR_CCABUSY_Pos) /*!< Bit mask of CCABUSY field. */
#define RADIO_INTENCLR_CCABUSY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CCABUSY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CCABUSY_Clear (1UL) /*!< Disable */

/* Bit 17 : Write '1' to disable interrupt for event CCAIDLE */
#define RADIO_INTENCLR_CCAIDLE_Pos (17UL) /*!< Position of CCAIDLE field. */
#define RADIO_INTENCLR_CCAIDLE_Msk (0x1UL << RADIO_INTENCLR_CCAIDLE_Pos) /*!< Bit mask of CCAIDLE field. */
#define RADIO_INTENCLR_CCAIDLE_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CCAIDLE_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CCAIDLE_Clear (1UL) /*!< Disable */

/* Bit 16 : Write '1' to disable interrupt for event EDSTOPPED */
#define RADIO_INTENCLR_EDSTOPPED_Pos (16UL) /*!< Position of EDSTOPPED field. */
#define RADIO_INTENCLR_EDSTOPPED_Msk (0x1UL << RADIO_INTENCLR_EDSTOPPED_Pos) /*!< Bit mask of EDSTOPPED field. */
#define RADIO_INTENCLR_EDSTOPPED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_EDSTOPPED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_EDSTOPPED_Clear (1UL) /*!< Disable */

/* Bit 15 : Write '1' to disable interrupt for event EDEND */
#define RADIO_INTENCLR_EDEND_Pos (15UL) /*!< Position of EDEND field. */
#define RADIO_INTENCLR_EDEND_Msk (0x1UL << RADIO_INTENCLR_EDEND_Pos) /*!< Bit mask of EDEND field. */
#define RADIO_INTENCLR_EDEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_EDEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_EDEND_Clear (1UL) /*!< Disable */

/* Bit 14 : Write '1' to disable interrupt for event FRAMESTART */
#define RADIO_INTENCLR_FRAMESTART_Pos (14UL) /*!< Position of FRAMESTART field. */
#define RADIO_INTENCLR_FRAMESTART_Msk (0x1UL << RADIO_INTENCLR_FRAMESTART_Pos) /*!< Bit mask of FRAMESTART field. */
#define RADIO_INTENCLR_FRAMESTART_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_FRAMESTART_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_FRAMESTART_Clear (1UL) /*!< Disable */

/* Bit 13 : Write '1' to disable interrupt for event CRCERROR */
#define RADIO_INTENCLR_CRCERROR_Pos (13UL) /*!< Position of CRCERROR field. */
#define RADIO_INTENCLR_CRCERROR_Msk (0x1UL << RADIO_INTENCLR_CRCERROR_Pos) /*!< Bit mask of CRCERROR field. */
#define RADIO_INTENCLR_CRCERROR_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CRCERROR_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CRCERROR_Clear (1UL) /*!< Disable */

/* Bit 12 : Write '1' to disable interrupt for event CRCOK */
#define RADIO_INTENCLR_CRCOK_Pos (12UL) /*!< Position of CRCOK field. */
#define RADIO_INTENCLR_CRCOK_Msk (0x1UL << RADIO_INTENCLR_CRCOK_Pos) /*!< Bit mask of CRCOK field. */
#define RADIO_INTENCLR_CRCOK_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_CRCOK_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_CRCOK_Clear (1UL) /*!< Disable */

/* Bit 10 : Write '1' to disable interrupt for event BCMATCH */
#define RADIO_INTENCLR_BCMATCH_Pos (10UL) /*!< Position of BCMATCH field. */
#define RADIO_INTENCLR_BCMATCH_Msk (0x1UL << RADIO_INTENCLR_BCMATCH_Pos) /*!< Bit mask of BCMATCH field. */
#define RADIO_INTENCLR_BCMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_BCMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_BCMATCH_Clear (1UL) /*!< Disable */

/* Bit 7 : Write '1' to disable interrupt for event RSSIEND */
#define RADIO_INTENCLR_RSSIEND_Pos (7UL) /*!< Position of RSSIEND field. */
#define RADIO_INTENCLR_RSSIEND_Msk (0x1UL << RADIO_INTENCLR_RSSIEND_Pos) /*!< Bit mask of RSSIEND field. */
#define RADIO_INTENCLR_RSSIEND_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_RSSIEND_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_RSSIEND_Clear (1UL) /*!< Disable */

/* Bit 6 : Write '1' to disable interrupt for event DEVMISS */
#define RADIO_INTENCLR_DEVMISS_Pos (6UL) /*!< Position of DEVMISS field. */
#define RADIO_INTENCLR_DEVMISS_Msk (0x1UL << RADIO_INTENCLR_DEVMISS_Pos) /*!< Bit mask of DEVMISS field. */
#define RADIO_INTENCLR_DEVMISS_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_DEVMISS_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_DEVMISS_Clear (1UL) /*!< Disable */

/* Bit 5 : Write '1' to disable interrupt for event DEVMATCH */
#define RADIO_INTENCLR_DEVMATCH_Pos (5UL) /*!< Position of DEVMATCH field. */
#define RADIO_INTENCLR_DEVMATCH_Msk (0x1UL << RADIO_INTENCLR_DEVMATCH_Pos) /*!< Bit mask of DEVMATCH field. */
#define RADIO_INTENCLR_DEVMATCH_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_DEVMATCH_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_DEVMATCH_Clear (1UL) /*!< Disable */

/* Bit 4 : Write '1' to disable interrupt for event DISABLED */
#define RADIO_INTENCLR_DISABLED_Pos (4UL) /*!< Position of DISABLED field. */
#define RADIO_INTENCLR_DISABLED_Msk (0x1UL << RADIO_INTENCLR_DISABLED_Pos) /*!< Bit mask of DISABLED field. */
#define RADIO_INTENCLR_DISABLED_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_DISABLED_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_DISABLED_Clear (1UL) /*!< Disable */

/* Bit 3 : Write '1' to disable interrupt for event END */
#define RADIO_INTENCLR_END_Pos (3UL) /*!< Position of END field. */
#define RADIO_INTENCLR_END_Msk (0x1UL << RADIO_INTENCLR_END_Pos) /*!< Bit mask of END field. */
#define RADIO_INTENCLR_END_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_END_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_END_Clear (1UL) /*!< Disable */

/* Bit 2 : Write '1' to disable interrupt for event PAYLOAD */
#define RADIO_INTENCLR_PAYLOAD_Pos (2UL) /*!< Position of PAYLOAD field. */
#define RADIO_INTENCLR_PAYLOAD_Msk (0x1UL << RADIO_INTENCLR_PAYLOAD_Pos) /*!< Bit mask of PAYLOAD field. */
#define RADIO_INTENCLR_PAYLOAD_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_PAYLOAD_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_PAYLOAD_Clear (1UL) /*!< Disable */

/* Bit 1 : Write '1' to disable interrupt for event ADDRESS */
#define RADIO_INTENCLR_ADDRESS_Pos (1UL) /*!< Position of ADDRESS field. */
#define RADIO_INTENCLR_ADDRESS_Msk (0x1UL << RADIO_INTENCLR_ADDRESS_Pos) /*!< Bit mask of ADDRESS field. */
#define RADIO_INTENCLR_ADDRESS_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_ADDRESS_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_ADDRESS_Clear (1UL) /*!< Disable */

/* Bit 0 : Write '1' to disable interrupt for event READY */
#define RADIO_INTENCLR_READY_Pos (0UL) /*!< Position of READY field. */
#define RADIO_INTENCLR_READY_Msk (0x1UL << RADIO_INTENCLR_READY_Pos) /*!< Bit mask of READY field. */
#define RADIO_INTENCLR_READY_Disabled (0UL) /*!< Read: Disabled */
#define RADIO_INTENCLR_READY_Enabled (1UL) /*!< Read: Enabled */
#define RADIO_INTENCLR_READY_Clear (1UL) /*!< Disable */

/* Register: RADIO_CRCSTATUS */
/* Description: CRC status */

/* Bit 0 : CRC status of packet received */
#define RADIO_CRCSTATUS_CRCSTATUS_Pos (0UL) /*!< Position of CRCSTATUS field. */
#define RADIO_CRCSTATUS_CRCSTATUS_Msk (0x1UL << RADIO_CRCSTATUS_CRCSTATUS_Pos) /*!< Bit mask of CRCSTATUS field. */
#define RADIO_CRCSTATUS_CRCSTATUS_CRCError (0UL) /*!< Packet received with CRC error */
#define RADIO_CRCSTATUS_CRCSTATUS_CRCOk (1UL) /*!< Packet received with CRC ok */

/* Register: RADIO_RXMATCH */
/* Description: Received address */

/* Bits 2..0 : Received address */
#define RADIO_RXMATCH_RXMATCH_Pos (0UL) /*!< Position of RXMATCH field. */
#define RADIO_RXMATCH_RXMATCH_Msk (0x7UL << RADIO_RXMATCH_RXMATCH_Pos) /*!< Bit mask of RXMATCH field. */

/* Register: RADIO_RXCRC */
/* Description: CRC field of previously received packet */

/* Bits 23..0 : CRC field of previously received packet */
#define RADIO_RXCRC_RXCRC_Pos (0UL) /*!< Position of RXCRC field. */
#define RADIO_RXCRC_RXCRC_Msk (0xFFFFFFUL << RADIO_RXCRC_RXCRC_Pos) /*!< Bit mask of RXCRC field. */

/* Register: RADIO_DAI */
/* Description: Device address match index */

/* Bits 2..0 : Device address match index */
#define RADIO_DAI_DAI_Pos (0UL) /*!< Position of DAI field. */
#define RADIO_DAI_DAI_Msk (0x7UL << RADIO_DAI_DAI_Pos) /*!< Bit mask of DAI field. */

/* Register: RADIO_PDUSTAT */
/* Description: Payload status */

/* Bits 2..1 : Status on what rate packet is received with in Long Range */
#define RADIO_PDUSTAT_CISTAT_Pos (1UL) /*!< Position of CISTAT field. */
#define RADIO_PDUSTAT_CISTAT_Msk (0x3UL << RADIO_PDUSTAT_CISTAT_Pos) /*!< Bit mask of CISTAT field. */
#define RADIO_PDUSTAT_CISTAT_LR125kbit (0UL) /*!< Frame is received at 125 kbps */
#define RADIO_PDUSTAT_CISTAT_LR500kbit (1UL) /*!< Frame is received at 500 kbps */

/* Bit 0 : Status on payload length vs. PCNF1.MAXLEN */
#define RADIO_PDUSTAT_PDUSTAT_Pos (0UL) /*!< Position of PDUSTAT field. */
#define RADIO_PDUSTAT_PDUSTAT_Msk (0x1UL << RADIO_PDUSTAT_PDUSTAT_Pos) /*!< Bit mask of PDUSTAT field. */
#define RADIO_PDUSTAT_PDUSTAT_LessThan (0UL) /*!< Payload less than PCNF1.MAXLEN */
#define RADIO_PDUSTAT_PDUSTAT_GreaterThan (1UL) /*!< Payload greater than PCNF1.MAXLEN */

/* Register: RADIO_CTESTATUS */
/* Description: CTEInfo parsed from received packet */

/* Bits 7..6 : CTEType parsed from packet */
#define RADIO_CTESTATUS_CTETYPE_Pos (6UL) /*!< Position of CTETYPE field. */
#define RADIO_CTESTATUS_CTETYPE_Msk (0x3UL << RADIO_CTESTATUS_CTETYPE_Pos) /*!< Bit mask of CTETYPE field. */

/* Bit 5 : RFU parsed from packet */
#define RADIO_CTESTATUS_RFU_Pos (5UL) /*!< Position of RFU field. */
#define RADIO_CTESTATUS_RFU_Msk (0x1UL << RADIO_CTESTATUS_RFU_Pos) /*!< Bit mask of RFU field. */

/* Bits 4..0 : CTETime parsed from packet */
#define RADIO_CTESTATUS_CTETIME_Pos (0UL) /*!< Position of CTETIME field. */
#define RADIO_CTESTATUS_CTETIME_Msk (0x1FUL << RADIO_CTESTATUS_CTETIME_Pos) /*!< Bit mask of CTETIME field. */

/* Register: RADIO_DFESTATUS */
/* Description: DFE status information */

/* Bit 4 : Internal state of sampling state machine */
#define RADIO_DFESTATUS_SAMPLINGSTATE_Pos (4UL) /*!< Position of SAMPLINGSTATE field. */
#define RADIO_DFESTATUS_SAMPLINGSTATE_Msk (0x1UL << RADIO_DFESTATUS_SAMPLINGSTATE_Pos) /*!< Bit mask of SAMPLINGSTATE field. */
#define RADIO_DFESTATUS_SAMPLINGSTATE_Idle (0UL) /*!< Sampling state Idle */
#define RADIO_DFESTATUS_SAMPLINGSTATE_Sampling (1UL) /*!< Sampling state Sampling */

/* Bits 2..0 : Internal state of switching state machine */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Pos (0UL) /*!< Position of SWITCHINGSTATE field. */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Msk (0x7UL << RADIO_DFESTATUS_SWITCHINGSTATE_Pos) /*!< Bit mask of SWITCHINGSTATE field. */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Idle (0UL) /*!< Switching state Idle */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Offset (1UL) /*!< Switching state Offset */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Guard (2UL) /*!< Switching state Guard */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Ref (3UL) /*!< Switching state Ref */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Switching (4UL) /*!< Switching state Switching */
#define RADIO_DFESTATUS_SWITCHINGSTATE_Ending (5UL) /*!< Switching state Ending */

/* Register: RADIO_PACKETPTR */
/* Description: Packet pointer */

/* Bits 31..0 : Packet pointer */
#define RADIO_PACKETPTR_PACKETPTR_Pos (0UL) /*!< Position of PACKETPTR field. */
#define RADIO_PACKETPTR_PACKETPTR_Msk (0xFFFFFFFFUL << RADIO_PACKETPTR_PACKETPTR_Pos) /*!< Bit mask of PACKETPTR field. */

/* Register: RADIO_FREQUENCY */
/* Description: Frequency */

/* Bit 8 : Channel map selection */
#define RADIO_FREQUENCY_MAP_Pos (8UL) /*!< Position of MAP field. */
#define RADIO_FREQUENCY_MAP_Msk (0x1UL << RADIO_FREQUENCY_MAP_Pos) /*!< Bit mask of MAP field. */
#define RADIO_FREQUENCY_MAP_Default (0UL) /*!< Channel map between 2400 MHz and 2500 MHz */
#define RADIO_FREQUENCY_MAP_Low (1UL) /*!< Channel map between 2360 MHz and 2460 MHz */

/* Bits 6..0 : Radio channel frequency */
#define RADIO_FREQUENCY_FREQUENCY_Pos (0UL) /*!< Position of FREQUENCY field. */
#define RADIO_FREQUENCY_FREQUENCY_Msk (0x7FUL << RADIO_FREQUENCY_FREQUENCY_Pos) /*!< Bit mask of FREQUENCY field. */

/* Register: RADIO_TXPOWER */
/* Description: Output power */

/* Bits 7..0 : RADIO output power */
#define RADIO_TXPOWER_TXPOWER_Pos (0UL) /*!< Position of TXPOWER field. */
#define RADIO_TXPOWER_TXPOWER_Msk (0xFFUL << RADIO_TXPOWER_TXPOWER_Pos) /*!< Bit mask of TXPOWER field. */
#define RADIO_TXPOWER_TXPOWER_0dBm (0x0UL) /*!< 0 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg40dBm (0xD8UL) /*!< -40 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg30dBm (0xE2UL) /*!< Deprecated enumerator -  -40 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg20dBm (0xECUL) /*!< -20 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg16dBm (0xF0UL) /*!< -16 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg12dBm (0xF4UL) /*!< -12 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg8dBm (0xF8UL) /*!< -8 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg7dBm (0xF9UL) /*!< -7 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg6dBm (0xFAUL) /*!< -6 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg5dBm (0xFBUL) /*!< -5 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg4dBm (0xFCUL) /*!< -4 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg3dBm (0xFDUL) /*!< -3 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg2dBm (0xFEUL) /*!< -2 dBm */
#define RADIO_TXPOWER_TXPOWER_Neg1dBm (0xFFUL) /*!< -1 dBm */

/* Register: RADIO_MODE */
/* Description: Data rate and modulation */

/* Bits 3..0 : Radio data rate and modulation setting. The radio supports frequency-shift keying (FSK) modulation. */
#define RADIO_MODE_MODE_Pos (0UL) /*!< Position of MODE field. */
#define RADIO_MODE_MODE_Msk (0xFUL << RADIO_MODE_MODE_Pos) /*!< Bit mask of MODE field. */
#define RADIO_MODE_MODE_Nrf_1Mbit (0UL) /*!< 1 Mbps Nordic proprietary radio mode */
#define RADIO_MODE_MODE_Nrf_2Mbit (1UL) /*!< 2 Mbps Nordic proprietary radio mode */
#define RADIO_MODE_MODE_Ble_1Mbit (3UL) /*!< 1 Mbps BLE */
#define RADIO_MODE_MODE_Ble_2Mbit (4UL) /*!< 2 Mbps BLE */
#define RADIO_MODE_MODE_Ble_LR125Kbit (5UL) /*!< Long Range 125 kbps TX, 125 kbps and 500 kbps RX */
#define RADIO_MODE_MODE_Ble_LR500Kbit (6UL) /*!< Long Range 500 kbps TX, 125 kbps and 500 kbps RX */
#define RADIO_MODE_MODE_Ieee802154_250Kbit (15UL) /*!< IEEE 802.15.4-2006 250 kbps */

/* Register: RADIO_PCNF0 */
/* Description: Packet configuration register 0 */

/* Bits 30..29 : Length of TERM field in Long Range operation */
#define RADIO_PCNF0_TERMLEN_Pos (29UL) /*!< Position of TERMLEN field. */
#define RADIO_PCNF0_TERMLEN_Msk (0x3UL << RADIO_PCNF0_TERMLEN_Pos) /*!< Bit mask of TERMLEN field. */

/* Bit 26 : Indicates if LENGTH field contains CRC or not */
#define RADIO_PCNF0_CRCINC_Pos (26UL) /*!< Position of CRCINC field. */
#define RADIO_PCNF0_CRCINC_Msk (0x1UL << RADIO_PCNF0_CRCINC_Pos) /*!< Bit mask of CRCINC field. */
#define RADIO_PCNF0_CRCINC_Exclude (0UL) /*!< LENGTH does not contain CRC */
#define RADIO_PCNF0_CRCINC_Include (1UL) /*!< LENGTH includes CRC */

/* Bits 25..24 : Length of preamble on air. Decision point: TASKS_START task */
#define RADIO_PCNF0_PLEN_Pos (24UL) /*!< Position of PLEN field. */
#define RADIO_PCNF0_PLEN_Msk (0x3UL << RADIO_PCNF0_PLEN_Pos) /*!< Bit mask of PLEN field. */
#define RADIO_PCNF0_PLEN_8bit (0UL) /*!< 8-bit preamble */
#define RADIO_PCNF0_PLEN_16bit (1UL) /*!< 16-bit preamble */
#define RADIO_PCNF0_PLEN_32bitZero (2UL) /*!< 32-bit zero preamble - used for IEEE 802.15.4 */
#define RADIO_PCNF0_PLEN_LongRange (3UL) /*!< Preamble - used for Bluetooth LE Long Range */

/* Bits 23..22 : Length of code indicator - Long Range */
#define RADIO_PCNF0_CILEN_Pos (22UL) /*!< Position of CILEN field. */
#define RADIO_PCNF0_CILEN_Msk (0x3UL << RADIO_PCNF0_CILEN_Pos) /*!< Bit mask of CILEN field. */

/* Bit 20 : Include or exclude S1 field in RAM */
#define RADIO_PCNF0_S1INCL_Pos (20UL) /*!< Position of S1INCL field. */
#define RADIO_PCNF0_S1INCL_Msk (0x1UL << RADIO_PCNF0_S1INCL_Pos) /*!< Bit mask of S1INCL field. */
#define RADIO_PCNF0_S1INCL_Automatic (0UL) /*!< Include S1 field in RAM only if S1LEN &gt; 0 */
#define RADIO_PCNF0_S1INCL_Include (1UL) /*!< Always include S1 field in RAM independent of S1LEN */

/* Bits 19..16 : Length on air of S1 field in number of bits */
#define RADIO_PCNF0_S1LEN_Pos (16UL) /*!< Position of S1LEN field. */
#define RADIO_PCNF0_S1LEN_Msk (0xFUL << RADIO_PCNF0_S1LEN_Pos) /*!< Bit mask of S1LEN field. */

/* Bit 8 : Length on air of S0 field in number of bytes */
#define RADIO_PCNF0_S0LEN_Pos (8UL) /*!< Position of S0LEN field. */
#define RADIO_PCNF0_S0LEN_Msk (0x1UL << RADIO_PCNF0_S0LEN_Pos) /*!< Bit mask of S0LEN field. */

/* Bits 3..0 : Length on air of LENGTH field in number of bits */
#define RADIO_PCNF0_LFLEN_Pos (0UL) /*!< Position of LFLEN field. */
#define RADIO_PCNF0_LFLEN_Msk (0xFUL << RADIO_PCNF0_LFLEN_Pos) /*!< Bit mask of LFLEN field. */

/* Register: RADIO_PCNF1 */
/* Description: Packet configuration register 1 */

/* Bit 25 : Enable or disable packet whitening */
#define RADIO_PCNF1_WHITEEN_Pos (25UL) /*!< Position of WHITEEN field. */
#define RADIO_PCNF1_WHITEEN_Msk (0x1UL << RADIO_PCNF1_WHITEEN_Pos) /*!< Bit mask of WHITEEN field. */
#define RADIO_PCNF1_WHITEEN_Disabled (0UL) /*!< Disable */
#define RADIO_PCNF1_WHITEEN_Enabled (1UL) /*!< Enable */

/* Bit 24 : On-air endianness of packet, this applies to the S0, LENGTH, S1, and the PAYLOAD fields. */
#define RADIO_PCNF1_ENDIAN_Pos (24UL) /*!< Position of ENDIAN field. */
#define RADIO_PCNF1_ENDIAN_Msk (0x1UL << RADIO_PCNF1_ENDIAN_Pos) /*!< Bit mask of ENDIAN field. */
#define RADIO_PCNF1_ENDIAN_Little (0UL) /*!< Least significant bit on air first */
#define RADIO_PCNF1_ENDIAN_Big (1UL) /*!< Most significant bit on air first */

/* Bits 18..16 : Base address length in number of bytes */
#define RADIO_PCNF1_BALEN_Pos (16UL) /*!< Position of BALEN field. */
#define RADIO_PCNF1_BALEN_Msk (0x7UL << RADIO_PCNF1_BALEN_Pos) /*!< Bit mask of BALEN field. */

/* Bits 15..8 : Static length in number of bytes */
#define RADIO_PCNF1_STATLEN_Pos (8UL) /*!< Position of STATLEN field. */
#define RADIO_PCNF1_STATLEN_Msk (0xFFUL << RADIO_PCNF1_STATLEN_Pos) /*!< Bit mask of STATLEN field. */

/* Bits 7..0 : Maximum length of packet payload. If the packet payload is larger than MAXLEN, the radio will truncate the payload to MAXLEN. */
#define RADIO_PCNF1_MAXLEN_Pos (0UL) /*!< Position of MAXLEN field. */
#define RADIO_PCNF1_MAXLEN_Msk (0xFFUL << RADIO_PCNF1_MAXLEN_Pos) /*!< Bit mask of MAXLEN field. */

/* Register: RADIO_BASE0 */
/* Description: Base address 0 */

/* Bits 31..0 : Base address 0 */
#define RADIO_BASE0_BASE0_Pos (0UL) /*!< Position of BASE0 field. */
#define RADIO_BASE0_BASE0_Msk (0xFFFFFFFFUL << RADIO_BASE0_BASE0_Pos) /*!< Bit mask of BASE0 field. */

/* Register: RADIO_BASE1 */
/* Description: Base address 1 */

/* Bits 31..0 : Base address 1 */
#define RADIO_BASE1_BASE1_Pos (0UL) /*!< Position of BASE1 field. */
#define RADIO_BASE1_BASE1_Msk (0xFFFFFFFFUL << RADIO_BASE1_BASE1_Pos) /*!< Bit mask of BASE1 field. */

/* Register: RADIO_PREFIX0 */
/* Description: Prefixes bytes for logical addresses 0-3 */

/* Bits 31..24 : Address prefix 3. */
#define RADIO_PREFIX0_AP3_Pos (24UL) /*!< Position of AP3 field. */
#define RADIO_PREFIX0_AP3_Msk (0xFFUL << RADIO_PREFIX0_AP3_Pos) /*!< Bit mask of AP3 field. */

/* Bits 23..16 : Address prefix 2. */
#define RADIO_PREFIX0_AP2_Pos (16UL) /*!< Position of AP2 field. */
#define RADIO_PREFIX0_AP2_Msk (0xFFUL << RADIO_PREFIX0_AP2_Pos) /*!< Bit mask of AP2 field. */

/* Bits 15..8 : Address prefix 1. */
#define RADIO_PREFIX0_AP1_Pos (8UL) /*!< Position of AP1 field. */
#define RADIO_PREFIX0_AP1_Msk (0xFFUL << RADIO_PREFIX0_AP1_Pos) /*!< Bit mask of AP1 field. */

/* Bits 7..0 : Address prefix 0. */
#define RADIO_PREFIX0_AP0_Pos (0UL) /*!< Position of AP0 field. */
#define RADIO_PREFIX0_AP0_Msk (0xFFUL << RADIO_PREFIX0_AP0_Pos) /*!< Bit mask of AP0 field. */

/* Register: RADIO_PREFIX1 */
/* Description: Prefixes bytes for logical addresses 4-7 */

/* Bits 31..24 : Address prefix 7. */
#define RADIO_PREFIX1_AP7_Pos (24UL) /*!< Position of AP7 field. */
#define RADIO_PREFIX1_AP7_Msk (0xFFUL << RADIO_PREFIX1_AP7_Pos) /*!< Bit mask of AP7 field. */

/* Bits 23..16 : Address prefix 6. */
#define RADIO_PREFIX1_AP6_Pos (16UL) /*!< Position of AP6 field. */
#define RADIO_PREFIX1_AP6_Msk (0xFFUL << RADIO_PREFIX1_AP6_Pos) /*!< Bit mask of AP6 field. */

/* Bits 15..8 : Address prefix 5. */
#define RADIO_PREFIX1_AP5_Pos (8UL) /*!< Position of AP5 field. */
#define RADIO_PREFIX1_AP5_Msk (0xFFUL << RADIO_PREFIX1_AP5_Pos) /*!< Bit mask of AP5 field. */

/* Bits 7..0 : Address prefix 4. */
#define RADIO_PREFIX1_AP4_Pos (0UL) /*!< Position of AP4 field. */
#define RADIO_PREFIX1_AP4_Msk (0xFFUL << RADIO_PREFIX1_AP4_Pos) /*!< Bit mask of AP4 field. */

/* Register: RADIO_TXADDRESS */
/* Description: Transmit address select */

/* Bits 2..0 : Transmit address select */
#define RADIO_TXADDRESS_TXADDRESS_Pos (0UL) /*!< Position of TXADDRESS field. */
#define RADIO_TXADDRESS_TXADDRESS_Msk (0x7UL << RADIO_TXADDRESS_TXADDRESS_Pos) /*!< Bit mask of TXADDRESS field. */

/* Register: RADIO_RXADDRESSES */
/* Description: Receive address select */

/* Bit 7 : Enable or disable reception on logical address 7. */
#define RADIO_RXADDRESSES_ADDR7_Pos (7UL) /*!< Position of ADDR7 field. */
#define RADIO_RXADDRESSES_ADDR7_Msk (0x1UL << RADIO_RXADDRESSES_ADDR7_Pos) /*!< Bit mask of ADDR7 field. */
#define RADIO_RXADDRESSES_ADDR7_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR7_Enabled (1UL) /*!< Enable */

/* Bit 6 : Enable or disable reception on logical address 6. */
#define RADIO_RXADDRESSES_ADDR6_Pos (6UL) /*!< Position of ADDR6 field. */
#define RADIO_RXADDRESSES_ADDR6_Msk (0x1UL << RADIO_RXADDRESSES_ADDR6_Pos) /*!< Bit mask of ADDR6 field. */
#define RADIO_RXADDRESSES_ADDR6_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR6_Enabled (1UL) /*!< Enable */

/* Bit 5 : Enable or disable reception on logical address 5. */
#define RADIO_RXADDRESSES_ADDR5_Pos (5UL) /*!< Position of ADDR5 field. */
#define RADIO_RXADDRESSES_ADDR5_Msk (0x1UL << RADIO_RXADDRESSES_ADDR5_Pos) /*!< Bit mask of ADDR5 field. */
#define RADIO_RXADDRESSES_ADDR5_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR5_Enabled (1UL) /*!< Enable */

/* Bit 4 : Enable or disable reception on logical address 4. */
#define RADIO_RXADDRESSES_ADDR4_Pos (4UL) /*!< Position of ADDR4 field. */
#define RADIO_RXADDRESSES_ADDR4_Msk (0x1UL << RADIO_RXADDRESSES_ADDR4_Pos) /*!< Bit mask of ADDR4 field. */
#define RADIO_RXADDRESSES_ADDR4_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR4_Enabled (1UL) /*!< Enable */

/* Bit 3 : Enable or disable reception on logical address 3. */
#define RADIO_RXADDRESSES_ADDR3_Pos (3UL) /*!< Position of ADDR3 field. */
#define RADIO_RXADDRESSES_ADDR3_Msk (0x1UL << RADIO_RXADDRESSES_ADDR3_Pos) /*!< Bit mask of ADDR3 field. */
#define RADIO_RXADDRESSES_ADDR3_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR3_Enabled (1UL) /*!< Enable */

/* Bit 2 : Enable or disable reception on logical address 2. */
#define RADIO_RXADDRESSES_ADDR2_Pos (2UL) /*!< Position of ADDR2 field. */
#define RADIO_RXADDRESSES_ADDR2_Msk (0x1UL << RADIO_RXADDRESSES_ADDR2_Pos) /*!< Bit mask of ADDR2 field. */
#define RADIO_RXADDRESSES_ADDR2_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR2_Enabled (1UL) /*!< Enable */

/* Bit 1 : Enable or disable reception on logical address 1. */
#define RADIO_RXADDRESSES_ADDR1_Pos (1UL) /*!< Position of ADDR1 field. */
#define RADIO_RXADDRESSES_ADDR1_Msk (0x1UL << RADIO_RXADDRESSES_ADDR1_Pos) /*!< Bit mask of ADDR1 field. */
#define RADIO_RXADDRESSES_ADDR1_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR1_Enabled (1UL) /*!< Enable */

/* Bit 0 : Enable or disable reception on logical address 0. */
#define RADIO_RXADDRESSES_ADDR0_Pos (0UL) /*!< Position of ADDR0 field. */
#define RADIO_RXADDRESSES_ADDR0_Msk (0x1UL << RADIO_RXADDRESSES_ADDR0_Pos) /*!< Bit mask of ADDR0 field. */
#define RADIO_RXADDRESSES_ADDR0_Disabled (0UL) /*!< Disable */
#define RADIO_RXADDRESSES_ADDR0_Enabled (1UL) /*!< Enable */

/* Register: RADIO_CRCCNF */
/* Description: CRC configuration */

/* Bits 9..8 : Include or exclude packet address field out of CRC calculation. */
#define RADIO_CRCCNF_SKIPADDR_Pos (8UL) /*!< Position of SKIPADDR field. */
#define RADIO_CRCCNF_SKIPADDR_Msk (0x3UL << RADIO_CRCCNF_SKIPADDR_Pos) /*!< Bit mask of SKIPADDR field. */
#define RADIO_CRCCNF_SKIPADDR_Include (0UL) /*!< CRC calculation includes address field */
#define RADIO_CRCCNF_SKIPADDR_Skip (1UL) /*!< CRC calculation does not include address field. The CRC calculation will start at the first byte after the address. */
#define RADIO_CRCCNF_SKIPADDR_Ieee802154 (2UL) /*!< CRC calculation as per 802.15.4 standard. Starting at first byte after length field. */

/* Bits 1..0 : CRC length in number of bytes For MODE Ble_LR125Kbit and Ble_LR500Kbit, only LEN set to 3 is supported */
#define RADIO_CRCCNF_LEN_Pos (0UL) /*!< Position of LEN field. */
#define RADIO_CRCCNF_LEN_Msk (0x3UL << RADIO_CRCCNF_LEN_Pos) /*!< Bit mask of LEN field. */
#define RADIO_CRCCNF_LEN_Disabled (0UL) /*!< CRC length is zero and CRC calculation is disabled */
#define RADIO_CRCCNF_LEN_One (1UL) /*!< CRC length is one byte and CRC calculation is enabled */
#define RADIO_CRCCNF_LEN_Two (2UL) /*!< CRC length is two bytes and CRC calculation is enabled */
#define RADIO_CRCCNF_LEN_Three (3UL) /*!< CRC length is three bytes and CRC calculation is enabled */

/* Register: RADIO_CRCPOLY */
/* Description: CRC polynomial */

/* Bits 23..0 : CRC polynomial */
#define RADIO_CRCPOLY_CRCPOLY_Pos (0UL) /*!< Position of CRCPOLY field. */
#define RADIO_CRCPOLY_CRCPOLY_Msk (0xFFFFFFUL << RADIO_CRCPOLY_CRCPOLY_Pos) /*!< Bit mask of CRCPOLY field. */

/* Register: RADIO_CRCINIT */
/* Description: CRC initial value */

/* Bits 23..0 : CRC initial value */
#define RADIO_CRCINIT_CRCINIT_Pos (0UL) /*!< Position of CRCINIT field. */
#define RADIO_CRCINIT_CRCINIT_Msk (0xFFFFFFUL << RADIO_CRCINIT_CRCINIT_Pos) /*!< Bit mask of CRCINIT field. */

/* Register: RADIO_TIFS */
/* Description: Interframe spacing in us */

/* Bits 9..0 : Interframe spacing in us. */
#define RADIO_TIFS_TIFS_Pos (0UL) /*!< Position of TIFS field. */
#define RADIO_TIFS_TIFS_Msk (0x3FFUL << RADIO_TIFS_TIFS_Pos) /*!< Bit mask of TIFS field. */

/* Register: RADIO_RSSISAMPLE */
/* Description: RSSI sample */

/* Bits 6..0 : RSSI sample. */
#define RADIO_RSSISAMPLE_RSSISAMPLE_Pos (0UL) /*!< Position of RSSISAMPLE field. */
#define RADIO_RSSISAMPLE_RSSISAMPLE_Msk (0x7FUL << RADIO_RSSISAMPLE_RSSISAMPLE_Pos) /*!< Bit mask of RSSISAMPLE field. */

/* Register: RADIO_STATE */
/* Description: Current radio state */

/* Bits 3..0 : Current radio state */
#define RADIO_STATE_STATE_Pos (0UL) /*!< Position of STATE field. */
#define RADIO_STATE_STATE_Msk (0xFUL << RADIO_STATE_STATE_Pos) /*!< Bit mask of STATE field. */
#define RADIO_STATE_STATE_Disabled (0UL) /*!< RADIO is in the Disabled state */
#define RADIO_STATE_STATE_RxRu (1UL) /*!< RADIO is in the RXRU state */
#define RADIO_STATE_STATE_RxIdle (2UL) /*!< RADIO is in the RXIDLE state */
#define RADIO_STATE_STATE_Rx (3UL) /*!< RADIO is in the RX state */
#define RADIO_STATE_STATE_RxDisable (4UL) /*!< RADIO is in the RXDISABLED state */
#define RADIO_STATE_STATE_TxRu (9UL) /*!< RADIO is in the TXRU state */
#define RADIO_STATE_STATE_TxIdle (10UL) /*!< RADIO is in the TXIDLE state */
#define RADIO_STATE_STATE_Tx (11UL) /*!< RADIO is in the TX state */
#define RADIO_STATE_STATE_TxDisable (12UL) /*!< RADIO is in the TXDISABLED state */

/* Register: RADIO_DATAWHITEIV */
/* Description: Data whitening initial value */

/* Bits 6..0 : Data whitening initial value. Bit 6 is hardwired to '1', writing '0' to it has no effect, and it will always be read back and used by the device as '1'. */
#define RADIO_DATAWHITEIV_DATAWHITEIV_Pos (0UL) /*!< Position of DATAWHITEIV field. */
#define RADIO_DATAWHITEIV_DATAWHITEIV_Msk (0x7FUL << RADIO_DATAWHITEIV_DATAWHITEIV_Pos) /*!< Bit mask of DATAWHITEIV field. */

/* Register: RADIO_BCC */
/* Description: Bit counter compare */

/* Bits 31..0 : Bit counter compare */
#define RADIO_BCC_BCC_Pos (0UL) /*!< Position of BCC field. */
#define RADIO_BCC_BCC_Msk (0xFFFFFFFFUL << RADIO_BCC_BCC_Pos) /*!< Bit mask of BCC field. */

/* Register: RADIO_DAB */
/* Description: Description collection: Device address base segment n */

/* Bits 31..0 : Device address base segment n */
#define RADIO_DAB_DAB_Pos (0UL) /*!< Position of DAB field. */
#define RADIO_DAB_DAB_Msk (0xFFFFFFFFUL << RADIO_DAB_DAB_Pos) /*!< Bit mask of DAB field. */

/* Register: RADIO_DAP */
/* Description: Description collection: Device address prefix n */

/* Bits 15..0 : Device address prefix n */
#define RADIO_DAP_DAP_Pos (0UL) /*!< Position of DAP field. */
#define RADIO_DAP_DAP_Msk (0xFFFFUL << RADIO_DAP_DAP_Pos) /*!< Bit mask of DAP field. */

/* Register: RADIO_DACNF */
/* Description: Device address match configuration */

/* Bit 15 : TxAdd for device address 7 */
#define RADIO_DACNF_TXADD7_Pos (15UL) /*!< Position of TXADD7 field. */
#define RADIO_DACNF_TXADD7_Msk (0x1UL << RADIO_DACNF_TXADD7_Pos) /*!< Bit mask of TXADD7 field. */

/* Bit 14 : TxAdd for device address 6 */
#define RADIO_DACNF_TXADD6_Pos (14UL) /*!< Position of TXADD6 field. */
#define RADIO_DACNF_TXADD6_Msk (0x1UL << RADIO_DACNF_TXADD6_Pos) /*!< Bit mask of TXADD6 field. */

/* Bit 13 : TxAdd for device address 5 */
#define RADIO_DACNF_TXADD5_Pos (13UL) /*!< Position of TXADD5 field. */
#define RADIO_DACNF_TXADD5_Msk (0x1UL << RADIO_DACNF_TXADD5_Pos) /*!< Bit mask of TXADD5 field. */

/* Bit 12 : TxAdd for device address 4 */
#define RADIO_DACNF_TXADD4_Pos (12UL) /*!< Position of TXADD4 field. */
#define RADIO_DACNF_TXADD4_Msk (0x1UL << RADIO_DACNF_TXADD4_Pos) /*!< Bit mask of TXADD4 field. */

/* Bit 11 : TxAdd for device address 3 */
#define RADIO_DACNF_TXADD3_Pos (11UL) /*!< Position of TXADD3 field. */
#define RADIO_DACNF_TXADD3_Msk (0x1UL << RADIO_DACNF_TXADD3_Pos) /*!< Bit mask of TXADD3 field. */

/* Bit 10 : TxAdd for device address 2 */
#define RADIO_DACNF_TXADD2_Pos (10UL) /*!< Position of TXADD2 field. */
#define RADIO_DACNF_TXADD2_Msk (0x1UL << RADIO_DACNF_TXADD2_Pos) /*!< Bit mask of TXADD2 field. */

/* Bit 9 : TxAdd for device address 1 */
#define RADIO_DACNF_TXADD1_Pos (9UL) /*!< Position of TXADD1 field. */
#define RADIO_DACNF_TXADD1_Msk (0x1UL << RADIO_DACNF_TXADD1_Pos) /*!< Bit mask of TXADD1 field. */

/* Bit 8 : TxAdd for device address 0 */
#define RADIO_DACNF_TXADD0_Pos (8UL) /*!< Position of TXADD0 field. */
#define RADIO_DACNF_TXADD0_Msk (0x1UL << RADIO_DACNF_TXADD0_Pos) /*!< Bit mask of TXADD0 field. */

/* Bit 7 : Enable or disable device address matching using device address 7 */
#define RADIO_DACNF_ENA7_Pos (7UL) /*!< Position of ENA7 field. */
#define RADIO_DACNF_ENA7_Msk (0x1UL << RADIO_DACNF_ENA7_Pos) /*!< Bit mask of ENA7 field. */
#define RADIO_DACNF_ENA7_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA7_Enabled (1UL) /*!< Enabled */

/* Bit 6 : Enable or disable device address matching using device address 6 */
#define RADIO_DACNF_ENA6_Pos (6UL) /*!< Position of ENA6 field. */
#define RADIO_DACNF_ENA6_Msk (0x1UL << RADIO_DACNF_ENA6_Pos) /*!< Bit mask of ENA6 field. */
#define RADIO_DACNF_ENA6_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA6_Enabled (1UL) /*!< Enabled */

/* Bit 5 : Enable or disable device address matching using device address 5 */
#define RADIO_DACNF_ENA5_Pos (5UL) /*!< Position of ENA5 field. */
#define RADIO_DACNF_ENA5_Msk (0x1UL << RADIO_DACNF_ENA5_Pos) /*!< Bit mask of ENA5 field. */
#define RADIO_DACNF_ENA5_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA5_Enabled (1UL) /*!< Enabled */

/* Bit 4 : Enable or disable device address matching using device address 4 */
#define RADIO_DACNF_ENA4_Pos (4UL) /*!< Position of ENA4 field. */
#define RADIO_DACNF_ENA4_Msk (0x1UL << RADIO_DACNF_ENA4_Pos) /*!< Bit mask of ENA4 field. */
#define RADIO_DACNF_ENA4_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA4_Enabled (1UL) /*!< Enabled */

/* Bit 3 : Enable or disable device address matching using device address 3 */
#define RADIO_DACNF_ENA3_Pos (3UL) /*!< Position of ENA3 field. */
#define RADIO_DACNF_ENA3_Msk (0x1UL << RADIO_DACNF_ENA3_Pos) /*!< Bit mask of ENA3 field. */
#define RADIO_DACNF_ENA3_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA3_Enabled (1UL) /*!< Enabled */

/* Bit 2 : Enable or disable device address matching using device address 2 */
#define RADIO_DACNF_ENA2_Pos (2UL) /*!< Position of ENA2 field. */
#define RADIO_DACNF_ENA2_Msk (0x1UL << RADIO_DACNF_ENA2_Pos) /*!< Bit mask of ENA2 field. */
#define RADIO_DACNF_ENA2_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA2_Enabled (1UL) /*!< Enabled */

/* Bit 1 : Enable or disable device address matching using device address 1 */
#define RADIO_DACNF_ENA1_Pos (1UL) /*!< Position of ENA1 field. */
#define RADIO_DACNF_ENA1_Msk (0x1UL << RADIO_DACNF_ENA1_Pos) /*!< Bit mask of ENA1 field. */
#define RADIO_DACNF_ENA1_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA1_Enabled (1UL) /*!< Enabled */

/* Bit 0 : Enable or disable device address matching using device address 0 */
#define RADIO_DACNF_ENA0_Pos (0UL) /*!< Position of ENA0 field. */
#define RADIO_DACNF_ENA0_Msk (0x1UL << RADIO_DACNF_ENA0_Pos) /*!< Bit mask of ENA0 field. */
#define RADIO_DACNF_ENA0_Disabled (0UL) /*!< Disabled */
#define RADIO_DACNF_ENA0_Enabled (1UL) /*!< Enabled */

/* Register: RADIO_MHRMATCHCONF */
/* Description: Search pattern configuration */

/* Bits 31..0 : Search pattern configuration */
#define RADIO_MHRMATCHCONF_MHRMATCHCONF_Pos (0UL) /*!< Position of MHRMATCHCONF field. */
#define RADIO_MHRMATCHCONF_MHRMATCHCONF_Msk (0xFFFFFFFFUL << RADIO_MHRMATCHCONF_MHRMATCHCONF_Pos) /*!< Bit mask of MHRMATCHCONF field. */

/* Register: RADIO_MHRMATCHMAS */
/* Description: Pattern mask */

/* Bits 31..0 : Pattern mask */
#define RADIO_MHRMATCHMAS_MHRMATCHMAS_Pos (0UL) /*!< Position of MHRMATCHMAS field. */
#define RADIO_MHRMATCHMAS_MHRMATCHMAS_Msk (0xFFFFFFFFUL << RADIO_MHRMATCHMAS_MHRMATCHMAS_Pos) /*!< Bit mask of MHRMATCHMAS field. */

/* Register: RADIO_MODECNF0 */
/* Description: Radio mode configuration register 0 */

/* Bits 9..8 : Default TX value */
#define RADIO_MODECNF0_DTX_Pos (8UL) /*!< Position of DTX field. */
#define RADIO_MODECNF0_DTX_Msk (0x3UL << RADIO_MODECNF0_DTX_Pos) /*!< Bit mask of DTX field. */
#define RADIO_MODECNF0_DTX_B1 (0UL) /*!< Transmit '1' */
#define RADIO_MODECNF0_DTX_B0 (1UL) /*!< Transmit '0' */
#define RADIO_MODECNF0_DTX_Center (2UL) /*!< Transmit center frequency */

/* Bit 0 : Radio ramp-up time */
#define RADIO_MODECNF0_RU_Pos (0UL) /*!< Position of RU field. */
#define RADIO_MODECNF0_RU_Msk (0x1UL << RADIO_MODECNF0_RU_Pos) /*!< Bit mask of RU field. */
#define RADIO_MODECNF0_RU_Default (0UL) /*!< Default ramp-up time (tRXEN and tTXEN), compatible with firmware written for nRF51 */
#define RADIO_MODECNF0_RU_Fast (1UL) /*!< Fast ramp-up (tRXEN,FAST and tTXEN,FAST), see electrical specifications for more information */

/* Register: RADIO_SFD */
/* Description: IEEE 802.15.4 start of frame delimiter */

/* Bits 7..0 : IEEE 802.15.4 start of frame delimiter */
#define RADIO_SFD_SFD_Pos (0UL) /*!< Position of SFD field. */
#define RADIO_SFD_SFD_Msk (0xFFUL << RADIO_SFD_SFD_Pos) /*!< Bit mask of SFD field. */

/* Register: RADIO_EDCNT */
/* Description: IEEE 802.15.4 energy detect loop count */

/* Bits 20..0 : IEEE 802.15.4 energy detect loop count */
#define RADIO_EDCNT_EDCNT_Pos (0UL) /*!< Position of EDCNT field. */
#define RADIO_EDCNT_EDCNT_Msk (0x1FFFFFUL << RADIO_EDCNT_EDCNT_Pos) /*!< Bit mask of EDCNT field. */

/* Register: RADIO_EDSAMPLE */
/* Description: IEEE 802.15.4 energy detect level */

/* Bits 7..0 : IEEE 802.15.4 energy detect level */
#define RADIO_EDSAMPLE_EDLVL_Pos (0UL) /*!< Position of EDLVL field. */
#define RADIO_EDSAMPLE_EDLVL_Msk (0xFFUL << RADIO_EDSAMPLE_EDLVL_Pos) /*!< Bit mask of EDLVL field. */

/* Register: RADIO_CCACTRL */
/* Description: IEEE 802.15.4 clear channel assessment control */

/* Bits 31..24 : Limit for occurances above CCACORRTHRES. When not equal to zero the corrolator based signal detect is enabled. */
#define RADIO_CCACTRL_CCACORRCNT_Pos (24UL) /*!< Position of CCACORRCNT field. */
#define RADIO_CCACTRL_CCACORRCNT_Msk (0xFFUL << RADIO_CCACTRL_CCACORRCNT_Pos) /*!< Bit mask of CCACORRCNT field. */

/* Bits 23..16 : CCA correlator busy threshold. Only relevant to CarrierMode, CarrierAndEdMode, and CarrierOrEdMode. */
#define RADIO_CCACTRL_CCACORRTHRES_Pos (16UL) /*!< Position of CCACORRTHRES field. */
#define RADIO_CCACTRL_CCACORRTHRES_Msk (0xFFUL << RADIO_CCACTRL_CCACORRTHRES_Pos) /*!< Bit mask of CCACORRTHRES field. */

/* Bits 15..8 : CCA energy busy threshold. Used in all the CCA modes except CarrierMode. */
#define RADIO_CCACTRL_CCAEDTHRES_Pos (8UL) /*!< Position of CCAEDTHRES field. */
#define RADIO_CCACTRL_CCAEDTHRES_Msk (0xFFUL << RADIO_CCACTRL_CCAEDTHRES_Pos) /*!< Bit mask of CCAEDTHRES field. */

/* Bits 2..0 : CCA mode of operation */
#define RADIO_CCACTRL_CCAMODE_Pos (0UL) /*!< Position of CCAMODE field. */
#define RADIO_CCACTRL_CCAMODE_Msk (0x7UL << RADIO_CCACTRL_CCAMODE_Pos) /*!< Bit mask of CCAMODE field. */
#define RADIO_CCACTRL_CCAMODE_EdMode (0UL) /*!< Energy above threshold */
#define RADIO_CCACTRL_CCAMODE_CarrierMode (1UL) /*!< Carrier seen */
#define RADIO_CCACTRL_CCAMODE_CarrierAndEdMode (2UL) /*!< Energy above threshold AND carrier seen */
#define RADIO_CCACTRL_CCAMODE_CarrierOrEdMode (3UL) /*!< Energy above threshold OR carrier seen */
#define RADIO_CCACTRL_CCAMODE_EdModeTest1 (4UL) /*!< Energy above threshold test mode that will abort when first ED measurement over threshold is seen. No averaging. */

/* Register: RADIO_DFEMODE */
/* Description: Whether to use Angle-of-Arrival (AOA) or Angle-of-Departure (AOD) */

/* Bits 1..0 : Direction finding operation mode */
#define RADIO_DFEMODE_DFEOPMODE_Pos (0UL) /*!< Position of DFEOPMODE field. */
#define RADIO_DFEMODE_DFEOPMODE_Msk (0x3UL << RADIO_DFEMODE_DFEOPMODE_Pos) /*!< Bit mask of DFEOPMODE field. */
#define RADIO_DFEMODE_DFEOPMODE_Disabled (0UL) /*!< Direction finding mode disabled */
#define RADIO_DFEMODE_DFEOPMODE_AoD (2UL) /*!< Direction finding mode set to AoD */
#define RADIO_DFEMODE_DFEOPMODE_AoA (3UL) /*!< Direction finding mode set to AoA */

/* Register: RADIO_CTEINLINECONF */
/* Description: Configuration for CTE inline mode */

/* Bits 31..24 : S0 bit mask to set which bit to match */
#define RADIO_CTEINLINECONF_S0MASK_Pos (24UL) /*!< Position of S0MASK field. */
#define RADIO_CTEINLINECONF_S0MASK_Msk (0xFFUL << RADIO_CTEINLINECONF_S0MASK_Pos) /*!< Bit mask of S0MASK field. */

/* Bits 23..16 : S0 bit pattern to match */
#define RADIO_CTEINLINECONF_S0CONF_Pos (16UL) /*!< Position of S0CONF field. */
#define RADIO_CTEINLINECONF_S0CONF_Msk (0xFFUL << RADIO_CTEINLINECONF_S0CONF_Pos) /*!< Bit mask of S0CONF field. */

/* Bits 15..13 : Spacing between samples for the samples in the SWITCHING period when CTEINLINEMODE is set. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_Pos (13UL) /*!< Position of CTEINLINERXMODE2US field. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_Msk (0x7UL << RADIO_CTEINLINECONF_CTEINLINERXMODE2US_Pos) /*!< Bit mask of CTEINLINERXMODE2US field. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_4us (1UL) /*!< 4 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_2us (2UL) /*!< 2 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_1us (3UL) /*!< 1 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_500ns (4UL) /*!< 0.5 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_250ns (5UL) /*!< 0.25 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE2US_125ns (6UL) /*!< 0.125 us */

/* Bits 12..10 : Spacing between samples for the samples in the SWITCHING period when CTEINLINEMODE is set. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_Pos (10UL) /*!< Position of CTEINLINERXMODE1US field. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_Msk (0x7UL << RADIO_CTEINLINECONF_CTEINLINERXMODE1US_Pos) /*!< Bit mask of CTEINLINERXMODE1US field. */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_4us (1UL) /*!< 4 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_2us (2UL) /*!< 2 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_1us (3UL) /*!< 1 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_500ns (4UL) /*!< 0.5 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_250ns (5UL) /*!< 0.25 us */
#define RADIO_CTEINLINECONF_CTEINLINERXMODE1US_125ns (6UL) /*!< 0.125 us */

/* Bits 7..6 : Max range of CTETime */
#define RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_Pos (6UL) /*!< Position of CTETIMEVALIDRANGE field. */
#define RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_Msk (0x3UL << RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_Pos) /*!< Bit mask of CTETIMEVALIDRANGE field. */
#define RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_20 (0UL) /*!< 20 in 8 us unit (default) Set to 20 if parsed CTETime is larger than 20 */
#define RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_31 (1UL) /*!< 31 in 8 us unit */
#define RADIO_CTEINLINECONF_CTETIMEVALIDRANGE_63 (2UL) /*!< 63 in 8 us unit */

/* Bit 4 : Sampling/switching if CRC is not OK */
#define RADIO_CTEINLINECONF_CTEERRORHANDLING_Pos (4UL) /*!< Position of CTEERRORHANDLING field. */
#define RADIO_CTEINLINECONF_CTEERRORHANDLING_Msk (0x1UL << RADIO_CTEINLINECONF_CTEERRORHANDLING_Pos) /*!< Bit mask of CTEERRORHANDLING field. */
#define RADIO_CTEINLINECONF_CTEERRORHANDLING_No (0UL) /*!< No sampling and antenna switching when CRC is not OK */
#define RADIO_CTEINLINECONF_CTEERRORHANDLING_Yes (1UL) /*!< Sampling and antenna switching also when CRC is not OK */

/* Bit 3 : CTEInfo is S1 byte or not */
#define RADIO_CTEINLINECONF_CTEINFOINS1_Pos (3UL) /*!< Position of CTEINFOINS1 field. */
#define RADIO_CTEINLINECONF_CTEINFOINS1_Msk (0x1UL << RADIO_CTEINLINECONF_CTEINFOINS1_Pos) /*!< Bit mask of CTEINFOINS1 field. */
#define RADIO_CTEINLINECONF_CTEINFOINS1_NotInS1 (0UL) /*!< CTEInfo is NOT in S1 byte (advertising PDU) */
#define RADIO_CTEINLINECONF_CTEINFOINS1_InS1 (1UL) /*!< CTEInfo is in S1 byte (data PDU) */

/* Bit 0 : Enable parsing of CTEInfo from received packet in BLE modes */
#define RADIO_CTEINLINECONF_CTEINLINECTRLEN_Pos (0UL) /*!< Position of CTEINLINECTRLEN field. */
#define RADIO_CTEINLINECONF_CTEINLINECTRLEN_Msk (0x1UL << RADIO_CTEINLINECONF_CTEINLINECTRLEN_Pos) /*!< Bit mask of CTEINLINECTRLEN field. */
#define RADIO_CTEINLINECONF_CTEINLINECTRLEN_Disabled (0UL) /*!< Parsing of CTEInfo is disabled */
#define RADIO_CTEINLINECONF_CTEINLINECTRLEN_Enabled (1UL) /*!< Parsing of CTEInfo is enabled */

/* Register: RADIO_DFECTRL1 */
/* Description: Various configuration for Direction finding */

/* Bits 27..24 : Gain will be lowered by the specified number of gain steps at the start of CTE */
#define RADIO_DFECTRL1_AGCBACKOFFGAIN_Pos (24UL) /*!< Position of AGCBACKOFFGAIN field. */
#define RADIO_DFECTRL1_AGCBACKOFFGAIN_Msk (0xFUL << RADIO_DFECTRL1_AGCBACKOFFGAIN_Pos) /*!< Bit mask of AGCBACKOFFGAIN field. */

/* Bits 23..20 : Repeat each individual antenna pattern N times sequentially, i.e. P0, P0, P1, P1, P2, P2, P3, P3, etc. */
#define RADIO_DFECTRL1_REPEATPATTERN_Pos (20UL) /*!< Position of REPEATPATTERN field. */
#define RADIO_DFECTRL1_REPEATPATTERN_Msk (0xFUL << RADIO_DFECTRL1_REPEATPATTERN_Pos) /*!< Bit mask of REPEATPATTERN field. */
#define RADIO_DFECTRL1_REPEATPATTERN_NoRepeat (0UL) /*!< Do not repeat (1 time in total) */

/* Bits 18..16 : Interval between samples in the SWITCHING period when CTEINLINECTRLEN is 0 */
#define RADIO_DFECTRL1_TSAMPLESPACING_Pos (16UL) /*!< Position of TSAMPLESPACING field. */
#define RADIO_DFECTRL1_TSAMPLESPACING_Msk (0x7UL << RADIO_DFECTRL1_TSAMPLESPACING_Pos) /*!< Bit mask of TSAMPLESPACING field. */
#define RADIO_DFECTRL1_TSAMPLESPACING_4us (1UL) /*!< 4 us */
#define RADIO_DFECTRL1_TSAMPLESPACING_2us (2UL) /*!< 2 us */
#define RADIO_DFECTRL1_TSAMPLESPACING_1us (3UL) /*!< 1 us */
#define RADIO_DFECTRL1_TSAMPLESPACING_500ns (4UL) /*!< 0.5 us */
#define RADIO_DFECTRL1_TSAMPLESPACING_250ns (5UL) /*!< 0.25 us */
#define RADIO_DFECTRL1_TSAMPLESPACING_125ns (6UL) /*!< 0.125 us */

/* Bit 15 : Whether to sample I/Q or magnitude/phase */
#define RADIO_DFECTRL1_SAMPLETYPE_Pos (15UL) /*!< Position of SAMPLETYPE field. */
#define RADIO_DFECTRL1_SAMPLETYPE_Msk (0x1UL << RADIO_DFECTRL1_SAMPLETYPE_Pos) /*!< Bit mask of SAMPLETYPE field. */
#define RADIO_DFECTRL1_SAMPLETYPE_IQ (0UL) /*!< Complex samples in I and Q */
#define RADIO_DFECTRL1_SAMPLETYPE_MagPhase (1UL) /*!< Complex samples as magnitude and phase */

/* Bits 14..12 : Interval between samples in the REFERENCE period */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_Pos (12UL) /*!< Position of TSAMPLESPACINGREF field. */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_Msk (0x7UL << RADIO_DFECTRL1_TSAMPLESPACINGREF_Pos) /*!< Bit mask of TSAMPLESPACINGREF field. */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_4us (1UL) /*!< 4 us */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_2us (2UL) /*!< 2 us */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_1us (3UL) /*!< 1 us */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_500ns (4UL) /*!< 0.5 us */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_250ns (5UL) /*!< 0.25 us */
#define RADIO_DFECTRL1_TSAMPLESPACINGREF_125ns (6UL) /*!< 0.125 us */

/* Bits 10..8 : Interval between every time the antenna is changed in the SWITCHING state */
#define RADIO_DFECTRL1_TSWITCHSPACING_Pos (8UL) /*!< Position of TSWITCHSPACING field. */
#define RADIO_DFECTRL1_TSWITCHSPACING_Msk (0x7UL << RADIO_DFECTRL1_TSWITCHSPACING_Pos) /*!< Bit mask of TSWITCHSPACING field. */
#define RADIO_DFECTRL1_TSWITCHSPACING_4us (1UL) /*!< 4 us */
#define RADIO_DFECTRL1_TSWITCHSPACING_2us (2UL) /*!< 2 us */
#define RADIO_DFECTRL1_TSWITCHSPACING_1us (3UL) /*!< 1 us */

/* Bit 7 : Add CTE extension and do antenna switching/sampling in this extension */
#define RADIO_DFECTRL1_DFEINEXTENSION_Pos (7UL) /*!< Position of DFEINEXTENSION field. */
#define RADIO_DFECTRL1_DFEINEXTENSION_Msk (0x1UL << RADIO_DFECTRL1_DFEINEXTENSION_Pos) /*!< Bit mask of DFEINEXTENSION field. */
#define RADIO_DFECTRL1_DFEINEXTENSION_Payload (0UL) /*!< Antenna switching/sampling is done in the packet payload */
#define RADIO_DFECTRL1_DFEINEXTENSION_CRC (1UL) /*!< AoA/AoD procedure triggered at end of CRC */

/* Bits 5..0 : Length of the AoA/AoD procedure in number of 8 us units */
#define RADIO_DFECTRL1_NUMBEROF8US_Pos (0UL) /*!< Position of NUMBEROF8US field. */
#define RADIO_DFECTRL1_NUMBEROF8US_Msk (0x3FUL << RADIO_DFECTRL1_NUMBEROF8US_Pos) /*!< Bit mask of NUMBEROF8US field. */

/* Register: RADIO_DFECTRL2 */
/* Description: Start offset for Direction finding */

/* Bits 27..16 : Signed value offset in number of 16 MHz clock cycles for fine tuning of the sampling instant for all IQ samples. With TSAMPLEOFFSET=0 the first sample is taken immediately at the start of the reference period */
#define RADIO_DFECTRL2_TSAMPLEOFFSET_Pos (16UL) /*!< Position of TSAMPLEOFFSET field. */
#define RADIO_DFECTRL2_TSAMPLEOFFSET_Msk (0xFFFUL << RADIO_DFECTRL2_TSAMPLEOFFSET_Pos) /*!< Bit mask of TSAMPLEOFFSET field. */

/* Bits 12..0 : Signed value offset after the end of the CRC before starting switching in number of 16 MHz clock cycles */
#define RADIO_DFECTRL2_TSWITCHOFFSET_Pos (0UL) /*!< Position of TSWITCHOFFSET field. */
#define RADIO_DFECTRL2_TSWITCHOFFSET_Msk (0x1FFFUL << RADIO_DFECTRL2_TSWITCHOFFSET_Pos) /*!< Bit mask of TSWITCHOFFSET field. */

/* Register: RADIO_SWITCHPATTERN */
/* Description: GPIO patterns to be used for each antenna */

/* Bits 7..0 : Fill array of GPIO patterns for antenna control. */
#define RADIO_SWITCHPATTERN_SWITCHPATTERN_Pos (0UL) /*!< Position of SWITCHPATTERN field. */
#define RADIO_SWITCHPATTERN_SWITCHPATTERN_Msk (0xFFUL << RADIO_SWITCHPATTERN_SWITCHPATTERN_Pos) /*!< Bit mask of SWITCHPATTERN field. */

/* Register: RADIO_CLEARPATTERN */
/* Description: Clear the GPIO pattern array for antenna control */

/* Bit 0 : Clears GPIO pattern array for antenna control */
#define RADIO_CLEARPATTERN_CLEARPATTERN_Pos (0UL) /*!< Position of CLEARPATTERN field. */
#define RADIO_CLEARPATTERN_CLEARPATTERN_Msk (0x1UL << RADIO_CLEARPATTERN_CLEARPATTERN_Pos) /*!< Bit mask of CLEARPATTERN field. */
#define RADIO_CLEARPATTERN_CLEARPATTERN_Clear (1UL) /*!< Clear the GPIO pattern */

/* Register: RADIO_PSEL_DFEGPIO */
/* Description: Description collection: Pin select for DFE pin n */

/* Bit 31 : Connection */
#define RADIO_PSEL_DFEGPIO_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define RADIO_PSEL_DFEGPIO_CONNECT_Msk (0x1UL << RADIO_PSEL_DFEGPIO_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define RADIO_PSEL_DFEGPIO_CONNECT_Connected (0UL) /*!< Connect */
#define RADIO_PSEL_DFEGPIO_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define RADIO_PSEL_DFEGPIO_PORT_Pos (5UL) /*!< Position of PORT field. */
#define RADIO_PSEL_DFEGPIO_PORT_Msk (0x1UL << RADIO_PSEL_DFEGPIO_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define RADIO_PSEL_DFEGPIO_PIN_Pos (0UL) /*!< Position of PIN field. */
#define RADIO_PSEL_DFEGPIO_PIN_Msk (0x1FUL << RADIO_PSEL_DFEGPIO_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: RADIO_DFEPACKET_PTR */
/* Description: Data pointer */

/* Bits 31..0 : Data pointer */
#define RADIO_DFEPACKET_PTR_PTR_Pos (0UL) /*!< Position of PTR field. */
#define RADIO_DFEPACKET_PTR_PTR_Msk (0xFFFFFFFFUL << RADIO_DFEPACKET_PTR_PTR_Pos) /*!< Bit mask of PTR field. */

/* Register: RADIO_DFEPACKET_MAXCNT */
/* Description: Maximum number of buffer words to transfer */

/* Bits 13..0 : Maximum number of buffer words to transfer */
#define RADIO_DFEPACKET_MAXCNT_MAXCNT_Pos (0UL) /*!< Position of MAXCNT field. */
#define RADIO_DFEPACKET_MAXCNT_MAXCNT_Msk (0x3FFFUL << RADIO_DFEPACKET_MAXCNT_MAXCNT_Pos) /*!< Bit mask of MAXCNT field. */

/* Register: RADIO_DFEPACKET_AMOUNT */
/* Description: Number of samples transferred in the last transaction */

/* Bits 15..0 : Number of samples transferred in the last transaction */
#define RADIO_DFEPACKET_AMOUNT_AMOUNT_Pos (0UL) /*!< Position of AMOUNT field. */
#define RADIO_DFEPACKET_AMOUNT_AMOUNT_Msk (0xFFFFUL << RADIO_DFEPACKET_AMOUNT_AMOUNT_Pos) /*!< Bit mask of AMOUNT field. */

/* Register: RADIO_POWER */
/* Description: Peripheral power control */

/* Bit 0 : Peripheral power control. The peripheral and its registers will be reset to its initial state by switching the peripheral off and then back on again. */
#define RADIO_POWER_POWER_Pos (0UL) /*!< Position of POWER field. */
#define RADIO_POWER_POWER_Msk (0x1UL << RADIO_POWER_POWER_Pos) /*!< Bit mask of POWER field. */
#define RADIO_POWER_POWER_Disabled (0UL) /*!< Peripheral is powered off */
#define RADIO_POWER_POWER_Enabled (1UL) /*!< Peripheral is powered on */



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
/* ================                                          TEMP                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief Temperature Sensor (TEMP)
  */

typedef struct {                                /*!< (@ 0x41010000) TEMP_NS Structure                                          */
  __OM  uint32_t  TASKS_START;                  /*!< (@ 0x00000000) Start temperature measurement                              */
  __OM  uint32_t  TASKS_STOP;                   /*!< (@ 0x00000004) Stop temperature measurement                               */
  __IM  uint32_t  RESERVED[30];
  __IOM uint32_t  SUBSCRIBE_START;              /*!< (@ 0x00000080) Subscribe configuration for task START                     */
  __IOM uint32_t  SUBSCRIBE_STOP;               /*!< (@ 0x00000084) Subscribe configuration for task STOP                      */
  __IM  uint32_t  RESERVED1[30];
  __IOM uint32_t  EVENTS_DATARDY;               /*!< (@ 0x00000100) Temperature measurement complete, data ready               */
  __IM  uint32_t  RESERVED2[31];
  __IOM uint32_t  PUBLISH_DATARDY;              /*!< (@ 0x00000180) Publish configuration for event DATARDY                    */
  __IM  uint32_t  RESERVED3[96];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED4[127];
  __IM  int32_t   TEMP;                         /*!< (@ 0x00000508) Temperature in degC (0.25deg steps)                        */
  __IM  uint32_t  RESERVED5[5];
  __IOM uint32_t  A0;                           /*!< (@ 0x00000520) Slope of first piecewise linear function                   */
  __IOM uint32_t  A1;                           /*!< (@ 0x00000524) Slope of second piecewise linear function                  */
  __IOM uint32_t  A2;                           /*!< (@ 0x00000528) Slope of third piecewise linear function                   */
  __IOM uint32_t  A3;                           /*!< (@ 0x0000052C) Slope of fourth piecewise linear function                  */
  __IOM uint32_t  A4;                           /*!< (@ 0x00000530) Slope of fifth piecewise linear function                   */
  __IOM uint32_t  A5;                           /*!< (@ 0x00000534) Slope of sixth piecewise linear function                   */
  __IM  uint32_t  RESERVED6[2];
  __IOM uint32_t  B0;                           /*!< (@ 0x00000540) y-intercept of first piecewise linear function             */
  __IOM uint32_t  B1;                           /*!< (@ 0x00000544) y-intercept of second piecewise linear function            */
  __IOM uint32_t  B2;                           /*!< (@ 0x00000548) y-intercept of third piecewise linear function             */
  __IOM uint32_t  B3;                           /*!< (@ 0x0000054C) y-intercept of fourth piecewise linear function            */
  __IOM uint32_t  B4;                           /*!< (@ 0x00000550) y-intercept of fifth piecewise linear function             */
  __IOM uint32_t  B5;                           /*!< (@ 0x00000554) y-intercept of sixth piecewise linear function             */
  __IM  uint32_t  RESERVED7[2];
  __IOM uint32_t  T0;                           /*!< (@ 0x00000560) Endpoint of first piecewise linear function                */
  __IOM uint32_t  T1;                           /*!< (@ 0x00000564) Endpoint of second piecewise linear function               */
  __IOM uint32_t  T2;                           /*!< (@ 0x00000568) Endpoint of third piecewise linear function                */
  __IOM uint32_t  T3;                           /*!< (@ 0x0000056C) Endpoint of fourth piecewise linear function               */
  __IOM uint32_t  T4;                           /*!< (@ 0x00000570) Endpoint of fifth piecewise linear function                */
} NRF_TEMP_Type;                                /*!< Size = 1396 (0x574)                                                       */


/* Peripheral: TEMP */
/* Description: Temperature Sensor */

/* Register: TEMP_TASKS_START */
/* Description: Start temperature measurement */

/* Bit 0 : Start temperature measurement */
#define TEMP_TASKS_START_TASKS_START_Pos (0UL) /*!< Position of TASKS_START field. */
#define TEMP_TASKS_START_TASKS_START_Msk (0x1UL << TEMP_TASKS_START_TASKS_START_Pos) /*!< Bit mask of TASKS_START field. */
#define TEMP_TASKS_START_TASKS_START_Trigger (1UL) /*!< Trigger task */

/* Register: TEMP_TASKS_STOP */
/* Description: Stop temperature measurement */

/* Bit 0 : Stop temperature measurement */
#define TEMP_TASKS_STOP_TASKS_STOP_Pos (0UL) /*!< Position of TASKS_STOP field. */
#define TEMP_TASKS_STOP_TASKS_STOP_Msk (0x1UL << TEMP_TASKS_STOP_TASKS_STOP_Pos) /*!< Bit mask of TASKS_STOP field. */
#define TEMP_TASKS_STOP_TASKS_STOP_Trigger (1UL) /*!< Trigger task */

/* Register: TEMP_SUBSCRIBE_START */
/* Description: Subscribe configuration for task START */

/* Bit 31 :   */
#define TEMP_SUBSCRIBE_START_EN_Pos (31UL) /*!< Position of EN field. */
#define TEMP_SUBSCRIBE_START_EN_Msk (0x1UL << TEMP_SUBSCRIBE_START_EN_Pos) /*!< Bit mask of EN field. */
#define TEMP_SUBSCRIBE_START_EN_Disabled (0UL) /*!< Disable subscription */
#define TEMP_SUBSCRIBE_START_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task START will subscribe to */
#define TEMP_SUBSCRIBE_START_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TEMP_SUBSCRIBE_START_CHIDX_Msk (0xFFUL << TEMP_SUBSCRIBE_START_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TEMP_SUBSCRIBE_STOP */
/* Description: Subscribe configuration for task STOP */

/* Bit 31 :   */
#define TEMP_SUBSCRIBE_STOP_EN_Pos (31UL) /*!< Position of EN field. */
#define TEMP_SUBSCRIBE_STOP_EN_Msk (0x1UL << TEMP_SUBSCRIBE_STOP_EN_Pos) /*!< Bit mask of EN field. */
#define TEMP_SUBSCRIBE_STOP_EN_Disabled (0UL) /*!< Disable subscription */
#define TEMP_SUBSCRIBE_STOP_EN_Enabled (1UL) /*!< Enable subscription */

/* Bits 7..0 : DPPI channel that task STOP will subscribe to */
#define TEMP_SUBSCRIBE_STOP_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TEMP_SUBSCRIBE_STOP_CHIDX_Msk (0xFFUL << TEMP_SUBSCRIBE_STOP_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TEMP_EVENTS_DATARDY */
/* Description: Temperature measurement complete, data ready */

/* Bit 0 : Temperature measurement complete, data ready */
#define TEMP_EVENTS_DATARDY_EVENTS_DATARDY_Pos (0UL) /*!< Position of EVENTS_DATARDY field. */
#define TEMP_EVENTS_DATARDY_EVENTS_DATARDY_Msk (0x1UL << TEMP_EVENTS_DATARDY_EVENTS_DATARDY_Pos) /*!< Bit mask of EVENTS_DATARDY field. */
#define TEMP_EVENTS_DATARDY_EVENTS_DATARDY_NotGenerated (0UL) /*!< Event not generated */
#define TEMP_EVENTS_DATARDY_EVENTS_DATARDY_Generated (1UL) /*!< Event generated */

/* Register: TEMP_PUBLISH_DATARDY */
/* Description: Publish configuration for event DATARDY */

/* Bit 31 :   */
#define TEMP_PUBLISH_DATARDY_EN_Pos (31UL) /*!< Position of EN field. */
#define TEMP_PUBLISH_DATARDY_EN_Msk (0x1UL << TEMP_PUBLISH_DATARDY_EN_Pos) /*!< Bit mask of EN field. */
#define TEMP_PUBLISH_DATARDY_EN_Disabled (0UL) /*!< Disable publishing */
#define TEMP_PUBLISH_DATARDY_EN_Enabled (1UL) /*!< Enable publishing */

/* Bits 7..0 : DPPI channel that event DATARDY will publish to. */
#define TEMP_PUBLISH_DATARDY_CHIDX_Pos (0UL) /*!< Position of CHIDX field. */
#define TEMP_PUBLISH_DATARDY_CHIDX_Msk (0xFFUL << TEMP_PUBLISH_DATARDY_CHIDX_Pos) /*!< Bit mask of CHIDX field. */

/* Register: TEMP_INTENSET */
/* Description: Enable interrupt */

/* Bit 0 : Write '1' to enable interrupt for event DATARDY */
#define TEMP_INTENSET_DATARDY_Pos (0UL) /*!< Position of DATARDY field. */
#define TEMP_INTENSET_DATARDY_Msk (0x1UL << TEMP_INTENSET_DATARDY_Pos) /*!< Bit mask of DATARDY field. */
#define TEMP_INTENSET_DATARDY_Disabled (0UL) /*!< Read: Disabled */
#define TEMP_INTENSET_DATARDY_Enabled (1UL) /*!< Read: Enabled */
#define TEMP_INTENSET_DATARDY_Set (1UL) /*!< Enable */

/* Register: TEMP_INTENCLR */
/* Description: Disable interrupt */

/* Bit 0 : Write '1' to disable interrupt for event DATARDY */
#define TEMP_INTENCLR_DATARDY_Pos (0UL) /*!< Position of DATARDY field. */
#define TEMP_INTENCLR_DATARDY_Msk (0x1UL << TEMP_INTENCLR_DATARDY_Pos) /*!< Bit mask of DATARDY field. */
#define TEMP_INTENCLR_DATARDY_Disabled (0UL) /*!< Read: Disabled */
#define TEMP_INTENCLR_DATARDY_Enabled (1UL) /*!< Read: Enabled */
#define TEMP_INTENCLR_DATARDY_Clear (1UL) /*!< Disable */

/* Register: TEMP_TEMP */
/* Description: Temperature in degC (0.25deg steps) */

/* Bits 31..0 : Temperature in degC (0.25deg steps) */
#define TEMP_TEMP_TEMP_Pos (0UL) /*!< Position of TEMP field. */
#define TEMP_TEMP_TEMP_Msk (0xFFFFFFFFUL << TEMP_TEMP_TEMP_Pos) /*!< Bit mask of TEMP field. */

/* Register: TEMP_A0 */
/* Description: Slope of first piecewise linear function */

/* Bits 11..0 : Slope of first piecewise linear function */
#define TEMP_A0_A0_Pos (0UL) /*!< Position of A0 field. */
#define TEMP_A0_A0_Msk (0xFFFUL << TEMP_A0_A0_Pos) /*!< Bit mask of A0 field. */

/* Register: TEMP_A1 */
/* Description: Slope of second piecewise linear function */

/* Bits 11..0 : Slope of second piecewise linear function */
#define TEMP_A1_A1_Pos (0UL) /*!< Position of A1 field. */
#define TEMP_A1_A1_Msk (0xFFFUL << TEMP_A1_A1_Pos) /*!< Bit mask of A1 field. */

/* Register: TEMP_A2 */
/* Description: Slope of third piecewise linear function */

/* Bits 11..0 : Slope of third piecewise linear function */
#define TEMP_A2_A2_Pos (0UL) /*!< Position of A2 field. */
#define TEMP_A2_A2_Msk (0xFFFUL << TEMP_A2_A2_Pos) /*!< Bit mask of A2 field. */

/* Register: TEMP_A3 */
/* Description: Slope of fourth piecewise linear function */

/* Bits 11..0 : Slope of fourth piecewise linear function */
#define TEMP_A3_A3_Pos (0UL) /*!< Position of A3 field. */
#define TEMP_A3_A3_Msk (0xFFFUL << TEMP_A3_A3_Pos) /*!< Bit mask of A3 field. */

/* Register: TEMP_A4 */
/* Description: Slope of fifth piecewise linear function */

/* Bits 11..0 : Slope of fifth piecewise linear function */
#define TEMP_A4_A4_Pos (0UL) /*!< Position of A4 field. */
#define TEMP_A4_A4_Msk (0xFFFUL << TEMP_A4_A4_Pos) /*!< Bit mask of A4 field. */

/* Register: TEMP_A5 */
/* Description: Slope of sixth piecewise linear function */

/* Bits 11..0 : Slope of sixth piecewise linear function */
#define TEMP_A5_A5_Pos (0UL) /*!< Position of A5 field. */
#define TEMP_A5_A5_Msk (0xFFFUL << TEMP_A5_A5_Pos) /*!< Bit mask of A5 field. */

/* Register: TEMP_B0 */
/* Description: y-intercept of first piecewise linear function */

/* Bits 11..0 : y-intercept of first piecewise linear function */
#define TEMP_B0_B0_Pos (0UL) /*!< Position of B0 field. */
#define TEMP_B0_B0_Msk (0xFFFUL << TEMP_B0_B0_Pos) /*!< Bit mask of B0 field. */

/* Register: TEMP_B1 */
/* Description: y-intercept of second piecewise linear function */

/* Bits 11..0 : y-intercept of second piecewise linear function */
#define TEMP_B1_B1_Pos (0UL) /*!< Position of B1 field. */
#define TEMP_B1_B1_Msk (0xFFFUL << TEMP_B1_B1_Pos) /*!< Bit mask of B1 field. */

/* Register: TEMP_B2 */
/* Description: y-intercept of third piecewise linear function */

/* Bits 11..0 : y-intercept of third piecewise linear function */
#define TEMP_B2_B2_Pos (0UL) /*!< Position of B2 field. */
#define TEMP_B2_B2_Msk (0xFFFUL << TEMP_B2_B2_Pos) /*!< Bit mask of B2 field. */

/* Register: TEMP_B3 */
/* Description: y-intercept of fourth piecewise linear function */

/* Bits 11..0 : y-intercept of fourth piecewise linear function */
#define TEMP_B3_B3_Pos (0UL) /*!< Position of B3 field. */
#define TEMP_B3_B3_Msk (0xFFFUL << TEMP_B3_B3_Pos) /*!< Bit mask of B3 field. */

/* Register: TEMP_B4 */
/* Description: y-intercept of fifth piecewise linear function */

/* Bits 11..0 : y-intercept of fifth piecewise linear function */
#define TEMP_B4_B4_Pos (0UL) /*!< Position of B4 field. */
#define TEMP_B4_B4_Msk (0xFFFUL << TEMP_B4_B4_Pos) /*!< Bit mask of B4 field. */

/* Register: TEMP_B5 */
/* Description: y-intercept of sixth piecewise linear function */

/* Bits 11..0 : y-intercept of sixth piecewise linear function */
#define TEMP_B5_B5_Pos (0UL) /*!< Position of B5 field. */
#define TEMP_B5_B5_Msk (0xFFFUL << TEMP_B5_B5_Pos) /*!< Bit mask of B5 field. */

/* Register: TEMP_T0 */
/* Description: Endpoint of first piecewise linear function */

/* Bits 7..0 : Endpoint of first piecewise linear function */
#define TEMP_T0_T0_Pos (0UL) /*!< Position of T0 field. */
#define TEMP_T0_T0_Msk (0xFFUL << TEMP_T0_T0_Pos) /*!< Bit mask of T0 field. */

/* Register: TEMP_T1 */
/* Description: Endpoint of second piecewise linear function */

/* Bits 7..0 : Endpoint of second piecewise linear function */
#define TEMP_T1_T1_Pos (0UL) /*!< Position of T1 field. */
#define TEMP_T1_T1_Msk (0xFFUL << TEMP_T1_T1_Pos) /*!< Bit mask of T1 field. */

/* Register: TEMP_T2 */
/* Description: Endpoint of third piecewise linear function */

/* Bits 7..0 : Endpoint of third piecewise linear function */
#define TEMP_T2_T2_Pos (0UL) /*!< Position of T2 field. */
#define TEMP_T2_T2_Msk (0xFFUL << TEMP_T2_T2_Pos) /*!< Bit mask of T2 field. */

/* Register: TEMP_T3 */
/* Description: Endpoint of fourth piecewise linear function */

/* Bits 7..0 : Endpoint of fourth piecewise linear function */
#define TEMP_T3_T3_Pos (0UL) /*!< Position of T3 field. */
#define TEMP_T3_T3_Msk (0xFFUL << TEMP_T3_T3_Pos) /*!< Bit mask of T3 field. */

/* Register: TEMP_T4 */
/* Description: Endpoint of fifth piecewise linear function */

/* Bits 7..0 : Endpoint of fifth piecewise linear function */
#define TEMP_T4_T4_Pos (0UL) /*!< Position of T4 field. */
#define TEMP_T4_T4_Msk (0xFFUL << TEMP_T4_T4_Pos) /*!< Bit mask of T4 field. */



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


/* =========================================================================================================================== */
/* ================                                           UART0                                           ================ */
/* =========================================================================================================================== */

/**
  * @brief Universal Asynchronous Receiver/Transmitter (UART0)
  */

typedef struct {                                /*!< (@ 0x40002000) UART0 Structure                                            */
  __OM  uint32_t  TASKS_STARTRX;                /*!< (@ 0x00000000) Start UART receiver                                        */
  __OM  uint32_t  TASKS_STOPRX;                 /*!< (@ 0x00000004) Stop UART receiver                                         */
  __OM  uint32_t  TASKS_STARTTX;                /*!< (@ 0x00000008) Start UART transmitter                                     */
  __OM  uint32_t  TASKS_STOPTX;                 /*!< (@ 0x0000000C) Stop UART transmitter                                      */
  __IM  uint32_t  RESERVED[3];
  __OM  uint32_t  TASKS_SUSPEND;                /*!< (@ 0x0000001C) Suspend UART                                               */
  __IM  uint32_t  RESERVED1[56];
  __IOM uint32_t  EVENTS_CTS;                   /*!< (@ 0x00000100) CTS is activated (set low). Clear To Send.                 */
  __IOM uint32_t  EVENTS_NCTS;                  /*!< (@ 0x00000104) CTS is deactivated (set high). Not Clear To Send.          */
  __IOM uint32_t  EVENTS_RXDRDY;                /*!< (@ 0x00000108) Data received in RXD                                       */
  __IM  uint32_t  RESERVED2[4];
  __IOM uint32_t  EVENTS_TXDRDY;                /*!< (@ 0x0000011C) Data sent from TXD                                         */
  __IM  uint32_t  RESERVED3;
  __IOM uint32_t  EVENTS_ERROR;                 /*!< (@ 0x00000124) Error detected                                             */
  __IM  uint32_t  RESERVED4[7];
  __IOM uint32_t  EVENTS_RXTO;                  /*!< (@ 0x00000144) Receiver timeout                                           */
  __IM  uint32_t  RESERVED5[46];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED6[64];
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED7[93];
  __IOM uint32_t  ERRORSRC;                     /*!< (@ 0x00000480) Error source                                               */
  __IM  uint32_t  RESERVED8[31];
  __IOM uint32_t  ENABLE;                       /*!< (@ 0x00000500) Enable UART                                                */
  __IM  uint32_t  RESERVED9;
  __IOM UART_PSEL_Type PSEL;                    /*!< (@ 0x00000508) Unspecified                                                */
  __IM  uint32_t  RXD;                          /*!< (@ 0x00000518) RXD register                                               */
  __OM  uint32_t  TXD;                          /*!< (@ 0x0000051C) TXD register                                               */
  __IM  uint32_t  RESERVED10;
  __IOM uint32_t  BAUDRATE;                     /*!< (@ 0x00000524) Baud rate. Accuracy depends on the HFCLK source
                                                                    selected.                                                  */
  __IM  uint32_t  RESERVED11[17];
  __IOM uint32_t  CONFIG;                       /*!< (@ 0x0000056C) Configuration of parity and hardware flow control          */
} NRF_UART_Type;                                /*!< Size = 1392 (0x570)                                                       */


/* Peripheral: UART */
/* Description: Universal Asynchronous Receiver/Transmitter */

/* Register: UART_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 4 : Shortcut between event NCTS and task STOPRX */
#define UART_SHORTS_NCTS_STOPRX_Pos (4UL) /*!< Position of NCTS_STOPRX field. */
#define UART_SHORTS_NCTS_STOPRX_Msk (0x1UL << UART_SHORTS_NCTS_STOPRX_Pos) /*!< Bit mask of NCTS_STOPRX field. */
#define UART_SHORTS_NCTS_STOPRX_Disabled (0UL) /*!< Disable shortcut */
#define UART_SHORTS_NCTS_STOPRX_Enabled (1UL) /*!< Enable shortcut */

/* Bit 3 : Shortcut between event CTS and task STARTRX */
#define UART_SHORTS_CTS_STARTRX_Pos (3UL) /*!< Position of CTS_STARTRX field. */
#define UART_SHORTS_CTS_STARTRX_Msk (0x1UL << UART_SHORTS_CTS_STARTRX_Pos) /*!< Bit mask of CTS_STARTRX field. */
#define UART_SHORTS_CTS_STARTRX_Disabled (0UL) /*!< Disable shortcut */
#define UART_SHORTS_CTS_STARTRX_Enabled (1UL) /*!< Enable shortcut */

/* Register: UART_ERRORSRC */
/* Description: Error source */

/* Bit 3 : Break condition */
#define UART_ERRORSRC_BREAK_Pos (3UL) /*!< Position of BREAK field. */
#define UART_ERRORSRC_BREAK_Msk (0x1UL << UART_ERRORSRC_BREAK_Pos) /*!< Bit mask of BREAK field. */
#define UART_ERRORSRC_BREAK_NotPresent (0UL) /*!< Read: error not present */
#define UART_ERRORSRC_BREAK_Present (1UL) /*!< Read: error present */

/* Bit 2 : Framing error occurred */
#define UART_ERRORSRC_FRAMING_Pos (2UL) /*!< Position of FRAMING field. */
#define UART_ERRORSRC_FRAMING_Msk (0x1UL << UART_ERRORSRC_FRAMING_Pos) /*!< Bit mask of FRAMING field. */
#define UART_ERRORSRC_FRAMING_NotPresent (0UL) /*!< Read: error not present */
#define UART_ERRORSRC_FRAMING_Present (1UL) /*!< Read: error present */

/* Bit 1 : Parity error */
#define UART_ERRORSRC_PARITY_Pos (1UL) /*!< Position of PARITY field. */
#define UART_ERRORSRC_PARITY_Msk (0x1UL << UART_ERRORSRC_PARITY_Pos) /*!< Bit mask of PARITY field. */
#define UART_ERRORSRC_PARITY_NotPresent (0UL) /*!< Read: error not present */
#define UART_ERRORSRC_PARITY_Present (1UL) /*!< Read: error present */

/* Bit 0 : Overrun error */
#define UART_ERRORSRC_OVERRUN_Pos (0UL) /*!< Position of OVERRUN field. */
#define UART_ERRORSRC_OVERRUN_Msk (0x1UL << UART_ERRORSRC_OVERRUN_Pos) /*!< Bit mask of OVERRUN field. */
#define UART_ERRORSRC_OVERRUN_NotPresent (0UL) /*!< Read: error not present */
#define UART_ERRORSRC_OVERRUN_Present (1UL) /*!< Read: error present */

/* Register: UART_ENABLE */
/* Description: Enable UART */

/* Bits 3..0 : Enable or disable UART */
#define UART_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
#define UART_ENABLE_ENABLE_Msk (0xFUL << UART_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
#define UART_ENABLE_ENABLE_Disabled (0UL) /*!< Disable UART */
#define UART_ENABLE_ENABLE_Enabled (4UL) /*!< Enable UART */

/* Register: UART_PSEL_RTS */
/* Description: Pin select for RTS */

/* Bit 31 : Connection */
#define UART_PSEL_RTS_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UART_PSEL_RTS_CONNECT_Msk (0x1UL << UART_PSEL_RTS_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UART_PSEL_RTS_CONNECT_Connected (0UL) /*!< Connect */
#define UART_PSEL_RTS_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UART_PSEL_RTS_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UART_PSEL_RTS_PORT_Msk (0x1UL << UART_PSEL_RTS_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UART_PSEL_RTS_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UART_PSEL_RTS_PIN_Msk (0x1FUL << UART_PSEL_RTS_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UART_PSEL_TXD */
/* Description: Pin select for TXD */

/* Bit 31 : Connection */
#define UART_PSEL_TXD_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UART_PSEL_TXD_CONNECT_Msk (0x1UL << UART_PSEL_TXD_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UART_PSEL_TXD_CONNECT_Connected (0UL) /*!< Connect */
#define UART_PSEL_TXD_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UART_PSEL_TXD_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UART_PSEL_TXD_PORT_Msk (0x1UL << UART_PSEL_TXD_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UART_PSEL_TXD_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UART_PSEL_TXD_PIN_Msk (0x1FUL << UART_PSEL_TXD_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UART_PSEL_CTS */
/* Description: Pin select for CTS */

/* Bit 31 : Connection */
#define UART_PSEL_CTS_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UART_PSEL_CTS_CONNECT_Msk (0x1UL << UART_PSEL_CTS_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UART_PSEL_CTS_CONNECT_Connected (0UL) /*!< Connect */
#define UART_PSEL_CTS_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UART_PSEL_CTS_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UART_PSEL_CTS_PORT_Msk (0x1UL << UART_PSEL_CTS_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UART_PSEL_CTS_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UART_PSEL_CTS_PIN_Msk (0x1FUL << UART_PSEL_CTS_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UART_PSEL_RXD */
/* Description: Pin select for RXD */

/* Bit 31 : Connection */
#define UART_PSEL_RXD_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UART_PSEL_RXD_CONNECT_Msk (0x1UL << UART_PSEL_RXD_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UART_PSEL_RXD_CONNECT_Connected (0UL) /*!< Connect */
#define UART_PSEL_RXD_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UART_PSEL_RXD_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UART_PSEL_RXD_PORT_Msk (0x1UL << UART_PSEL_RXD_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UART_PSEL_RXD_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UART_PSEL_RXD_PIN_Msk (0x1FUL << UART_PSEL_RXD_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UART_RXD */
/* Description: RXD register */

/* Bits 7..0 : RX data received in previous transfers, double buffered */
#define UART_RXD_RXD_Pos (0UL) /*!< Position of RXD field. */
#define UART_RXD_RXD_Msk (0xFFUL << UART_RXD_RXD_Pos) /*!< Bit mask of RXD field. */

/* Register: UART_TXD */
/* Description: TXD register */

/* Bits 7..0 : TX data to be transferred */
#define UART_TXD_TXD_Pos (0UL) /*!< Position of TXD field. */
#define UART_TXD_TXD_Msk (0xFFUL << UART_TXD_TXD_Pos) /*!< Bit mask of TXD field. */

/* Register: UART_BAUDRATE */
/* Description: Baud rate. Accuracy depends on the HFCLK source selected. */

/* Bits 31..0 : Baud rate */
#define UART_BAUDRATE_BAUDRATE_Pos (0UL) /*!< Position of BAUDRATE field. */
#define UART_BAUDRATE_BAUDRATE_Msk (0xFFFFFFFFUL << UART_BAUDRATE_BAUDRATE_Pos) /*!< Bit mask of BAUDRATE field. */
#define UART_BAUDRATE_BAUDRATE_Baud1200 (0x0004F000UL) /*!< 1200 baud (actual rate: 1205) */
#define UART_BAUDRATE_BAUDRATE_Baud2400 (0x0009D000UL) /*!< 2400 baud (actual rate: 2396) */
#define UART_BAUDRATE_BAUDRATE_Baud4800 (0x0013B000UL) /*!< 4800 baud (actual rate: 4808) */
#define UART_BAUDRATE_BAUDRATE_Baud9600 (0x00275000UL) /*!< 9600 baud (actual rate: 9598) */
#define UART_BAUDRATE_BAUDRATE_Baud14400 (0x003B0000UL) /*!< 14400 baud (actual rate: 14414) */
#define UART_BAUDRATE_BAUDRATE_Baud19200 (0x004EA000UL) /*!< 19200 baud (actual rate: 19208) */
#define UART_BAUDRATE_BAUDRATE_Baud28800 (0x0075F000UL) /*!< 28800 baud (actual rate: 28829) */
#define UART_BAUDRATE_BAUDRATE_Baud31250 (0x00800000UL) /*!< 31250 baud */
#define UART_BAUDRATE_BAUDRATE_Baud38400 (0x009D5000UL) /*!< 38400 baud (actual rate: 38462) */
#define UART_BAUDRATE_BAUDRATE_Baud56000 (0x00E50000UL) /*!< 56000 baud (actual rate: 55944) */
#define UART_BAUDRATE_BAUDRATE_Baud57600 (0x00EBF000UL) /*!< 57600 baud (actual rate: 57762) */
#define UART_BAUDRATE_BAUDRATE_Baud76800 (0x013A9000UL) /*!< 76800 baud (actual rate: 76923) */
#define UART_BAUDRATE_BAUDRATE_Baud115200 (0x01D7E000UL) /*!< 115200 baud (actual rate: 115942) */
#define UART_BAUDRATE_BAUDRATE_Baud230400 (0x03AFB000UL) /*!< 230400 baud (actual rate: 231884) */
#define UART_BAUDRATE_BAUDRATE_Baud250000 (0x04000000UL) /*!< 250000 baud */
#define UART_BAUDRATE_BAUDRATE_Baud460800 (0x075F7000UL) /*!< 460800 baud (actual rate: 470588) */
#define UART_BAUDRATE_BAUDRATE_Baud921600 (0x0EBED000UL) /*!< 921600 baud (actual rate: 941176) */
#define UART_BAUDRATE_BAUDRATE_Baud1M (0x10000000UL) /*!< 1Mega baud */

/* Register: UART_CONFIG */
/* Description: Configuration of parity and hardware flow control */

/* Bit 8 : Even or odd parity type */
#define UART_CONFIG_PARITYTYPE_Pos (8UL) /*!< Position of PARITYTYPE field. */
#define UART_CONFIG_PARITYTYPE_Msk (0x1UL << UART_CONFIG_PARITYTYPE_Pos) /*!< Bit mask of PARITYTYPE field. */
#define UART_CONFIG_PARITYTYPE_Even (0UL) /*!< Even parity */
#define UART_CONFIG_PARITYTYPE_Odd (1UL) /*!< Odd parity */

/* Bit 4 : Stop bits */
#define UART_CONFIG_STOP_Pos (4UL) /*!< Position of STOP field. */
#define UART_CONFIG_STOP_Msk (0x1UL << UART_CONFIG_STOP_Pos) /*!< Bit mask of STOP field. */
#define UART_CONFIG_STOP_One (0UL) /*!< One stop bit */
#define UART_CONFIG_STOP_Two (1UL) /*!< Two stop bits */

/* Bits 3..1 : Parity */
#define UART_CONFIG_PARITY_Pos (1UL) /*!< Position of PARITY field. */
#define UART_CONFIG_PARITY_Msk (0x7UL << UART_CONFIG_PARITY_Pos) /*!< Bit mask of PARITY field. */
#define UART_CONFIG_PARITY_Excluded (0x0UL) /*!< Exclude parity bit */
#define UART_CONFIG_PARITY_Included (0x7UL) /*!< Include parity bit */

/* Bit 0 : Hardware flow control */
#define UART_CONFIG_HWFC_Pos (0UL) /*!< Position of HWFC field. */
#define UART_CONFIG_HWFC_Msk (0x1UL << UART_CONFIG_HWFC_Pos) /*!< Bit mask of HWFC field. */
#define UART_CONFIG_HWFC_Disabled (0UL) /*!< Disabled */
#define UART_CONFIG_HWFC_Enabled (1UL) /*!< Enabled */


/* =========================================================================================================================== */
/* ================                                         UARTE0_NS                                         ================ */
/* =========================================================================================================================== */


/**
  * @brief UART with EasyDMA 0 (UARTE0_NS)
  */

typedef struct {                                /*!< (@ 0x40008000) UARTE0_NS Structure                                        */
  __OM  uint32_t  TASKS_STARTRX;                /*!< (@ 0x00000000) Start UART receiver                                        */
  __OM  uint32_t  TASKS_STOPRX;                 /*!< (@ 0x00000004) Stop UART receiver                                         */
  __OM  uint32_t  TASKS_STARTTX;                /*!< (@ 0x00000008) Start UART transmitter                                     */
  __OM  uint32_t  TASKS_STOPTX;                 /*!< (@ 0x0000000C) Stop UART transmitter                                      */
  __IM  uint32_t  RESERVED[7];
  __OM  uint32_t  TASKS_FLUSHRX;                /*!< (@ 0x0000002C) Flush RX FIFO into RX buffer                               */
  __IM  uint32_t  RESERVED1[20];
  __IOM uint32_t  SUBSCRIBE_STARTRX;            /*!< (@ 0x00000080) Subscribe configuration for task STARTRX                   */
  __IOM uint32_t  SUBSCRIBE_STOPRX;             /*!< (@ 0x00000084) Subscribe configuration for task STOPRX                    */
  __IOM uint32_t  SUBSCRIBE_STARTTX;            /*!< (@ 0x00000088) Subscribe configuration for task STARTTX                   */
  __IOM uint32_t  SUBSCRIBE_STOPTX;             /*!< (@ 0x0000008C) Subscribe configuration for task STOPTX                    */
  __IM  uint32_t  RESERVED2[7];
  __IOM uint32_t  SUBSCRIBE_FLUSHRX;            /*!< (@ 0x000000AC) Subscribe configuration for task FLUSHRX                   */
  __IM  uint32_t  RESERVED3[20];
  __IOM uint32_t  EVENTS_CTS;                   /*!< (@ 0x00000100) CTS is activated (set low). Clear To Send.                 */
  __IOM uint32_t  EVENTS_NCTS;                  /*!< (@ 0x00000104) CTS is deactivated (set high). Not Clear To Send.          */
  __IOM uint32_t  EVENTS_RXDRDY;                /*!< (@ 0x00000108) Data received in RXD (but potentially not yet
                                                                    transferred to Data RAM)                                   */
  __IM  uint32_t  RESERVED4;
  __IOM uint32_t  EVENTS_ENDRX;                 /*!< (@ 0x00000110) Receive buffer is filled up                                */
  __IM  uint32_t  RESERVED5[2];
  __IOM uint32_t  EVENTS_TXDRDY;                /*!< (@ 0x0000011C) Data sent from TXD                                         */
  __IOM uint32_t  EVENTS_ENDTX;                 /*!< (@ 0x00000120) Last TX byte transmitted                                   */
  __IOM uint32_t  EVENTS_ERROR;                 /*!< (@ 0x00000124) Error detected                                             */
  __IM  uint32_t  RESERVED6[7];
  __IOM uint32_t  EVENTS_RXTO;                  /*!< (@ 0x00000144) Receiver timeout                                           */
  __IM  uint32_t  RESERVED7;
  __IOM uint32_t  EVENTS_RXSTARTED;             /*!< (@ 0x0000014C) UART receiver has started                                  */
  __IOM uint32_t  EVENTS_TXSTARTED;             /*!< (@ 0x00000150) UART transmitter has started                               */
  __IM  uint32_t  RESERVED8;
  __IOM uint32_t  EVENTS_TXSTOPPED;             /*!< (@ 0x00000158) Transmitter stopped                                        */
  __IM  uint32_t  RESERVED9[9];
  __IOM uint32_t  PUBLISH_CTS;                  /*!< (@ 0x00000180) Publish configuration for event CTS                        */
  __IOM uint32_t  PUBLISH_NCTS;                 /*!< (@ 0x00000184) Publish configuration for event NCTS                       */
  __IOM uint32_t  PUBLISH_RXDRDY;               /*!< (@ 0x00000188) Publish configuration for event RXDRDY                     */
  __IM  uint32_t  RESERVED10;
  __IOM uint32_t  PUBLISH_ENDRX;                /*!< (@ 0x00000190) Publish configuration for event ENDRX                      */
  __IM  uint32_t  RESERVED11[2];
  __IOM uint32_t  PUBLISH_TXDRDY;               /*!< (@ 0x0000019C) Publish configuration for event TXDRDY                     */
  __IOM uint32_t  PUBLISH_ENDTX;                /*!< (@ 0x000001A0) Publish configuration for event ENDTX                      */
  __IOM uint32_t  PUBLISH_ERROR;                /*!< (@ 0x000001A4) Publish configuration for event ERROR                      */
  __IM  uint32_t  RESERVED12[7];
  __IOM uint32_t  PUBLISH_RXTO;                 /*!< (@ 0x000001C4) Publish configuration for event RXTO                       */
  __IM  uint32_t  RESERVED13;
  __IOM uint32_t  PUBLISH_RXSTARTED;            /*!< (@ 0x000001CC) Publish configuration for event RXSTARTED                  */
  __IOM uint32_t  PUBLISH_TXSTARTED;            /*!< (@ 0x000001D0) Publish configuration for event TXSTARTED                  */
  __IM  uint32_t  RESERVED14;
  __IOM uint32_t  PUBLISH_TXSTOPPED;            /*!< (@ 0x000001D8) Publish configuration for event TXSTOPPED                  */
  __IM  uint32_t  RESERVED15[9];
  __IOM uint32_t  SHORTS;                       /*!< (@ 0x00000200) Shortcuts between local events and tasks                   */
  __IM  uint32_t  RESERVED16[63];
  __IOM uint32_t  INTEN;                        /*!< (@ 0x00000300) Enable or disable interrupt                                */
  __IOM uint32_t  INTENSET;                     /*!< (@ 0x00000304) Enable interrupt                                           */
  __IOM uint32_t  INTENCLR;                     /*!< (@ 0x00000308) Disable interrupt                                          */
  __IM  uint32_t  RESERVED17[93];
  __IOM uint32_t  ERRORSRC;                     /*!< (@ 0x00000480) Error source                                               */
  __IM  uint32_t  RESERVED18[31];
  __IOM uint32_t  ENABLE;                       /*!< (@ 0x00000500) Enable UART                                                */
  __IM  uint32_t  RESERVED19;
  __IOM UARTE_PSEL_Type PSEL;                   /*!< (@ 0x00000508) Unspecified                                                */
  __IM  uint32_t  RESERVED20[3];
  __IOM uint32_t  BAUDRATE;                     /*!< (@ 0x00000524) Baud rate. Accuracy depends on the HFCLK source
                                                                    selected.                                                  */
  __IM  uint32_t  RESERVED21[3];
  __IOM UARTE_RXD_Type RXD;                     /*!< (@ 0x00000534) RXD EasyDMA channel                                        */
  __IM  uint32_t  RESERVED22;
  __IOM UARTE_TXD_Type TXD;                     /*!< (@ 0x00000544) TXD EasyDMA channel                                        */
  __IM  uint32_t  RESERVED23[7];
  __IOM uint32_t  CONFIG;                       /*!< (@ 0x0000056C) Configuration of parity and hardware flow control          */
} NRF_UARTE_Type;                               /*!< Size = 1392 (0x570)                                                       */


/* Peripheral: UARTE */
/* Description: UART with EasyDMA 0 */

/* Register: UARTE_SHORTS */
/* Description: Shortcuts between local events and tasks */

/* Bit 6 : Shortcut between event ENDRX and task STOPRX */
#define UARTE_SHORTS_ENDRX_STOPRX_Pos (6UL) /*!< Position of ENDRX_STOPRX field. */
#define UARTE_SHORTS_ENDRX_STOPRX_Msk (0x1UL << UARTE_SHORTS_ENDRX_STOPRX_Pos) /*!< Bit mask of ENDRX_STOPRX field. */
#define UARTE_SHORTS_ENDRX_STOPRX_Disabled (0UL) /*!< Disable shortcut */
#define UARTE_SHORTS_ENDRX_STOPRX_Enabled (1UL) /*!< Enable shortcut */

/* Bit 5 : Shortcut between event ENDRX and task STARTRX */
#define UARTE_SHORTS_ENDRX_STARTRX_Pos (5UL) /*!< Position of ENDRX_STARTRX field. */
#define UARTE_SHORTS_ENDRX_STARTRX_Msk (0x1UL << UARTE_SHORTS_ENDRX_STARTRX_Pos) /*!< Bit mask of ENDRX_STARTRX field. */
#define UARTE_SHORTS_ENDRX_STARTRX_Disabled (0UL) /*!< Disable shortcut */
#define UARTE_SHORTS_ENDRX_STARTRX_Enabled (1UL) /*!< Enable shortcut */

/* Register: UARTE_INTENSET */
/* Description: Enable interrupt */

/* Bit 22 : Write '1' to enable interrupt for event TXSTOPPED */
#define UARTE_INTENSET_TXSTOPPED_Pos (22UL) /*!< Position of TXSTOPPED field. */
#define UARTE_INTENSET_TXSTOPPED_Msk (0x1UL << UARTE_INTENSET_TXSTOPPED_Pos) /*!< Bit mask of TXSTOPPED field. */
#define UARTE_INTENSET_TXSTOPPED_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_TXSTOPPED_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_TXSTOPPED_Set (1UL) /*!< Enable */

/* Bit 20 : Write '1' to enable interrupt for event TXSTARTED */
#define UARTE_INTENSET_TXSTARTED_Pos (20UL) /*!< Position of TXSTARTED field. */
#define UARTE_INTENSET_TXSTARTED_Msk (0x1UL << UARTE_INTENSET_TXSTARTED_Pos) /*!< Bit mask of TXSTARTED field. */
#define UARTE_INTENSET_TXSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_TXSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_TXSTARTED_Set (1UL) /*!< Enable */

/* Bit 19 : Write '1' to enable interrupt for event RXSTARTED */
#define UARTE_INTENSET_RXSTARTED_Pos (19UL) /*!< Position of RXSTARTED field. */
#define UARTE_INTENSET_RXSTARTED_Msk (0x1UL << UARTE_INTENSET_RXSTARTED_Pos) /*!< Bit mask of RXSTARTED field. */
#define UARTE_INTENSET_RXSTARTED_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_RXSTARTED_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_RXSTARTED_Set (1UL) /*!< Enable */

/* Bit 17 : Write '1' to enable interrupt for event RXTO */
#define UARTE_INTENSET_RXTO_Pos (17UL) /*!< Position of RXTO field. */
#define UARTE_INTENSET_RXTO_Msk (0x1UL << UARTE_INTENSET_RXTO_Pos) /*!< Bit mask of RXTO field. */
#define UARTE_INTENSET_RXTO_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_RXTO_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_RXTO_Set (1UL) /*!< Enable */

/* Bit 9 : Write '1' to enable interrupt for event ERROR */
#define UARTE_INTENSET_ERROR_Pos (9UL) /*!< Position of ERROR field. */
#define UARTE_INTENSET_ERROR_Msk (0x1UL << UARTE_INTENSET_ERROR_Pos) /*!< Bit mask of ERROR field. */
#define UARTE_INTENSET_ERROR_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_ERROR_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_ERROR_Set (1UL) /*!< Enable */

/* Bit 8 : Write '1' to enable interrupt for event ENDTX */
#define UARTE_INTENSET_ENDTX_Pos (8UL) /*!< Position of ENDTX field. */
#define UARTE_INTENSET_ENDTX_Msk (0x1UL << UARTE_INTENSET_ENDTX_Pos) /*!< Bit mask of ENDTX field. */
#define UARTE_INTENSET_ENDTX_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_ENDTX_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_ENDTX_Set (1UL) /*!< Enable */

/* Bit 7 : Write '1' to enable interrupt for event TXDRDY */
#define UARTE_INTENSET_TXDRDY_Pos (7UL) /*!< Position of TXDRDY field. */
#define UARTE_INTENSET_TXDRDY_Msk (0x1UL << UARTE_INTENSET_TXDRDY_Pos) /*!< Bit mask of TXDRDY field. */
#define UARTE_INTENSET_TXDRDY_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_TXDRDY_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_TXDRDY_Set (1UL) /*!< Enable */

/* Bit 4 : Write '1' to enable interrupt for event ENDRX */
#define UARTE_INTENSET_ENDRX_Pos (4UL) /*!< Position of ENDRX field. */
#define UARTE_INTENSET_ENDRX_Msk (0x1UL << UARTE_INTENSET_ENDRX_Pos) /*!< Bit mask of ENDRX field. */
#define UARTE_INTENSET_ENDRX_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_ENDRX_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_ENDRX_Set (1UL) /*!< Enable */

/* Bit 2 : Write '1' to enable interrupt for event RXDRDY */
#define UARTE_INTENSET_RXDRDY_Pos (2UL) /*!< Position of RXDRDY field. */
#define UARTE_INTENSET_RXDRDY_Msk (0x1UL << UARTE_INTENSET_RXDRDY_Pos) /*!< Bit mask of RXDRDY field. */
#define UARTE_INTENSET_RXDRDY_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_RXDRDY_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_RXDRDY_Set (1UL) /*!< Enable */

/* Bit 1 : Write '1' to enable interrupt for event NCTS */
#define UARTE_INTENSET_NCTS_Pos (1UL) /*!< Position of NCTS field. */
#define UARTE_INTENSET_NCTS_Msk (0x1UL << UARTE_INTENSET_NCTS_Pos) /*!< Bit mask of NCTS field. */
#define UARTE_INTENSET_NCTS_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_NCTS_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_NCTS_Set (1UL) /*!< Enable */

/* Bit 0 : Write '1' to enable interrupt for event CTS */
#define UARTE_INTENSET_CTS_Pos (0UL) /*!< Position of CTS field. */
#define UARTE_INTENSET_CTS_Msk (0x1UL << UARTE_INTENSET_CTS_Pos) /*!< Bit mask of CTS field. */
#define UARTE_INTENSET_CTS_Disabled (0UL) /*!< Read: Disabled */
#define UARTE_INTENSET_CTS_Enabled (1UL) /*!< Read: Enabled */
#define UARTE_INTENSET_CTS_Set (1UL) /*!< Enable */


/* Register: UARTE_ERRORSRC */
/* Description: Error source This register is read/write one to clear. */

/* Bit 3 : Break condition */
#define UARTE_ERRORSRC_BREAK_Pos (3UL) /*!< Position of BREAK field. */
#define UARTE_ERRORSRC_BREAK_Msk (0x1UL << UARTE_ERRORSRC_BREAK_Pos) /*!< Bit mask of BREAK field. */
#define UARTE_ERRORSRC_BREAK_NotPresent (0UL) /*!< Read: error not present */
#define UARTE_ERRORSRC_BREAK_Present (1UL) /*!< Read: error present */

/* Bit 2 : Framing error occurred */
#define UARTE_ERRORSRC_FRAMING_Pos (2UL) /*!< Position of FRAMING field. */
#define UARTE_ERRORSRC_FRAMING_Msk (0x1UL << UARTE_ERRORSRC_FRAMING_Pos) /*!< Bit mask of FRAMING field. */
#define UARTE_ERRORSRC_FRAMING_NotPresent (0UL) /*!< Read: error not present */
#define UARTE_ERRORSRC_FRAMING_Present (1UL) /*!< Read: error present */

/* Bit 1 : Parity error */
#define UARTE_ERRORSRC_PARITY_Pos (1UL) /*!< Position of PARITY field. */
#define UARTE_ERRORSRC_PARITY_Msk (0x1UL << UARTE_ERRORSRC_PARITY_Pos) /*!< Bit mask of PARITY field. */
#define UARTE_ERRORSRC_PARITY_NotPresent (0UL) /*!< Read: error not present */
#define UARTE_ERRORSRC_PARITY_Present (1UL) /*!< Read: error present */

/* Bit 0 : Overrun error */
#define UARTE_ERRORSRC_OVERRUN_Pos (0UL) /*!< Position of OVERRUN field. */
#define UARTE_ERRORSRC_OVERRUN_Msk (0x1UL << UARTE_ERRORSRC_OVERRUN_Pos) /*!< Bit mask of OVERRUN field. */
#define UARTE_ERRORSRC_OVERRUN_NotPresent (0UL) /*!< Read: error not present */
#define UARTE_ERRORSRC_OVERRUN_Present (1UL) /*!< Read: error present */

/* Register: UARTE_ENABLE */
/* Description: Enable UART */

/* Bits 3..0 : Enable or disable UARTE */
#define UARTE_ENABLE_ENABLE_Pos (0UL) /*!< Position of ENABLE field. */
#define UARTE_ENABLE_ENABLE_Msk (0xFUL << UARTE_ENABLE_ENABLE_Pos) /*!< Bit mask of ENABLE field. */
#define UARTE_ENABLE_ENABLE_Disabled (0UL) /*!< Disable UARTE */
#define UARTE_ENABLE_ENABLE_Enabled (8UL) /*!< Enable UARTE */

/* Register: UARTE_PSEL_RTS */
/* Description: Pin select for RTS signal */

/* Bit 31 : Connection */
#define UARTE_PSEL_RTS_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UARTE_PSEL_RTS_CONNECT_Msk (0x1UL << UARTE_PSEL_RTS_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UARTE_PSEL_RTS_CONNECT_Connected (0UL) /*!< Connect */
#define UARTE_PSEL_RTS_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UARTE_PSEL_RTS_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UARTE_PSEL_RTS_PORT_Msk (0x1UL << UARTE_PSEL_RTS_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UARTE_PSEL_RTS_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UARTE_PSEL_RTS_PIN_Msk (0x1FUL << UARTE_PSEL_RTS_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UARTE_PSEL_TXD */
/* Description: Pin select for TXD signal */

/* Bit 31 : Connection */
#define UARTE_PSEL_TXD_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UARTE_PSEL_TXD_CONNECT_Msk (0x1UL << UARTE_PSEL_TXD_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UARTE_PSEL_TXD_CONNECT_Connected (0UL) /*!< Connect */
#define UARTE_PSEL_TXD_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UARTE_PSEL_TXD_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UARTE_PSEL_TXD_PORT_Msk (0x1UL << UARTE_PSEL_TXD_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UARTE_PSEL_TXD_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UARTE_PSEL_TXD_PIN_Msk (0x1FUL << UARTE_PSEL_TXD_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UARTE_PSEL_CTS */
/* Description: Pin select for CTS signal */

/* Bit 31 : Connection */
#define UARTE_PSEL_CTS_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UARTE_PSEL_CTS_CONNECT_Msk (0x1UL << UARTE_PSEL_CTS_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UARTE_PSEL_CTS_CONNECT_Connected (0UL) /*!< Connect */
#define UARTE_PSEL_CTS_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UARTE_PSEL_CTS_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UARTE_PSEL_CTS_PORT_Msk (0x1UL << UARTE_PSEL_CTS_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UARTE_PSEL_CTS_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UARTE_PSEL_CTS_PIN_Msk (0x1FUL << UARTE_PSEL_CTS_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UARTE_PSEL_RXD */
/* Description: Pin select for RXD signal */

/* Bit 31 : Connection */
#define UARTE_PSEL_RXD_CONNECT_Pos (31UL) /*!< Position of CONNECT field. */
#define UARTE_PSEL_RXD_CONNECT_Msk (0x1UL << UARTE_PSEL_RXD_CONNECT_Pos) /*!< Bit mask of CONNECT field. */
#define UARTE_PSEL_RXD_CONNECT_Connected (0UL) /*!< Connect */
#define UARTE_PSEL_RXD_CONNECT_Disconnected (1UL) /*!< Disconnect */

/* Bit 5 : Port number */
#define UARTE_PSEL_RXD_PORT_Pos (5UL) /*!< Position of PORT field. */
#define UARTE_PSEL_RXD_PORT_Msk (0x1UL << UARTE_PSEL_RXD_PORT_Pos) /*!< Bit mask of PORT field. */

/* Bits 4..0 : Pin number */
#define UARTE_PSEL_RXD_PIN_Pos (0UL) /*!< Position of PIN field. */
#define UARTE_PSEL_RXD_PIN_Msk (0x1FUL << UARTE_PSEL_RXD_PIN_Pos) /*!< Bit mask of PIN field. */

/* Register: UARTE_BAUDRATE */
/* Description: Baud rate. Accuracy depends on the HFCLK source selected. */

/* Bits 31..0 : Baud rate */
#define UARTE_BAUDRATE_BAUDRATE_Pos (0UL) /*!< Position of BAUDRATE field. */
#define UARTE_BAUDRATE_BAUDRATE_Msk (0xFFFFFFFFUL << UARTE_BAUDRATE_BAUDRATE_Pos) /*!< Bit mask of BAUDRATE field. */
#define UARTE_BAUDRATE_BAUDRATE_Baud1200 (0x0004F000UL) /*!< 1200 baud (actual rate: 1205) */
#define UARTE_BAUDRATE_BAUDRATE_Baud2400 (0x0009D000UL) /*!< 2400 baud (actual rate: 2396) */
#define UARTE_BAUDRATE_BAUDRATE_Baud4800 (0x0013B000UL) /*!< 4800 baud (actual rate: 4808) */
#define UARTE_BAUDRATE_BAUDRATE_Baud9600 (0x00275000UL) /*!< 9600 baud (actual rate: 9598) */
#define UARTE_BAUDRATE_BAUDRATE_Baud14400 (0x003AF000UL) /*!< 14400 baud (actual rate: 14401) */
#define UARTE_BAUDRATE_BAUDRATE_Baud19200 (0x004EA000UL) /*!< 19200 baud (actual rate: 19208) */
#define UARTE_BAUDRATE_BAUDRATE_Baud28800 (0x0075C000UL) /*!< 28800 baud (actual rate: 28777) */
#define UARTE_BAUDRATE_BAUDRATE_Baud31250 (0x00800000UL) /*!< 31250 baud */
#define UARTE_BAUDRATE_BAUDRATE_Baud38400 (0x009D0000UL) /*!< 38400 baud (actual rate: 38369) */
#define UARTE_BAUDRATE_BAUDRATE_Baud56000 (0x00E50000UL) /*!< 56000 baud (actual rate: 55944) */
#define UARTE_BAUDRATE_BAUDRATE_Baud57600 (0x00EB0000UL) /*!< 57600 baud (actual rate: 57554) */
#define UARTE_BAUDRATE_BAUDRATE_Baud76800 (0x013A9000UL) /*!< 76800 baud (actual rate: 76923) */
#define UARTE_BAUDRATE_BAUDRATE_Baud115200 (0x01D60000UL) /*!< 115200 baud (actual rate: 115108) */
#define UARTE_BAUDRATE_BAUDRATE_Baud230400 (0x03B00000UL) /*!< 230400 baud (actual rate: 231884) */
#define UARTE_BAUDRATE_BAUDRATE_Baud250000 (0x04000000UL) /*!< 250000 baud */
#define UARTE_BAUDRATE_BAUDRATE_Baud460800 (0x07400000UL) /*!< 460800 baud (actual rate: 457143) */
#define UARTE_BAUDRATE_BAUDRATE_Baud921600 (0x0F000000UL) /*!< 921600 baud (actual rate: 941176) */
#define UARTE_BAUDRATE_BAUDRATE_Baud1M (0x10000000UL) /*!< 1 megabaud */

/* Register: UARTE_RXD_PTR */
/* Description: Data pointer */

/* Bits 31..0 : Data pointer */
#define UARTE_RXD_PTR_PTR_Pos (0UL) /*!< Position of PTR field. */
#define UARTE_RXD_PTR_PTR_Msk (0xFFFFFFFFUL << UARTE_RXD_PTR_PTR_Pos) /*!< Bit mask of PTR field. */

/* Register: UARTE_RXD_MAXCNT */
/* Description: Maximum number of bytes in receive buffer */

/* Bits 15..0 : Maximum number of bytes in receive buffer */
#define UARTE_RXD_MAXCNT_MAXCNT_Pos (0UL) /*!< Position of MAXCNT field. */
#define UARTE_RXD_MAXCNT_MAXCNT_Msk (0xFFFFUL << UARTE_RXD_MAXCNT_MAXCNT_Pos) /*!< Bit mask of MAXCNT field. */

/* Register: UARTE_RXD_AMOUNT */
/* Description: Number of bytes transferred in the last transaction */

/* Bits 15..0 : Number of bytes transferred in the last transaction */
#define UARTE_RXD_AMOUNT_AMOUNT_Pos (0UL) /*!< Position of AMOUNT field. */
#define UARTE_RXD_AMOUNT_AMOUNT_Msk (0xFFFFUL << UARTE_RXD_AMOUNT_AMOUNT_Pos) /*!< Bit mask of AMOUNT field. */

/* Register: UARTE_TXD_PTR */
/* Description: Data pointer */

/* Bits 31..0 : Data pointer */
#define UARTE_TXD_PTR_PTR_Pos (0UL) /*!< Position of PTR field. */
#define UARTE_TXD_PTR_PTR_Msk (0xFFFFFFFFUL << UARTE_TXD_PTR_PTR_Pos) /*!< Bit mask of PTR field. */

/* Register: UARTE_TXD_MAXCNT */
/* Description: Maximum number of bytes in transmit buffer */

/* Bits 15..0 : Maximum number of bytes in transmit buffer */
#define UARTE_TXD_MAXCNT_MAXCNT_Pos (0UL) /*!< Position of MAXCNT field. */
#define UARTE_TXD_MAXCNT_MAXCNT_Msk (0xFFFFUL << UARTE_TXD_MAXCNT_MAXCNT_Pos) /*!< Bit mask of MAXCNT field. */

/* Register: UARTE_TXD_AMOUNT */
/* Description: Number of bytes transferred in the last transaction */

/* Bits 15..0 : Number of bytes transferred in the last transaction */
#define UARTE_TXD_AMOUNT_AMOUNT_Pos (0UL) /*!< Position of AMOUNT field. */
#define UARTE_TXD_AMOUNT_AMOUNT_Msk (0xFFFFUL << UARTE_TXD_AMOUNT_AMOUNT_Pos) /*!< Bit mask of AMOUNT field. */

/* Register: UARTE_CONFIG */
/* Description: Configuration of parity and hardware flow control */

/* Bit 8 : Even or odd parity type */
#define UARTE_CONFIG_PARITYTYPE_Pos (8UL) /*!< Position of PARITYTYPE field. */
#define UARTE_CONFIG_PARITYTYPE_Msk (0x1UL << UARTE_CONFIG_PARITYTYPE_Pos) /*!< Bit mask of PARITYTYPE field. */
#define UARTE_CONFIG_PARITYTYPE_Even (0UL) /*!< Even parity */
#define UARTE_CONFIG_PARITYTYPE_Odd (1UL) /*!< Odd parity */

/* Bit 4 : Stop bits */
#define UARTE_CONFIG_STOP_Pos (4UL) /*!< Position of STOP field. */
#define UARTE_CONFIG_STOP_Msk (0x1UL << UARTE_CONFIG_STOP_Pos) /*!< Bit mask of STOP field. */
#define UARTE_CONFIG_STOP_One (0UL) /*!< One stop bit */
#define UARTE_CONFIG_STOP_Two (1UL) /*!< Two stop bits */

/* Bits 3..1 : Parity */
#define UARTE_CONFIG_PARITY_Pos (1UL) /*!< Position of PARITY field. */
#define UARTE_CONFIG_PARITY_Msk (0x7UL << UARTE_CONFIG_PARITY_Pos) /*!< Bit mask of PARITY field. */
#define UARTE_CONFIG_PARITY_Excluded (0x0UL) /*!< Exclude parity bit */
#define UARTE_CONFIG_PARITY_Included (0x7UL) /*!< Include even parity bit */

/* Bit 0 : Hardware flow control */
#define UARTE_CONFIG_HWFC_Pos (0UL) /*!< Position of HWFC field. */
#define UARTE_CONFIG_HWFC_Msk (0x1UL << UARTE_CONFIG_HWFC_Pos) /*!< Bit mask of HWFC field. */
#define UARTE_CONFIG_HWFC_Disabled (0UL) /*!< Disabled */
#define UARTE_CONFIG_HWFC_Enabled (1UL) /*!< Enabled */


/* =========================================================================================================================== */
/* ================                                          UICR                                             ================ */
/* =========================================================================================================================== */


/**
  * @brief User Information Configuration Registers (UICR)
  * At this point just a chunk of reserved space of 2KiB
  */

typedef struct {                                /*!< (@ 0x10001000) UICR Structure                                          */
  __IM  uint32_t  RESERVED[1024];
} NRF_UICR_Type;                                /*!< Size = 4096 */



/* =========================================================================================================================== */
/* ================                                        VREQCTRL                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief Voltage request control (VREQCTRL)
  */

typedef struct {                                /*!< (@ 0x41004000) VREQCTRL_NS Structure                                      */
  __IM  uint32_t  RESERVED[320];
  __IOM VREQCTRL_VREGRADIO_Type VREGRADIO;      /*!< (@ 0x00000500) Unspecified                                                */
} NRF_VREQCTRL_Type;                            /*!< Size = 1292 (0x50c)                                                       */



#ifdef __cplusplus
}
#endif

#endif /* _NRF5340_PERI_TYPES_H */
