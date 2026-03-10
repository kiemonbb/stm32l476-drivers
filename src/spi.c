#include "spi.h"
#include "gpio.h"
#include "stm32l476xx.h"
 
#define SPI_MAX_INSTANCES 3
static spi_handle_t *spi_table[SPI_MAX_INSTANCES] = {0};
 
extern volatile uint32_t systick_ms;
 
static void SPI1_Config(void){
	/* MOSI */
	GPIO_Enable_AF(GPIOA,7,5UL, GPIO_PULL_UP, GPIO_SPEED_HIGH);
	/* MISO */
	GPIO_Enable_AF(GPIOA,6,5UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
	/* SCK */
	GPIO_Enable_AF(GPIOA,5,5UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
 
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	(void)RCC->APB2ENR;
 
	NVIC_EnableIRQ(SPI1_IRQn);
}
 
static void SPI2_Config(void){
	/* MOSI */
	GPIO_Enable_AF(GPIOB,15,5UL, GPIO_PULL_UP, GPIO_SPEED_HIGH);
	/* MISO */
	GPIO_Enable_AF(GPIOB,14,5UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
	/* SCK */
	GPIO_Enable_AF(GPIOB,10,5UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
 
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
	(void)RCC->APB1ENR1;
 
	NVIC_EnableIRQ(SPI2_IRQn);
}
 
static void SPI3_Config(void){
	/* MOSI */
	GPIO_Enable_AF(GPIOC,12,6UL, GPIO_PULL_UP, GPIO_SPEED_HIGH);
	/* MISO */
	GPIO_Enable_AF(GPIOC,11,6UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
	/* SCK */
	GPIO_Enable_AF(GPIOC,10,6UL, GPIO_PULL_NONE, GPIO_SPEED_HIGH);
 
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
	(void)RCC->APB1ENR1;
 
	NVIC_EnableIRQ(SPI3_IRQn);
}
 
static void SPI_Config(SPI_TypeDef * instance){
	if(instance == SPI1) SPI1_Config();
	else if(instance == SPI2) SPI2_Config();
	else if(instance == SPI3) SPI3_Config();
}
 
static void SPI_Poll_Init(spi_handle_t*hspi){
	SPI_Config(hspi->instance);
 
	hspi->instance->CR1 |= (hspi->baud_divider << SPI_CR1_BR_Pos);
 
	hspi->instance->CR1 |= (hspi->clock_mode | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR);

 
	hspi->instance->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_RXONLY | SPI_CR1_LSBFIRST | SPI_CR1_CRCEN);
 
	hspi->instance->CR2 &= ~SPI_CR2_DS;
	hspi->instance->CR2 |= (hspi->data_size << SPI_CR2_DS_Pos); 
 
	hspi->instance->CR2 &= ~SPI_CR2_FRF;

	if (hspi->data_size == SPI_DATASIZE_8BIT) {
		hspi->instance->CR2 |= SPI_CR2_FRXTH;
	}
	else {
	    hspi->instance->CR2 &= ~SPI_CR2_FRXTH;  
	}
 
	//hspi->instance->CR2 |= (SPI_CR2_ERRIE | SPI_CR2_RXNEIE | SPI_CR2_TXEIE);

	hspi->instance->CR1 |= SPI_CR1_SPE;
}

static spi_status_t SPI_Bind_DMA(spi_handle_t *hspi) {
    if (hspi->instance == SPI1) {
        spi_table[SPI1_TABLE_POS] = hspi;
        hspi->rx_dma     = DMA1_Channel2;
        hspi->tx_dma     = DMA1_Channel3;
        hspi->rx_dma_irq = DMA1_Channel2_IRQn;
        hspi->tx_dma_irq = DMA1_Channel3_IRQn;
    }
    else if (hspi->instance == SPI2) {
        spi_table[SPI2_TABLE_POS] = hspi;
        hspi->rx_dma     = DMA1_Channel4;
        hspi->tx_dma     = DMA1_Channel5;
        hspi->rx_dma_irq = DMA1_Channel4_IRQn;
        hspi->tx_dma_irq = DMA1_Channel5_IRQn;
    }
    else if (hspi->instance == SPI3) {
        spi_table[SPI3_TABLE_POS] = hspi;
        hspi->rx_dma     = DMA2_Channel1;
        hspi->tx_dma     = DMA2_Channel2;
        hspi->rx_dma_irq = DMA2_Channel1_IRQn;
        hspi->tx_dma_irq = DMA2_Channel2_IRQn;
    }
    else {
        return SPI_ERROR_INVALID_PARAM;
    }
    return SPI_OK;
}

static void SPI_Apply_Config(spi_handle_t *hspi, spi_config_t *config) {
	hspi->instance = config->instance;
	hspi->clock_mode = config->clock_mode; 
	hspi->baud_divider = config->baud;
	hspi->data_size = config->data_size;
	hspi->mode = config->mode;
	hspi->error = SPI_OK;
}

spi_status_t SPI_TransmitReceive8(spi_handle_t *hspi, const uint8_t *tx, uint8_t *rx, uint32_t size, uint32_t timeout_ms){
	if(hspi->data_size != SPI_DATASIZE_8BIT) return SPI_ERROR_INVALID_PARAM;

	if(hspi == (void*)0 || rx == (void*)0 || tx == (void*)0 || size == 0){
		return SPI_ERROR_INVALID_PARAM;
	}

	hspi->state = SPI_STATE_BUSY;

	uint32_t deadline = systick_ms + timeout_ms;

	for(uint32_t i = 0; i < size; i++){
		while(!(hspi->instance->SR & SPI_SR_TXE)) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		*(volatile uint8_t *)&hspi->instance->DR = tx[i];

		while(!(hspi->instance->SR & SPI_SR_RXNE)) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		rx[i] = *(volatile uint8_t *)&hspi->instance->DR;
	}

		while (hspi->instance->SR & SPI_SR_BSY) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		
	hspi->state = SPI_STATE_READY;
	return SPI_OK;
}

spi_status_t SPI_TransmitReceive16(spi_handle_t *hspi, const uint16_t *tx, uint16_t *rx, uint32_t size, uint32_t timeout_ms){
	if(hspi->data_size != SPI_DATASIZE_16BIT) return SPI_ERROR_INVALID_PARAM;

	if(hspi == (void*)0 || rx == (void*)0 || tx == (void*)0 || size == 0){
		return SPI_ERROR_INVALID_PARAM;
	}

	hspi->state = SPI_STATE_BUSY;

	uint32_t deadline = systick_ms + timeout_ms;

	for(uint32_t i = 0; i < size; i++){
		while(!(hspi->instance->SR & SPI_SR_TXE)) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		*(volatile uint16_t *)&hspi->instance->DR = tx[i];

		while(!(hspi->instance->SR & SPI_SR_RXNE)) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		rx[i] = *(volatile uint16_t *)&hspi->instance->DR;
	}

		while (hspi->instance->SR & SPI_SR_BSY) {
			if(timeout_ms == SPI_NO_DELAY){
				hspi->state = SPI_STATE_READY;
				return SPI_ERROR_TIMEOUT;
			}	
			 if(timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline) {
                hspi->state = SPI_STATE_READY;
                return SPI_ERROR_TIMEOUT;
            }
		}
		
	hspi->state = SPI_STATE_READY;
	return SPI_OK;
}
 
spi_status_t SPI_Init(spi_handle_t *hspi, spi_config_t *config){
	if(hspi == (void*)0 || config == (void*)0){
		return SPI_ERROR_INVALID_PARAM;
	}

	SPI_Apply_Config(hspi,config);
 
	spi_status_t status = SPI_Bind_DMA(hspi);
	if(status != SPI_OK) return status; 
 
	if(hspi->mode == SPI_MODE_POLLING)			SPI_Poll_Init(hspi);
	//else if(hspi->mode == SPI_MODE_INTERRUPT)	SPI_IT_Init(hspi);
	//else if(hspi->mode == SPI_MODE_DMA)			SPI_DMA_Init(hspi);
 
	return SPI_OK;
}
