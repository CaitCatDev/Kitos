#pragma 

#include <stdint.h>

/*TODO IMPLEMENT THIS*/
#define PHYS_TO_VIRT(ptr)
#define VIRT_TO_PHYS(ptr) 

#define X86_PAE_PAGE_NOEXEC	(1 << 63)
#define X86_PAE_PAGE_PRESENT (1)
#define X86_PAE_PAGE_RDONLY (0)
#define X86_PAE_PAGE_RDWR		(1 << 1)
#define X86_PAE_PAGE_SUPER (0)
#define X86_PAE_PAGE_USER (1 << 2)
#define X86_PAE_PAGE_NOPWT (0)
#define X86_PAE_PAGE_PWT (1 << 3)
#define X86_PAE_PAGE_PLCACHE_DISABLE (1 << 4)
#define X86_PAE_PAGE_ACCESSED (1 << 5)
#define X86_PAE_PAGE_DIRTY (1 << 6)


typedef uint64_t page_t;
