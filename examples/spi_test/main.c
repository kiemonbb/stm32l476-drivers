#include "spi.h"
#include "usart.h"
#include "systick.h"
#include "system_init.h"

extern volatile uint32_t systick_ms;

void main(void){
	System_Init();

	uint8_t usart_rx_buffer[256];
	usart_handle_t usart2;

	uint8_t tx[] = "ABCD\r\n";
    uint8_t rx[6] = {0};
	spi_handle_t spi1;
	spi_config_t cfg = {
		.instance = SPI1,
		.baud = SPI_BAUD_DIV_2,
		.clock_mode = SPI_CLOCK_MODE_0,
		.mode = SPI_MODE_POLLING,
		.data_size = SPI_DATASIZE_8BIT,
	};

	SPI_Init(&spi1, &cfg);		
	USART_Handle_Init(&usart2, USART2, USART_MODE_POLLING, usart_rx_buffer , 256);

	SPI_TransmitReceive8(&spi1,tx, rx, 6,100);
	USART_Transmit(&usart2, rx, 6, 100);

	while(1) {
	}
}
