#ifndef SPI_H
#define SPI_H
 
#include "stm32l476xx.h"
 
#define SPI_NO_DELAY 0x0
#define SPI_MAX_DELAY 0xFFFFFFFF
 
#define SPI1_TABLE_POS 0U
#define SPI2_TABLE_POS 1U
#define SPI3_TABLE_POS 2U
 
typedef enum {
  SPI_BAUD_DIV_2 = 0L,
  SPI_BAUD_DIV_4 = 1L,
  SPI_BAUD_DIV_8 = 2L,
  SPI_BAUD_DIV_16 = 3L,
  SPI_BAUD_DIV_32 = 4L,
  SPI_BAUD_DIV_64 = 5L,
  SPI_BAUD_DIV_128 = 6L,
  SPI_BAUD_DIV_256 = 7L,
} spi_baud_divider_t;
 
typedef enum {
  SPI_CLOCK_MODE_0 = 0, // CPOL=0 CPHA=0
  SPI_CLOCK_MODE_1 = SPI_CR1_CPHA, // CPOL=0 CPHA=1
  SPI_CLOCK_MODE_2 = SPI_CR1_CPOL, // CPOL=1 CPHA=0
  SPI_CLOCK_MODE_3 = SPI_CR1_CPOL | SPI_CR1_CPHA, // CPOL=1 CPHA=1
} spi_clock_mode_t;
 
typedef enum {
  SPI_DATASIZE_8BIT = 7L,
  SPI_DATASIZE_16BIT = 15L
} spi_datasize_t;
 
typedef enum { SPI_MODE_POLLING, SPI_MODE_INTERRUPT, SPI_MODE_DMA } spi_mode_t;
 
typedef enum {
  SPI_OK = 0,
  SPI_ERROR_INVALID_PARAM,
  SPI_ERROR_TIMEOUT,
  SPI_ERROR_BUSY,
} spi_status_t;
 
typedef enum { SPI_STATE_READY, SPI_STATE_BUSY } spi_state_t;
 
typedef struct {
  SPI_TypeDef *instance;
  spi_mode_t mode;
  spi_baud_divider_t baud;
  spi_clock_mode_t clock_mode;
  spi_datasize_t data_size;
} spi_config_t;
 
typedef struct {
  SPI_TypeDef *instance;
  spi_mode_t mode;
  spi_clock_mode_t clock_mode;
  spi_baud_divider_t baud_divider;
  spi_datasize_t data_size;
 
  const void *tx_buffer;
  uint32_t tx_size;
  volatile uint32_t tx_index;
 
  void* *rx_buffer;
  uint32_t rx_size;
  volatile uint32_t rx_index;
 
  volatile spi_state_t state;
  volatile spi_status_t error;
 
  DMA_Channel_TypeDef *rx_dma;
  DMA_Channel_TypeDef *tx_dma;
  IRQn_Type tx_dma_irq;
  IRQn_Type rx_dma_irq;
} spi_handle_t;
 
/* API Functions */
 
spi_status_t SPI_Init(spi_handle_t *hspi, spi_config_t *config);

spi_status_t SPI_TransmitReceive8(spi_handle_t *hspi, const uint8_t *tx,
                                 uint8_t *rx, uint32_t size,
                                 uint32_t timeout_ms);

spi_status_t SPI_TransmitReceive16(spi_handle_t *hspi, const uint16_t *tx,
                                 uint16_t *rx, uint32_t size,
                                 uint32_t timeout_ms);

spi_status_t SPI_Transmit(spi_handle_t *hspi, const uint8_t *tx, uint32_t size,
                          uint32_t timeout_ms);

spi_status_t SPI_Receive(spi_handle_t *hspi, uint8_t *rx, uint32_t size,
                         uint32_t timeout_ms);
 
spi_status_t SPI_TransmitReceive_IT(spi_handle_t *hspi, const uint8_t *tx,
                                    uint8_t *rx, uint32_t size);
 
spi_status_t SPI_TransmitReceive_DMA(spi_handle_t *hspi, const uint8_t *tx,
                                     uint8_t *rx, uint32_t size);
 
void SPI_CS_Low(GPIO_TypeDef *port, uint8_t pin);
void SPI_CS_High(GPIO_TypeDef *port, uint8_t pin);
 
#endif
