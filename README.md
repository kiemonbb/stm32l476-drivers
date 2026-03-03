# STM32L476xx Peripheral Library

![MCU](https://img.shields.io/badge/MCU-STM32L476xx-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white)
![Architecture](https://img.shields.io/badge/Architecture-ARM%20Cortex--M4-00979D?style=flat-square)
![Toolchain](https://img.shields.io/badge/Toolchain-arm--none--eabi--gcc-A22846?style=flat-square&logo=gnu&logoColor=white)
![Language](https://img.shields.io/badge/Language-C-A8B9CC?style=flat-square&logo=c&logoColor=white)

## Overview

A lightweight bare-metal peripheral drivers library for **STM32L476xx** built with the usage of CMSIS, to avoid HAL's bloat.

## Simple Examples

### GPIO Blink
```c
	GPIO_Enable_Output(GPIOA,5,GPIO_OTYPE_PUSH_PULL,GPIO_PULL_UP,GPIO_SPEED_LOW);

	while(1) {
		GPIO_Toggle_Pin(GPIOA, 5);
		delay_ms(1000);
	}

```
Full example: [examples/gpio_blink/](examples/gpio_blink/)
### USART DMA
```c
uint8_t rx_buffer[64];
usart_handle_t husart2;

USART_Handle_Init(&husart2, USART2, USART_MODE_DMA, rx_buffer, sizeof(rx_buf));

uint8_t msg[] = "USART2\r\n";
USART_Transmit_DMA(&husart2, msg, sizeof(msg));

uint8_t data[8];
uint32_t received = USART_Read_DMA(&husart2, data, sizeof(data));
```
Full example: [examples/usart_send/](examples/usart_send/)

## Features

| Peripheral | Status | Notes |
|:-----------|:------:|-------|
| GPIO | ✅ Ready | input, output, af|
| USART | ✅ Ready | polling, interrupt, DMA |
| SysTick | ✅ Ready | 1ms tick, delay support |
| System Init | ✅ Ready | Clock config, startup code |

## Requirements
* arm-none-eabi-gcc
* arm-none-eabi-objcopy
* make
* openocd

## Targets

| Command | Description |
|:--------|-------------|
| `make` | Build `gpio_blink` |
| `make example_name` | Build *example_name* |
| `make flash` | Flash `gpio_blink.elf` with OpenOCD |
| `make flash-example_name` | Flash *example_name*  with OpenOCD |
| `make clean` | Clean the build files |

## License
This project is licensed under the MIT License.
