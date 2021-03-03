#ifndef _STDIO_H
#define _STDIO_H 1

//#include "cdefs.h"

#define EOF (-1)

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int itoa(int value, char *dst, int base);
void reverse(char*);

#endif
