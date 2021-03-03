/*
 * very low level routine wrappers for hardware drivers
 *
 */

#include "hardware.h" 


// ======================= port I/O ====================================
// ref: https://github.com/torvalds/linux/blob/db54615e21419c3cb4d699a0b0aa16cc44d0e9da/arch/x86/boot/boot.h

// https://www.tldp.org/HOWTO/IO-Port-Programming-1.html
// http://www.nongnu.org/torsion/docs/html/asm_8h-source.html


// reads byte from {port}
uint8_t ports_inb ( uint16_t port ) {
	// "=a" (result): save AL register in variable {result} 
	// "d"  (port): load EDX with {port}
	uint8_t result;
	__asm__ __volatile__ ("inb %%dx, %%al" : "=a" (result) : "d" (port));
	
	return result ;
}

// writes byte {data} to {port}
void ports_outb ( uint16_t port, uint8_t data ) {
	// "a" (data): load EAX with {data}
	// "d" (port): load EDX with {port}
	__asm__ __volatile__ ("outb %%al, %%dx" : : "a" (data), "d" (port));
}

// reads word from {port}
uint16_t ports_inw ( uint16_t port ) {
	uint16_t result;
	__asm__ __volatile__ ("inw %%dx, %%ax" : "=a" (result) : "d" (port));
	
	return result;
}

// writes word {data} to {port}
void ports_outw ( uint16_t port, uint16_t data ) {
	__asm__ __volatile__ ("outw %%ax, %%dx" : : "a" (data), "d" (port));
}

// reads long from {port}
uint32_t ports_inl(uint16_t port) {
	uint32_t result;
	__asm__ __volatile__ ("inl %%edx, %%eax" : "=a" (result) : "d" (port));

	return result;
}

// writes a long {data} to {port}
void ports_outl(uint16_t port, uint32_t data) {
	__asm__ __volatile__ ("outl %%eax, %%edx" : : "a" (data), "d" (port));
}

// relax to make sure devices have time to process data
void io_wait (void) {
	// XXX: this is probably fragile
	__asm__ __volatile__ ( "jmp 1f\n\t"
			       "1:jmp 2f\n\t"
			       "2:" 
			      );
}


// ========================= memory access ============================

// read a 32-bit value at a given segment offset address TODO: 8, 16
uint32_t farpeekl(uint16_t sel, void* off) {
	uint32_t ret;

	__asm__ ( "push %%fs\n\t"
		  "mov %1, %%fs\n\t"
		  "mov %%fs:(%2), %0\n\t"
		  "pop %%fs"
		  : "=r"(ret) : "g"(sel), "r"(off) 
		 );
	
	return ret;
}

// write a 8-bit value to a segment offset address TODO: 16, 32
void farpokeb(uint16_t sel, void* off, uint8_t v) {
	__asm__ ( "push %%fs\n\t"
		  "mov  %0, %%fs\n\t"
		  "movb %2, %%fs:(%1)\n\t"
		  "pop %%fs"
		  : : "g"(sel), "r"(off), "r"(v) 
		 );
	// XXX: Should "memory" be in the clobber list here? 
}


// ========================== CPU related =============================
// https://wiki.osdev.org/CPUID
// https://wiki.osdev.org/Detecting_CPU_Topology_(80x86)

// longer nop
void cpu_relax(void) {
	__asm__ __volatile__ ("rep; nop");
}

// CPU Identification. Issue a single request to CPUID.
void cpuid(int code, uint32_t *a, uint32_t *d) {
	__asm__ __volatile__ ("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
 
// CPU Identification. issue a complete request, storing general registers output as a string
int cpuid_string(int code, uint32_t where[4]) {
	__asm__ __volatile__ ( "cpuid":"=a"(*where),"=b"(*(where+1)),
			         "=c"(*(where+2)),"=d"(*(where+3)):"a"(code)
			      );
	
	return (int)where[0];
}

// read CPU time-stamp counter (amount of clock ticks elapsed since last reset)
uint64_t rdtsc(void) {
	uint64_t ret;
	__asm__ __volatile__ ( "rdtsc" : "=A"(ret) );
	
	return ret;
}

// read a value in a control register
// XXX: pass register with inline asm?
uint32_t read_cr0(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr0, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr1(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr1, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr2(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr2, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr3(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr3, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr4(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr4, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr5(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr5, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr6(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr6, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr7(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr7, %0" : "=r"(val) );

	return val;
}


uint32_t read_cr8(void) {
	uint32_t val;
	__asm__ __volatile__ ( "mov %%cr8, %0" : "=r"(val) );

	return val;
}

/*
#define WRITE_CRX(x) \
	static inline void write_cr##x(uint32_t cr) { \
		__asm__ __volatile__ ("mov %%eax, %%cr"#x :: "r" (cr)); \
	}

#define READ_CRX(x) \
	static inline uint32_t write_cr##x(void) { \
		uint32_t val; \
		__asm__ __volatile__ ("mov %%cr"#x", %0" : "=r" (val)); \
		return val; \
	}
*/

// write a 64bit value into a msr
void wrmsr(uint32_t msr, uint32_t lo, uint32_t hi) {
	__asm__ __volatile__ ( "wrmsr" : : "a"(lo), "d"(hi), "c"(msr) );
}

// read a 64 bit value from a msr
void rdmsr(uint32_t msr, uint32_t *lo, uint32_t *hi) {
	uint64_t msr_value;
	__asm__ __volatile__ ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

// Invalidates the TLB (Translation Lookaside Buffer) for one specific virtual address. 
// The next memory reference for the page will be forced to re-read PDE and PTE from main memory. 
// Must be used every time those tables are updated. The m pointer points to a logical address, 
// not a physical or virtual one: an offset for your ds segment.
void invlpg(void* m) {
	// Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. 
	__asm__ __volatile__ ( "invlpg (%0)" : : "b"(m) : "memory" );
}

void reboot(void) {
	ports_outb(0x64, 0xFE);
}

// ====================== set/get registers ===========================
// look at isr.h
// void initregs(struct biosregs *regs);


// ======================  interrupt related ===========================
// look at isr.c 

// void intcall(u8 int_no, const struct biosregs *ireg, struct biosregs *oreg);

