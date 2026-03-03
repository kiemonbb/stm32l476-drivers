#include "usart.h"
#include "gpio.h"
#include "stm32l476xx.h"
#include "system_stm32l4xx.h"
#include <stdint.h>
 
#define USART_MAX_INSTANCES 3
static usart_handle_t *usart_table[USART_MAX_INSTANCES] = {0};
 
extern volatile uint32_t systick_ms;
 
static void USART1_Config(void){
	/* Enable Receiver and Transmitter GPIO pins*/
	GPIO_Enable_AF(GPIOA, 9, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOA, 10, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	/* Enable GPIO Clock */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	(void)RCC->APB2ENR;
 
	NVIC_EnableIRQ(USART1_IRQn);
}
 
static void USART2_Config(void){
	/* Enable Receiver and Transmitter GPIO pins*/
	GPIO_Enable_AF(GPIOA, 2, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOA, 3, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	/* Enable GPIO Clock */
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	(void)RCC->APB1ENR1;
 
	NVIC_EnableIRQ(USART2_IRQn);
}
 
static void USART3_Config(void){
	/* Enable Receiver and Transmitter GPIO pins*/
	GPIO_Enable_AF(GPIOC, 10, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOC, 11, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);
 
	/* Enable GPIO Clock */
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
	/* Init the low level hardware: GPIO, CLOCK */
	USART_Config(instance);
 
	/* Disable the USART Peripheral */
	instance->CR1 &= ~USART_CR1_UE;
 
	/* Set the word size to 8 bits and disable the parity bit */
	instance->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1 | USART_CR1_PCE);
 
	/* Enable 1 stop bit */
	instance->CR2 &= ~USART_CR2_STOP;
 
	/* Set the Baud Rate (TODO: Take prescalers into account) */
	instance->BRR = (uint32_t)(SystemCoreClock/115200);
 
	/* Enable the USART Parity Error (PCE) and Error (EIE) interrupts */
	instance->CR1 |= USART_CR1_PEIE;
	instance->CR3 |= USART_CR3_EIE;

	/* Enable the USART Peripheral */
	instance->CR1 |= USART_CR1_UE;
 
	/* Enable Transmitter and Receiver */
	instance->CR1 |= USART_CR1_TE | USART_CR1_RE;
	while(!(instance->ISR & USART_ISR_TEACK));
	while(!(instance->ISR & USART_ISR_REACK));
}
 
static void USART_IT_Init(USART_TypeDef*instance) {
	USART_Init(instance);
 
	/* Enable the USART Data Register Not Empty Interrupt */
	instance->CR1 |= USART_CR1_RXNEIE;
}

usart_status_t USART_Transmit(usart_handle_t* husart, const uint8_t * data, uint32_t byte_size, uint32_t timeout_ms) {
	/* Check whether USART TX line is ready */
	if(husart->tx_state == USART_STATE_BUSY) return USART_ERROR_BUSY;
	husart->tx_state = USART_STATE_BUSY;
 
	uint32_t i = 0;
	while(i < byte_size){
		uint32_t current_ms = systick_ms;
		/* Wait until Transmit Data Register (TDR) is empty */
		while(!(husart->instance->ISR & USART_ISR_TXE)) {	
			/* Check timeout condition */
			if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0){
				husart->tx_state = USART_STATE_READY;
				return USART_ERROR_TIMEOUT;
			}	
		}
		/* Load data into TDR and increment index */
		husart->instance->TDR = data[i++];
	}
 
	uint32_t current_ms = systick_ms;
	/* Wait until Transmission Complete flag is set to ensure all bits are shifted out */
	while(!(husart->instance->ISR & USART_ISR_TC)) {	
		/* Check timeout condition */
		if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0) {
			husart->tx_state = USART_STATE_READY;
			return USART_ERROR_TIMEOUT;
		}
	}
 
	husart->tx_state = USART_STATE_READY;
	return USART_OK;
}
 
