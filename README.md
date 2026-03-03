# STM32L476xx Peripheral Library

![MCU](https://img.shields.io/badge/MCU-STM32L476xx-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white)
![Architecture](https://img.shields.io/badge/Architecture-ARM%20Cortex--M4-00979D?style=flat-square)
![Toolchain](https://img.shields.io/badge/Toolchain-arm--none--eabi--gcc-A22846?style=flat-square&logo=gnu&logoColor=white)
![Language](https://img.shields.io/badge/Language-C-A8B9CC?style=flat-square&logo=c&logoColor=white)

## Overview

A lightweight bare-metal peripheral drivers library for **STM32L476xx** built with the usage of CMSIS, to avoid HAL's bloat.

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

### Targets

| Command | Description |
|:--------|-------------|
| `make` | Build `gpio_blink` |
| `make example_name` | Build *example_name* |
| `make flash` | Flash `gpio_blink.elf` with OpenOCD |
| `make flash-example_name` | Flash *example_name*  with OpenOCD |
| `make clean` | Clean the build files |
