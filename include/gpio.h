#ifndef GPIO_H
#define GPIO_H

#include "stm32l476xx.h"
#include <stdint.h>

typedef enum {
	GPIO_MODE_INPUT = 0x0,
	GPIO_MODE_OUTPUT = 0x1,
	GPIO_MODE_AF = 0x2,
	GPIO_MODE_ANALOG = 0x3
} gpio_mode_t;

typedef enum {
	GPIO_PULL_NONE = 0x0,
	GPIO_PULL_UP = 0x1,
	GPIO_PULL_DOWN = 0x2
} gpio_pull_t;

typedef enum {
	GPIO_SPEED_LOW = 0x0,
	GPIO_SPEED_MEDIUM = 0x1,
	GPIO_SPEED_HIGH = 0x2,
	GPIO_SPEED_VERY_HIGH = 0x3
} gpio_speed_t;

typedef enum {
	GPIO_OTYPE_PUSH_PULL = 0x0,
	GPIO_OTYPE_OPEN_DRAIN = 0x1
} gpio_otype_t;

typedef enum {
	GPIO_OK = 0,
	GPIO_ERROR_INVALID_PIN,
	GPIO_ERROR_INVALID_AF,
	GPIO_ERROR_VALUE_NULLPTR
} gpio_status_t;


static inline uint32_t GPIO_Is_Valid_Pin(uint32_t pin){
	return (pin < 16U);
}

static inline uint32_t GPIO_Is_Valid_AF(uint32_t af){
	return (af <16U);
}

void GPIO_Enable_RCC(GPIO_TypeDef* port);
void GPIO_Disable_RCC(GPIO_TypeDef* port);

gpio_status_t GPIO_Enable_Output(GPIO_TypeDef* port, uint32_t pin, gpio_otype_t otype, gpio_pull_t pull, gpio_speed_t speed);
gpio_status_t GPIO_Enable_Input(GPIO_TypeDef* port, uint32_t pin,  gpio_pull_t pull);
gpio_status_t GPIO_Enable_AF(GPIO_TypeDef* port, uint32_t pin, uint32_t af,  gpio_pull_t pull, gpio_speed_t speed);

gpio_status_t GPIO_Read_Pin(GPIO_TypeDef* port, uint32_t pin, uint32_t* value);
gpio_status_t GPIO_Write_Pin(GPIO_TypeDef* port, uint32_t pin, uint32_t value);
gpio_status_t GPIO_Toggle_Pin(GPIO_TypeDef* port, uint32_t pin);

uint32_t GPIO_Is_High(GPIO_TypeDef*port, uint32_t pin);
uint32_t GPIO_Is_Low(GPIO_TypeDef*port, uint32_t pin);

#endif
