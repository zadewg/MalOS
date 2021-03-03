#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H 1

#include <stddef.h>
#include "../drivers/vga.h"

void terminal_initialize(void);
void terminal_setcolor(enum vga_color fg, enum vga_color bg);
void terminal_writestring_at(const char *message, int8_t col, int8_t row);
void terminal_writestring(const char *message);
void terminal_putchar(const char message);

#endif
