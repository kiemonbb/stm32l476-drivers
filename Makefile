CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CMSIS_SRC = cmsis/device/startup_stm32l476xx.c cmsis/device/system_stm32l4xx.c

CMSIS_OBJ = cmsis/device/startup_stm32l476xx.o cmsis/device/system_stm32l4xx.o
GPIO_OBJ = src/gpio.o

CFLAGS =	-mcpu=cortex-m4 
CFLAGS +=	-DSTM32L476xx 
CFLAGS +=	-Icmsis/core 
CFLAGS +=	-Icmsis/device 
CFLAGS +=	-Iinclude 
CFLAGS +=	-mthumb 
CFLAGS +=	-mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS +=	-g -O2 

LINKER_FILE = linker/stm32l476xx.ld
LDFLAGS = -T $(LINKER_FILE) --specs=nosys.specs -nostartfiles


PROGRAMMER = openocd
PROGRAMMER_FLAGS = -f interface/stlink.cfg -f target/stm32l4x.cfg

.PHONY: all clean flash flash-% gpio_blink

all: gpio_blink

gpio_blink: gpio_blink.elf

gpio_blink.elf: $(CMSIS_OBJ) $(GPIO_OBJ) examples/gpio_blink/main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@


flash: flash-gpio_blink

flash-%: %.elf
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -c "program $< verify reset exit"

clean:
	rm -f $(CMSIS_OBJ) $(GPIO_OBJ) $(wildcard examples/**/main.o) *.elf *.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
