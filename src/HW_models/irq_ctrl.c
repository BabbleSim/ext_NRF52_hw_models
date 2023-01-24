/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * HW IRQ controller model
 * Note: In principle this should have been a model of the ARM NVIC
 *  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html
 * But so far it is just a quick thing that provides a reasonable emulation of its
 * used functionality in Zephyr
 */

#include <stdint.h>
#include "bs_types.h"
#include "bs_tracing.h"
#include "irq_ctrl.h"
#include "time_machine_if.h"
#include "NRF_HW_model_top.h"

bs_time_t Timer_irq_ctrl = TIME_NEVER;

static uint64_t irq_lines; /*Level of interrupt lines from peripherals*/
static uint64_t irq_status;  /*pended and not masked interrupts*/
static uint64_t irq_premask; /*pended interrupts before the mask*/

/*
 * Mask of which interrupts will actually cause the cpu to vector into its
 * irq handler
 * If an interrupt is masked in this way, it will be pending in the premask in
 * case it is enabled later before clearing it.
 * If the irq_mask enables and interrupt pending in irq_premask, it will cause
 * the controller to raise the interrupt immediately
 */
static uint64_t irq_mask;

/*
 * Interrupts lock/disable. When set, interrupts are registered
 * (in the irq_status) but do not awake the cpu. if when unlocked,
 * irq_status != 0 an interrupt will be raised immediately
 */
static bool irqs_locked;
static bool lock_ignore; /*For the hard fake IRQ, temporarily ignore lock*/

static uint8_t irq_prio[NRF_HW_NBR_IRQs]; /*Priority of each interrupt*/
/*note that prio = 0 == highest, prio=255 == lowest*/

static int currently_running_prio = 256; /*255 is the lowest prio interrupt*/

/*
 * These functions are provided by the board
 */
extern void posix_interrupt_raised(void);
extern void posix_irq_handler_im_from_sw(void);


void hw_irq_ctrl_init(void)
{
	irq_mask = 0; /*Let's assume all interrupts are disable at boot*/
	irq_premask = 0;
	irqs_locked = false;
	lock_ignore = false;

	for (int i = 0 ; i < NRF_HW_NBR_IRQs; i++) {
		irq_prio[i] = 255;
	}
}

void hw_irq_ctrl_cleanup(void)
{
	/*Nothing to be done*/
}

void hw_irq_ctrl_set_cur_prio(int new)
{
	currently_running_prio = new;
}

int hw_irq_ctrl_get_cur_prio(void)
{
	return currently_running_prio;
}

void hw_irq_ctrl_prio_set(unsigned int irq, unsigned int prio)
{
	irq_prio[irq] = prio;
}

uint8_t hw_irq_ctrl_get_prio(unsigned int irq)
{
	return irq_prio[irq];
}

/**
 * Get the currently pending highest priority interrupt which has a priority
 * higher than a possibly currently running interrupt
 *
 * If none, return -1
 */
int hw_irq_ctrl_get_highest_prio_irq(void)
{
	if (irqs_locked) {
		return -1;
	}

	uint64_t irq_status = hw_irq_ctrl_get_irq_status();
	int winner = -1;
	int winner_prio = 256;

	while (irq_status != 0) {
		int irq_nbr = __builtin_ffs(irq_status) - 1;

		irq_status &= ~((uint64_t) 1 << irq_nbr);
		if ((winner_prio > (int)irq_prio[irq_nbr])
		   && (currently_running_prio > (int)irq_prio[irq_nbr])) {
			winner = irq_nbr;
			winner_prio = irq_prio[irq_nbr];
		}
	}
	return winner;
}

uint32_t hw_irq_ctrl_get_current_lock(void)
{
	return irqs_locked;
}

/*
 * Change the overall interrupt controller "interrupt lock"
 * The interrupt lock is a flag that provisionally disables all interrupts
 * without affecting their status or their ability to be pended in the meanwhile
 */
uint32_t hw_irq_ctrl_change_lock(uint32_t new_lock)
{
	uint32_t previous_lock = irqs_locked;

	irqs_locked = new_lock;

	if ((previous_lock == true) && (new_lock == false)) {
		if (irq_status != 0) {
			posix_irq_handler_im_from_sw();
		}
	}
	return previous_lock;
}

