/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file configures the HW models based on the variant being built
 */
#ifndef _NRF_HW_CONFIG_H
#define _NRF_HW_CONFIG_H

/*
 * For each peripheral the following may be defined
 * NHW_<PERIPH_TYPE>_TOTAL_INST <val> : Total number of instances of the peripheral in the whole SOC
 * NHW_<PERIPH_TYPE>_<CORE>_<INST> <val> : Index of that peripheral instant in list of peripherals
 *
 * NHW_<PERIPH_TYPE>_INT_MAP : List of irq controller mapping, for each instante: {irq ctrl number, irq line}
 */

#if defined(NRF52833) || defined(NRF52833_XXAA)

#define NHW_CORE_NAMES {""}

#define NHW_HAS_PPI  1
#define NHW_HAS_DPPI 0
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */

#define NHW_AAR_TOTAL_INST 1
#define NHW_AAR_0 0
#define NHW_AAR_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/
#define NHW_AAR_t_AAR    6

#define NHW_CCM_TOTAL_INST 1
#define NHW_CCM_0 0
#define NHW_CCM_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/

#define NHW_CLKPWR_TOTAL_INST 1
#define NHW_CLKPWR_0 0
#define NHW_CLKPWR_INT_MAP {{0 , 0}} /*Only core, POWER_CLOCK_IRQn*/
#define NHW_CLKPWR_HAS_RESET 0
#define NHW_CLKPWR_HAS_CALTIMER 1
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK 0
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK_I {0}
#define NHW_CLKPWR_HAS_HFCLK192MCLK 0
#define NHW_CLKPWR_HAS_HFCLK192MCLK_I  {0}

#define NHW_ECB_TOTAL_INST 1
#define NHW_ECB_0 0
#define NHW_ECB_INT_MAP {{0 , 14}} /*Only core,ECB_IRQn*/
#define NHW_ECB_t_ECB 7 /* 7.2 */

#define NHW_EGU_TOTAL_INST 6
#define NHW_EGU_0 0
#define NHW_EGU_1 1
#define NHW_EGU_2 2
#define NHW_EGU_3 3
#define NHW_EGU_4 4
#define NHW_EGU_5 5
#define NHW_EGU_INT_MAP {{0 , 20}, \
                         {0 , 21}, \
                         {0 , 22}, \
                         {0 , 23}, \
                         {0 , 24}, \
                         {0 , 25}, \
                        } /*Only core,SWI0..5_EGU0_IRQn*/
#define NHW_EGU_N_EVENTS {16, 16, 16, 16, 16, 16}

#define NHW_GPIOTE_TOTAL_INST 1
#define NHW_GPIOTE_0 0
#define NHW_GPIOTE_INT_MAP {{0 , 6}} /*Only core,GPIOTE_IRQn*/

#define NHW_INTCTRL_TOTAL_INST 1
#define NHW_INTCTRL_MAX_INTLINES 48

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0] = {\
[0 ] = "POWER_CLOCK",\
[1 ] = "RADIO",\
[2 ] = "UARTE0_UART0",\
[3 ] = "SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0",\
[4 ] = "SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1",\
[5 ] = "NFCT",\
[6 ] = "GPIOTE",\
[7 ] = "SAADC",\
[8 ] = "TIMER0",\
[9 ] = "TIMER1",\
[10] = "TIMER2",\
[11] = "RTC0",\
[12] = "TEMP",\
[13] = "RNG",\
[14] = "ECB",\
[15] = "CCM_AAR",\
[16] = "WDT",\
[17] = "RTC1",\
[18] = "QDEC",\
[19] = "COMP_LPCOMP",\
[20] = "SWI0_EGU0",\
[21] = "SWI1_EGU1",\
[22] = "SWI2_EGU2",\
[23] = "SWI3_EGU3",\
[24] = "SWI4_EGU4",\
[25] = "SWI5_EGU5",\
[26] = "TIMER3",\
[27] = "TIMER4",\
[28] = "PWM0",\
[29] = "PDM",\
[32] = "MWU",\
[33] = "PWM1",\
[34] = "PWM2",\
[35] = "SPIM2_SPIS2_SPI2",\
[36] = "RTC2",\
[37] = "I2S",\
[38] = "FPU",\
[39] = "USBD",\
[40] = "UARTE1",\
[45] = "PWM3",\
[47] = "SPIM3",\
}}

