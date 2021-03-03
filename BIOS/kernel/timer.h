#ifndef TIMER_H
#define TIMER_H 1

#include <stddef.h>
#include <stdint.h>

#include "hardware.h"

#define PIC_INTERNAL_CLK_HZ 1193182 // 1.1931 MHz // some people use 3579545 / 3 ?

void init_timer(uint32_t frequency);
void sleep(uint32_t delay_ms);

#endif