uint64_t hw_irq_ctrl_get_irq_status(void)
{
	return irq_status;
}

void hw_irq_ctrl_clear_all_enabled_irqs(void)
{
	irq_status  = 0;
	irq_premask &= ~irq_mask;
}

void hw_irq_ctrl_clear_all_irqs(void)
{
	irq_status  = 0;
	irq_premask = 0;
}

void hw_irq_ctrl_disable_irq(unsigned int irq)
{
	irq_mask &= ~((uint64_t)1<<irq);
}

int hw_irq_ctrl_is_irq_enabled(unsigned int irq)
{
	return (irq_mask & ((uint64_t)1 << irq))?1:0;
}

/**
 * Get the current interrupt enable mask
 */
uint64_t hw_irq_ctrl_get_irq_mask(void)
{
	return irq_mask;
}

/*
 * Un-pend an interrupt from the interrupt controller.
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC)
 */
void hw_irq_ctrl_clear_irq(unsigned int irq)
{
	irq_status  &= ~((uint64_t)1<<irq);
	irq_premask &= ~((uint64_t)1<<irq);
}

/*
 * Reevaluate if an interrupt still has its interrupt line up,
 * and if it does, re-pend immediately the interrupt
 *
 * This is an API between the MCU model/IRQ handling side and the IRQ controller
 * model (NVIC).
 * To properly model an NVIC behavior call it only when the MCU is exiting
 * the interrupt handler for this irq
 */
void hw_irq_ctrl_reeval_level_irq(unsigned int irq)
{
	uint64_t irq_bit = ((uint64_t)1<<irq);

	if ((irq_lines & irq_bit) != 0) {
		irq_premask |= irq_bit;

		if (irq_mask & irq_bit) {
			irq_status |= irq_bit;
		}
	}
}

/**
 *
 * Enable an interrupt
 *
 * This function may only be called from SW threads
 *
 * If the enabled interrupt is pending, it will immediately vector to its
 * interrupt handler and continue (maybe with some swap() before)
 */
void hw_irq_ctrl_enable_irq(unsigned int irq)
{
	irq_mask |= ((uint64_t)1<<irq);
	if (irq_premask & ((uint64_t)1<<irq)) { /*if the interrupt is pending*/
		hw_irq_ctrl_raise_im_from_sw(irq);
	}
}

static inline void hw_irq_ctrl_irq_raise_prefix(unsigned int irq)
{
	if ( irq < NRF_HW_NBR_IRQs ) {
		irq_premask |= ((uint64_t)1<<irq);

		if (irq_mask & ((uint64_t)1<<irq)) {
			irq_status |= ((uint64_t)1<<irq);
		}
	} else if (irq == PHONY_HARD_IRQ) {
		lock_ignore = true;
	}
}

/**
 * Set/Raise/Pend an interrupt
 *
 * This function is meant to be used by either the SW manual IRQ raising
 * or by HW which wants the IRQ to be raised in one delta cycle from now
 *
 * Note that this is equivalent to a HW peripheral sending a *pulse* interrupt
 * to the interrupt controller
 */
void hw_irq_ctrl_set_irq(unsigned int irq)
{
	hw_irq_ctrl_irq_raise_prefix(irq);
	if ((irqs_locked == false) || (lock_ignore)) {
		/*
		 * Awake CPU in 1 delta
		 * Note that we awake the CPU even if the IRQ is disabled
		 * => we assume the CPU is always idling in a WFE() like
		 * instruction and the CPU is allowed to awake just with the irq
		 * being marked as pending
		 */
	    Timer_irq_ctrl = tm_get_hw_time();
	    nrf_hw_find_next_timer_to_trigger();
	}
}

/**
 * Raise an interrupt line from a HW peripheral to the interrupt controller
 *
 * This function is meant to be used only from a HW model which wants
 * to emulate level interrupts.
 * An IRQ will be raised in one delta cycle from now
 *
 * Any call from the hardware models to this function must be eventually
 * followed by a call to hw_irq_ctrl_lower_level_irq_line(), otherwise
 * the interrupt controller will keep interrupting the CPU and causing it to
 * re-enter the interrupt handler
 */
