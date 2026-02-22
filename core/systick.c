#include "systick.h"
#include "stm32l476xx.h"
#include <stdint.h>

volatile uint32_t systick_ms = 0U;

void SysTick_Init(void){
	SysTick->LOAD = (uint32_t)((SystemCoreClock/1000UL) - 1UL);
	SysTick->VAL = 0UL;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 
					SysTick_CTRL_CLKSOURCE_Msk | 
					SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(void){
	systick_ms++;
}

void delay_s(uint32_t s) {
	delay_ms(s*1000U);
}

void delay_ms(uint32_t ms) {
	uint32_t start = systick_ms;
	while((systick_ms - start) < ms) {
		__NOP();
	}
}
