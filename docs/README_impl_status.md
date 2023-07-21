# Current implementation status

Currently these models aim at modelling the peripherals of a nRF52833 SOC at varying degrees:

* &#x2713; **NVMC** — Non-volatile memory controller
    * See [NRF_NVMC.c](../src/HW_models/NRF_NVMC.c)
* &#x2713; **FICR** — Factory information configuration registers
    * See [NRF_FICR.c](../src/HW_models/NRF_FICR.c)
* &#x2713; **UICR** — User information configuration registers
    * See [NRF_NVMC.c](../src/HW_models/NRF_NVMC.c)
* &#x10102; **POWER** — Power supply
    * Only register stubs
* &#x2713; **CLOCK** — Clock control
    * See [NRF_CLOCK.c](../src/HW_models/NRF_CLOCK.c)
* &#x2713; **AAR** — Accelerated address resolver
    * See [NRF_AAR.c](../src/HW_models/NRF_AAR.c)
* &#x10102; **ACL** — Access control lists
    * Not modelled yet
* &#x2713; **CCM** — AES CCM mode encryption
    * See [NRF_AES_CCM.c](../src/HW_models/NRF_AES_CCM.c)
* &#x10102; **COMP** — Comparator
    * Not modelled yet
* &#x2714; **ECB** — AES electronic codebook mode encryption
    * Complete
* &#x2714; **EGU** — Event generator unit
    * Complete
* &#x2713; **GPIO** — General purpose input/output
    * See [NRF_GPIO.c](../src/HW_models/NRF_GPIO.c)
* &#x2714; **GPIOTE** — GPIO tasks and events
    * Complete with very minor differences
    * See [NRF_GPIOTE.c](../src/HW_models/NRF_GPIOTE.c)
* &#x10102; **I2S** — Inter-IC sound interface
    * Not modelled yet
* &#x10102; **LPCOMP** — Low-power comparator
    * Not modelled yet
* &#x10102; **MWU** — Memory watch unit
    * Not modelled yet
* &#x10102; **NFCT** — Near field communication tag
    * Not modelled yet
* &#x10102; **PDM** — Pulse density modulation interface
    * Not modelled yet
* &#x2714; **PPI** — Programmable peripheral interconnect
    * Complete but some peripheral connections are missing
* &#x10102; **PWM** — Pulse width modulation
    * Not modelled yet
* &#x10102; **QDEC** — Quadrature decoder
    * Not modelled yet
* &#x2713; **RADIO** — 2.4 GHz radio
    * See [NRF_RADIO.c](../src/HW_models/NRF_RADIO.c)
* &#x2713; **RNG** — Random number generator
    * See [NRF_RNG.c](../src/HW_models/NRF_RNG.c)
* &#x2713; **RTC** — Real-time counter
    * See [NRF_RTC.c](../src/HW_models/NRF_RTC.c)
* &#x10102; **SAADC** — Successive approximation analog-to-digital converter
    * Not modelled yet
* &#x10102; **SPI** — Serial peripheral interface master
    * Not modelled yet
* &#x10102; **SPIM** — Serial peripheral interface master with EasyDMA
    * Not modelled yet
* &#x10102; **SPIS** — Serial peripheral interface slave with EasyDMA
    * Not modelled yet
* &#x2714; **SWI** — Software interrupts
    * See EGU
* &#x2713; **TEMP** — Temperature sensor
    * See [NRF_TEMP.c](../src/HW_models/NRF_TEMP.c)
* &#x10102; **TWI** — I2C compatible two-wire interface
    * Not modelled yet
* &#x2713; **TIMER** — Timer/counter
    * See [NRF_TIMER.c](../src/HW_models/NRF_TIMER.c)
* &#x10102; **TWIM** — I2C compatible two-wire interface master with EasyDMA
    * Not modelled yet
* &#x10102; **TWIS** — I2C compatible two-wire interface slave with EasyDMA
    * Not modelled yet
* &#x10102; **UART** — Universal asynchronous receiver/transmitter
    * Not modelled yet
* &#x10102; **UARTE** — Universal asynchronous receiver/transmitter with EasyDMA
    * Not modelled yet
* &#x10102; **USBD** — Universal serial bus device
    * Not modelled yet
* &#x10102; **WDT** — Watchdog timer
    * Not modelled yet


ARM processor peripherals or the AHB interconnect are not part of these models