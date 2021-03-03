#include "../libc/stdio.h"
#include "isr.h"

static void divhandler(registers_t *regs) {
	(void) regs;
	printf("caught!");
}

void init_test(void) {
	register_interrupt_handler(ISR0, &divhandler);
}




