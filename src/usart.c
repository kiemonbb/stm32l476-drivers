#include "usart.h"
#include "gpio.h"
#include "stm32l476xx.h"
#include "system_stm32l4xx.h"
#include <stdint.h>
 
#define USART_MAX_INSTANCES 3
static usart_handle_t *usart_table[USART_MAX_INSTANCES] = {0};
 
extern volatile uint32_t systick_ms;
 
static void USART1_Config(void){
	GPIO_Enable_AF(GPIOA, 9, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOA, 10, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	(void)RCC->APB2ENR;
 
	NVIC_EnableIRQ(USART1_IRQn);
}
 
static void USART2_Config(void){
	GPIO_Enable_AF(GPIOA, 2, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOA, 3, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	(void)RCC->APB1ENR1;
 
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 8);
}
 
static void USART3_Config(void){
	GPIO_Enable_AF(GPIOC, 10, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOC, 11, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	(void)RCC->APB1ENR1;
 
	NVIC_EnableIRQ(USART3_IRQn);
}
 
static void USART_Config(USART_TypeDef* instance) {
	if(instance == USART1) USART1_Config();
	else if(instance == USART2) USART2_Config();
	else if(instance == USART3) USART3_Config();
}
 
static void USART_Init(USART_TypeDef* instance) {
	USART_Config(instance);
	instance->CR1 &= ~USART_CR1_UE;
 
	instance->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);
 
	instance->CR1 &= ~USART_CR1_PCE;
 
	instance->CR2 &= ~USART_CR2_STOP;
 
	//TODO: Take prescalers into account
	instance->BRR = (uint32_t)(SystemCoreClock/115200);
 
	instance->CR1 |= USART_CR1_UE;
 
	instance->CR1 |= USART_CR1_TE | USART_CR1_RE;
 
	while(!(instance->ISR & USART_ISR_TEACK));
	while(!(instance->ISR & USART_ISR_REACK));
 
}
 
void USART_Transmit(usart_handle_t* husart, const uint8_t * data, uint32_t byte_size, uint32_t timeout_ms) {
	if(husart->tx_busy) return;
	husart->tx_busy = 1;
 
	uint32_t i = 0;
	while(i < byte_size){
		uint32_t current_ms = systick_ms;
		while(!(husart->instance->ISR & USART_ISR_TXE)) {	
			if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0){
				husart->tx_busy = 0;
				return;
			}	
		}
		husart->instance->TDR = data[i++];
	}
 
	uint32_t current_ms = systick_ms;
	while(!(husart->instance->ISR & USART_ISR_TC)) {	
		if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0) break;
	}
 
	husart->tx_busy = 0;
}
 
void USART_Receive(usart_handle_t*husart,uint8_t *buffer, uint32_t byte_size, uint32_t timeout_ms) {
	uint32_t i = 0;
	while(i < byte_size){
		uint32_t current_ms = systick_ms;
		while(!(husart->instance->ISR & USART_ISR_RXNE)) {	
		if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0) return;
		}
		buffer[i++] = husart->instance->RDR;
	}
}
 
void USART_Transmit_IT(usart_handle_t*husart, const uint8_t*data,uint32_t byte_size) {
	if(husart->tx_busy) return;
	husart->tx_busy = 1;
	husart->tx_buffer = data;
	husart->tx_size = byte_size;
	husart->tx_index = 0;
 
	husart->instance->CR1 |= USART_CR1_TXEIE;
}
 
uint32_t USART_Read_IT(usart_handle_t* husart, uint8_t *buffer, uint32_t byte_size) {
    uint32_t count = 0;
    while(count < byte_size && husart->rx_tail != husart->rx_head) {
        buffer[count++] = husart->rx_buffer[husart->rx_tail];
        husart->rx_tail = (husart->rx_tail + 1) % husart->rx_size;
    }
    return count;
}
 
uint32_t USART_Read_DMA(usart_handle_t* husart, uint8_t*buffer,uint32_t byte_size){
	if(!husart->rx_ready) return 0;
	husart->rx_ready = 0;
	uint32_t remaining = husart->rx_dma->CNDTR;
	uint32_t new_head = husart->rx_size - remaining;
 
	if(new_head == husart->rx_dma_head) return 0;
 
    uint32_t received;
    if(new_head > husart->rx_dma_head) {
        received = new_head - husart->rx_dma_head;
    } else {
        received = (husart->rx_size - husart->rx_dma_head) + new_head;
    }
 
	if(received > byte_size) received = byte_size;
 
	uint32_t i =0;
	while(i < received){
		buffer[i] = husart->rx_buffer[(husart->rx_dma_head+i)%husart->rx_size];
		i++;
	}
	husart->rx_dma_head = new_head;
	return received;
}
 
