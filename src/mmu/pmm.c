#include "kstdio.h"
#include <boot/multiboot2.h>

#include <stdint.h>
#include <stdbool.h>
#include <mmu/pmm.h>
#include <mmu/vmm.h>

#include <lib/memory.h>

static uint32_t *bitmap;
static uint64_t bitmap_len;

extern uint64_t _kernel_start;
extern uint64_t _kernel_end;

static bool pmm_init_done;
__attribute__((aligned(PMM_BLOCK_ALIGN)))
static uint8_t page[PMM_BLOCK_SIZE]; /*One page of working memory to init the PMM with*/

void pmm_bitmap_reserve_block(void *base) {
	uint64_t base_ptr = (uint64_t)base;
	uint64_t offset = base_ptr / 0x1000 / 32;
	uint32_t bit = base_ptr / 0x1000 % 32;
	bitmap[offset] |= (1 << bit);
}

void pmm_reserve_range(void *start, void *end) {
	kprintf("Reserving: %p-%p\n", start, end);
	for(void *tmp = start; tmp < end; tmp += 0x1000) {
		pmm_bitmap_reserve_block(tmp);
	}
}

void pmm_bitmap_unreserve_block(void *base) {
	uint64_t base_ptr = (uint64_t)base;
	uint64_t offset = base_ptr / 0x1000 / 32;
	uint32_t bit = base_ptr / 0x1000 % 32;
	bitmap[offset] &= ~(1 << bit);
}

void pmm_unreserve_range(void *start, void *end) {
	kprintf("Freeing: %p-%p\n", start, end);
	for(void *tmp = start; tmp < end; tmp += 0x1000) {
		pmm_bitmap_unreserve_block(tmp);
	}
}

void *pmm_allocate_block() {
	uint64_t i;
	for(i = 0; i < bitmap_len; i++) {
		for(uint32_t j = 0; j < sizeof(uint32_t)*8; j++) {
			if(!(bitmap[i] & (1 << j))) {
				uint64_t out = i * 32 * 0x1000 + (0x1000 * j);
				pmm_bitmap_reserve_block((void*)out);
				return (void*)out;
			}
		}
	}
	return NULL;
}

void pmm_free_block(void *block) {
	pmm_bitmap_unreserve_block(block);
}

void pmm_init_mb2_mmap(multiboot2_boot_info_t *bi, multiboot2_mmap_t *mmap) {
	kprintf("Kernel %p-%p\n", &_kernel_start, &_kernel_end);
	kprintf("BI: %p-%p\n", bi, (void*)bi + bi->size);
	uint64_t highest_entry = 0;
	uint64_t memory_required = 0;
	uint64_t pkstart = (uint64_t)VIRT_TO_PHYS(&_kernel_start);
	uint64_t pkend = (uint64_t)VIRT_TO_PHYS(&_kernel_end);
	uint64_t pbistart = (uint64_t)VIRT_TO_PHYS(bi);

	for(uint32_t i = 0; i < (mmap->hdr.size - sizeof(*mmap)) / mmap->entry_size; i++) {
		if(mmap->entries[i].type == 1 && mmap->entries[i].base_addr > highest_entry) {
			memory_required = mmap->entries[i].base_addr + mmap->entries[i].length;
			memory_required /= 0x1000;
			highest_entry = mmap->entries[i].base_addr;
		}
	}

	kprintf("Page Count %d\n", memory_required);
	kprintf("uint32_t's needed :%d\n", memory_required / 32);
	kprintf("highest_entry: %d\n", highest_entry);	
	bitmap = PHYS_TO_VIRT(0x200000);
	bitmap_len = memory_required / 32;
	memset(bitmap, 0xff, memory_required / 8);

	for(uint32_t i = 0; i <= (mmap->hdr.size - sizeof(*mmap)) / mmap->entry_size; i++) {
		if(mmap->entries[i].base_addr > highest_entry) break;
		if(mmap->entries[i].type == 1) {
			pmm_unreserve_range((void*)mmap->entries[i].base_addr, (void*)mmap->entries[i].base_addr + mmap->entries[i].length);
		} else {
			pmm_reserve_range((void*)mmap->entries[i].base_addr, (void*)mmap->entries[i].base_addr + mmap->entries[i].length);
		}
	}

	pmm_bitmap_reserve_block((void*)0x0000);
	pmm_reserve_range((void*)pkstart, (void*)pkend);
	pmm_reserve_range((void*)pbistart, (void*)pbistart + bi->size);
	pmm_reserve_range(VIRT_TO_PHYS(bitmap), (void*)((uint64_t)VIRT_TO_PHYS(bitmap) + memory_required / 8));
}