usart_status_t USART_Receive(usart_handle_t*husart,uint8_t *buffer, uint32_t byte_size, uint32_t timeout_ms) {
	/* Check whether USART RX line is ready */
	if(husart->rx_state == USART_STATE_BUSY) return USART_ERROR_BUSY;
	husart->rx_state = USART_STATE_BUSY;

	uint32_t i = 0;
	while(i < byte_size){
		uint32_t current_ms = systick_ms;
		/* Wait until data comes into Receive Data Register (RDR) */
		while(!(husart->instance->ISR & USART_ISR_RXNE)) {	
			/* Check the timeout condition */
			if(systick_ms-current_ms >= timeout_ms || timeout_ms == 0) {
				husart->rx_state = USART_STATE_READY;
				return USART_ERROR_TIMEOUT;
			}
		}

		/* Load data from RDR onto the buffer */
		buffer[i++] = husart->instance->RDR;
	}

	husart->rx_state = USART_STATE_READY;
	return USART_OK;
}
 
usart_status_t USART_Transmit_IT(usart_handle_t*husart, const uint8_t*data,uint32_t byte_size) {
	/* Check whether USART TX line is ready */
	if(husart->tx_state == USART_STATE_BUSY) return USART_ERROR_BUSY;
	husart->tx_state = USART_STATE_BUSY;
 
	/* Configure the handle parameters responsible for Interrupt Transmission */
	husart->tx_buffer = data;
	husart->tx_size = byte_size;
	husart->tx_index = 0;
 
	/* Enable the USART Transmit Data Register Empty Interrupt */
	husart->instance->CR1 |= USART_CR1_TXEIE;
	return USART_OK;
}
 
uint32_t USART_Read_IT(usart_handle_t* husart, uint8_t *buffer, uint32_t byte_size) {
	/* Save the rx_head in case of USART RXNE interrupt happening and modifying it */
	husart->instance->CR1 &= ~USART_CR1_RXNEIE;
    uint32_t head = husart->rx_head;
    husart->instance->CR1 |= USART_CR1_RXNEIE;
 
    uint32_t count = 0;
    while(count < byte_size && husart->rx_tail != head) {
        buffer[count++] = husart->rx_buffer[husart->rx_tail];
        husart->rx_tail = (husart->rx_tail + 1) % husart->rx_size;
    }
    return count;
}
 
uint32_t USART_Read_DMA(usart_handle_t* husart, uint8_t*buffer,uint32_t byte_size){
	/* Get the current DMA transfer count to determine how many bytes were reveived */
	uint32_t remaining = husart->rx_dma->CNDTR;
	uint32_t new_head = husart->rx_size - remaining;
 
	/* Return if no new data reveived */
	if(new_head == husart->rx_dma_head) return 0;
 
	/* Calculate total bytes received, accounting for circular buffer */
    uint32_t received;
    if(new_head > husart->rx_dma_head) {
        received = new_head - husart->rx_dma_head;
    } else {
        received = (husart->rx_size - husart->rx_dma_head) + new_head;
    }
 
	/* Cap the received size to the user provided value */
	if(received > byte_size) received = byte_size;
 
	/* Copy data from the internal DMA circular buffer onto the user provided one */
	uint32_t i =0;
	while(i < received){
		buffer[i] = husart->rx_buffer[(husart->rx_dma_head+i)%husart->rx_size];
		i++;
	}
	/* Update the DMA RX head pointer to the new position */
	husart->rx_dma_head = (husart->rx_dma_head+received)%husart->rx_size;
	return received;
}
 