void hw_irq_ctrl_raise_level_irq_line(unsigned int irq)
{
	if ( irq >= NRF_HW_NBR_IRQs ) {
		bs_trace_error_line_time("Phony interrupts cannot use this API\n");
	}

	if ((irq_lines & ((uint64_t)1<<irq)) == 0) {
		irq_lines |= ((uint64_t)1<<irq);
		hw_irq_ctrl_set_irq(irq);
	}
}

/**
 * Lower an interrupt line from a HW peripheral to the interrupt controller
 *
 * This function is meant to be used only from a HW model which wants
 * to emulate level interrupts.
 */
void hw_irq_ctrl_lower_level_irq_line(unsigned int irq)
{
	if ( irq >= NRF_HW_NBR_IRQs ) {
		bs_trace_error_line_time("Phony interrupts cannot use this API\n");
	}

	irq_lines &= ~((uint64_t)1<<irq);
}


static void irq_raising_from_hw_now(void)
{
	/*
	 * We always awake the CPU even if the IRQ was masked,
	 * but not if irqs are locked unless this is due to a
	 * PHONY_HARD_IRQ
	 */
	if ((irqs_locked == false) || (lock_ignore)) {
		lock_ignore = false;
		posix_interrupt_raised();
	}
}

/**
 * Set/Raise/Pend an interrupt immediately.
 * Like hw_irq_ctrl_set_irq() but awake immediately the CPU instead of in
 * 1 delta cycle
 *
 * Call only from HW threads; Should be used with care
 */
void hw_irq_ctrl_raise_im(unsigned int irq)
{
	hw_irq_ctrl_irq_raise_prefix(irq);
	irq_raising_from_hw_now();
}

/**
 * Like hw_irq_ctrl_raise_im() but for SW threads
 *
 * Call only from SW threads; Should be used with care
 */
void hw_irq_ctrl_raise_im_from_sw(unsigned int irq)
{
	hw_irq_ctrl_irq_raise_prefix(irq);

	if (irqs_locked == false) {
		posix_irq_handler_im_from_sw();
	}
}

/*
 * Event timer handler for the IRQ controller HW model
 */
void hw_irq_ctrl_timer_triggered(void)
{
  Timer_irq_ctrl = TIME_NEVER;
  irq_raising_from_hw_now();
  nrf_hw_find_next_timer_to_trigger();
}

const char *hw_irq_ctrl_get_name(unsigned int irq)
{
	/* The names are taken from the IRQn_Type in the MDK header.
	 * with the suffix '_IRQn' removed.
	 */
	static const char *irqnames_nrf52833[] = {
		[0]  = "POWER_CLOCK",
		[1]  = "RADIO",
		[2]  = "UARTE0_UART0",
		[3]  = "SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0",
		[4]  = "SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1",
		[5]  = "NFCT",
		[6]  = "GPIOTE",
		[7]  = "SAADC",
		[8]  = "TIMER0",
		[9]  = "TIMER1",
		[10] = "TIMER2",
		[11] = "RTC0",
		[12] = "TEMP",
		[13] = "RNG",
		[14] = "ECB",
		[15] = "CCM_AAR",
		[16] = "WDT",
		[17] = "RTC1",
		[18] = "QDEC",
		[19] = "COMP_LPCOMP",
		[20] = "SWI0_EGU0",
		[21] = "SWI1_EGU1",
		[22] = "SWI2_EGU2",
		[23] = "SWI3_EGU3",
		[24] = "SWI4_EGU4",
		[25] = "SWI5_EGU5",
		[26] = "TIMER3",
		[27] = "TIMER4",
		[28] = "PWM0",
		[29] = "PDM",
		[30] = "MWU",
		[31] = "PWM1",
		[32] = "PWM2",
		[33] = "SPIM2_SPIS2_SPI2",
		[34] = "RTC2",
		[35] = "I2S",
		[36] = "FPU",
		[37] = "USBD",
		[38] = "UARTE1",
		[39] = "PWM3",
		[40] = "SPIM3",
	};

	if (irq < sizeof(irqnames_nrf52833)/sizeof(irqnames_nrf52833[0])) {
		return irqnames_nrf52833[irq];
	} else {
		return NULL;
	}
}
