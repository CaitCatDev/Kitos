#pragma once

#include "kstdio.h"
#include <boot/multiboot2.h>

#include <stdint.h>
#include <mmu/pmm.h>

static uint32_t *bitmap;
static uint64_t bitmap_len;


extern uint64_t _kernel_start;
extern uint64_t _kernel_end;

void pmm_reserve_range(void *start, void *end) {

}

/*Initialize using counts of how many KB's are in conventional
 * and UPPER memory i.e. 
 * lower = KB's free from 0-1MB
 * upper = KB's free from 1MB-Next Gap
 */
void pmm_init_basic(uint32_t lower, uint32_t upper) {

}

void pmm_init_mb2_mmap(multiboot2_boot_info_t *bi, multiboot2_mmap_t *mmap) {
	kprintf("Kernel %p-%p\n", &_kernel_start, &_kernel_end);
	kprintf("BI: %p-%p\n", bi, (void*)bi + bi->size);

	for(uint32_t i = 0; i < (mmap->hdr.size - sizeof(*mmap)) / mmap->entry_size; i++) {
		kprintf("%p-%p %d\n", mmap->entries[i].base_addr, mmap->entries[i].base_addr + mmap->entries[i].length, mmap->entries[i].type);
	}
}
