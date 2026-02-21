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
void NMI_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)      __attribute__((weak, alias("Default_Handler")));

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
};
void Default_Handler(void) {
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
