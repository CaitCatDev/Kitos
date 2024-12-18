#pragma once

#include <stdint.h>

#define ACPI_MADT_TABLE_SIGNATURE "APIC"
#define ACPI_HPET_TABLE_SIGNATURE "HPET"
#define ACPI_FACP_TABLE_SIGNATURE "FACP"

#define ACPI_MADT_TYPE_LAPIC 0
#define ACPI_MADT_TYPE_IOAPIC 1
#define ACPI_MADT_TYPE_INT_OVERRIDE 2
#define ACPI_MADT_TYPE_NMI_SOURCE 3
#define ACPI_MADT_TYPE_LAPIC_NMI 4

#define ACPI_MADT_LAPIC_ENABLED (1)
#define ACPI_MADT_LAPIC_ONLINE_CAP (1 << 1);

typedef struct acpi_rsdp {
	char signature[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt_address;

	/*Only valid if revisions is ACPI 2.0*/
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t ext_checksum;
	uint8_t reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_std_hdr {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemid[6];
	char oemtable[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_rev;
} __attribute__((packed)) acpi_std_hdr_t;

typedef struct acpi_rsdt {
	acpi_std_hdr_t hdr;
	uint32_t entries[];
} __attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_xsdt {
	acpi_std_hdr_t hdr;
	uint64_t entries[];
} __attribute__((packed)) acpi_xsdt_t;

typedef struct acpi_madt_entry {
	uint8_t type;
	uint8_t length;
} __attribute__((packed)) acpi_madt_entry_t;

typedef struct acpi_madt_lapic_t {
	acpi_madt_entry_t hdr;
	uint8_t acpi_processor_uid;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed)) acpi_madt_lapic_t;

typedef struct acpi_madt_ioapic_t {
	acpi_madt_entry_t hdr;
	uint8_t ioapic_id;
	uint8_t reserved;
	uint32_t ioapic_address;
	uint32_t gsi_base;
} __attribute__((packed)) acpi_madt_ioapic_t;

typedef struct acpi_madt_int_override_t {
	acpi_madt_entry_t hdr;
	uint8_t bus;
	uint8_t source;
	uint32_t gsi;
	uint16_t flags;
} __attribute__((packed)) acpi_madt_int_override_t;

typedef struct acpi_madt_nmi_src_t {
	acpi_madt_entry_t hdr;
	uint16_t flags;
	uint32_t gsi;
} __attribute__((packed)) acpi_madt_nmi_src_t;

typedef struct acpi_madt_lapic_nmi {
	acpi_madt_entry_t hdr;
	uint8_t acpi_processor_uid;
	uint16_t flags;
	uint8_t lapic_lint;
} __attribute__((packed)) acpi_madt_lapic_nmi_t ;

typedef struct acpi_madt {
	acpi_std_hdr_t hdr;
	uint32_t lic_address;
	uint32_t flags;
	acpi_madt_entry_t entries[];
} __attribute__((packed)) acpi_madt_t;

void acpi_init(acpi_rsdp_t *rsdp);
