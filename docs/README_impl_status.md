# Current implementation status

Currently peripherals of a nRF52833 and nrf5340 SOCs are modelled at varying degrees:

Notation:

| Mark | Meaning |
|---|---|
|&#x2714;| Fully completed |
|&#x2705;| Implemented |
|&#x2610;| Minimal/stubbed implementation |
|&#x10102;| Missing |

<br>

|  |  | **nRF52833** | **nRF5340** | Notes |
|---|---|---|---|---|
| **AAR** | Accelerated address resolver | &#x2705; | &#x2705; | See [NRF_AAR.c](../src/HW_models/NRF_AAR.c) |
| **ACL** | Access control lists | &#x10102; | &#x10102; |  |
| **CACHE** | Instruction/data cache | N/A | &#x10102; |  |
| **CCM** | AES CCM mode encryption | &#x2705; | &#x2705; | See [NRF_AES_CCM.c](../src/HW_models/NRF_AES_CCM.c) |
| **CLOCK** | Clock control | &#x2705; | &#x2705; | See [NRF_CLOCK.c](../src/HW_models/NRF_CLOCK.c) |
| **COMP** | Comparator | &#x10102; | &#x10102; |  |
| **DPPI** | Distributed programmable peripheral interconnect | N/A | &#x2714; |  |
| **ECB** | AES electronic codebook mode encryption | &#x2714; | &#x2714; |  |
| **EGU** | Event generator unit | &#x2714; | &#x2714; |  |
| **FICR** | Factory information configuration registers | &#x2705; | &#x2705; | See [NRF_FICR.c](../src/HW_models/NRF_FICR.c) |
| **GPIO** | General purpose input/output | &#x2705; | &#x10102; | For 52: See [NRF_GPIO.c](../src/HW_models/NRF_GPIO.c) |
| **GPIOTE** | GPIO tasks and events | &#x2714; | &#x10102; | For 52: Complete with very minor differences, see [NRF_GPIOTE.c](../src/HW_models/NRF_GPIOTE.c) |
| **I2S** | Inter-IC sound interface | &#x10102; | &#x10102; |  |
| **IPC** | Interprocessor communication | &#x10102; | &#x10102; |  |
| **KMU** | Key management unit | &#x10102; | &#x10102; |  |
| **LPCOMP** | Low-power comparator | &#x10102; | &#x10102; |  |
| **MUTEX** | Mutual exclusive peripheral | N/A | &#x10102; |  |
| **MWU** | Memory watch unit | &#x10102; | N/A |  |
| **NFCT** | Near field communication tag | &#x10102; | &#x10102; |  |
| **NVMC** | Non-volatile memory controller | &#x2705; | &#x2705; | See [NRF_NVMC.c](../src/HW_models/NRF_NVMC.c) |
| **OSCILLATORS** | Oscillator control | N/A | &#x10102; |  |
| **PDM** | Pulse density modulation interface | &#x10102; | &#x10102; |  |
| **POWER** | Power supply | &#x2610; | &#x2610; | Only register stubs |
| **PPI** | Programmable peripheral interconnect | &#x2714; | N/A | Complete but some peripheral connections are missing |
| **PWM** | Pulse width modulation | &#x10102; | &#x10102; |  |
| **QDEC** | Quadrature decoder | &#x10102; | &#x10102; |  |
| **RADIO** | 2.4 GHz radio | &#x2705; | &#x2705; | See [NRF_RADIO.c](../src/HW_models/NRF_RADIO.c) |
| **REGULATORS** | Regulator control | N/A | &#x10102; |  |
| **RESET** | Reset control | N/A | &#x2610; | Only register stubs |
| **RNG** | Random number generator | &#x2705; | &#x2705; | See [NRF_RNG.c](../src/HW_models/NRF_RNG.c) |
| **RTC** | Real-time counter | &#x2705; | &#x2705; | See [NRF_RTC.c](../src/HW_models/NRF_RTC.c) |
| **SAADC** | Successive approximation analog-to-digital converter | &#x10102; | &#x10102; |  |
| **[Q]SPI[M/S]** | [Quad] Serial peripheral interface [master/slave] | &#x10102; | &#x10102; |  |
| **SPU** | System protection unit | N/A | &#x10102; |  |
| **SWI** | Software interrupts | &#x2714; | &#x2714; |  |
| **TEMP** | Temperature sensor | &#x2705; | &#x2705; | See [NRF_TEMP.c](../src/HW_models/NRF_TEMP.c) |
| **TIMER** | Timer/counter | &#x2714; | &#x2714; |  |
| **TWI[M/S]** | I2C compatible two-wire interface | &#x10102; | &#x10102; |  |
| **UART[E]** | Universal asynchronous receiver/transmitter [with EasyDMA] | &#x10102; | &#x10102; |  |
| **UICR** | User information configuration registers | &#x2705; | &#x2705; | See [NRF_NVMC.c](../src/HW_models/NRF_NVMC.c) |
| **USBD** | Universal serial bus device | &#x10102; | &#x10102; |  |
| **USBREG** | Universal serial bus device | N/A | &#x10102; |  |
| **VREQCTRL** | Voltage request control | N/A | &#x2610; | Only register stubs |
| **VMC** | Volatile memory controller | N/A | &#x10102; |  |
| **WDT** | Watchdog timer | &#x10102; | &#x10102; |  |

ARM processor peripherals or the AHB interconnect are not part of these models
