/*
 * AHDI Tools
 *
 * tos_root_sector.c
 * Charlotte Koch <dressupgeekout@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "tos_root_sector.h"

/*
 * The given file-pointer is assumed to have already been advanced (or
 * rewound) to the beginning of the hard disk image. Returns NULL in case of
 * error. Caller needs to free the result.
 *
 * The file-pointer is NOT closed -- the caller needs to take care of that.
 */
tos_root_sector *
trs_read(FILE *fp)
{
	tos_root_sector *trs = malloc(sizeof(tos_root_sector));
	memset(trs, 0, sizeof(*trs));

	fseek(fp, 438L, SEEK_SET);  /* Skip past loader code. */
	uint16_t val2;
	uint32_t val4;

	fread(&val2, sizeof(trs->cylinders), 1, fp);
	trs->cylinders = ntohs(val2);

	fread(&trs->heads, sizeof(trs->heads), 1, fp);
	fread(&trs->iface, sizeof(trs->iface), 1, fp);

	fread(&val2, sizeof(trs->write_precomp_cylinder), 1, fp);
	trs->write_precomp_cylinder = ntohs(val2);

	fread(&val2, sizeof(trs->reduce_write_cylinder), 1, fp);
	trs->reduce_write_cylinder = ntohs(val2);

	fread(&trs->parking_cylinder_off, sizeof(trs->parking_cylinder_off), 1, fp);
	fread(&trs->step_rate, sizeof(trs->step_rate), 1, fp);
	fread(&trs->interleave, sizeof(trs->interleave), 1, fp);
	fread(&trs->sectors_per_track, sizeof(trs->sectors_per_track), 1, fp);

	fread(&val4, sizeof(trs->secsize), 1, fp);
	trs->secsize = ntohl(val4);

	int i;
	for (i = 0; i < TOS_MAX_PARTITION_COUNT; i++) {
		tos_partition *part = &trs->parts[i];
		fread(&part->status, sizeof(part->status), 1, fp);
		fread(&part->name, sizeof(part->name), 1, fp);

		fread(&val4, sizeof(part->offset), 1, fp);
		part->offset = ntohl(val4);

		fread(&val4, sizeof(part->size), 1, fp);
		part->size = ntohl(val4);
	}

	fread(&val4, sizeof(trs->bad_sectors_list_off), 1, fp);
	trs->bad_sectors_list_off = ntohl(val4);

	fread(&val4, sizeof(trs->bad_sectors_count), 1, fp);
	trs->bad_sectors_count = ntohl(val4);

	fread(&trs->checksum, sizeof(trs->checksum), 1, fp);
	return trs;
}


trs_validate_status
trs_validate(const tos_root_sector *trs)
{
	trs_validate_status status; 

	switch (trs->iface) {
	case 0x00: /* FALLTHROUGH */
	case 0xFF:
		status = TRS_VALIDATE_OK;
		break;
	default:
		status = TRS_VALIDATE_BAD_INTERFACE;
	}

	return status;
}