#define NHW_NVMC_UICR_TOTAL_INST 1
#define NHW_NVMC_HAS_ERASEREGS 1
#define NHW_FLASH_START_ADDR {0x00000000}
#define NHW_FLASH_PAGESIZE {(4*1024)}
#define NHW_FLASH_N_PAGES {128}
#define NHW_UICR_SIZE {776 /*64*4*/ /*bytes*/}
        /* In case somebody tries to access the UICR registers, we book
         * more space than its actual flash area (64*4)*/
#define NHW_NVMC_FLASH_T_ERASEALL (173000)
#define NHW_NVMC_FLASH_T_ERASEPAGE (87500)
#define NHW_NVMC_FLASH_T_WRITE        (42)
#define NHW_NVMC_FLASH_PARTIAL_ERASE_FACTOR (1.0)

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_0 0
#define NHW_RADIO_INT_MAP {{0 , 1}} /*Only core,RADIO_IRQn*/
#define NHW_RADIO_ED_RSSIOFFS (-93)

#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_0 0
#define NHW_RNG_INT_MAP {{0 , 13}} /*Only core,RNG_IRQn*/
#define NHW_RNG_tRNG_START 128
#define NHW_RNG_tRNG_RAW    30
#define NHW_RNG_tRNG_BC    120

#define NHW_RTC_TOTAL_INST 3
#define NHW_RTC_0 0
#define NHW_RTC_1 1
#define NHW_RTC_2 2
#define NHW_RTC_INT_MAP {{0 , 11}, \
                         {0 , 17}, \
                         {0 , 36}, \
                         } /*Only core,RTC0..2_IRQn*/
#define NHW_RTC_HAS_CAPTURE 0
#define NHW_RTC_HAS_SHORT_COMP_CLEAR 0
#define NHW_RTC_N_CC {3, 4, 4}

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_0 0
#define NHW_TEMP_INT_MAP {{0 , 12}} /*Only core,TEMP_IRQn*/
#define NHW_TEMP_t_TEMP 36 /* microseconds */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 5
#define NHW_TIMER_0 0
#define NHW_TIMER_1 1
#define NHW_TIMER_2 2
#define NHW_TIMER_3 3
#define NHW_TIMER_4 4
#define NHW_TIMER_INT_MAP {{0 , 8}, \
                           {0 , 9}, \
                           {0 , 10}, \
                           {0 , 26}, \
                           {0 , 27}, \
                          } /* Only core, TIMER0..4_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 0
#define NHW_TIMER_N_CC {4, 4, 4, 6, 6}
#define NHW_TIMER_MAX_N_CC 6
#define NHW_TIMER_FREQ {16, 16, 16, 16, 16}

#define NHW_UARTE_TOTAL_INST 2
#define NHW_UART_0 0
#define NHW_UART_1 1
#define NHW_UARTE_INT_MAP {{0 , 2}, \
                          {0 , 40}, \
                          } /* Only core, UARTE0_UART0_IRQn, UARTE1_IRQn */
#define NHW_UARTE_HAS_UART 1
#define NHW_UARTE_NAMES {"UATE0", \
                         "UATE1"}

#define NHW_BSTICKER_TOTAL_INST 1
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}} /*Only core, -*/

#define NHW_FAKE_TIMER_TOTAL_INST 1
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}} /*Only core, -*/

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
#elif defined(NRF5340) || defined(NRF5340_XXAA_NETWORK) || defined(NRF5340_XXAA_APPLICATION)

/*
 * The Application core/domain is indexed as domain 0
 * The Network core/domain is indexed as domain 1
 */

#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 0

#define NHW_AAR_TOTAL_INST 1
#define NHW_AAR_NET0 0
#define NHW_AAR_INT_MAP {{1 , 14}} /*Net core,AAR_CCM */
#define NHW_AAR_DPPI_MAP {1}
#define NHW_AAR_t_AAR    6

#define NHW_CCM_TOTAL_INST 1
#define NHW_CCM_NET0 0
#define NHW_CCM_INT_MAP {{1 , 14}} /*Net core,AAR_CCM*/
#define NHW_CCM_DPPI_MAP {1}

