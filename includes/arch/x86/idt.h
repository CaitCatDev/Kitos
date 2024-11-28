#pragma once

#include <stdint.h>
#include <types.h>

typedef struct {
 uint16_t offset_low;
 uint16_t segment;
 uint8_t ist;
 uint8_t attributes;
 uint16_t offset_mid;
 uint32_t offset_hig;
 uint32_t reserved;
} __attribute__((packed)) idt_t;

typedef struct {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed)) idtr_t;

void idt_init();
