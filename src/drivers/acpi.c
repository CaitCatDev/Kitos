#include <pio.h>
#include <kstdio.h>
#include <drivers/acpi.h>

#include <stdint.h>
#include <types.h>

#include <mmu/pmm.h>
#include <mmu/vmm.h>

#include <lib/string.h>
#include <lib/memory.h>

#include <msr.h>

void ioapic_init(acpi_madt_ioapic_t *ioapic) {
	volatile uint32_t *base = PHYS_TO_VIRT(ioapic->ioapic_address);
	mmap_page((void*)(uintptr_t)ioapic->ioapic_address, PHYS_TO_VIRT(ioapic->ioapic_address),
			X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR | X86_PAE_PAGE_UNCACHEABLE);
	/*Timer Interrupt for QEMU*/	
	base[0] = 0x14;
	base[4] = 0x20;
	kprintf("\tIOAPIC TEST 0x%x\n", base[4]);
}

void acpi_madt_init(acpi_madt_t *madt) {
	kprintf("Local Int Controller: %p\n", madt->lic_address);
	kprintf("APIC_FLAGS: 0x%x\n", madt->flags);
	acpi_madt_entry_t *tmp = madt->entries;
	
	/*Disable Legacy PIC*/
	out8(0x21, 0xff);
	out8(0xa1, 0xff);

	kprintf("APIC Base: %p\n", readmsr(0x1b));
	writemsr(0x1b, readmsr(0x1b) | 0x800);
	volatile uint32_t *apic = PHYS_TO_VIRT(madt->lic_address);

	__asm__ volatile("sti");
		mmap_page((void*)(uintptr_t)madt->lic_address, PHYS_TO_VIRT(madt->lic_address), X86_PAE_PAGE_PRESENT | X86_PAE_PAGE_RDWR | X86_PAE_PAGE_UNCACHEABLE);

	/*TODO SPRIOUS INT*/
	apic[0x3c] |= 0x110;

	for(uint32_t i = 0; i < madt->hdr.length - sizeof(acpi_madt_t); i += tmp->length) {
		kprintf("Type: %d Len: %d\n", tmp->type, tmp->length);
		
		switch(tmp->type) {
			case ACPI_MADT_TYPE_LAPIC: {
				acpi_madt_lapic_t *lapic = (acpi_madt_lapic_t*)tmp;
				kprintf("\tACPI ID: %d APIC ID: %d FLAGS: %d\n", 
						lapic->acpi_processor_uid, lapic->apic_id, lapic->flags);
				break;
			}
			case ACPI_MADT_TYPE_IOAPIC: {
				acpi_madt_ioapic_t *ioapic = (acpi_madt_ioapic_t*)tmp;
				kprintf("\tIOAPIC ID: %d, Address: %p GSI: %d\n", ioapic->ioapic_id, ioapic->ioapic_address, ioapic->gsi_base);
				ioapic_init(ioapic);
				break;
			}
			case ACPI_MADT_TYPE_INT_OVERRIDE: {
				acpi_madt_int_override_t *override = (acpi_madt_int_override_t*)tmp;
				kprintf("\tBUS: %d SOURCE: %d, FLAGS: %d, GSI: %d\n", override->bus, override->source,
						override->flags, override->gsi);
			}
		}
		tmp = (void*)(((uintptr_t)tmp) + tmp->length);
	}
}

void acpi_init(acpi_rsdp_t *rsdp) {
	acpi_rsdt_t *rsdt;
	kprintf("ACPI OEM: ");
	for(uint32_t i = 0; i < 6; ++i) {
		put(rsdp->oemid[i]);
	}
	kprintf("\n");
	kprintf("ACPI Sig: ");
	for(uint32_t i = 0; i < 8; ++i) {
		put(rsdp->signature[i]);
	}
	kprintf("\n");
	
	kprintf("%p\n", rsdp);	

	kprintf("RSDT: %p\n", rsdp->rsdt_address);
	rsdt = UINT32_TO_POINTER(rsdp->rsdt_address);
	
	/*Map just one page of this table*/
	vmm_map_range((void*)(uintptr_t)rsdp->rsdt_address, 
			(void*)(uintptr_t)((rsdp->rsdt_address & X86_64_PAE_ADDRESS_MASK) + 0x1000), 
			PHYS_TO_VIRT(rsdp->rsdt_address), X86_PAE_PAGE_PRESENT);
	
	rsdt = PHYS_TO_VIRT(rsdt);
	kprintf("RSDT Virtual: %p\n", rsdt);
	

	/*Map the full table if it's bigger than one page*/
	vmm_map_range((void*)(uintptr_t)rsdp->rsdt_address, 
			(void*)(uintptr_t)(((rsdp->rsdt_address + rsdt->hdr.length) & X86_64_PAE_ADDRESS_MASK) + 0x1000), 
			PHYS_TO_VIRT(rsdp->rsdt_address), X86_PAE_PAGE_PRESENT);
	
	for(uint32_t i = 0; i < (rsdt->hdr.length - sizeof(acpi_std_hdr_t)) / sizeof(uint32_t); i++) {
		acpi_std_hdr_t *hdr = UINT32_TO_POINTER(rsdt->entries[i]);
		vmm_map_range((void*)(uintptr_t)rsdt->entries[i],
				(void*)(uintptr_t)((rsdt->entries[i] & X86_64_PAE_ADDRESS_MASK) + 0x1000), 
				PHYS_TO_VIRT(rsdt->entries[i]), X86_PAE_PAGE_PRESENT);
		
		hdr = PHYS_TO_VIRT(hdr);

		kprintf("ACPI Table  ");
		for(uint32_t i = 0; i < 4; ++i) {
			put(hdr->signature[i]);
		}
		kprintf(" at %p\n", rsdt->entries[i]);

		vmm_map_range(VIRT_TO_PHYS(hdr), 
				(void*)((((uintptr_t)VIRT_TO_PHYS(hdr) + hdr->length) & X86_64_PAE_ADDRESS_MASK) + 0x1000), 
				hdr, X86_PAE_PAGE_PRESENT);
		if(memcmp(hdr->signature, "APIC", 4) == 0) {
			acpi_madt_init((void*)hdr);
		}

	}
}
