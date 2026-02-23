#include "usart.h"
#include "system_init.h"

extern volatile uint32_t systick_ms;

void main(void){
	System_Init();
	USART_Init(USART2);

	while(1) {
		USART_Write_String(USART2, "Current time: ");
		USART_Write_Int(USART2, systick_ms);
		USART_Write_String(USART2, "ms");
		USART_Write_Char(USART2, '\r');
		USART_Write_Char(USART2, '\n');
	}
}