#define NHW_CLKPWR_TOTAL_INST 2
#define NHW_CLKPWR_APP0 0
#define NHW_CLKPWR_NET0 1
#define NHW_CLKPWR_INT_MAP {{0 , 5}, \
                            {1 , 5}  \
                           } /* {App, CLOCK_POWER}
                              * {Net, CLOCK_POWER}
                              */
#define NHW_CLKPWR_DPPI_MAP {0, 1}
#define NHW_CLKPWR_HAS_RESET 1
#define NHW_CLKPWR_HAS_CALTIMER 0
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK 1
#define NHW_CLKPWR_HAS_HFCLKAUDIOCLK_I {1, 0}
#define NHW_CLKPWR_HAS_HFCLK192MCLK 1
#define NHW_CLKPWR_HAS_HFCLK192MCLK_I  {1, 0}

#define NHW_ECB_TOTAL_INST 1
#define NHW_ECB_NET0 0
#define NHW_ECB_INT_MAP {{1 , 13}} /*Net core, ECB_IRQn*/
#define NHW_ECB_DPPI_MAP {1}
#define NHW_ECB_t_ECB 6 /* 6.2 */

#define NHW_EGU_TOTAL_INST 7
#define NHW_EGU_APP0 0
#define NHW_EGU_APP1 1
#define NHW_EGU_APP2 2
#define NHW_EGU_APP3 3
#define NHW_EGU_APP4 4
#define NHW_EGU_APP5 5
#define NHW_EGU_NET0 6
#define NHW_EGU_INT_MAP {{0 , 27}, \
                         {0 , 28}, \
                         {0 , 29}, \
                         {0 , 30}, \
                         {0 , 31}, \
                         {0 , 32}, \
                         {1 , 20}, \
                        }
                        /* {App, EGU0}
                         * ..
                         * {App, EGU5}
                         * {Network, EGU0}
                         * */
#define NHW_EGU_DPPI_MAP {0, 0, 0, 0, 0, 0,\
                          1}
#define NHW_EGU_N_EVENTS {16, 16, 16, 16, 16, 16,\
                          16}

#define NHW_DPPI_TOTAL_INST 2
#define NHW_DPPI_APP_0 0
#define NHW_DPPI_NET_0 1
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0,1} /*App DPPI connects to itself, network DPPI to itself*/
#define NHW_DPPI_N_CH {32, 32} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {6, 6}  /* Number of channel groups in each DPPI */

#define NHW_IPC_TOTAL_INST 2
#define NHW_IPC_APP0 0
#define NHW_IPC_NET0 1
#define NHW_IPC_INT_MAP {{0 , 42}, \
                         {1 , 18}}
                        /* {App, IPC}
                         * {Network, IPC}
                         * */
#define NHW_IPC_DPPI_MAP {0, 1} /*App, network */
#define NHW_IPC_N_CH     {16, 16}

#define NHW_FICR_APP 0
#define NHW_FICR_NET 1

#define NHW_INTCTRL_TOTAL_INST 2
#define NHW_INTCTRL_MAX_INTLINES 58

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0 /*Application core*/] = {\
[0 ]= "FPU",\
[1 ]= "CACHE",\
[3 ]= "SPU",\
[5 ]= "CLOCK_POWER",\
[8 ]= "SERIAL0",\
[9 ]= "SERIAL1",\
[10]= "SPIM4",\
[11]= "SERIAL2",\
[12]= "SERIAL3",\
[13]= "GPIOTE0",\
[14]= "SAADC",\
[15]= "TIMER0",\
[16]= "TIMER1",\
[17]= "TIMER2",\
[20]= "RTC0",\
[21]= "RTC1",\
[24]= "WDT0",\
[25]= "WDT1",\
[26]= "COMP_LPCOMP",\
[27]= "EGU0",\
[28]= "EGU1",\
[29]= "EGU2",\
[30]= "EGU3",\
[31]= "EGU4",\
[32]= "EGU5",\
[33]= "PWM0",\
[34]= "PWM1",\
[35]= "PWM2",\
[36]= "PWM3",\
[38]= "PDM0",\
[40]= "I2S0",\
[42]= "IPC",\
[43]= "QSPI",\
[45]= "NFCT",\
[47]= "GPIOTE1",\
[51]= "QDEC0",\
[52]= "QDEC1",\
[54]= "USBD",\
[55]= "USBREGULATOR",\
[57]= "KMU",\
/*[68]= "CRYPTOCELL",*/\
}, [1 /*Network core*/] = {\
[5 ] = "CLOCK_POWER",\
[8 ] = "RADIO",\
[9 ] = "RNG",\
[10] = "GPIOTE",\
[11] = "WDT",\
[12] = "TIMER0",\
[13] = "ECB",\
[14] = "AAR_CCM",\
[16] = "TEMP",\
[17] = "RTC0",\
[18] = "IPC",\
[19] = "SERIAL0",\
[20] = "EGU0",\
[22] = "RTC1",\
[24] = "TIMER1",\
[25] = "TIMER2",\
[26] = "SWI0",\
[27] = "SWI1",\
[28] = "SWI2",\
[29] = "SWI3",\
}}

