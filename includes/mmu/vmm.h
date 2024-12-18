#pragma 

#include <stdint.h>

#include <boot/multiboot2.h>

extern uint64_t KERNEL_VIRT;

/*TODO IMPLEMENT THIS*/
#define PHYS_TO_VIRT(ptr) (void*)((uint64_t)ptr | (uint64_t)&KERNEL_VIRT)
#define VIRT_TO_PHYS(ptr) (void*)((uint64_t)ptr & ~((uint64_t)&KERNEL_VIRT))

#define X86_PAE_PAGE_NOEXEC	(1 << 63)
#define X86_PAE_PAGE_PRESENT (1)
#define X86_PAE_PAGE_RDONLY (0)
#define X86_PAE_PAGE_RDWR		(1 << 1)
#define X86_PAE_PAGE_SUPER (0)
#define X86_PAE_PAGE_USER (1 << 2)
#define X86_PAE_PAGE_NOPWT (0)
#define X86_PAE_PAGE_PWT (1 << 3)
#define X86_PAE_PAGE_UNCACHEABLE (1 << 4)
#define X86_PAE_PAGE_ACCESSED (1 << 5)
#define X86_PAE_PAGE_DIRTY (1 << 6)

#define X86_64_PAE_ADDRESS_MASK 0x000ffffffffff000ULL

typedef uint64_t page_t;


int mmap_page(void *phy, void *virt, uint16_t flags);
void vmm_map_range(void *start, void *end, void *virt, uint16_t flags);
int vmm_init(multiboot2_mmap_t *mmap);


