CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

SRC = $(wildcard src/*.c) cmsis/device/startup_stm32l476xx.c cmsis/device/system_stm32l4xx.c
OBJ = $(SRC:.c=.o)

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

.PHONY: all clean flash

all: firmware.elf firmware.bin

firmware.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

flash: firmware.elf
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -c "program firmware.elf verify reset exit"

clean:
	rm -f $(OBJ) *.elf *.bin
