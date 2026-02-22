#include "gpio.h"
void main(void){
	GPIO_Enable_RCC(GPIOA);
	GPIO_Enable_Output(GPIOA,5,GPIO_OTYPE_PUSH_PULL,GPIO_PULL_UP,GPIO_SPEED_LOW);

	while(1) {
		GPIO_Toggle_Pin(GPIOA, 5);
		for(volatile int i = 1; i<500000;i++);
	}
}
