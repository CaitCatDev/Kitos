#pragma once

#include <stdint.h>
#include <boot/multiboot2.h>

/*We use a bitmap of free/used blocks*/
#define PMM_BLOCK_SIZE 0x1000
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

void pmm_reserve_range(void *start, void *end);
void pmm_init_basic(uint32_t lower, uint32_t upper);
void pmm_init_mb2_mmap(multiboot2_boot_info_t *bi, multiboot2_mmap_t *mmap);
