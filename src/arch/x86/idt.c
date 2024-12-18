#include <idt.h>

#include <stdint.h>
#include <lib/memory.h>
static idt_t idt[256] = { 0 };

extern uint64_t isr_functions[];

void idt_init() {
	memset(idt, 0, sizeof(idt));
	idtr_t idtr;

	idtr.size = sizeof(idt) - 1;
	idtr.offset = (uint64_t)idt;

	asm __volatile__("lidt %0" : : "m" (idtr));

	for(uint32_t i = 0; i < 33; i++) {
		idt[i].segment = 0x28;
		idt[i].attributes = 0x8e;
		idt[i].offset_hig = isr_functions[i] >> 32;
		idt[i].offset_mid = (isr_functions[i] >> 16) & 0xffff;
		idt[i].offset_low = isr_functions[i] & 0xffff;
	}

}
