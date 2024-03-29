/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _NRF_HW_MODEL_PPI_H
#define _NRF_HW_MODEL_PPI_H

#include "nrfx.h"

#ifdef __cplusplus
extern "C"{
#endif

//Signals/Events types HW models may send to the PPI
typedef enum { //Note that, for performance, it is better to leave commented the unused ones
  //0 0x40000000 CLOCK
  //0 0x40000000 POWER
  //CLOCK:
  CLOCK_EVENTS_HFCLKSTARTED ,
  CLOCK_EVENTS_LFCLKSTARTED ,
  CLOCK_EVENTS_DONE         ,
  CLOCK_EVENTS_CTTO         ,
  CLOCK_EVENTS_CTSTARTED    ,
  CLOCK_EVENTS_CTSTOPPED    ,

  //POWER:
//  POWER_EVENTS_POFWARN     ,
//  POWER_EVENTS_SLEEPENTER  ,
//  POWER_EVENTS_SLEEPEXIT   ,
//  POWER_EVENTS_USBDETECTED ,
//  POWER_EVENTS_USBREMOVED  ,
//  POWER_EVENTS_USBPWRRDY   ,

  //1 0x40001000 RADIO
  //RADIO:
  RADIO_EVENTS_READY      ,
  RADIO_EVENTS_ADDRESS    ,
  RADIO_EVENTS_PAYLOAD    ,
  RADIO_EVENTS_END        ,
  RADIO_EVENTS_DISABLED   ,
  RADIO_EVENTS_DEVMATCH   ,
  RADIO_EVENTS_DEVMISS    ,
  RADIO_EVENTS_RSSIEND    ,
  RADIO_EVENTS_BCMATCH    ,
  RADIO_EVENTS_CRCOK      ,
  RADIO_EVENTS_CRCERROR   ,
  RADIO_EVENTS_FRAMESTART ,
  RADIO_EVENTS_EDEND      ,
  RADIO_EVENTS_EDSTOPPED  ,
  RADIO_EVENTS_CCAIDLE    ,
  RADIO_EVENTS_CCABUSY    ,
  RADIO_EVENTS_CCASTOPPED ,
  RADIO_EVENTS_RATEBOOST  ,
  RADIO_EVENTS_TXREADY    ,
  RADIO_EVENTS_RXREADY    ,
  RADIO_EVENTS_MHRMATCH   ,
  RADIO_EVENTS_SYNC       ,
  RADIO_EVENTS_PHYEND     ,
  RADIO_EVENTS_CTEPRESENT ,

//  2 0x40002000 UARTE
//  2 0x40002000 UART
//  3 0x40003000 TWIM
//  3 0x40003000 SPIS
//  3 0x40003000 SPIM
//  3 0x40003000 SPI
//  3 0x40003000 TWIS
//  3 0x40003000 TWI
//  4 0x40004000 TWIS
//  4 0x40004000 SPIS
//  4 0x40004000 SPIM
//  4 0x40004000 TWI
//  4 0x40004000 TWIM
//  4 0x40004000 SPI
//  5 0x40005000 NFCT
//  6 0x40006000 GPIOTE
  GPIOTE_EVENTS_IN_0,
  GPIOTE_EVENTS_IN_1,
  GPIOTE_EVENTS_IN_2,
  GPIOTE_EVENTS_IN_3,
  GPIOTE_EVENTS_IN_4,
  GPIOTE_EVENTS_IN_5,
  GPIOTE_EVENTS_IN_6,
  GPIOTE_EVENTS_IN_7,
  GPIOTE_EVENTS_PORT,

//  7 0x40007000 SAADC

  //8 0x40008000 TIMER0
  //TIMER
  TIMER0_EVENTS_COMPARE_0 ,
  TIMER0_EVENTS_COMPARE_1 ,
  TIMER0_EVENTS_COMPARE_2 ,
  TIMER0_EVENTS_COMPARE_3 ,
//  TIMER0_EVENTS_COMPARE_4 ,
//  TIMER0_EVENTS_COMPARE_5 ,

//9 0x40009000 Timer 1
  TIMER1_EVENTS_COMPARE_0 ,
  TIMER1_EVENTS_COMPARE_1 ,
  TIMER1_EVENTS_COMPARE_2 ,
  TIMER1_EVENTS_COMPARE_3 ,
//  TIMER1_EVENTS_COMPARE_4 ,
//  TIMER1_EVENTS_COMPARE_5 ,

//10 0x4000A000 Timer 2
  TIMER2_EVENTS_COMPARE_0 ,
  TIMER2_EVENTS_COMPARE_1 ,
  TIMER2_EVENTS_COMPARE_2 ,
  TIMER2_EVENTS_COMPARE_3 ,
//  TIMER2_EVENTS_COMPARE_4 ,
//  TIMER2_EVENTS_COMPARE_5 ,


  //11 0x4000B000 RTC0
  //RTC
  RTC0_EVENTS_TICK      ,
  RTC0_EVENTS_OVRFLW    ,
  RTC0_EVENTS_COMPARE_0 ,
  RTC0_EVENTS_COMPARE_1 ,
  RTC0_EVENTS_COMPARE_2 ,
  RTC0_EVENTS_COMPARE_3 ,

  //12 0x4000C000 Temperature sensor
  TEMP_EVENTS_DATARDY,

  //13 0x4000D000 Random number generator
  //RNG
  RNG_EVENTS_VALRDY     ,

  //14 0x4000E000 ECB AES
  ECB_EVENTS_ENDECB,
  ECB_EVENTS_ERRORECB,

  //15 0x4000F000 AAR
  AAR_EVENTS_END,
  AAR_EVENTS_RESOLVED,
  AAR_EVENTS_NOTRESOLVED,

  //15 0x4000F000 CCM AES
  CCM_EVENTS_ENDKSGEN,
  CCM_EVENTS_ENDCRYPT,
  CCM_EVENTS_ERROR,

  //16 0x40010000 WDT

  //17 0x40011000 RTC1
  RTC1_EVENTS_TICK      ,
  RTC1_EVENTS_OVRFLW    ,
  RTC1_EVENTS_COMPARE_0 ,
  RTC1_EVENTS_COMPARE_1 ,
  RTC1_EVENTS_COMPARE_2 ,
  RTC1_EVENTS_COMPARE_3 ,

//  18 0x40012000 QDEC
//  19 0x40013000 LPCOMP
//  19 0x40013000 COMP
//  20 0x40014000 EGU
  EGU0_EVENTS_TRIGGERED_0, //Careful!: These EGU events (inside each instance) are assumed consecutive in the EGU model
  EGU0_EVENTS_TRIGGERED_1,
  EGU0_EVENTS_TRIGGERED_2,
  EGU0_EVENTS_TRIGGERED_3,
  EGU0_EVENTS_TRIGGERED_4,
  EGU0_EVENTS_TRIGGERED_5,
  EGU0_EVENTS_TRIGGERED_6,
  EGU0_EVENTS_TRIGGERED_7,
  EGU0_EVENTS_TRIGGERED_8,
  EGU0_EVENTS_TRIGGERED_9,
  EGU0_EVENTS_TRIGGERED_10,
  EGU0_EVENTS_TRIGGERED_11,
  EGU0_EVENTS_TRIGGERED_12,
  EGU0_EVENTS_TRIGGERED_13,
  EGU0_EVENTS_TRIGGERED_14,
  EGU0_EVENTS_TRIGGERED_15,
//  20 0x40014000 SWI
//  21 0x40015000 EGU
  EGU1_EVENTS_TRIGGERED_0,
  EGU1_EVENTS_TRIGGERED_1,
  EGU1_EVENTS_TRIGGERED_2,
  EGU1_EVENTS_TRIGGERED_3,
  EGU1_EVENTS_TRIGGERED_4,
  EGU1_EVENTS_TRIGGERED_5,
  EGU1_EVENTS_TRIGGERED_6,
  EGU1_EVENTS_TRIGGERED_7,
  EGU1_EVENTS_TRIGGERED_8,
  EGU1_EVENTS_TRIGGERED_9,
  EGU1_EVENTS_TRIGGERED_10,
  EGU1_EVENTS_TRIGGERED_11,
  EGU1_EVENTS_TRIGGERED_12,
  EGU1_EVENTS_TRIGGERED_13,
  EGU1_EVENTS_TRIGGERED_14,
  EGU1_EVENTS_TRIGGERED_15,
//  21 0x40015000 SWI
//  22 0x40016000 EGU
  EGU2_EVENTS_TRIGGERED_0,
  EGU2_EVENTS_TRIGGERED_1,
  EGU2_EVENTS_TRIGGERED_2,
  EGU2_EVENTS_TRIGGERED_3,
  EGU2_EVENTS_TRIGGERED_4,
  EGU2_EVENTS_TRIGGERED_5,
  EGU2_EVENTS_TRIGGERED_6,
  EGU2_EVENTS_TRIGGERED_7,
  EGU2_EVENTS_TRIGGERED_8,
  EGU2_EVENTS_TRIGGERED_9,
  EGU2_EVENTS_TRIGGERED_10,
  EGU2_EVENTS_TRIGGERED_11,
  EGU2_EVENTS_TRIGGERED_12,
  EGU2_EVENTS_TRIGGERED_13,
  EGU2_EVENTS_TRIGGERED_14,
  EGU2_EVENTS_TRIGGERED_15,
//  22 0x40016000 SWI
//  23 0x40017000 EGU
  EGU3_EVENTS_TRIGGERED_0,
  EGU3_EVENTS_TRIGGERED_1,
  EGU3_EVENTS_TRIGGERED_2,
  EGU3_EVENTS_TRIGGERED_3,
  EGU3_EVENTS_TRIGGERED_4,
  EGU3_EVENTS_TRIGGERED_5,
  EGU3_EVENTS_TRIGGERED_6,
  EGU3_EVENTS_TRIGGERED_7,
  EGU3_EVENTS_TRIGGERED_8,
  EGU3_EVENTS_TRIGGERED_9,
  EGU3_EVENTS_TRIGGERED_10,
  EGU3_EVENTS_TRIGGERED_11,
  EGU3_EVENTS_TRIGGERED_12,
  EGU3_EVENTS_TRIGGERED_13,
  EGU3_EVENTS_TRIGGERED_14,
  EGU3_EVENTS_TRIGGERED_15,
//  23 0x40017000 SWI
//  24 0x40018000 SWI
//  24 0x40018000 EGU
  EGU4_EVENTS_TRIGGERED_0,
  EGU4_EVENTS_TRIGGERED_1,
  EGU4_EVENTS_TRIGGERED_2,
  EGU4_EVENTS_TRIGGERED_3,
  EGU4_EVENTS_TRIGGERED_4,
  EGU4_EVENTS_TRIGGERED_5,
  EGU4_EVENTS_TRIGGERED_6,
  EGU4_EVENTS_TRIGGERED_7,
  EGU4_EVENTS_TRIGGERED_8,
  EGU4_EVENTS_TRIGGERED_9,
  EGU4_EVENTS_TRIGGERED_10,
  EGU4_EVENTS_TRIGGERED_11,
  EGU4_EVENTS_TRIGGERED_12,
  EGU4_EVENTS_TRIGGERED_13,
  EGU4_EVENTS_TRIGGERED_14,
  EGU4_EVENTS_TRIGGERED_15,
//  25 0x40019000 SWI
//  25 0x40019000 EGU
  EGU5_EVENTS_TRIGGERED_0,
  EGU5_EVENTS_TRIGGERED_1,
  EGU5_EVENTS_TRIGGERED_2,
  EGU5_EVENTS_TRIGGERED_3,
  EGU5_EVENTS_TRIGGERED_4,
  EGU5_EVENTS_TRIGGERED_5,
  EGU5_EVENTS_TRIGGERED_6,
  EGU5_EVENTS_TRIGGERED_7,
  EGU5_EVENTS_TRIGGERED_8,
  EGU5_EVENTS_TRIGGERED_9,
  EGU5_EVENTS_TRIGGERED_10,
  EGU5_EVENTS_TRIGGERED_11,
  EGU5_EVENTS_TRIGGERED_12,
  EGU5_EVENTS_TRIGGERED_13,
  EGU5_EVENTS_TRIGGERED_14,
  EGU5_EVENTS_TRIGGERED_15,

//  26 0x4001A000 TIMER3
    TIMER3_EVENTS_COMPARE_0 ,
    TIMER3_EVENTS_COMPARE_1 ,
    TIMER3_EVENTS_COMPARE_2 ,
    TIMER3_EVENTS_COMPARE_3 ,
    TIMER3_EVENTS_COMPARE_4 ,
    TIMER3_EVENTS_COMPARE_5 ,

//  27 0x4001B000 TIMER4
    TIMER4_EVENTS_COMPARE_0 ,
    TIMER4_EVENTS_COMPARE_1 ,
    TIMER4_EVENTS_COMPARE_2 ,
    TIMER4_EVENTS_COMPARE_3 ,
    TIMER4_EVENTS_COMPARE_4 ,
    TIMER4_EVENTS_COMPARE_5 ,

//  28 0x4001C000 PWM
//  29 0x4001D000 PDM
//  30 0x4001E000 ACL
//  30 0x4001E000 NVMC

//31 0x4001F000 PPI
//PPI
//No events

//  32 0x40020000 MWU
//  33 0x40021000 PWM
//  34 0x40022000 PWM
//  35 0x40023000 SPIM
//  35 0x40023000 SPIS
//  35 0x40023000 SPI
//  36 0x40024000 RTC
  RTC2_EVENTS_TICK      ,
  RTC2_EVENTS_OVRFLW    ,
  RTC2_EVENTS_COMPARE_0 ,
  RTC2_EVENTS_COMPARE_1 ,
  RTC2_EVENTS_COMPARE_2 ,
  RTC2_EVENTS_COMPARE_3 ,

//  37 0x40025000 I2S
//  38 0x40026000 FPU
//  39 0x40027000 USBD
//  40 0x40028000 UARTE
//  41 0x40029000 QSPI
//  45 0x4002D000 PWM
//  47 0x4002F000 SPIM
//  0 0x50000000 GPIO
//  0 0x50000000 GPIO
//  0 0x50000300 GPIO
//  42 0x5002A000 CRYPTOCELL
//  N/A 0x10000000 FICR
//  N/A 0x10001000 UICR

  NUMBER_PPI_EVENTS
} ppi_event_types_t;

#define NUMBER_PPI_CHANNELS 32

void nrf_ppi_init();
void nrf_ppi_clean_up();
void nrf_ppi_event(ppi_event_types_t event);
void nrf_ppi_regw_sideeffects();
void nrf_ppi_regw_sideeffects_TEP(int ch_nbr);
void nrf_ppi_regw_sideeffects_EEP(int ch_nbr);
void nrf_ppi_regw_sideeffects_FORK_TEP(int ch_nbr);
void nrf_ppi_regw_sideeffects_TASKS_CHG_DIS(int i);
void nrf_ppi_regw_sideeffects_TASKS_CHG_EN(int i);
void nrf_ppi_regw_sideeffects_CHENSET();
void nrf_ppi_regw_sideeffects_CHENCLR();

#ifdef __cplusplus
}
#endif

#endif