usart_status_t USART_Transmit_DMA(usart_handle_t * husart, const uint8_t*data,uint32_t byte_size){
	/* Check whether USART TX line is ready */
	if(husart->tx_state == USART_STATE_BUSY) return USART_ERROR_BUSY;
	husart->tx_state = USART_STATE_BUSY;
 
	husart->tx_buffer = data;
	husart->tx_size = byte_size;
 
	/* Disable the DMA TX Channel */
	husart->tx_dma->CCR &= ~(DMA_CCR_EN);
	while(husart->tx_dma->CCR & DMA_CCR_EN);
 
	/* Set the DMA Memory Address and the number of bytes to transfer */
	husart->tx_dma->CMAR = (uint32_t)data;
	husart->tx_dma->CNDTR = byte_size;
 
	/* Enable the DMA TX Channel */
	husart->tx_dma->CCR |= DMA_CCR_EN;
	return USART_OK;
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
	/* Disable DMA on RX Channel */ 
	husart->rx_dma->CCR &= ~DMA_CCR_EN;
 
	/* Set the DMA Peripheral Address to USARTX Receive Data Register */ 
	husart->rx_dma->CPAR = (uint32_t)&husart->instance->RDR;
 
	/* Set the DMA Memory Address to the provided Buffer */
	husart->rx_dma->CMAR = (uint32_t)husart->rx_buffer;
 
	/* Set the number of data to transfer to size of the Buffer */
	husart->rx_dma->CNDTR = husart->rx_size;
 
	/* Set the corresponding DMA Channel to work in USARTX_RX mode */
	DMA1_CSELR->CSELR &= ~(0xFU << dma_channel_to_cselr_pos(husart->rx_dma));
	DMA1_CSELR->CSELR |= (0x2U<< dma_channel_to_cselr_pos(husart->rx_dma));
 
	/* Disable memory-to-memory mode, set data size of each DMA transfer to 8 bits */
	husart->rx_dma->CCR &= ~(DMA_CCR_MEM2MEM | DMA_CCR_PSIZE | DMA_CCR_MSIZE);
 
	/* Set the data transfer direction to Read from Peripheral */
	husart->rx_dma->CCR &= ~DMA_CCR_DIR;
 
	/* Enable Memory Increment Mode and Circular Mode */
	husart->rx_dma->CCR |= (DMA_CCR_MINC | DMA_CCR_CIRC);
 
	/* Enable DMA on RX Channel */
	husart->rx_dma->CCR |= DMA_CCR_EN;
 
	/* Enable USARTX Receiver DMA Mode */
	husart->instance->CR3 |= USART_CR3_DMAR;
 
	/* Enable the USART IDLE Interrupt */
	husart->instance->CR1 |= USART_CR1_IDLEIE;
}
 
static void USART_TX_DMA_Config(usart_handle_t *husart){
	/* Disable DMA on TX Channel */ 
	husart->tx_dma->CCR &= ~DMA_CCR_EN;
 
	/* Set the corresponding DMA Channel to work in USARTX_TX mode */
	DMA1_CSELR->CSELR &= ~(0xFU << dma_channel_to_cselr_pos(husart->tx_dma));
	DMA1_CSELR->CSELR |= (0x2U<< dma_channel_to_cselr_pos(husart->tx_dma));
 
	/* Disable memory-to-memory mode and set data size of each DMA transfer to 8 bits */
	husart->tx_dma->CCR &= ~(DMA_CCR_MEM2MEM | DMA_CCR_PSIZE| DMA_CCR_MSIZE);
 
	/* Enable memory increment mode and set the data transfer direction to Read from Memory */
	husart->tx_dma->CCR |= (DMA_CCR_MINC | DMA_CCR_DIR);
 
	/* Set the DMA Peripheral Address to USARTX Transmit Data Register */ 
	husart->tx_dma->CPAR = (uint32_t)&husart->instance->TDR;
 
	/* Enable the DMA Transfer Complete Interrupt */
	husart->tx_dma->CCR |= DMA_CCR_TCIE;
 
	/* Enable USARTX Transmission DMA Mode */
	husart->instance->CR3 |= USART_CR3_DMAT;
 
	NVIC_EnableIRQ(husart->tx_dma_irq);
}
 