#define NHW_CORE_NAMES {"Application", "Network"}

#define NHW_NVMC_UICR_TOTAL_INST 2
#define NHW_NVMC_APP0 0
#define NHW_NVMC_NET0 1
#define NHW_UICR_APP0 0
#define NHW_UICR_NET0 1
#define NHW_NVMC_HAS_ERASEREGS 0
#define NHW_FLASH_START_ADDR {0x00000000, 0x01000000}
#define NHW_FLASH_PAGESIZE {(4*1024), (2*1024)}
#define NHW_FLASH_N_PAGES {256, 128}
#define NHW_UICR_SIZE {4096, 800 /*bytes*/}
         //App UICR size including the KEYSLOT
#define NHW_NVMC_FLASH_T_ERASEALL (173000)
#define NHW_NVMC_FLASH_T_ERASEPAGE (87500)
#define NHW_NVMC_FLASH_T_WRITE        (43)
#define NHW_NVMC_FLASH_PARTIAL_ERASE_FACTOR (1.0)

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_NET0 0
#define NHW_RADIO_INT_MAP {{1 , 8}} /*Net core,RADIO_IRQn*/
#define NHW_RADIO_DPPI_MAP {1} /*Network core*/
#define NHW_RADIO_ED_RSSIOFFS (-93)

#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_NET_0 0
#define NHW_RNG_INT_MAP  {{1, 9}} /*Network core, "RNG_IRQn"*/
#define NHW_RNG_DPPI_MAP {1} /*Network core*/
#define NHW_RNG_tRNG_START 128
#define NHW_RNG_tRNG_RAW 32
#define NHW_RNG_tRNG_BC 122

#define NHW_RTC_TOTAL_INST 4
#define NHW_RTC_APP0 0
#define NHW_RTC_APP1 1
#define NHW_RTC_NET0 2
#define NHW_RTC_NET1 3
#define NHW_RTC_INT_MAP {{0 , 20}, \
                         {0 , 21}, \
                         {1 , 17}, \
                         {1 , 22}, \
                         } /*App core,RTC0..1_IRQn*/
                           /*Net core,RTC0..1_IRQn*/
#define NHW_RTC_DPPI_MAP {0, 0, 1, 1} /*2xApp core, 2xNetwork core*/
#define NHW_RTC_HAS_CAPTURE 1
#define NHW_RTC_HAS_SHORT_COMP_CLEAR 1
#define NHW_RTC_N_CC {4, 4, 4, 4}

#define NHW_SWI_TOTAL_INST 4
#define NHW_SWI_NET0 0
#define NHW_SWI_NET1 1
#define NHW_SWI_NET2 2
#define NHW_SWI_NET3 3

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_NET0 0
#define NHW_TEMP_INT_MAP {{1 , 16}} /*Net core,TEMP_IRQn*/
#define NHW_TEMP_DPPI_MAP {1} /*Network core*/
#define NHW_TEMP_t_TEMP 36 /* microseconds */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 6
#define NHW_TIMER_APP0 0
#define NHW_TIMER_APP1 1
#define NHW_TIMER_APP2 2
#define NHW_TIMER_NET0 3
#define NHW_TIMER_NET1 4
#define NHW_TIMER_NET2 5
#define NHW_TIMER_INT_MAP {{0 , 15}, \
                           {0 , 16}, \
                           {0 , 17}, \
                           {1 , 12}, \
                           {1 , 24}, \
                           {1 , 25}, \
                          } /* AppCore, TIMER0..2_IRQn */
                            /* NetCore, TIMER0..2_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 1
#define NHW_TIMER_N_CC {6, 6, 6, 8, 8, 8}
#define NHW_TIMER_MAX_N_CC 8
#define NHW_TIMER_FREQ {16, 16, 16, 16, 16, 16}
#define NHW_TIMER_DPPI_MAP {0, 0, 0, 1, 1, 1} /*3xApp core, 3xNetwork core*/

