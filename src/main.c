#include <boot/multiboot2.h>
#include <stdint.h>
#include <kstdio.h>
#include <types.h>

#include <drivers/serial.h>
#include <drivers/ata.h>
#include <drivers/pci.h>

#include <mmu/vmm.h>
#include <mmu/pmm.h>

#include <lib/string.h>
#include <lib/memory.h>

#if defined(__x86_64__) || defined(__i386__)
#include <pio.h>
#endif

#include <pci-class.h>


static uint32_t x = 0;
static uint32_t y = 0;
static uint32_t xmax = 80;
static uint32_t ymax = 25;

void vga_clear_line(uint32_t line, uint8_t color) {
	volatile short *vga = (volatile void*)0xffffff80000b8000;
	for(uint32_t lx = 0; lx < xmax; ++lx) {
		vga[lx + line * xmax] = (color << 8) + ' ';
	}
}

void vga_mov_cursor(uint32_t x, uint32_t y) {
	uint16_t pos = y * 80 + x;

	out8(0x3d4, 0x0f);
	out8(0x3d5, pos);
	out8(0x3d4, 0x0e);
	out8(0x3d5, pos >> 8);
}

void vga_put_ch(uint8_t ch) {
	volatile char *vga;

	/*Scrolls the screen*/
	if(y >= ymax) {
		for(y = 0; y < ymax-1; ++y) {
			memcpy((void*)((uint64_t)0xffffff80000b8000 + (y * 160)), (const void*)((uint64_t)0xffffff80000b8000 + ((y+1) * 160)), 160);
		}
		y = ymax - 1;
		x = 0;
		vga_clear_line(y, 0x1f);
	}

	vga = (volatile void*)0xffffff80000b8000 + x * 2 + y * 160;

	if(ch == '\n') {
		y++;
		x=0;
	} else {
		*vga = ch;
		x++;
	}

	vga_mov_cursor(x, y);
}

void vga_clear_screen(uint8_t color) {
	volatile short *vga = (volatile void*)0xffffff80000b8000;
	for(uint32_t ly = 0; ly < ymax; ++ly) {
		for(uint32_t lx = 0; lx < xmax; ++lx) {
			vga[lx + ly * xmax] = (color << 8) + ' ';
		}
	}
	x = 0;
	y = 0;
	vga_mov_cursor(x, y);
}


int vmm_init();	

void cmain() {
	mmap_page((void*)0xb8000, (void*)0xdead0000);
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

void cinit_cmdline(char *cmdline) {
	char *token = NULL;

	token = strtok(cmdline, " ");

	while(token) {
		kprintf("Arg: %s\n", token);
		if(strcmp(token, "-vga") == 0) {
			stdio_init(vga_put_ch);
			token = strtok(NULL, " ");
		}
	}
}

void multiboot2_cinit(multiboot2_boot_info_t *info) {
	stdio_init(write_serial);
	if(init_serial(COM1) < 0) {
		stdio_init(vga_put_ch);
	}
	
	kprintf("Hello Multiboot2 world: %p\n", info);
	
	/*scan for the command line first*/
	for(uint32_t i = 0; i < info->size;) {
		multiboot2_tag_t *tag = (multiboot2_tag_t*)&info->tags[i];
		//kprintf("Multiboot 2 tag: %d size(%d)\n", tag->type, tag->size);
		i += (tag->size + (7)) & ~(7);
	
		switch(tag->type) {
			case MULTIBOOT2_CMDLINE_TAG: {
				multiboot2_cmdline_t *cmdline = (multiboot2_cmdline_t*)tag;
				cinit_cmdline((char*)cmdline->str);
				break;
			}
		}
		
		if(tag->type == MULTIBOOT2_END_TAG) {
			break;
		}
	}

	/*scan for the command line first*/
	for(uint32_t i = 0; i < info->size;) {
		multiboot2_tag_t *tag = (multiboot2_tag_t*)&info->tags[i];
		//kprintf("Multiboot 2 tag: %d size(%d)\n", tag->type, tag->size);
		i += (tag->size + (7)) & ~(7);
	
		switch(tag->type) {
			case MULTIBOOT2_MEMOMAP_TAG: {
				multiboot2_mmap_t *mmap = (multiboot2_mmap_t*)tag;
				pmm_init_mb2_mmap(info, mmap);
				break;
			}
		}
		
		if(tag->type == MULTIBOOT2_END_TAG) {
			break;
		}
	}
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

