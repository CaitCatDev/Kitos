#include <boot/multiboot2.h>
#include <stdint.h>
#include <kstdio.h>
#include <types.h>

#include <drivers/serial.h>
#include <drivers/ata.h>
#include <drivers/pci.h>

#include <mmu/vmm.h>
#include <mmu/pmm.h>

#if defined(__x86_64__) || defined(__i386__)
#include <pio.h>
#endif

#include <pci-class.h>

static uint32_t x = 0;
static uint32_t y = 0;

void vga_put_ch(uint8_t ch) {
	volatile char *vga = (void*) 0xFFFFFF80000b8000 + x * 2 + y * 160;

	if(ch == '\n') {
		y++;
		x=0;
	} else {
		*vga = ch;
		x++;
	}
}

void cmain() {
	pci_init();
	
	return;
}

static const char *mmap_type_to_str(uint32_t type) {
	switch(type) {
		case 1:
			return "Free";
		case 2:
			return "Reserved";
		default: return "Unknown";
	}
}

/*Global so asm can set these initially*/
uint64_t pml4[512] __attribute__((aligned(0x1000)));
uint64_t pdp[512] __attribute__((aligned(0x1000)));
uint64_t pd[512] __attribute__((aligned(0x1000)));
uint64_t pt1[512] __attribute__((aligned(0x1000)));
uint64_t pt2[512] __attribute__((aligned(0x1000)));

extern uint64_t KERNEL_VIRT;

void multiboot2_cinit(multiboot2_boot_info_t *info) {
	stdio_init(write_serial);
	if(init_serial(COM1) < 0) {
		stdio_init(vga_put_ch);
	}
	
	kprintf("Hello Multiboot2 world: %p\n", info);

	/* Two ways to break out of this if END tag is missing
	 * Break on going over the size of the multiboot info struct
	 * else break on encountering end tag
	 */
	for(uint32_t i = 0; i < info->size;) {
		multiboot2_tag_t *tag = (multiboot2_tag_t*)&info->tags[i];
		//kprintf("Multiboot 2 tag: %d size(%d)\n", tag->type, tag->size);
		i += (tag->size + (7)) & ~(7);
	
		switch(tag->type) {
			case MULTIBOOT2_CMDLINE_TAG: {
				multiboot2_cmdline_t *cmdline = (multiboot2_cmdline_t*)tag;
				kprintf("\tMultiboot2 Cmdline: %s\n", cmdline->str);
				break;
			}
			case MULTIBOOT2_LDRNAME_TAG: {
				multiboot2_loader_name_t *ldr = (multiboot2_loader_name_t*)tag;
				kprintf("\tBootloader Name: %s\n", ldr->str);
				break;
			}
			case MULTIBOOT2_MODULES_TAG: {
				multiboot2_modules_t *module = (multiboot2_modules_t*)tag;
				break;
			}
			case MULTIBOOT2_MEMINFO_TAG: {
				multiboot2_meminfo_t *meminfo = (multiboot2_meminfo_t*)tag;
				kprintf("\tConventional Memory: %dKB\n\tUpper memory: %dMB\n", meminfo->lower, meminfo->upper / 1024);	
				break;
			}
			case MULTIBOOT2_BOOTDEV_TAG: {
				multiboot2_bios_boot_dev_t *dev = (multiboot2_bios_boot_dev_t*)tag;
				kprintf("\tLegacy Boot Device: %d\n", dev->dev);
				break;
			}
			case MULTIBOOT2_MEMOMAP_TAG: {
				multiboot2_mmap_t *mmap = (multiboot2_mmap_t*)tag;
				kprintf("\tmmap addr: %p\n\tLegacy mmap size: %d, Version: %d\n", mmap, mmap->hdr.size - 0x10, mmap->entry_version);
				break;
			}
			case MULTIBOOT2_VBEINFO_TAG: {
				multiboot2_vbeinfo_t *vbe = (multiboot2_vbeinfo_t*)tag;
				kprintf("\tVBE info found\n");
				break;
			}
			case MULTIBOOT2_FBUINFO_TAG: {
				multiboot2_fbinfo_t *info = (multiboot2_fbinfo_t*)tag;
				kprintf("\tBase: %p\n\tRes: %dx%d\n\tType: %d\n\tBPP: %d\n\tPITCH: %d\n", info->base,
					info->width, info->height, info->type, info->bpp, info->pitch);
				break;
			}
		}
		
		if(tag->type == MULTIBOOT2_END_TAG) {
			break;
		}
	}

	cmain();
}