#define NHW_UARTE_TOTAL_INST 5
#define NHW_UARTE_APP0 0
#define NHW_UARTE_APP1 1
#define NHW_UARTE_APP2 2
#define NHW_UARTE_APP3 3
#define NHW_UARTE_NET0 4
#define NHW_UARTE_INT_MAP {{0 , 8}, \
                          {0 , 9}, \
                          {0 , 11}, \
                          {0 , 12}, \
                          {1 , 19}, \
                          } /* App core, SERIAL0..3_IRQn,
                               Net core, SERIAL0_IRQn */
#define NHW_UARTE_DPPI_MAP {0, 0, 0, 0, 1} /*4xApp core, 1xNetwork core*/
#define NHW_UARTE_HAS_UART 0
#define NHW_UARTE_NAMES {"App UATE0", \
                         "App UATE1", \
                         "App UATE2", \
                         "App UATE3", \
                         "Net UATE0"}

#define NHW_FAKE_TIMER_TOTAL_INST 2
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}, \
		                            {1 , 0}} /*App core & Net core, -*/

#define NHW_BSTICKER_TOTAL_INST 2
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}, \
                                    {1 , 0}} /*App core & Net core, -*/

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
#elif defined(NRF54L15) || defined(NRF54L15_XXAA)

#define NHW_HAS_PPI  0
#define NHW_HAS_DPPI 1
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */
#define NHW_CORE_NAMES {"Application", "Flipper"}


#define NHW_DPPI_TOTAL_INST 4
#define NHW_DPPI_00 0
#define NHW_DPPI_10 1
#define NHW_DPPI_20 2
#define NHW_DPPI_30 3
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0, 1, 2, 3} /* DPPI connect to themselves */
#define NHW_DPPI_N_CH {8, 24, 16, 4} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {2, 6, 6, 2}  /* Number of channel groups in each DPPI */

#define NHW_EGU_TOTAL_INST 2
#define NHW_EGU_10 0
#define NHW_EGU_20 1
#define NHW_EGU_INT_MAP {{0 , 135}, \
                         {0 , 201}, \
                        }
                        /* {App, EGU10}
                         * {App, EGU20}
                         * */
#define NHW_EGU_DPPI_MAP {1, 2}
#define NHW_EGU_N_EVENTS {16, 6}

#define NHW_GRTC_TOTAL_INST 1
#define NHW_GRTC_N_INT 4
#define NHW_GRTC_INT_MAP { \
                           {0 , 226}, \
                           {0 , 227}, \
                           {0 , 228}, \
                           {0 , 229}, \
                         }
                        /* {App, GRTC_0..3_IRQn} */
#define NHW_GRTC_DPPI_MAP {2 /* Peripheral domain */}
#define NHW_GRTC_N_CC 12
#define NHW_GRTC_N_DOMAINS 4
#define NHW_GRTC_SYSCOUNTER_BW 52
#define NHW_GRTC_HAS_CLKOUT 0
#define NHW_GRTC_HAS_PWM 1


#define NHW_INTCTRL_TOTAL_INST 2
#define NHW_INTCTRL_MAX_INTLINES 271

