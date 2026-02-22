#include "gpio.h"
#include "system_init.h"
#include "systick.h"

void main(void){
	System_Init();
	GPIO_Enable_RCC(GPIOA);
	GPIO_Enable_Output(GPIOA,5,GPIO_OTYPE_PUSH_PULL,GPIO_PULL_UP,GPIO_SPEED_LOW);

	while(1) {
		GPIO_Toggle_Pin(GPIOA, 5);
		delay_s(1);
	}
}
