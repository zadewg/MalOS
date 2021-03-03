#include "stdio.h"
#include "../kernel/tty.h"


int putchar(int ic) {
	char c = (char) ic;
	//terminal_write(&c, sizeof(c));
	terminal_putchar(c);
	return ic;
}