void USART_Transmit_DMA(usart_handle_t * husart, const uint8_t*data,uint32_t byte_size){
	if(husart->tx_busy) return;
 
	husart->tx_busy = 1;
	husart->tx_buffer = data;
	husart->tx_size = byte_size;
 
	husart->tx_dma->CCR &= ~(DMA_CCR_EN);
	while(husart->tx_dma->CCR & DMA_CCR_EN);
 
	husart->tx_dma->CMAR = (uint32_t)data;
	husart->tx_dma->CNDTR = byte_size;
 
	husart->tx_dma->CCR |= DMA_CCR_EN;
}
 
//--- DMA ------------------------------
 
static inline uint32_t dma_channel_to_cselr_pos(DMA_Channel_TypeDef* channel) {
	if(channel == DMA1_Channel1) return DMA_CSELR_C1S_Pos;
	else if(channel == DMA1_Channel2) return DMA_CSELR_C2S_Pos;
	else if(channel == DMA1_Channel3) return DMA_CSELR_C3S_Pos;
	else if(channel == DMA1_Channel4) return DMA_CSELR_C4S_Pos;
	else if(channel == DMA1_Channel5) return DMA_CSELR_C5S_Pos;
	else if(channel == DMA1_Channel6) return DMA_CSELR_C6S_Pos;
	else if(channel == DMA1_Channel7) return DMA_CSELR_C7S_Pos;
 
	return 0;
}
 
static inline uint32_t dma_channel_to_number(DMA_Channel_TypeDef* channel) {
	if(channel == DMA1_Channel1) return 1U;
	else if(channel == DMA1_Channel2) return 2U;
	else if(channel == DMA1_Channel3) return 3U;
	else if(channel == DMA1_Channel4) return 4U;
	else if(channel == DMA1_Channel5) return 5U;
	else if(channel == DMA1_Channel6) return 6U;
	else if(channel == DMA1_Channel7) return 7U;
 
	return 0;
}
 
static void USART_RX_DMA_Config(usart_handle_t* husart) {
	husart->rx_dma->CCR &= ~DMA_CCR_EN;
 
	husart->rx_dma->CPAR = (uint32_t)&husart->instance->RDR;
	husart->rx_dma->CMAR = (uint32_t)husart->rx_buffer;
	husart->rx_dma->CNDTR = husart->rx_size;
 
	DMA1_CSELR->CSELR &= ~(0xFU << dma_channel_to_cselr_pos(husart->rx_dma));
	DMA1_CSELR->CSELR |= (0x2U<< dma_channel_to_cselr_pos(husart->rx_dma));
 
	husart->rx_dma->CCR &= ~(DMA_CCR_MEM2MEM | DMA_CCR_PSIZE | DMA_CCR_DIR );
	husart->rx_dma->CCR |= (DMA_CCR_MINC | DMA_CCR_CIRC);
 
	husart->rx_dma->CCR |= DMA_CCR_EN;
 
	husart->instance->CR3 |= USART_CR3_DMAR;
	husart->instance->CR1 |= USART_CR1_IDLEIE;
 
}
 
static void USART_TX_DMA_Config(usart_handle_t *husart){
	husart->tx_dma->CCR &= ~DMA_CCR_EN;
 
 
	DMA1_CSELR->CSELR &= ~(0xFU << dma_channel_to_cselr_pos(husart->tx_dma));
	DMA1_CSELR->CSELR |= (0x2U<< dma_channel_to_cselr_pos(husart->tx_dma));
 
 
	husart->tx_dma->CCR &= ~(DMA_CCR_MEM2MEM | DMA_CCR_PSIZE);
	husart->tx_dma->CCR |= (DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE);
	husart->tx_dma->CPAR = (uint32_t)&husart->instance->TDR;
 
	husart->instance->CR3 |= USART_CR3_DMAT;
 
	NVIC_EnableIRQ(husart->tx_dma_irq);
	NVIC_SetPriority(husart->tx_dma_irq, 8);
}
 
static void USART_DMA_Init(usart_handle_t * husart){
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	(void)RCC->AHB1ENR;
 
	USART_TX_DMA_Config(husart);
	USART_RX_DMA_Config(husart);
}
 
