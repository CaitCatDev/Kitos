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
uint64_t pt3[512] __attribute__((aligned(0x1000)));

int vmm_init() {
	pd[2] = X86_PAE_PAGE_PRESENT | 3 | (uint64_t)VIRT_TO_PHYS(pt3);
	kprintf("PT3 %p\n", VIRT_TO_PHYS(pt3));
	return 0;
}

/* TODO: when PML4 Entries or other Entries are missing we should
 * get memory from the PMM map it and then map that memory as the 
 * missing entry
 */
int mmap_page(void *phy, void *virt) {
	uint16_t pml4_off, pdp_off, pd_off, pt_off;
	uint64_t phy64, virt64;

	phy64 = (uint64_t)phy;
	virt64 = (uint64_t)virt;

	pml4_off = virt64 >> 39 & 0x1ff;
	pdp_off = virt64 >> 30 & 0x1ff;
	pd_off = virt64 >> 21 & 0x1ff;
	pt_off = virt64 >> 12 & 0x1ff;
	kprintf("%p->%p\n", phy, virt);
	kprintf("PML4: %d, PDP %d, PD %d, PT %d\n",
			pml4_off, pdp_off, pd_off, pt_off);
	
	if(!(pml4[pml4_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();
		if(new == NULL) kprintf("Alloc Failed\n");
		mmap_page(new, PHYS_TO_VIRT(new));

		memset(PHYS_TO_VIRT(new), 0, 0x1000);

		pml4[pml4_off] = (uint64_t)new | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;	
	}
	uint64_t *lpdp = (uint64_t*)(pml4[pml4_off] & X86_64_PAE_ADDRESS_MASK);
	lpdp = PHYS_TO_VIRT(lpdp);
	kprintf("PDP Address: %p\n", lpdp);
	if(!(lpdp[pdp_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();
		mmap_page(new, PHYS_TO_VIRT(new));
		memset(PHYS_TO_VIRT(new), 0, 0x1000);
		lpdp[pdp_off] = (uint64_t)new  | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;
	}

	uint64_t *lpd = (uint64_t *)(lpdp[pdp_off] & X86_64_PAE_ADDRESS_MASK);
	lpd = PHYS_TO_VIRT(lpd);
	kprintf("PD Address: %p\n", lpd);
	if(!(lpd[pd_off] & X86_PAE_PAGE_PRESENT)) {
		void *new = pmm_allocate_block();
		mmap_page(new, PHYS_TO_VIRT(new));
		memset(PHYS_TO_VIRT(new), 0, 0x1000);
		lpd[pd_off] = (uint64_t)new  | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;
	}
	
	uint64_t *lpt = (uint64_t *)(lpd[pd_off] & X86_64_PAE_ADDRESS_MASK);
	lpt = PHYS_TO_VIRT(lpt);
	kprintf("PT Address %p\n", lpt);
	lpt[pt_off]	= (phy64 & X86_64_PAE_ADDRESS_MASK) | X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR;


	return 0;
}
