ENTRY(_start)

SECTIONS
{
    /* offset */
    . = 2M;

    /* multiboot header + text section */
	.text BLOCK(4K) : ALIGN(4K)
	{
		. = ALIGN(8);
		KEEP(*(.multiboot)) /* align and keep multiboot header as a struct at the beginning of file */
		*(.text)
	}

	/* read-only data */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}