/* These names are taken from the IRQn_Type in the MDK header */
#define NHW_INT_NAMES { [0 /*Application core*/] = {\
[28 ]="SWI00",\
[29 ]="SWI01",\
[30 ]="SWI02",\
[31 ]="SWI03",\
[64 ]="SPU00",\
[65 ]="MPC00",\
[70 ]="AAR00_CCM00",\
[71 ]="ECB00",\
[72 ]="CRACEN",\
[74 ]="SERIAL00",\
[75 ]="RRAMC",\
[76 ]="VPR00",\
[82 ]="CTRLAP",\
[83 ]="CM33SS",\
[85 ]="TIMER00",\
[128]="SPU10",\
[133]="TIMER10",\
[134]="RTC10",\
[135]="EGU10",\
[138]="RADIO_0",\
[139]="RADIO_1",\
[192]="SPU20",\
[198]="SERIAL20",\
[199]="SERIAL21",\
[200]="SERIAL22",\
[201]="EGU20",\
[202]="TIMER20",\
[203]="TIMER21",\
[204]="TIMER22",\
[205]="TIMER23",\
[206]="TIMER24",\
[208]="PDM20",\
[209]="PDM21",\
[210]="PWM20",\
[211]="PWM21",\
[212]="PWM22",\
[213]="SAADC",\
[214]="NFCT",\
[215]="TEMP",\
[218]="GPIOTE20_0",\
[219]="GPIOTE20_1",\
[220]="TAMPC",\
[221]="I2S20",\
[224]="QDEC20",\
[225]="QDEC21",\
[226]="GRTC_0",\
[227]="GRTC_1",\
[228]="GRTC_2",\
[229]="GRTC_3",\
[256]="SPU30",\
[260]="SERIAL30",\
[261]="RTC30",\
[262]="COMP_LPCOMP",\
[264]="WDT30",\
[265]="WDT31",\
[268]="GPIOTE30_0",\
[269]="GPIOTE30_1",\
[270]="CLOCK_POWER",\
}, [1 /*Flipper core*/] = {\
[0  ]="VPRCLIC_0",\
[1  ]="VPRCLIC_1",\
[2  ]="VPRCLIC_2",\
[3  ]="VPRCLIC_3",\
[4  ]="VPRCLIC_4",\
[5  ]="VPRCLIC_5",\
[6  ]="VPRCLIC_6",\
[7  ]="VPRCLIC_7",\
[8  ]="VPRCLIC_8",\
[9  ]="VPRCLIC_9",\
[10 ]="VPRCLIC_10",\
[11 ]="VPRCLIC_11",\
[12 ]="VPRCLIC_12",\
[13 ]="VPRCLIC_13",\
[14 ]="VPRCLIC_14",\
[15 ]="VPRCLIC_15",\
[16 ]="VPRCLIC_16",\
[17 ]="VPRCLIC_17",\
[18 ]="VPRCLIC_18",\
[19 ]="VPRCLIC_19",\
[20 ]="VPRCLIC_20",\
[21 ]="VPRCLIC_21",\
[22 ]="VPRCLIC_22",\
[23 ]="VPRCLIC_23",\
[24 ]="VPRCLIC_24",\
[25 ]="VPRCLIC_25",\
[26 ]="VPRCLIC_26",\
[27 ]="VPRCLIC_27",\
[28 ]="VPRCLIC_28",\
[29 ]="VPRCLIC_29",\
[30 ]="VPRCLIC_30",\
[31 ]="VPRCLIC_31",\
[64 ]="SPU00",\
[65 ]="MPC00",\
[70 ]="AAR00_CCM00",\
[71 ]="ECB00",\
[72 ]="CRACEN",\
[74 ]="SERIAL00",\
[75 ]="RRAMC",\
[76 ]="VPR00",\
[82 ]="CTRLAP",\
[85 ]="TIMER00",\
[128]="SPU10",\
[133]="TIMER10",\
[134]="RTC10",\
[135]="EGU10",\
[138]="RADIO_0",\
[139]="RADIO_1",\
[192]="SPU20",\
[198]="SERIAL20",\
[199]="SERIAL21",\
[200]="SERIAL22",\
[201]="EGU20",\
[202]="TIMER20",\
[203]="TIMER21",\
[204]="TIMER22",\
[205]="TIMER23",\
[206]="TIMER24",\
[208]="PDM20",\
[209]="PDM21",\
[210]="PWM20",\
[211]="PWM21",\
[212]="PWM22",\
[213]="SAADC",\
[214]="NFCT",\
[215]="TEMP",\
[218]="GPIOTE20_0",\
[219]="GPIOTE20_1",\
[220]="TAMPC",\
[221]="I2S20",\
[224]="QDEC20",\
[225]="QDEC21",\
[226]="GRTC_0",\
[227]="GRTC_1",\
[228]="GRTC_2",\
[229]="GRTC_3",\
[256]="SPU30",\
[260]="SERIAL30",\
[261]="RTC30",\
[262]="COMP_LPCOMP",\
[264]="WDT30",\
[265]="WDT31",\
[268]="GPIOTE30_0",\
[269]="GPIOTE30_1",\
[270]="CLOCK_POWER",\
}}

