#include "usart.h"
#include "systick.h"
#include "system_init.h"

extern volatile uint32_t systick_ms;

int main(void){
	System_Init();
			
	const uint8_t * text = "USART2\r\n";
	uint8_t rx_buffer[256];

	usart_handle_t usart2;
	usart_config_t config = {
		.instance = USART2,
		.mode = USART_MODE_DMA,
		.rx_buffer = rx_buffer,
		.rx_size = 256
	};

	USART_Init(&usart2, &config);
	while(1) {
		USART_Transmit_DMA(&usart2, text, 8);
		delay_ms(1000);
	}
}
