#pragma once

#include <stdint.h>

#include <utils.h>

#define PCI_CONF_ADDR_ENABLE (1 << 31)
#define PCI_CONF_ADDR_PORT 0xcf8
#define PCI_CONF_DATA_PORT 0xcfc
#define PCI_DEVICE_TO_BDF(b, d, f) ((b << 8) | (d << 3) | f)

#define PCI_DEVICE_MAX 32
#define PCI_BUS_MAX 256
#define PCI_FUNCTION_MAX 8

#define PCI_GET_VENDOR_ID(bdf) pci_pio_read(bdf, 0, 2)
#define PCI_GET_DEVICE_ID(bdf) pci_pio_read(bdf, 2, 2)
#define PCI_GET_HEADER_TYPE(bdf) pci_pio_read(bdf, 0x0e, 1)
#define PCI_GET_SECONDARY_BUS(bdf) pci_pio_read(bdf, 0x19, 1)
#define PCI_GET_PRIMARY_BUS(bdf) pci_pio_read(bdf, 0x18, 1)


typedef struct pci_device {
	uint16_t bdf;

	llist_t list;
} pci_device_t;

static pci_device_t *pci_devices;

#if defined(__x86_64__) || defined(__i386__)
#include <pio.h>

uint32_t pci_pio_read(uint32_t bdf, uint8_t offset, uint8_t bytes);
uint32_t pci_pio_write(uint32_t bdf, uint8_t offset, uint32_t data, uint8_t bytes, uint8_t merge);
#endif

int pci_init();
