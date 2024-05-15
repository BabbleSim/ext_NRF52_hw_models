# Current implementation status

Currently peripherals of a nRF52833 and nrf5340 SOCs are modelled at varying degrees:

Notation:

| Mark | Meaning |
|---|---|
| &#x2705; | Fully completed |
| &#x2714; | Implemented |
| &#x2610; | Minimal/stubbed implementation |
| &#x10102; | Missing |

<br>

|                 |                                                            | **nRF52833** | **nRF5340** | **nRF54L15** | Notes                                                                                                                      |
|-----------------|------------------------------------------------------------|--------------|-------------|--------------|----------------------------------------------------------------------------------------------------------------------------|
| **AAR**         | Accelerated address resolver                               | &#x2714;     | &#x2714;    | &#x10102;    | See [NHW_AAR.c](../src/HW_models/NHW_AAR.c)                                                                                |
| **ACL**         | Access control lists                                       | &#x10102;    | &#x10102;   | N/A          |                                                                                                                            |
| **CACHE**       | Instruction/data cache                                     | N/A          | &#x10102;   | &#x10102;    |                                                                                                                            |
| **CCM**         | AES CCM mode encryption                                    | &#x2714;     | &#x2714;    | &#x10102;    | See [NHW_AES_CCM.c](../src/HW_models/NHW_AES_CCM.c)                                                                        |
| **CLOCK**       | Clock control                                              | &#x2714;     | &#x2714;    | &#x10102;    | See [NHW_CLOCK.c](../src/HW_models/NHW_CLOCK.c)                                                                            |
| **COMP**        | Comparator                                                 | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **CRACEN**      |                                                            | N/A          | N/A         | &#x10102;    |                                                                                                                            |
| **DPPI**        | Distributed programmable peripheral interconnect           | N/A          | &#x2705;    | &#x2705;     |                                                                                                                            |
| **ECB**         | AES electronic codebook mode encryption                    | &#x2705;     | &#x2705;    | &#x10102;    |                                                                                                                            |
| **EGU**         | Event generator unit                                       | &#x2705;     | &#x2705;    | &#x2705;     |                                                                                                                            |
| **FICR**        | Factory information configuration registers                | &#x2714;     | &#x2714;    | &#x2714;     | For 52: See [NHW_52_FICR.c](../src/HW_models/NHW_52_FICR.c)<br>For 53: See [NHW_53_FICR.c](../src/HW_models/NHW_53_FICR.c) |
| **GLITCHDET**   | Voltage glitch detectors                                   | N/A          | N/A         | &#x10102;    |                                                                                                                            |
| **GPIO**        | General purpose input/output                               | &#x2714;     | &#x10102;   | &#x10102;    | For 52: See [NRF_GPIO.c](../src/HW_models/NRF_GPIO.c)                                                                      |
| **GPIOTE**      | GPIO tasks and events                                      | &#x2705;     | &#x10102;   | &#x10102;    | For 52: Complete with very minor differences, see [NRF_GPIOTE.c](../src/HW_models/NRF_GPIOTE.c)                            |
| **GRTC**        | Global real-time counter                                   | N/A          | N/A         | &#x2714;     |                                                                                                                            |
| **I2S**         | Inter-IC sound interface                                   | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **IPC**         | Interprocessor communication                               | N/A          | &#x2714;    | N/A          | See [NHW_IPC.c](../src/HW_models/NHW_IPC.c)                                                                                |
| **KMU**         | Key management unit                                        | N/A          | &#x10102;   | &#x10102;    |                                                                                                                            |
| **LPCOMP**      | Low-power comparator                                       | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **MEMCONF**     | Memory configuration                                       | N/A          | N/A         | &#x10102;    |                                                                                                                            |
| **MPC**         | Memory Privilege Controller                                | N/A          | N/A         | &#x10102;    |                                                                                                                            |
| **MUTEX**       | Mutual exclusive peripheral                                | N/A          | &#x2705;    | N/A          |                                                                                                                            |
| **MWU**         | Memory watch unit                                          | &#x10102;    | N/A         | N/A          |                                                                                                                            |
| **NFCT**        | Near field communication tag                               | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **NVMC**        | Non-volatile memory controller                             | &#x2714;     | &#x2714;    | N/A          | See [NHW_NVMC.c](../src/HW_models/NHW_NVMC.c)                                                                              |
| **OSCILLATORS** | Oscillator control                                         | N/A          | &#x10102;   | &#x10102;    |                                                                                                                            |
| **PDM**         | Pulse density modulation interface                         | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **POWER**       | Power supply                                               | &#x2610;     | &#x2610;    | &#x10102;    | Only register stubs                                                                                                        |
| **PPI**         | Programmable peripheral interconnect                       | &#x2705;     | N/A         | N/A          | Complete but some peripheral connections are missing                                                                       |
| **PPIB**        | PPI Bridge                                                 | N/A          | N/A         | &#x2714;     |                                                                                                                            |
| **PWM**         | Pulse width modulation                                     | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **QDEC**        | Quadrature decoder                                         | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **RADIO**       | 2.4 GHz radio                                              | &#x2714;     | &#x2714;    | &#x10102;    | See [NHW_RADIO.c](../src/HW_models/NHW_RADIO.c)                                                                            |
| **REGULATORS**  | Regulator control                                          | N/A          | &#x10102;   | &#x10102;    |                                                                                                                            |
| **RESET**       | Reset control                                              | N/A          | &#x2610;    | &#x10102;    | Only register stubs                                                                                                        |
| **RNG**         | Random number generator                                    | &#x2714;     | &#x2714;    | N/A          | See [NHW_RNG.c](../src/HW_models/NHW_RNG.c)                                                                                |
| **RRAMC**       | Resistive random access memory controller                  | N/A          | N/A         | &#x2714;     | See [NHW_RRAMC.c](../src/HW_models/NHW_RRAMC.c)                                                                                                                           |
| **RTC**         | Real-time counter                                          | &#x2714;     | &#x2714;    | &#x2714;     | See [NHW_RTC.c](../src/HW_models/NHW_RTC.c)                                                                                |
| **SAADC**       | Successive approximation analog-to-digital converter       | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **[Q]SPI[M/S]** | [Quad] Serial peripheral interface [master/slave]          | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **SPU**         | System protection unit                                     | N/A          | &#x10102;   | &#x10102;    |                                                                                                                            |
| **SWI**         | Software interrupts                                        | &#x2705;     | &#x2705;    | &#x2705;     |                                                                                                                            |
| **TAMPC**       | Tamper controller                                          | N/A          | N/A         | &#x10102;    |                                                                                                                            |
| **TEMP**        | Temperature sensor                                         | &#x2714;     | &#x2714;    | &#x2714;     | See [NHW_TEMP.c](../src/HW_models/NHW_TEMP.c)                                                                              |
| **TIMER**       | Timer/counter                                              | &#x2705;     | &#x2705;    | &#x2705;     |                                                                                                                            |
| **TWI[M/S]**    | I2C compatible two-wire interface                          | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |
| **UART[E]**     | Universal asynchronous receiver/transmitter [with EasyDMA] | &#x2714;     | &#x2714;    | &#x10102;    | For 53: It cannot be used yet w Zephyr as the Zephyr driver requires a working nRF53 GPIO                                  |
| **UICR**        | User information configuration registers                   | &#x2714;     | &#x2714;    | &#x2714;     | See [NHW_NVMC.c](../src/HW_models/NHW_NVMC.c)                                                                              |
| **USBD**        | Universal serial bus device                                | &#x10102;    | &#x10102;   | N/A          |                                                                                                                            |
| **USBREG**      | Universal serial bus device                                | N/A          | &#x10102;   | N/A          |                                                                                                                            |
| **VMC**         | Volatile memory controller                                 | N/A          | &#x10102;   | N/A          |                                                                                                                            |
| **VREQCTRL**    | Voltage request control                                    | N/A          | &#x2610;    | N/A          | Only register stubs                                                                                                        |
| **WDT**         | Watchdog timer                                             | &#x10102;    | &#x10102;   | &#x10102;    |                                                                                                                            |

ARM processor peripherals or the AHB interconnect are not part of these models
