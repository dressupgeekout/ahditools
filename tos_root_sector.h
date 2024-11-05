/*
 * AHDI Tools
 *
 * tos_root_sector.h
 * Charlotte Koch <dressupgeekout@gmail.com>
 */

#ifndef TOS_ROOT_SECTOR_H
#define TOS_ROOT_SECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TOS_ROOT_SECTOR_SIZE		512
#define TOS_ROOT_SECTOR_CHECKSUM_MAGIC	0x1234
#define TOS_MAX_PARTITION_COUNT		4

struct tos_partition {
	uint8_t status;
	char name[3];
	uint32_t offset;
	uint32_t size;
};
typedef struct tos_partition tos_partition;

struct tos_root_sector {
	uint16_t cylinders;
	uint8_t heads;
	uint8_t iface;
	uint16_t write_precomp_cylinder;
	uint16_t reduce_write_cylinder;
	uint8_t parking_cylinder_off;
	uint8_t step_rate;
	uint8_t interleave;
	uint8_t sectors_per_track;
	uint32_t secsize;
	tos_partition parts[TOS_MAX_PARTITION_COUNT];
	uint32_t bad_sectors_list_off;
	uint32_t bad_sectors_count;
	uint16_t checksum;
};
typedef struct tos_root_sector tos_root_sector;

enum trs_validate_status {
	TRS_VALIDATE_OK,
	TRS_VALIDATE_BAD_INTERFACE
};
typedef enum trs_validate_status trs_validate_status;

tos_root_sector *trs_read(FILE *fp);
trs_validate_status trs_validate(const tos_root_sector *trs);

#endif /* TOS_ROOT_SECTOR_H */
