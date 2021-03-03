# MalOS
> Malos means bad guys in Spanish. Mal means wrong. This is a pretty wrong OS for the bad guys.

I use this code for OS and low level security research. The kernel is written in C and ssembler routines are in NASM (intel like) syntax where possible - inline C uses AT&T style. 

### **Features**
* BIOS bootloader with 16-bit real mode and 32-bit protected mode debugging display routines in assembler.
* C/Cpp kernel
* Full control over GDT and IDT (memory and interrupt protections), [documented](./BIOS/boot/gdt.asm) ASM GDT setting
* PIT control
* IRQ registering
* PCI device discovery
* CPU model detection
* A reasonably large libc
* Integrated QEMU emulation



### **TODO**
* Ring 3 (User Mode)
* Ether/Serial drivers for better debugging
* Convenient kernel side (.c) GDT setting
* More hardware detection (ACPI, USB ...)
* Virtual memory, heap, paging
* CPU feature detection - as in [/proc/cpuinfo](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/x86/kernel/cpu/proc.c)
* UEFI bootloader (grub?)
* Filesystem 

### **Usage**
Run `make` in ./BIOS to compile the code and `make run` to launch QEMU.

### **Registering Interrupts**
``` c
#include "../libc/stdio.h"
#include "isr.h"

static void poc_handler(registers_t *regs) {
    // register dump at time of interrupt
	(void) regs;

	printf("caught!");
}

// register interrupt 0 to poc_handler()
void init_test(void) {
	register_interrupt_handler(ISR0, &poc_handler);
}
```
### **libc**
`itoa, memcmp, memcpy, memmove, memset, printf, putchar, puts, strlen (in stdio.h, string.h) `

### **Bootloader features example**
``` nasm
%include "pm_routines_display.asm"
%include "boot_routines_display.asm"
%include "boot_routines_disk.asm"

; Reading from disk in 16 bit mode.
[bits 16]
mov bx, KERNEL_OFFSET	; tell the disk routine where to load the kernel
mov dh, 34		        ; load 15 sectors ( starting from 2nd as hardcoded in routine )
mov dl, [BOOT_DRIVE] 	; same disk as booted from
call disk_load		    ; load it

; display in 16 bit mode
[bits 16]
mov bx, MSG_REAL_MODE    
call prints

; display in 32 bit mode

[bits 32]
mov ebx, MSG_PROT_MODE	
call print_string_pm 

MSG_REAL_MODE db "Running MBR in 16-bit real mode", 0
MSG_PROT_MODE db "Landed in 32-bit protected mode", 0 

```
[boot_sector.asm]() is the main bootloader code and the place to link your 16-bit real mode or asm PM code.

### **Kernel features example**
``` c
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
```
[kernel.c](./BIOS/kernel/kernel.c) is the kernel entry point and the place to link your 32-bit ring 0 protected mode code.

&nbsp;
---

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

**mapez** - [telegram](https://t.me/mapezz)
