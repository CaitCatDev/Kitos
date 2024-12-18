#include "boot/multiboot2.h"
#include "kstdio.h"
#include "mmu/pmm.h"
#include <mmu/vmm.h> 
#include <stdint.h>
#include <string.h>

/*Global so asm can set these initially*/
uint64_t pml4[512] __attribute__((aligned(0x1000)));
uint64_t pdp[512] __attribute__((aligned(0x1000)));
uint64_t pd[512] __attribute__((aligned(0x1000)));
uint64_t pt1[512] __attribute__((aligned(0x1000)));
uint64_t pt2[512] __attribute__((aligned(0x1000)));

void vmm_map_range(void *start, void *end, void *virt, uint16_t flags) {
	kprintf("mapping: %p-%p\n", start, end);
	for(void *tmp = start; tmp < end; tmp += 0x1000) {
		mmap_page(tmp, virt, flags);
		virt += 0x1000;
	}
}

int vmm_init(multiboot2_mmap_t *mmap) {

	for(uint32_t i = 0; i < (mmap->hdr.size - sizeof(*mmap)) / mmap->entry_size; i++) {
		
		if(mmap->entries[i].type == 1) {
			vmm_map_range((void*)mmap->entries[i].base_addr, (void*)(mmap->entries[i].base_addr + mmap->entries[i].length), PHYS_TO_VIRT(mmap->entries[i].base_addr), X86_PAE_PAGE_RDWR | X86_PAE_PAGE_PRESENT);
		}
	}

	return 0;
}

static inline int page_present(uint64_t page) {
	return page & X86_PAE_PAGE_PRESENT;
}

/* TODO: when PML4 Entries or other Entries are missing we should
 * get memory from the PMM map it and then map that memory as the 
 * missing entry
 */

static inline void invlpg(void *address) {
	asm volatile ( "invlpg (%0)" : : "b"(address) : "memory" );
	kprintf("Evicted %p from TLB\n", address);
}

int mmap_page(void *phy, void *virt, uint16_t flags) {
	uint16_t pml4_off, pdp_off, pd_off, pt_off;
	uint64_t phy64, virt64;

	phy64 = (uint64_t)phy;
	virt64 = (uint64_t)virt;

	pml4_off = virt64 >> 39 & 0x1ff;
	pdp_off = virt64 >> 30 & 0x1ff;
	pd_off = virt64 >> 21 & 0x1ff;
	pt_off = virt64 >> 12 & 0x1ff;
	
	if(!(pml4[pml4_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();

		if(new == NULL) kprintf("Alloc Failed\n");
		pml4[pml4_off] = (uint64_t)new | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;	
		memset(PHYS_TO_VIRT(new), 0, 0x1000);
		mmap_page(new, PHYS_TO_VIRT(new), X86_PAE_PAGE_RDWR | X86_PAE_PAGE_PRESENT);
	}
	
	uint64_t *lpdp = (uint64_t*)(pml4[pml4_off] & X86_64_PAE_ADDRESS_MASK);
	lpdp = PHYS_TO_VIRT(lpdp);
	if(!(lpdp[pdp_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();
		memset(PHYS_TO_VIRT(new), 0, 0x1000);

		lpdp[pdp_off] = (uint64_t)new  | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;
		mmap_page(new, PHYS_TO_VIRT(new), X86_PAE_PAGE_RDWR | X86_PAE_PAGE_PRESENT);
	}

	uint64_t *lpd = (uint64_t *)(lpdp[pdp_off] & X86_64_PAE_ADDRESS_MASK);
	lpd = PHYS_TO_VIRT(lpd);
	if(!(lpd[pd_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();
		memset(PHYS_TO_VIRT(new), 0, 0x1000);

		lpd[pd_off] = (uint64_t)new  | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;
		mmap_page(new, PHYS_TO_VIRT(new), X86_PAE_PAGE_RDWR | X86_PAE_PAGE_PRESENT);	
	}
	
	uint64_t *lpt = (uint64_t *)(lpd[pd_off] & X86_64_PAE_ADDRESS_MASK);

	lpt = PHYS_TO_VIRT(lpt);
	lpt[pt_off]	= (phy64 & X86_64_PAE_ADDRESS_MASK) | flags;


	return 0;
}
