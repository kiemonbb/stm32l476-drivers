#include "usart.h"
#include "systick.h"
#include "system_init.h"

extern volatile uint32_t systick_ms;

void main(void){
	System_Init();
			
	const uint8_t * text = "USART2\r\n";
	uint8_t rx_buffer[256];
	usart_handle_t usart2;
	USART_Handle_Init(&usart2, USART2, USART_MODE_DMA, rx_buffer , 256);

	while(1) {
		USART_Transmit_DMA(&usart2, text, 8);
		delay_ms(1000);
	}
}