static void USART_DMA_Init(usart_handle_t * husart){
	USART_Init(husart->instance);
 
	/* Enable DMA1 Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	(void)RCC->AHB1ENR;
 
	/* Configure TX and RX Pins for DMA usage */
	USART_TX_DMA_Config(husart);
	USART_RX_DMA_Config(husart);
}
 
//--- IRQ Handlers ------------------------------
 
static void USART_IRQ_Handler(usart_handle_t * husart) {
	uint8_t byte_lost = 0;

	if(!(husart->instance->ISR & (USART_ISR_ORE | USART_ISR_PE | USART_ISR_NE | USART_ISR_FE))) {
		husart->error = USART_OK;
	}

	/* USART Over-run Error interrupt occurred -------------------------*/
	if(husart->instance->ISR & USART_ISR_ORE) {
		husart->instance->ICR = USART_ICR_ORECF;
		husart->error = USART_ERROR_ORE;
		byte_lost=1;
	}

	/* USART Parity Error interrupt occurred -------------------------*/
	if(husart->instance->ISR & USART_ISR_PE) {
		husart->instance->ICR = USART_ICR_PECF;
		husart->error = USART_ERROR_PE;
		byte_lost=1;
	}

	/* USART Noise Error interrupt occurred -------------------------*/
	if(husart->instance->ISR & USART_ISR_NE) {
		husart->instance->ICR = USART_ICR_NECF;

		/* Discard possibly invalid data */
		(void)husart->instance->RDR;

		husart->error = USART_ERROR_NE;
		byte_lost=1;
	}

	/* USART Frame Error interrupt occurred -------------------------*/
	if(husart->instance->ISR & USART_ISR_FE) {
		husart->instance->ICR = USART_ICR_FECF;

		/* Discard invalid data */
		(void)husart->instance->RDR;

		husart->error = USART_ERROR_FE;
		byte_lost=1;
	}

	/* USART Transmit Data Regiser Empty interrupt occurred ------------*/
	if((husart->instance->ISR & USART_ISR_TXE) && (husart->instance->CR1 & USART_CR1_TXEIE)) {
		if(husart->tx_index < husart->tx_size) {
			/* Load next byte into TDR and increment the index */
			husart->instance->TDR = husart->tx_buffer[husart->tx_index++];
		}
		else{
			/* No data left to transfer: disable USART TXE interrupt and enable TC interrupt */
			husart->instance->CR1 &= ~USART_CR1_TXEIE;
			husart->instance->CR1 |= USART_CR1_TCIE; 
		}
	}
 
	/* USART Transfer Complete interrupt occurred ------------*/
	if((husart->instance->ISR & USART_ISR_TC) && (husart->instance->CR1 & USART_CR1_TCIE)) {
		husart->instance->ICR = USART_ICR_TCCF;
		husart->instance->CR1 &= ~USART_CR1_TCIE; 
		husart->error = USART_OK;
		husart->tx_state = USART_STATE_READY;
	}
 
	/* USART Receive Data Register Not Empty interrupt occurred --------*/
	if((husart->instance->ISR & USART_ISR_RXNE) && (husart->instance->CR1 & USART_CR1_RXNEIE)){
		uint32_t next_head = (husart->rx_head+1) % husart->rx_size;
 
		/* Check whether there is space available in the RX Buffer */
		if(next_head != husart->rx_tail) {
			/* Read incoming data */
			husart->rx_buffer[husart->rx_head] = husart->instance->RDR;
			husart->rx_head = next_head;
		} else {
			/* RX Buffer is full: incoming data is discarded and lost bytes counter must be incremented */
			(void)husart->instance->RDR;
			byte_lost=1;
		}
	}
 
	/* USART IDLE interrupt occurred -----------------------------------*/ 
	if((husart->instance->ISR & USART_ISR_IDLE) && 
	   (husart->instance->CR1 & USART_CR1_IDLEIE)){
 
		husart->instance->ICR = USART_ICR_IDLECF;
		husart->rx_state = USART_STATE_READY;
	}

	/* If byte was lost increment the counter */
	if(byte_lost) {
		husart->rx_lost_bytes++;
	}
}
 
