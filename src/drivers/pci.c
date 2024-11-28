#include "kstdio.h"
#include "pci-class.h"
#include <stdint.h>

#include <drivers/pci.h>
#include <types.h>

#if defined(__x86_64__) || defined(__i386__)
#include <pio.h>

uint32_t pci_pio_read(uint32_t bdf, uint8_t offset, uint8_t bytes) {
	uint32_t address, mask;

	address = PCI_CONF_ADDR_ENABLE | (bdf << 8) | (offset & 0xfc);
	offset = offset & 3;
	mask = 0;

	switch (bytes) {
		case 4:
			mask |= 0xff000000U;
		case 3:
			mask |= 0xff0000U;
		case 2:
			mask |= 0xff00U;
		case 1:
			mask |= 0xffU;
		case 0:
		default:
			/*Mask Remains as 0*/
			break;
	}
	
	/*4bytes is the max that can be read and must be aligned*/
	if(bytes + offset > 4) {
		return UINT32_FULL_MASK;
	}

	out32(PCI_CONF_ADDR_PORT, address);
	
	return (in32(PCI_CONF_DATA_PORT) >> (offset * 8)) & mask;
}

uint32_t pci_pio_write(uint32_t bdf, uint8_t offset, uint32_t data, uint8_t bytes, uint8_t merge) {
	uint32_t address = PCI_CONF_ADDR_ENABLE | (bdf << 8) | (offset & 0xfc);
	uint32_t output;
	if(bytes > sizeof(uint32_t)) {
		return 0xffffffff;
	}

	output = pci_pio_read(bdf, offset, 4);
	if(merge) {
		data = data & (UINT32_FULL_MASK >> ((4-bytes) * 8));
		output |= data;
	} else {
	}


	out32(PCI_CONF_ADDR_PORT, address);

	out32(PCI_CONF_DATA_PORT, output);
	return 0;
}
#endif

static void pci_scan_device(uint8_t bus, uint8_t device);


static inline void pci_scan_bus(uint8_t bus) {
	uint8_t dev = 0;
	
	for(dev = 0; dev < PCI_DEVICE_MAX; ++dev) {
		pci_scan_device(bus, dev);
	}
}

static inline void pci_get_class_info(uint32_t bdf, uint8_t *class, uint8_t *sub, uint8_t *pif) {
	uint32_t data = 0;

	data = pci_pio_read(bdf, 0x9, 3);

	*pif = data & 0xff;
	*sub = (data >> 8) & 0xff;
	*class = (data >> 16) & 0xff;
}

static inline void pci_scan_function(uint8_t bus, uint8_t device, uint8_t func) {
	uint16_t vendor = 0;
	uint16_t devid = 0;
	uint8_t type = 0;
	uint8_t class = 0;
	uint8_t subclass = 0;
	uint8_t prog_if = 0;


	uint16_t bdf = PCI_DEVICE_TO_BDF(bus, device, 0);	
	vendor = PCI_GET_VENDOR_ID(bdf);
	devid = PCI_GET_DEVICE_ID(bdf);
	
	kprintf("Bus %d, Device %d, Func %d:\n", bus, device, func);
	kprintf("    Vendor %x, Dev ID: %x\n", vendor, devid);


	pci_get_class_info(bdf, &class, &subclass, &prog_if);
	kprintf("    Class: %s,\n    Sublass: %s,\n    Prog IF: %s\n", pci_class_to_str(class), 
			pci_subclass_to_str(class, subclass), pci_prog_if_to_str(class, subclass, prog_if));


	if(class == 0x6 && subclass == 0x4) {
		kprintf("    Found Bus: %d\n", PCI_GET_SECONDARY_BUS(bdf));
		pci_scan_bus(PCI_GET_SECONDARY_BUS(bdf));
	}

}

static void pci_scan_device(uint8_t bus, uint8_t device) {
	uint16_t vendor = 0;
	uint8_t type = 0;
	uint16_t bdf = PCI_DEVICE_TO_BDF(bus, device, 0);	
	
	vendor = PCI_GET_VENDOR_ID(bdf);
	if(vendor == 0xffff) return; /*No Device connected*/
	
	type = PCI_GET_HEADER_TYPE(bdf);

	if(type & 0x80) {
		for(uint8_t f = 1; f < PCI_FUNCTION_MAX; ++f) {
			if(PCI_GET_VENDOR_ID(PCI_DEVICE_TO_BDF(bus, device, f)) != 0xffff) {
				pci_scan_function(bus, device, f);
			}
		}
	}
	pci_scan_function(bus, device, 0);
}

int pci_init() {
	pci_scan_bus(0);
	return 0;
}

