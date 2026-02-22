#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void SysTick_Init(void);

void SysTick_Handler(void);

void delay_s(uint32_t s);

void delay_ms(uint32_t ms);

#endif
