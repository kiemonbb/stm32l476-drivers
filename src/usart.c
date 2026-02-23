#include "usart.h"
#include "gpio.h"
#include "stm32l476xx.h"
#include "system_stm32l4xx.h"
#include <stdint.h>

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
}

static void USART3_Config(void){
	GPIO_Enable_AF(GPIOC, 10, 7UL, GPIO_PULL_NONE,GPIO_SPEED_HIGH);
	GPIO_Enable_AF(GPIOC, 11, 7UL, GPIO_PULL_UP,GPIO_SPEED_HIGH);

	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	(void)RCC->APB1ENR1;

	NVIC_EnableIRQ(USART3_IRQn);
}

static void USART_Config(USART_TypeDef* usart) {
	if(usart == USART1) USART1_Config();
	else if(usart == USART2) USART2_Config();
	else if(usart == USART3) USART3_Config();
}


void USART_Init(USART_TypeDef* usart) {
	USART_Config(usart);

	usart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

	usart->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);

	usart->CR1 &= ~USART_CR1_PCE;

	usart->CR2 &= ~USART_CR2_STOP;

	//TODO: Take prescalers into account
	usart->BRR = (uint32_t)(SystemCoreClock/115200);

	usart->CR1 |= USART_CR1_UE;
}

void USART_Write_Char(USART_TypeDef*usart, char c) {
	while(!(usart->ISR & USART_ISR_TXE));
	usart->TDR = c;
}

void USART_Write_String(USART_TypeDef*usart, const char *str) {
	while (*str){
		USART_Write_Char(usart, *str++);
	}
}

void USART_Write_Int(USART_TypeDef*usart, uint32_t num) {
	char buffer[11];
	int i = 10;

	buffer[i--] = '\0';

	if(num == 0) {
		USART_Write_Char(usart, '0');
		return;
	}
	
	while(num > 0 && i >= 0){
		buffer[i--] = (num%10) + '0';
		num /= 10;
	}

	USART_Write_String(usart, &buffer[i+1]);
}

static void USART_IRQ_Handler(USART_TypeDef* usart) {
	if(usart->ISR & USART_ISR_RXNE) {
		volatile char data = usart->RDR;
	}
}

void USART1_IRQHandler(void){ USART_IRQ_Handler(USART1); }

void USART2_IRQHandler(void){ USART_IRQ_Handler(USART2); }

void USART3_IRQHandler(void){ USART_IRQ_Handler(USART3); }
