/*
 * Copyright (c) 2015 - 2023, Nordic Semiconductor ASA
 *
 * Modifications to the GPIO HAL to use with simulation models
 *
 * Code which is not copied from the nRFx HAL is licensed as:
 * SPDX-License-Identifier: Apache-2.0
 *
 * Most of the code of the functions below is taken from the NRFx HAL with minor
 * modifications, specially:
 * 	nrf_gpio_cfg()
 * 	nrf_gpio_reconfigure()
 * 	nrf_gpio_pin_dir_set()
 * 	nrf_gpio_pin_toggle()
 * 	nrf_gpio_port_detect_latch_set()
 * 	nrf_gpio_pin_latch_clear()
 * 	nrf_gpio_latches_read_and_clear()
 * For that code, the original license applies:
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "NRF_GPIO.h"
#include "hal/nrf_gpio.h"
#include "bs_tracing.h"

static int gpio_number_from_ptr(NRF_GPIO_Type const * p_reg){
	int i = ( (int)p_reg - (int)&NRF_GPIO_regs[0] ) / sizeof(NRF_GPIO_Type);
	return i;
}

extern NRF_GPIO_Type * nrf_gpio_pin_port_decode(uint32_t * p_pin);

void nrf_gpio_cfg(
    uint32_t             pin_number,
    nrf_gpio_pin_dir_t   dir,
    nrf_gpio_pin_input_t input,
    nrf_gpio_pin_pull_t  pull,
    nrf_gpio_pin_drive_t drive,
    nrf_gpio_pin_sense_t sense)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t cnf = reg->PIN_CNF[pin_number];

    uint32_t to_update = GPIO_PIN_CNF_DIR_Msk    |
                         GPIO_PIN_CNF_INPUT_Msk  |
                         GPIO_PIN_CNF_PULL_Msk   |
#if defined(GPIO_PIN_CNF_DRIVE_Msk)
                         GPIO_PIN_CNF_DRIVE_Msk  |
#else
                         GPIO_PIN_CNF_DRIVE0_Msk |
                         GPIO_PIN_CNF_DRIVE1_Msk |
#endif
                         GPIO_PIN_CNF_SENSE_Msk;

    /* Clear fields that will be updated. */
    cnf &= ~to_update;
    cnf |= ((uint32_t)dir << GPIO_PIN_CNF_DIR_Pos)      |
           ((uint32_t)input << GPIO_PIN_CNF_INPUT_Pos)  |
           ((uint32_t)pull << GPIO_PIN_CNF_PULL_Pos)    |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)drive << GPIO_PIN_CNF_DRIVE_Pos)  |
#else
           ((uint32_t)drive << GPIO_PIN_CNF_DRIVE0_Pos) |
#endif
           ((uint32_t)sense << GPIO_PIN_CNF_SENSE_Pos);

    reg->PIN_CNF[pin_number] = cnf;

    nrf_gpio_regw_sideeffects_PIN_CNF(gpio_number_from_ptr(reg), pin_number);
}

void nrf_gpio_reconfigure(uint32_t                     pin_number,
                          const nrf_gpio_pin_dir_t *   p_dir,
                          const nrf_gpio_pin_input_t * p_input,
                          const nrf_gpio_pin_pull_t *  p_pull,
                          const nrf_gpio_pin_drive_t * p_drive,
                          const nrf_gpio_pin_sense_t * p_sense)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t cnf = reg->PIN_CNF[pin_number];
    uint32_t to_update = (p_dir   ? GPIO_PIN_CNF_DIR_Msk                                : 0) |
                         (p_input ? GPIO_PIN_CNF_INPUT_Msk                              : 0) |
                         (p_pull  ? GPIO_PIN_CNF_PULL_Msk                               : 0) |
#if defined(GPIO_PIN_CNF_DRIVE_Msk)
                         (p_drive ? GPIO_PIN_CNF_DRIVE_Msk                              : 0) |
#else
                         (p_drive ? (GPIO_PIN_CNF_DRIVE0_Msk | GPIO_PIN_CNF_DRIVE1_Msk) : 0) |
#endif
                         (p_sense ? GPIO_PIN_CNF_SENSE_Msk                              : 0);

    /* Clear fields that will be updated. */
    cnf &= ~to_update;
    cnf |= ((uint32_t)(p_dir   ? *p_dir   : 0) << GPIO_PIN_CNF_DIR_Pos)    |
           ((uint32_t)(p_input ? *p_input : 0) << GPIO_PIN_CNF_INPUT_Pos)  |
           ((uint32_t)(p_pull  ? *p_pull  : 0) << GPIO_PIN_CNF_PULL_Pos)   |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)(p_drive ? *p_drive : 0) << GPIO_PIN_CNF_DRIVE_Pos)  |
