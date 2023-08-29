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

#define NHW_HAS_PPI  1
#define NHW_HAS_DPPI 0
#define NHW_USE_MDK_TYPES 1 /* The HW registers layout types are taken from the MDK */

#define NHW_AAR_TOTAL_INST 1
#define NHW_AAR_0 0
#define NHW_AAR_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/

#define NHW_CCM_TOTAL_INST 1
#define NHW_CCM_0 0
#define NHW_CCM_INT_MAP {{0 , 15}} /*Only core,CCM_AAR_IRQn*/

#define NHW_CLOCK_TOTAL_INST 1
#define NHW_CLOCK_0 0
#define NHW_CLOCK_INT_MAP {{0 , 0}} /*Only core,POWER_CLOCK_IRQn*/

#define NHW_ECB_TOTAL_INST 1
#define NHW_ECB_0 0
#define NHW_ECB_INT_MAP {{0 , 14}} /*Only core,ECB_IRQn*/

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

#define NHW_GPIOTE_TOTAL_INST 1
#define NHW_GPIOTE_0 0
#define NHW_GPIOTE_INT_MAP {{0 , 6}} /*Only core,GPIOTE_IRQn*/

#define NHW_RADIO_TOTAL_INST 1
#define NHW_RADIO_0 0
#define NHW_RADIO_INT_MAP {{0 , 1}} /*Only core,RADIO_IRQn*/

#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_0 0
#define NHW_RNG_INT_MAP {{0 , 13}} /*Only core,RNG_IRQn*/

#define NHW_RTC_TOTAL_INST 3
#define NHW_RTC_0 0
#define NHW_RTC_1 1
#define NHW_RTC_2 2
#define NHW_RTC_INT_MAP {{0 , 11}, \
                         {0 , 17}, \
                         {0 , 36}, \
                         } /*Only core,RTC0..2_IRQn*/

#define NHW_TEMP_TOTAL_INST 1
#define NHW_TEMP_0 0
#define NHW_TEMP_INT_MAP {{0 , 12}} /*Only core,TEMP_IRQn*/

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
                          } /*Only core,TIMER0..4_IRQn*/


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

#define NHW_DPPI_TOTAL_INST 2
#define NHW_DPPI_APP_0 0
#define NHW_DPPI_NET_0 1
/* The DPPI does not generate interrupts */
#define NHW_DPPI_DPPI_MAP {0,1} /*App DPPI connects to itself, network DPPI to itself*/
#define NHW_DPPI_N_CH {32, 32} /* Number of channels in each DPPI */
#define NHW_DPPI_N_CHG {6, 6}  /* Number of channel groups in each DPPI */


#define NHW_RNG_TOTAL_INST 1
#define NHW_RNG_NET_0 0
#define NHW_RNG_INT_MAP  {{1, 9}} /*Network core, "RNG_IRQn"*/
#define NHW_RNG_DPPI_MAP {1} /*Network core*/

#define NHW_FAKE_TIMER_TOTAL_INST 2
#define NHW_FAKE_TIMER_INT_MAP {{0 , 0}, \
		                            {1 , 0}} /*App core & Net core, -*/

#else
#error "No valid platform was selected"
#endif

#endif /* _NRF_HW_CONFIG_H */
