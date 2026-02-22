#include "system_stm32l4xx.h"
#include <stdint.h>
#define SRAM_START (0x20000000u)
#define SRAM_SIZE (96u * 1024u)
#define SRAM_END ( SRAM_START + SRAM_SIZE)
#define STACK_POINTER_INIT_ADDRESS (SRAM_END)

// Symbols defined by linker script
extern uint32_t _etext, _sidata;
extern uint32_t _sdata,_edata;
extern uint32_t _sbss, _ebss;

void main(void);

void Reset_Handler(void);
void NMI_Handler(void)					__attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)    		__attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)    		__attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)     		__attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)   		__attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)          		__attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)     		__attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)       		__attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)				__attribute__((weak, alias("Default_Handler")));

void WWDG_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void PVD_PVM_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void DMA1_CH1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH6_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_CH7_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void CAN1_TX_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void CAN1_RX0_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler(void)__attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT3_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void ADC3_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void FMC_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void SDMMC1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void DMA2_CH1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_CH2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_CH3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_CH4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_CH5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT0_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT1_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT2_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void COMP_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void LPTIM1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void LPTIM2_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void DMA2_CH6_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_CH7_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void LPUART1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void QUADSPI_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void SAI1_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void SAI2_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void SWPMI1_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TSC_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void LCD_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void RNG_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector")))
void(* const isr_vector[])(void) = {
    (void (*)(void))STACK_POINTER_INIT_ADDRESS,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_PVM_IRQHandler,
    TAMP_STAMP_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_CH1_IRQHandler,
    DMA1_CH2_IRQHandler,
    DMA1_CH3_IRQHandler,
    DMA1_CH4_IRQHandler,
    DMA1_CH5_IRQHandler,
    DMA1_CH6_IRQHandler,
    DMA1_CH7_IRQHandler,
    ADC1_2_IRQHandler,
    CAN1_TX_IRQHandler,
    CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_TIM15_IRQHandler,
    TIM1_UP_TIM16_IRQHandler,
    TIM1_TRG_COM_TIM17_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    DFSDM1_FLT3_IRQHandler,
    TIM8_BRK_IRQHandler,
    TIM8_UP_IRQHandler,
    TIM8_TRG_COM_IRQHandler,
    TIM8_CC_IRQHandler,
    ADC3_IRQHandler,
    FMC_IRQHandler,
    SDMMC1_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    TIM6_DAC_IRQHandler,
    TIM7_IRQHandler,
    DMA2_CH1_IRQHandler,
    DMA2_CH2_IRQHandler,
    DMA2_CH3_IRQHandler,
    DMA2_CH4_IRQHandler,
    DMA2_CH5_IRQHandler,
    DFSDM1_FLT0_IRQHandler,
    DFSDM1_FLT1_IRQHandler,
    DFSDM1_FLT2_IRQHandler,
    COMP_IRQHandler,
    LPTIM1_IRQHandler,
    LPTIM2_IRQHandler,
    OTG_FS_IRQHandler,
    DMA2_CH6_IRQHandler,
    DMA2_CH7_IRQHandler,
    LPUART1_IRQHandler,
    QUADSPI_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    SAI1_IRQHandler,
    SAI2_IRQHandler,
    SWPMI1_IRQHandler,
    TSC_IRQHandler,
    LCD_IRQHandler,
    0,
    RNG_IRQHandler,
    FPU_IRQHandler,
};void Default_Handler(void) {
	while(1);
}
void Reset_Handler(void) {
	uint32_t *src,*dst;
	src = &_sidata;
	dst =  &_sdata;
	while(dst < &_edata) {
		*dst++ = *src++;
	}
	
	dst = &_sbss;
	while(dst < &_ebss) {
		*dst++ = 0;
	}
	SystemInit();
	main();
}
