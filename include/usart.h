#ifndef USART_H
#define USART_H

#include "stm32l476xx.h"
#include <stdint.h>

void USART_Init(USART_TypeDef* usart);

void USART_Write_Char(USART_TypeDef*usart, char c);

void USART_Write_String(USART_TypeDef*usart, const char *str);

void USART_Write_Int(USART_TypeDef*usart, uint32_t num);

void USART1_IRQHandler(void);

void USART2_IRQHandler(void);

void USART3_IRQHandler(void);

#endif
