#include "main.h"
int main(void) {
	SystemCoreClockUpdate();
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
    GPIOA->MODER |= (1 << GPIO_MODER_MODE5_Pos);
    while(1) {
        GPIOA->ODR ^= GPIO_ODR_OD5;
        for(volatile int i = 0; i < 50000; i++);
    }
	
}