#else
           ((uint32_t)(p_drive ? *p_drive : 0) << GPIO_PIN_CNF_DRIVE0_Pos) |
#endif
           ((uint32_t)(p_sense ? *p_sense : 0)<< GPIO_PIN_CNF_SENSE_Pos);

    reg->PIN_CNF[pin_number] = cnf;

    nrf_gpio_regw_sideeffects_PIN_CNF(gpio_number_from_ptr(reg), pin_number);
}

void nrf_gpio_pin_dir_set(uint32_t pin_number, nrf_gpio_pin_dir_t direction)
{
    if (direction == NRF_GPIO_PIN_DIR_INPUT)
    {
        nrf_gpio_cfg(
            pin_number,
            NRF_GPIO_PIN_DIR_INPUT,
            NRF_GPIO_PIN_INPUT_CONNECT,
            NRF_GPIO_PIN_NOPULL,
            NRF_GPIO_PIN_S0S1,
            NRF_GPIO_PIN_NOSENSE);
    }
    else
    {
        NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
        reg->DIRSET = (1UL << pin_number);

        nrf_gpio_regw_sideeffects_DIRSET(gpio_number_from_ptr(reg));
    }
}

void nrf_gpio_pin_toggle(uint32_t pin_number)
{
    NRF_GPIO_Type * reg        = nrf_gpio_pin_port_decode(&pin_number);
    unsigned int port = gpio_number_from_ptr(reg);

    uint32_t        pins_state = reg->OUT;

    reg->OUTSET = (~pins_state & (1UL << pin_number));
    nrf_gpio_regw_sideeffects_OUTSET(port);
    reg->OUTCLR = (pins_state & (1UL << pin_number));
    nrf_gpio_regw_sideeffects_OUTCLR(port);
}


void nrf_gpio_port_dir_output_set(NRF_GPIO_Type * p_reg, uint32_t out_mask)
{
    p_reg->DIRSET = out_mask;
    nrf_gpio_regw_sideeffects_DIRSET(gpio_number_from_ptr(p_reg));
}


void nrf_gpio_port_dir_input_set(NRF_GPIO_Type * p_reg, uint32_t in_mask)
{
    p_reg->DIRCLR = in_mask;
    nrf_gpio_regw_sideeffects_DIRCLR(gpio_number_from_ptr(p_reg));
}

void nrf_gpio_port_dir_write(NRF_GPIO_Type * p_reg, uint32_t value)
{
    p_reg->DIR = value;
    nrf_gpio_regw_sideeffects_DIR(gpio_number_from_ptr(p_reg));
}

void nrf_gpio_port_out_write(NRF_GPIO_Type * p_reg, uint32_t value)
{
    p_reg->OUT = value;
    nrf_gpio_regw_sideeffects_OUT(gpio_number_from_ptr(p_reg));
}

void nrf_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask)
{
    p_reg->OUTSET = set_mask;
    nrf_gpio_regw_sideeffects_OUTSET(gpio_number_from_ptr(p_reg));
}


void nrf_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask)
{
    p_reg->OUTCLR = clr_mask;
    nrf_gpio_regw_sideeffects_OUTCLR(gpio_number_from_ptr(p_reg));
}

void nrf_gpio_port_detect_latch_set(NRF_GPIO_Type * p_reg, bool enable)
{
    p_reg->DETECTMODE = (enable ? GPIO_DETECTMODE_DETECTMODE_LDETECT :
                                  GPIO_DETECTMODE_DETECTMODE_Default);
    nrf_gpio_regw_sideeffects_DETECTMODE(gpio_number_from_ptr(p_reg));
}

void nrf_gpio_latches_read_and_clear(uint32_t   start_port,
                                     uint32_t   length,
                                     uint32_t * p_masks)
{
    NRF_GPIO_Type * gpio_regs[GPIO_COUNT] = GPIO_REG_LIST;
    uint32_t        i;

    for (i = start_port; i < (start_port + length); i++)
    {
        *p_masks = gpio_regs[i]->LATCH;

        // The LATCH register is cleared by writing a '1' to the bit that shall be cleared.
        gpio_regs[i]->LATCH = *p_masks;
        nrf_gpio_regw_sideeffects_LATCH(gpio_number_from_ptr(gpio_regs[i]));

        p_masks++;
    }
}

void nrf_gpio_pin_latch_clear(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    reg->LATCH = (1 << pin_number);
    nrf_gpio_regw_sideeffects_LATCH(gpio_number_from_ptr(reg));
}
