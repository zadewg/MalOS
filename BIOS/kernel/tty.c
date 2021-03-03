#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "tty.h"
#include "../kernel/hardware.h"
#include "../libc/stdio.h"
#include "../libc/string.h"

// private globals
static uint8_t T_COLOR;
static uint8_t T_ERR_COLOR;
static uint16_t *T_BUFF;

// private functions
static uint16_t print_char(const char c, uint8_t col, uint8_t row, uint16_t attr);
static inline uint16_t get_cursor_offset(void);
static inline void set_cursor_offset(uint16_t offset);
static inline void enable_cursor(uint8_t start, uint8_t end);
static inline void disable_cursor(void);
static inline uint16_t get_offset(uint8_t col, uint8_t row);
static inline uint8_t get_offset_row(uint16_t offset);
static inline uint8_t get_offset_col(uint16_t offset);


/* ******************************* public functions *****************************/

void terminal_initialize(void) {
	// XXX: make these passable paramms
	T_COLOR = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
	T_ERR_COLOR = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
	T_BUFF = (uint16_t*) VIDEO_MEM_ADDR;
		
	for (size_t y = 0; y < MAX_ROWS; y++) {
		for (size_t x = 0; x < MAX_COLS; x++) {
			const size_t index = y * MAX_COLS + x;
			T_BUFF[index] = vga_entry(' ', T_COLOR);
		}
	}
	
	set_cursor_offset(get_offset(0, 0));
}


void terminal_setcolor(enum vga_color fg, enum vga_color bg) {
	T_COLOR = vga_entry_color(fg, bg);
}


void terminal_writestring_at(const char *message, int8_t col, int8_t row) { 
	uint16_t offset;

	// internal feature: use current cursor on negative coords
	if (col >= 0 && row >= 0)
		offset = get_offset(col, row);
	else {
		offset = get_cursor_offset();
		row = get_offset_row(offset);
		col = get_offset_col(offset);
	}
 
	size_t i = 0; 
	while (message[i] != 0) {
		offset = print_char(message[i++], col, row, T_COLOR);
		
		row = get_offset_row(offset);
		col = get_offset_col(offset);
	}
}


void terminal_writestring(const char *message) {
	terminal_writestring_at(message, -1, -1);
}


void terminal_putchar(const char message) {
	uint16_t offset = get_cursor_offset();
	uint8_t row = get_offset_row(offset);
	uint8_t col = get_offset_col(offset);

	offset = print_char(message, col, row, T_COLOR);
}


/* ***************** private functions **************************/
 
static uint16_t handle_scrolling (uint16_t cursor_offset) {
	// recall: 2 memory bytes per video cell (char + attributes)
	if ( cursor_offset < MAX_ROWS * MAX_COLS *2) {
		return cursor_offset ;
	}
	
	// NOTE: typecasting T_BUFF to void* here is C/C++ ilegal ( GCC: -pedantic ) 
	
	// if mem buff full, move every line up (use memmove?)
	for (uint8_t i =1; i < MAX_ROWS ; i ++) {
		memcpy( get_offset (0, i-1) + (uint8_t *) T_BUFF,
			get_offset (0, i)   + (uint8_t *) T_BUFF,
			MAX_COLS *2
		);
	}
	
	// and blank the last one (keeping attributes), (use memset?)
	uint16_t* last_line = (uint16_t*) (get_offset(0, MAX_ROWS -1) + (uint8_t *) T_BUFF);
	for (uint8_t i = 0; i < MAX_COLS*2; i ++) {
		last_line[i] = vga_entry(' ', T_COLOR);
	}
	
	// bring cursor back to bottom line
	cursor_offset -= 2* MAX_COLS ;
	
	return cursor_offset ;
}


static uint16_t print_char(const char c, uint8_t col, uint8_t row, uint16_t attr) {
	if (!attr) attr = T_COLOR;

	// check OUB coords, display error
	if (col >= MAX_COLS || row >= MAX_ROWS) {
		T_BUFF[MAX_COLS*MAX_ROWS-1] = vga_entry('E', T_ERR_COLOR);

		return get_offset(col, row);
	}

	uint16_t offset = get_offset(col, row);

	if (c == '\n') {
		row = get_offset_row(offset);
		offset = get_offset(0, row+1);
	} else if (c == '\t'){
		col = get_offset_col(offset);
		offset = get_offset(col+4, row);
	} else {
		T_BUFF[offset/2] = vga_entry(c, T_COLOR);
		offset += 2;
	}
	
	offset = handle_scrolling(offset);	

	set_cursor_offset(offset);
	return offset;
}


static inline void enable_cursor(uint8_t start, uint8_t end) {
	ports_outb(REG_SCREEN_CTRL, 0x0A);
	ports_outb(REG_SCREEN_DATA, (ports_inb(REG_SCREEN_DATA) & 0xC0) | start);

	ports_outb(REG_SCREEN_CTRL, 0x0B);
	ports_outb(REG_SCREEN_DATA, (ports_inb(REG_SCREEN_DATA) & 0xE0) | end);
}


static inline void disable_cursor(void) {
	ports_outb(REG_SCREEN_CTRL, 0x0A);
	ports_outb(REG_SCREEN_DATA, 0x20);
}


static inline uint16_t get_cursor_offset(void) {
	// requested through device ports
	uint16_t offset = 0;

	// LSBs
	ports_outb(REG_SCREEN_CTRL, 0x0F);
	offset |= ports_inb(REG_SCREEN_DATA);

	// MSBs
	ports_outb(REG_SCREEN_CTRL, 0x0E);
	offset |= ((uint16_t) ports_inb(REG_SCREEN_DATA)) << 8;

	// 8bit memory mapped (not 16 bite cell mapped)
	return offset * 2; 
}


static inline void set_cursor_offset(uint16_t offset) {
	offset /= 2;

	// cusor offset register, highbits value
	ports_outb(REG_SCREEN_CTRL, 0x0E);
	ports_outb(REG_SCREEN_DATA, (uint8_t) ((offset >> 8) & 0xFF));
	
	// cursor offset register, lowbits value
	ports_outb(REG_SCREEN_CTRL, 0x0F);
	ports_outb(REG_SCREEN_DATA, (uint8_t) (offset & 0xFF));
}


static inline uint16_t get_offset(uint8_t col, uint8_t row) { 
	return 2 * (row * MAX_COLS + col); 
}

static inline uint8_t get_offset_row(uint16_t offset) { 
	return offset / (2 * MAX_COLS); 
}

static inline uint8_t get_offset_col(uint16_t offset) { 
	return (offset - (get_offset_row(offset) *2* MAX_COLS)) /2; 
}