static void DMA1_IRQHandler(usart_handle_t*husart) {
	uint32_t tx_channel = dma_channel_to_number(husart->tx_dma);
 
	/* Check whether the DMA Transfer Complete interrupt flag for the specific channel is set */
	if(DMA1->ISR & (1U << ((tx_channel -1U)*4U + 1U))){

		/* Clear the DMA1 Channel X Transfer Complete interrupt flag */
		DMA1->IFCR = (1U << ((tx_channel -1U)*4U + 1U));

		/* Disable the DMA TX Channel */
		husart->tx_dma->CCR &= ~(DMA_CCR_EN);

		/* Enable the USART Transmission Complete interrupt */
		husart->instance->CR1 |= USART_CR1_TCIE;
	}
 
}
 
void DMA1_CH4_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[USART1_TABLE_POS]);
}
 
void DMA1_CH7_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[USART2_TABLE_POS]);
}
 
void DMA1_CH2_IRQHandler(void) {
	DMA1_IRQHandler(usart_table[USART3_TABLE_POS]);
}
 
void USART1_IRQHandler(void){
	USART_IRQ_Handler(usart_table[USART1_TABLE_POS]); 
}
 
void USART2_IRQHandler(void){ 
	USART_IRQ_Handler(usart_table[USART2_TABLE_POS]); 
}
 
void USART3_IRQHandler(void){
	USART_IRQ_Handler(usart_table[USART3_TABLE_POS]); 
}

//--- Main Entry --------------------------------

usart_status_t USART_Handle_Init(usart_handle_t*husart, USART_TypeDef* instance, usart_mode_t mode, uint8_t* rx_buffer, uint32_t rx_size)  {
 
	/* Check the validity of given parameters */
	if(husart == (void*)0 || rx_buffer == (void*)0 || instance == (void*)0) {
		return USART_ERROR_INVALID_PARAM;	
	}
 
	/* Set the USART communication parameters */
	husart->instance = instance;
	husart->rx_buffer = rx_buffer;
	husart->rx_size = rx_size;
	husart->rx_head = 0;
	husart->rx_dma_head = 0;
	husart->rx_tail = 0;
	husart->rx_state = USART_STATE_READY;
	husart->rx_lost_bytes = 0;
	husart->tx_state = USART_STATE_READY;

	husart->error = USART_OK;
	husart->mode = mode;
	if(instance == USART1) {
		usart_table[USART1_TABLE_POS] = husart;
		husart->tx_dma = DMA1_Channel4;
		husart->rx_dma = DMA1_Channel5;
		husart->tx_dma_irq = DMA1_Channel4_IRQn; 
	} else if(instance == USART2) {
		usart_table[USART2_TABLE_POS] = husart;
		husart->tx_dma = DMA1_Channel7;
		husart->rx_dma = DMA1_Channel6;
		husart->tx_dma_irq = DMA1_Channel7_IRQn; 
	} else if(instance == USART3) {
		usart_table[USART3_TABLE_POS] = husart;
		husart->tx_dma = DMA1_Channel2;
		husart->rx_dma = DMA1_Channel3;
		husart->tx_dma_irq = DMA1_Channel2_IRQn; 
	}
 
	/* Initiiate USART based on provided mode */
	if(mode == USART_MODE_POLLING) {
		USART_Init(instance);
	}
	else if(mode == USART_MODE_INTERRUPT) {
		USART_IT_Init(instance);
	}
	else if(mode == USART_MODE_DMA){
		USART_DMA_Init(husart);		
	}
	return USART_OK;
}

