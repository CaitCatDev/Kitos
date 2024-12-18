#include <types.h>
#include <stdint.h>

#include <kstdio.h>

#include <mmu/vmm.h>

void isr_handler(uint64_t vector, uint64_t error, void *regs) {
	kprintf("INT: %d(ERR: %x)\n", vector, error);
	if(vector == 0x20) {
		kprintf("Timer\n");
		volatile uint32_t *apic = PHYS_TO_VIRT(0x00000000fee00000);
		apic[0x2c] = 0;
		return;
	}
	while(1) {
		__asm__ volatile("cli");
		__asm__ volatile("hlt");
	}
}
