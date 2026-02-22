#include "system_init.h"
#include "systick.h"
#include "system_stm32l4xx.h"

void System_Init(void) {
	SystemCoreClockUpdate();
	SysTick_Init();
}
