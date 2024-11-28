#include <types.h>
#include <stdint.h>

#include <kstdio.h>

void isr_handler(uint64_t vector, uint64_t error, void *regs) {
	kprintf("INT: %d(ERR: %x)\n", vector, error);

	while(1) {
	}
}
