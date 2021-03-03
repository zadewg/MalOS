#include "idt.h"
#include "isr.h"
#include "timer.h"
#include "hardware.h"
#include "tty.h"
#include "../libc/stdio.h"

#include "test_int.h"
#include "detect_cpu.h"

#include "pci.h"

void kmain(void) {
	
	// install interrupts
	int_install();

	// configure timer at 50 Hz
	init_timer(50); // Hz

	// display bootloader messages
	sleep(3000);
	
	//pass params to this one	
	terminal_initialize();

	// register an interrupt handler
	init_test();
		
	// display cpu info
	(void) detect_cpu();
	
	// riase interrupt manually
	//asm volatile("int $0x00");
	
	// showcase scrolling, printf
	for (int a=0; a<50; a++){
		printf("test %d\n", a);
		sleep(200);
	}

	// list PCI devices
	pci_explore();

	// raise interrupt
	printf("\nForcing fault without registered handler ...");

	int a= 2/0;

	// halt and catch fire
	for(;;);
}


