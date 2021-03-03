#ifndef _IDT_H
#define _IDT_H 1

#include <stddef.h>
#include <stdint.h>

#define KERNEL_CS 0x08 // XXX: extern CODE_SEG in gdt.asm

// interrupt gate descriptor struct
struct idt_entry_struct
{
	uint16_t base_lo;             // handler address (bits 0..15) 
	uint16_t sel;                 // code segment selector in GDT or LDT
	uint8_t  always0;             // unused
	uint8_t  flags;               // Flags. XXX: See documentation.
	uint16_t base_hi;             // handler address (bits 16..31)
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

/*
 * brief flag overview:
 * 
 * bit  7:    (P field): entry present -> 0 will cause "Interrupt Not Handled" exception.
 * bits 6..5: (DPL field): Describe Privilege Level (it can be called from) -> 0..3
 * bits 4..0: (Constant ?) -> 0b0110
 *
 */

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for 'lidt'.
struct idt_ptr_struct
{
	uint16_t limit;
	uint32_t base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// changed:
#define IDT_ENTRIES 256
idt_entry_t idt[256];
idt_ptr_t idt_reg;

void set_idt_gate(uint8_t n, uint32_t handler);
void idt_load(void);

/*
// external ASM ISR handler declarations
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10 ();
extern void isr11 ();
extern void isr12 ();
extern void isr13 ();
extern void isr14 ();
extern void isr15 ();
extern void isr16 ();
extern void isr17 ();
extern void isr18 ();
extern void isr19 ();
extern void isr20 ();
extern void isr21 ();
extern void isr22 ();
extern void isr23 ();
extern void isr24 ();
extern void isr25 ();
extern void isr26 ();
extern void isr27 ();
extern void isr28 ();
extern void isr29 ();
extern void isr30 ();
extern void isr31 ();
*/

#endif
