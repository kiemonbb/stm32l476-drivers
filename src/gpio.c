#include "gpio.h"
#include "stm32l476xx.h"


void GPIO_Enable_RCC(GPIO_TypeDef *port) {
	if(port == GPIOA) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	else if(port == GPIOB) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	else if(port == GPIOC) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	else if(port == GPIOD) RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	else if(port == GPIOE) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	else if(port == GPIOF) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
	else if(port == GPIOG) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
	else if(port == GPIOH) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;

	(void)RCC->AHB2ENR;
}

void GPIO_Disable_RCC(GPIO_TypeDef * port) {
	if(port == GPIOA) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOAEN;
	else if(port == GPIOB) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOBEN;
	else if(port == GPIOC) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOCEN;
	else if(port == GPIOD) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIODEN;
	else if(port == GPIOE) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOEEN;
	else if(port == GPIOF) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOFEN;
	else if(port == GPIOG) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOGEN;
	else if(port == GPIOH) RCC->AHB2ENR &= ~RCC_AHB2ENR_GPIOHEN;

	(void)RCC->AHB2ENR;
}

gpio_status_t GPIO_Enable_Output(GPIO_TypeDef *port, uint32_t pin,gpio_otype_t otype, gpio_pull_t pull, gpio_speed_t speed){

	if(!GPIO_Is_Valid_Pin(pin)){
		return GPIO_ERROR_INVALID_PIN;
	}

	GPIO_Enable_RCC(port);

	port->MODER &= ~(0x3U << (pin*2));
	port->MODER |= (GPIO_MODE_OUTPUT << (pin*2));

	port->OTYPER &= ~(0x1U << (pin*2));
	port->OTYPER |= (otype << (pin*2));
	
	port->OSPEEDR &= ~(0x3U << (pin*2));
	port->OSPEEDR |= (speed << (pin*2));

	port->PUPDR &= ~(0x3U << (pin*2));
	port->PUPDR |= (pull <<(pin*2));

	return GPIO_OK;
}

gpio_status_t GPIO_Enable_Input(GPIO_TypeDef *port, uint32_t pin, gpio_pull_t pull){

	if(!GPIO_Is_Valid_Pin(pin)){
		return GPIO_ERROR_INVALID_PIN;
	}

	GPIO_Enable_RCC(port);

	port->MODER &= ~(0x3U << (pin*2));
	port->MODER |= (GPIO_MODE_INPUT << (pin*2));

	port->PUPDR &= ~(0x3U << (pin*2));
	port->PUPDR |= (pull <<(pin*2));

	return GPIO_OK;
}

gpio_status_t GPIO_Enable_AF(GPIO_TypeDef *port, uint32_t pin, uint32_t af, gpio_pull_t pull, gpio_speed_t speed){

	if(!GPIO_Is_Valid_Pin(pin)){
		return GPIO_ERROR_INVALID_PIN;
	}
	if(!GPIO_Is_Valid_AF(af)){
		return GPIO_ERROR_INVALID_AF;
	}

	GPIO_Enable_RCC(port);

	port->MODER &= ~(0x3U << (pin*2));
	port->MODER |= (GPIO_MODE_AF << (pin*2));
	
	port->OSPEEDR &= ~(0x3U << (pin*2));
	port->OSPEEDR |= (speed << (pin*2));

	port->PUPDR &= ~(0x3U << (pin*2));
	port->PUPDR |= (pull <<(pin*2));

	if(pin <8){
		port->AFR[0] &= ~(0xFU <<(pin*4));
		port->AFR[0] |= (af << (pin*4));
	}
	else{
		uint32_t shift = (pin - 0x8U) * 0x4U;
		port->AFR[1] &= ~(0xFU << shift);
		port->AFR[1] |= (af << shift);
	}
	return GPIO_OK;
}

gpio_status_t GPIO_Read_Pin(GPIO_TypeDef* port, uint32_t pin, uint32_t *value){
	if(!GPIO_Is_Valid_Pin(pin)) {
		return GPIO_ERROR_INVALID_PIN;
	}
	if(value == (void*)0){
		return GPIO_ERROR_VALUE_NULLPTR;
	}


	*value = ((port->IDR >> pin) & 0x1U);
	return GPIO_OK;
}

gpio_status_t GPIO_Write_Pin(GPIO_TypeDef* port, uint32_t pin, uint32_t value){
	if(!GPIO_Is_Valid_Pin(pin)) {
		return GPIO_ERROR_INVALID_PIN;
	}

	if(value){
		port->BSRR = (0x1U << pin);
	}
	else{
		port->BSRR = (0x1U << (pin+16U));
	}

	return GPIO_OK;
}

uint32_t GPIO_Is_High(GPIO_TypeDef*port, gpio_status_t pin) {
	uint32_t value;
	if(GPIO_Read_Pin(port,pin, &value) !=GPIO_OK){
		return -1;
	}
	return (value != 0x0U);
}

uint32_t GPIO_Is_Low(GPIO_TypeDef*port, gpio_status_t pin) {
	uint32_t value;
	if(GPIO_Read_Pin(port,pin, &value) !=GPIO_OK){
		return -1;
	}
	return (value == 0x0U);
}
