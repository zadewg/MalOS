#ifndef _VGA_H
#define _VGA_H 1

// text mode display (VGA-like)

#include <stdint.h>

// mapped memory ( CGA / MDA )
#define VIDEO_MEM_ADDR 0xb8000	
#define VIDEO_MEM_ADDR_NC 0Xb0000       

// display
#define MAX_ROWS 25
#define MAX_COLS 80

// screen device I/O ports
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

// NOTE: consider using #define instead of enum to mute -Wsign-conversion warnings (which are irrelevant here)

/*
 Bit 76543210
     ||||||||
     |||||^^^-fore colour
     ||||^----fore colour bright bit
     |^^^-----back colour
     ^--------back colour bright bit OR enables blinking Text

 Number     Color     Number + bright bit     Color
    0       black           0+8=8             dark gray
    1       blue            1+8=9             light blue
    2       green           2+8=10            light green
    3       cyan            3+8=11            light cyan
    4       red             4+8=12            light red
    5       magenta         5+8=13            light magenta
    6       brown           6+8=14            light brown (yellow)
    7       light gray      7+8=15            light light gray (white)
*/

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_YELLOW= 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

#endif
