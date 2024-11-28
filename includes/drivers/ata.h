#pragma once

#include <stdint.h>


/*Legacy compat IO ports*/
#define ATA_PRIMARY_IO_PORT_BASE 0x1f0
#define ATA_PRIMARY_IO_CONTROL_PORT 0x3f6

#define ATA_SECONDARY_IO_PORT_BASE 0x170
#define ATA_SECONDARY_IO_CONTROL_PORT 0x376

#define ATA_PARENT_DRIVE 0xa0
#define ATA_CHILD_DRIVE 0xb0

/*ATA STATUS REGISTER VALUES*/
#define ATA_STATUS_BSY (1 << 7) /**< Drive Busy*/
#define ATA_STATUS_DRD (1 << 6) /**< Data Ready*/
#define ATA_STATUS_DWF (1 << 5) /**< Drive Write Fault*/
#define ATA_STATUS_DSC (1 << 4) /**< Drive Seek Complete*/
#define ATA_STATUS_DRQ (1 << 3) /**< Data Request Ready*/
#define ATA_STATUS_COR (1 << 2) /**< Data Corrected*/
#define ATA_STATUS_IDX (1 << 1) /**< Drive Index*/
#define ATA_STATUS_ERR (1) /**< Drive Error*/

/*ATA ERROR REGISTER VALUES*/
#define ATA_ERROR_BBLK (1 << 7) /**< Drive Block Error*/
#define ATA_ERROR_UNCO (1 << 6) /**< Uncorrectable Data*/
#define ATA_ERROR_MCHG (1 << 5) /**< Drive Media Changed*/
#define ATA_ERROR_IDNF (1 << 4) /**< ID Mark not found*/
#define ATA_ERROR_MCHR (1 << 3) /**< Drive Media Change Request*/
#define ATA_ERROR_ABRT (1 << 2) /**< Command Aborted*/
#define ATA_ERROR_TRNF (1 << 1) /**< Drive Track 0 not found*/
#define ATA_ERROR_AMNF (1) /**< Address Mark Not Found*/


typedef struct _ata_identify_s {
	uint16_t type;
	uint16_t cylinders;
	uint16_t configuration;
	uint16_t heads;
	uint16_t retired[2];
	uint16_t sectors_per_track;
	uint16_t reserved0[2];
	uint16_t retired1;
	int8_t serial_num[20];
	uint16_t retired2[2];
	uint16_t obsolete;
	int8_t firmware_rev[8];
	int8_t model_num[40];
	uint16_t max_sects_read;
	uint16_t reserved1;
	uint16_t capabilities;
	uint16_t capabilities2;
	uint16_t obsolete1[2];
	uint16_t reserved2;
	uint16_t cur_log_cyl;
	uint16_t cur_log_heads;
	uint16_t cur_log_sects_per_track;
	uint16_t capacity_in_sectors[2];
	uint16_t word59WorkOutLaterCat;
	uint16_t total_sectors28bit;
	uint16_t obsolete2;
	uint16_t rand[256];
}__attribute__((packed)) ata_id_t;
