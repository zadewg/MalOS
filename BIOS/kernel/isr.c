#include "isr.h"
#include "idt.h"
#include "tty.h"
#include "hardware.h"
#include "../libc/stdio.h"

// SEND THESE TO .H
#define REG_MPIC_CTRL   0x20		// master PIC command
#define REG_MPIC_DATA   0x21		// master PIC data
#define REG_SPIC_CTRL   0xA0		// slave PIC command
#define REG_SPIC_DATA   0xA1		// slave PIC data
#define PIC_EOI         0x20		// End Of Interrupt

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

isr_t interrupt_handlers[256];

// Default PC Interrupt Vector Asignment
// [0-31] Intel-reserved Protected Mode Exceptions
// [8-15] BIOS mapped IRQ0-7 (block_a)
// [70h-78h] BIOS mapped IRQ8-15 (block_b)
void irq_pic_remap(uint8_t block_a, uint8_t block_b)
{
	uint16_t mask_a, mask_b;
		 
	mask_a = ports_inb(REG_MPIC_DATA);                  // save masks
	mask_b = ports_inb(REG_SPIC_DATA);
				 
	ports_outb(REG_MPIC_CTRL, ICW1_INIT | ICW1_ICW4);   // starts the initialization sequence (in cascade mode)
	io_wait();
	ports_outb(REG_SPIC_CTRL, ICW1_INIT | ICW1_ICW4);
	io_wait();
	ports_outb(REG_MPIC_DATA, block_a);                 // ICW2: Master PIC vector offset
	io_wait();
	ports_outb(REG_SPIC_DATA, block_b);                 // ICW2: Slave PIC vector offset
	io_wait();
	ports_outb(REG_MPIC_DATA, 0x04);                    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	ports_outb(REG_SPIC_DATA, 0x02);                    // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
																 
	ports_outb(REG_MPIC_DATA, ICW4_8086);
	io_wait();
	ports_outb(REG_SPIC_DATA, ICW4_8086);
	io_wait();
	ports_outb(REG_MPIC_DATA, mask_a);  		    // restore saved masks.
	ports_outb(REG_SPIC_DATA, mask_b);
}


bool are_interrupts_enabled(void) {
	uint32_t flags;
	__asm__ __volatile__ ( "pushf\n\t"
			       "pop %0"
			       : "=g"(flags) 
			     );
	
	return flags & (1 << 9);
}

void irq_install(void) {

	// irq table (master 0x8-0xF conflicts with CPU ISRs)
	// remap irq [0, 15] -> [32, 47] (31 is the last CPU ISR)
	// https://wiki.osdev.org/PIC
	
	irq_pic_remap(32, 40);

	set_idt_gate(32, (uint32_t) irq0);
	set_idt_gate(33, (uint32_t) irq1);
	set_idt_gate(34, (uint32_t) irq2);
	set_idt_gate(35, (uint32_t) irq3);
	set_idt_gate(36, (uint32_t) irq4);
	set_idt_gate(37, (uint32_t) irq5);
	set_idt_gate(38, (uint32_t) irq6);
	set_idt_gate(39, (uint32_t) irq7);
	set_idt_gate(40, (uint32_t) irq8);
	set_idt_gate(41, (uint32_t) irq9);
	set_idt_gate(42, (uint32_t) irq10);
	set_idt_gate(43, (uint32_t) irq11);
	set_idt_gate(44, (uint32_t) irq12);
	set_idt_gate(45, (uint32_t) irq13);
	set_idt_gate(46, (uint32_t) irq14);
	set_idt_gate(47, (uint32_t) irq15);
}

void isr_install(void) {
	
	set_idt_gate(0, (uint32_t) isr0);
	set_idt_gate(1, (uint32_t) isr1);
	set_idt_gate(2, (uint32_t) isr2);
	set_idt_gate(3, (uint32_t) isr3);
	set_idt_gate(4, (uint32_t) isr4);
	set_idt_gate(5, (uint32_t) isr5);
	set_idt_gate(6, (uint32_t) isr6);
	set_idt_gate(7, (uint32_t) isr7);
	set_idt_gate(8, (uint32_t) isr8);
	set_idt_gate(9, (uint32_t) isr9);
	set_idt_gate(10, (uint32_t) isr10);
	set_idt_gate(11, (uint32_t) isr11);
	set_idt_gate(12, (uint32_t) isr12);
	set_idt_gate(13, (uint32_t) isr13);
	set_idt_gate(14, (uint32_t) isr14);
	set_idt_gate(15, (uint32_t) isr15);
	set_idt_gate(16, (uint32_t) isr16);
	set_idt_gate(17, (uint32_t) isr17);
	set_idt_gate(18, (uint32_t) isr18);
	set_idt_gate(19, (uint32_t) isr19);
	set_idt_gate(20, (uint32_t) isr20);
	set_idt_gate(21, (uint32_t) isr21);
	set_idt_gate(22, (uint32_t) isr22);
	set_idt_gate(23, (uint32_t) isr23);
	set_idt_gate(24, (uint32_t) isr24);
	set_idt_gate(25, (uint32_t) isr25);
	set_idt_gate(26, (uint32_t) isr26);
	set_idt_gate(27, (uint32_t) isr27);
	set_idt_gate(28, (uint32_t) isr28);
	set_idt_gate(29, (uint32_t) isr29);
	set_idt_gate(30, (uint32_t) isr30);
	set_idt_gate(31, (uint32_t) isr31);
}


void int_install(void) {
	irq_install();
	isr_install();
	idt_load(); // lidt
}

/* To print the message which defines every exception */
char *exception_messages[]= {
	"Division by Zero",
	"Single-step interrupt (trap flag)",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"Coprocessor not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"Math Fault",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point",
	"Virtualization",
	"Control Protection",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

// handlers get called from the ASM interrupt handler stub.

void isr_handler(registers_t *regs)
{
	terminal_setcolor(VGA_COLOR_RED, VGA_COLOR_BLACK);

	printf("\nreceived interrupt: %xh -> %s Exception !\n", regs->int_no, exception_messages[regs->int_no]);

	// you can hook to ISRs here


	if (interrupt_handlers[regs->int_no] != 0) {
		isr_t handler = interrupt_handlers[regs->int_no];
		handler(regs);
	} else {
		printf(">> no registered interrupt handler - PANIC (hlt)      ");
		__asm__ ("hlt");
	}

	terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);	
	// for now we assume no handler fixes anything. 
	__asm__ ("hlt");
}


void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}


void irq_handler(registers_t *regs)
{
	// Send an EOI (end of interrupt) signal to the PICs.
	
	// ctatch slave PIC.
	if (regs->int_no >= 40) {
		// Send reset signal to slave.
		ports_outb(REG_SPIC_CTRL, PIC_EOI);
	}

	// Send reset signal to master. 
	ports_outb(REG_MPIC_CTRL, PIC_EOI);
	
	if (interrupt_handlers[regs->int_no] != 0) {
		isr_t handler = interrupt_handlers[regs->int_no];
		handler(regs); //(void) handler?, so handler can return _func_ name. need an extra flag
	}

}