//--- IRQ Handlers ------------------------------
 
static void USART_IRQ_Handler(usart_handle_t * husart) {
	// TX Empty
	if((husart->instance->ISR & USART_ISR_TXE) && 
	   (husart->instance->CR1 & USART_CR1_TXEIE)) {
		if(husart->tx_index < husart->tx_size) {
			husart->instance->TDR = husart->tx_buffer[husart->tx_index++];
		}
		else{
			husart->instance->CR1 &= ~USART_CR1_TXEIE;
			husart->instance->CR1 |= USART_CR1_TCIE; 
		}
	}
 
	// TC
	if((husart->instance->ISR & USART_ISR_TC) && 
	   (husart->instance->CR1 & USART_CR1_TCIE)) {
 
 
		husart->instance->ICR = USART_ICR_TCCF;
		husart->instance->CR1 &= ~USART_CR1_TCIE; 
		husart->tx_busy = 0;
	}
 
	// RX Not Empty
	if((husart->instance->ISR & USART_ISR_RXNE) &&
	   (husart->instance->CR1 & USART_CR1_RXNEIE)){
 
		uint32_t next_head = (husart->rx_head+1) % husart->rx_size;
 
		if(next_head != husart->rx_tail) {
			husart->rx_buffer[husart->rx_head] = husart->instance->RDR;
			husart->rx_head = next_head;
		} else {
			(void)husart->instance->RDR;
			husart->rx_lost_bytes++;	
		}
	}
 
	// IDLE line
	if((husart->instance->ISR & USART_ISR_IDLE) && 
	   (husart->instance->CR1 & USART_CR1_IDLEIE)){
 
		husart->instance->ICR = USART_ICR_IDLECF;
		husart->rx_ready = 1;
	}
}
 
static void DMA1_IRQHandler(usart_handle_t*husart) {
	//Transfer Complete
	uint32_t tx_channel = dma_channel_to_number(husart->tx_dma);
 
	if(DMA1->ISR & (1U << ((tx_channel -1U)*4U + 1U))){
		DMA1->IFCR = (1U << ((tx_channel -1U)*4U + 1U));
		husart->tx_dma->CCR &= ~(DMA_CCR_EN);
		husart->instance->CR1 |= USART_CR1_TCIE;
	}
 
}
 
void DMA1_CH4_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[0]);
}
 
void DMA1_CH7_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[1]);
}
 
void DMA1_CH2_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[2]);
}
 
void USART1_IRQHandler(void){
	USART_IRQ_Handler(usart_table[0]); 
}
 
void USART2_IRQHandler(void){ 
	USART_IRQ_Handler(usart_table[1]); 
}
 
void USART3_IRQHandler(void){
	USART_IRQ_Handler(usart_table[2]); 
}
 
void DMA1_CH6_IRQHandler(){
	while(1);
}
 
 
void USART_Handle_Init(usart_handle_t*husart, USART_TypeDef* instance, usart_mode_t mode, uint8_t* rx_buffer, uint32_t rx_size)  {
	husart->instance = instance;
	husart->rx_buffer = rx_buffer;
	husart->rx_size = rx_size;
	husart->rx_head = 0;
	husart->rx_dma_head = 0;
	husart->rx_tail = 0;
	husart->rx_ready = 0;
	husart->rx_lost_bytes = 0;
	husart->mode = mode;
	husart->tx_busy = 0;
	if(instance == USART1) {
		usart_table[0] = husart;
		husart->tx_dma = DMA1_Channel4;
		husart->rx_dma = DMA1_Channel5;
		husart->tx_dma_irq = DMA1_Channel4_IRQn; 
	} else if(instance == USART2) {
		usart_table[1] = husart;
		husart->tx_dma = DMA1_Channel7;
		husart->rx_dma = DMA1_Channel6;
		husart->tx_dma_irq = DMA1_Channel7_IRQn; 
	} else if(instance == USART3) {
		usart_table[2] = husart;
		husart->tx_dma = DMA1_Channel2;
		husart->rx_dma = DMA1_Channel3;
		husart->tx_dma_irq = DMA1_Channel2_IRQn; 
	}
	if(mode == USART_MODE_INTERRUPT) {
		USART_Init(instance);
		instance->CR1 |= USART_CR1_RXNEIE;
	}
	else if(mode == USART_MODE_DMA){
		USART_Init(instance);
		USART_DMA_Init(husart);		
	}
}

