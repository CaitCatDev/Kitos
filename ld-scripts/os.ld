ENTRY(_start)

KERNEL_VIRT = 0xFFFFFF8000000000;
KERNEL_PHYS = 0x100000;

SECTIONS {
	. = KERNEL_PHYS + KERNEL_VIRT;
	_kernel_start = .;

	.text : AT(ADDR(.text) - KERNEL_VIRT) ALIGN(0x1000) {
		*(.multiboot)
		*(.text)
		. = ALIGN(0x1000);
	}

	.rodata : AT(ADDR(.rodata) - KERNEL_VIRT) ALIGN(0x1000) {
		*(.rodata*)
	}

	.data : AT(ADDR(.data) - KERNEL_VIRT) ALIGN(0x1000) {
		*(.data)
	}

	.bss : AT(ADDR(.bss) - KERNEL_VIRT) ALIGN(0x1000) {
		*(.bss)
	}
	_kernel_end = .;
}