#define NHW_PPIB_TOTAL_INST 8
#define NHW_PPIB_00 0
#define NHW_PPIB_01 1
#define NHW_PPIB_10 2
#define NHW_PPIB_11 3
#define NHW_PPIB_20 4
#define NHW_PPIB_21 5
#define NHW_PPIB_22 6
#define NHW_PPIB_30 7
#define NHW_PPIB_DPPI_MAP {0, 0, 1, 1, 2, 2, 2, 3}
#define NHW_PPIB_N_CH {8, 8, 8, 16, 8, 16, 4, 4}
#define NHW_PPIB_MATE {2,/*00->10*/ \
                       4,/*01->20*/ \
                       0,/*10->00*/ \
                       5,/*11->21*/ \
                       1,/*20->01*/ \
                       3,/*21->11*/ \
                       7,/*22->30*/ \
                       6,/*30->22*/ \
                       }
#define HWH_PPIB_HARDWIRESCHANNELS 0

#define NHW_RRAMC_UICR_TOTAL_INST 1
#define NHW_RRAM_START_ADDR {0x00000000}
#define NHW_RRAM_SIZE       {(1524*1024)}
#define NHW_UICR_SIZE {2560 /*bytes*/}


#define NHW_RTC_TOTAL_INST 2
#define NHW_RTC_10 0
#define NHW_RTC_30 1
#define NHW_RTC_INT_MAP {{0 , 134}, \
                         {0 , 261}, \
                         } /*App core,RTC10_IRQn*/
                           /*Net core,RTC30_IRQn*/
#define NHW_RTC_DPPI_MAP {1, 3}
#define NHW_RTC_HAS_CAPTURE 1
#define NHW_RTC_HAS_SHORT_COMP_CLEAR 1
#define NHW_RTC_N_CC {4, 4}

#define NHW_SWI_TOTAL_INST 4

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_APP0 0
#define NHW_TEMP_INT_MAP {{0 , 215}} /*App core,TEMP_IRQn*/
#define NHW_TEMP_DPPI_MAP {2} /*Peri domain*/
#define NHW_TEMP_t_TEMP 36 /* microseconds, Unknown, assuming by now same as 52833 & 5340 */
#define NHW_TEMP_FBITS  2 /* fractional bits => 0.25C resolution */

#define NHW_TIMER_TOTAL_INST 7
#define NHW_TIMER_00 0
#define NHW_TIMER_10 1
#define NHW_TIMER_20 2
#define NHW_TIMER_21 3
#define NHW_TIMER_22 4
#define NHW_TIMER_23 5
#define NHW_TIMER_24 6
#define NHW_TIMER_INT_MAP {{0 , 85}, \
                           {0 , 133}, \
                           {0 , 202}, \
                           {0 , 203}, \
                           {0 , 204}, \
                           {0 , 205}, \
                           {0 , 206}, \
                          } /* App, TIMER00..24_IRQn */
#define NHW_TIMER_HAS_ONE_SHOT 1
#define NHW_TIMER_N_CC {6, 8, 6, 6, 6, 6, 6}
#define NHW_TIMER_MAX_N_CC 8
#define NHW_TIMER_FREQ {128, 32, 16, 16, 16, 16, 16}
#define NHW_TIMER_DPPI_MAP {0, 1, 2, 2, 2, 2, 2}


#define NHW_FAKE_TIMER_TOTAL_INST 2
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}, \
                                {1 , 0}} /*App core & flpr core, -*/

#define NHW_BSTICKER_TOTAL_INST 2
#define NHW_BSTICKER_TIMER_INT_MAP {{0 , 0}, \
                                    {1 , 0}} /*App core & flpr core, -*/

#else
#error "No valid platform was selected"
#endif

#endif /* _NRF_HW_CONFIG_H */
