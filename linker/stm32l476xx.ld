ENTRY( Reset_Handler )

_estack = ORIGIN(SRAM1) + LENGTH(SRAM1);   
_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

MEMORY 
{
	FLASH (rx): ORIGIN = 0x08000000, LENGTH = 1024K
	SRAM1 (rwx): ORIGIN = 0x20000000, LENGTH = 96K
	SRAM2 (rwx): ORIGIN = 0x10000000, LENGTH = 32K
}

SECTIONS
{
	.isr_vector :
	{
		. = ALIGN(4);

		KEEP(*(.isr_vector))

		. = ALIGN(4);

	} >FLASH

	.text :
	{
		. = ALIGN(4);

		*(.text)
		*(.text*)
		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		_etext = .;
	} >FLASH

	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)

		. = ALIGN(4);
	} >FLASH

	_sidata = LOADADDR(.data);
	.data :
	{
		. = ALIGN(4);
		_sdata = .;

		*(.data)
		*(.data*)

		. = ALIGN(4);
		_edata = .;



	} >SRAM1 AT>FLASH

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;
		_bss_start__ = _sbss;

		*(.bss)
		*(.bss*)

		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = _ebss;
	} > SRAM1

	._user_heap_stack :
	{
		. = ALIGN(4);
	
		PROVIDE ( end = . );
		PROVIDE ( _end = . );
		. = . + _Min_Heap_Size;
		. = . + _Min_Stack_Size;
	
		. = ALIGN(4);
	} > SRAM1
}

