// timer.c -- Initialises the PIT, and handles clock updates.
// https://wiki.osdev.org/Programmable_Interval_Timer


#include "timer.h"
#include "isr.h"
#include "../libc/stdio.h"

#define REG_PIT_CTRL 0x43
#define REG_PIT_C0_DATA 0x40

uint32_t tick = 0;
uint32_t pit_frequency;

void sleep(uint32_t delay_ms) {
	uint32_t ref = tick;
	uint32_t tick_goal = delay_ms / pit_frequency;
	
	while ((tick - ref) < tick_goal) { } 
}

static void timer_callback(registers_t *regs)
{
	tick++;
	
	(void) regs;
	//printf("Tick: %d\n", tick);
}

void init_timer(uint32_t frequency)
{
	// update global
	pit_frequency = frequency;
	
	// register timer callback.
	register_interrupt_handler(IRQ0, &timer_callback);
	
     	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.

	//printf("D: Interrupt handler registered\n");

	uint16_t divisor = (uint16_t) (PIC_INTERNAL_CLK_HZ / frequency);
	 
	// Send the command byte.
	ports_outb(REG_PIT_CTRL, 0x36);
	 
	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = (uint8_t) (divisor & 0xFF);
	uint8_t h = (uint8_t) ((divisor>>8) & 0xFF );
	
	// Send the frequency divisor.
	ports_outb(REG_PIT_C0_DATA, l);
	ports_outb(REG_PIT_C0_DATA, h);

	//printf("D: PIC frequency set\n");
}
