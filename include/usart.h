#ifndef USART_H
#define USART_H
 
#include "stm32l476xx.h"
#include <stdint.h>
 
#define USART_NO_DELAY 0x0
#define USART_MAX_DELAY 0xFFFFFFFF

#define USART1_TABLE_POS 0U
#define USART2_TABLE_POS 1U
#define USART3_TABLE_POS 2U
 
typedef enum {
	USART_MODE_POLLING,
	USART_MODE_INTERRUPT,
	USART_MODE_DMA
} usart_mode_t;
 
typedef enum {
	USART_STATE_READY,
	USART_STATE_BUSY
}usart_state_t;
 
typedef enum {
	USART_OK = 0,
	USART_ERROR_INVALID_PARAM,
	USART_ERROR_TIMEOUT,
	USART_ERROR_BUSY
} usart_status_t;
 
typedef struct{
	USART_TypeDef* instance;
	usart_mode_t mode;
	const uint8_t *tx_buffer;
	uint32_t tx_size;
	volatile uint32_t tx_index;
	volatile usart_state_t tx_state;
 
    uint8_t *rx_buffer;
    uint32_t rx_size;
	volatile uint32_t rx_head;
	volatile uint32_t rx_dma_head;
	volatile uint32_t rx_tail;
	volatile uint32_t rx_lost_bytes;
	volatile usart_state_t rx_state;
 
	DMA_Channel_TypeDef* rx_dma;
	DMA_Channel_TypeDef* tx_dma;
	IRQn_Type tx_dma_irq;
} usart_handle_t;
 
/* API Functions */
 
usart_status_t USART_Handle_Init(usart_handle_t*husart, USART_TypeDef* instance, usart_mode_t mode, uint8_t *rx_buffer, uint32_t rx_size);
 
usart_status_t USART_Transmit(usart_handle_t* husart, const uint8_t * data, uint32_t byte_size, uint32_t timeout_ms);
 
usart_status_t USART_Receive(usart_handle_t*husart,uint8_t *buffer, uint32_t byte_size, uint32_t timeout_ms);
 
usart_status_t USART_Transmit_IT(usart_handle_t*husart,const uint8_t*data,uint32_t byte_size);
 
uint32_t USART_Read_IT(usart_handle_t* husart, uint8_t *buffer, uint32_t byte_size);
 
usart_status_t USART_Transmit_DMA(usart_handle_t * husart, const uint8_t*data,uint32_t byte_size);
 
uint32_t USART_Read_DMA(usart_handle_t* husart, uint8_t*buffer,uint32_t byte_size);
 
#endif

