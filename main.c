/*
 * AHDI Tools
 * Charlotte Koch <dressupgeekout@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include "tos_root_sector.h"
#include "util.h"

static void report(const tos_root_sector *trs);
static void extract(const tos_root_sector *trs, FILE *fp);

/* Globals. */
char *progname = NULL;

/* ************************** */

int
main(int argc, char *argv[])
{
#if HAVE_GETPROGNAME
	progname = (char *)getprogname();
#else
	progname = (char *)argv[0];
#endif

	/* Validate command-line arguments. */
	if (argc < 2) {
		printf("%s: usage: %s disk-image\n", progname, progname);
		return EXIT_FAILURE;
	}

	int ch;
	int extract_index;

	while ((ch = getopt(argc, argv, "e:")) != -1) {
		switch (ch) {
		case 'e':
			extract_index = atoi(optarg);
			break;
		case '?': /* FALLTHROUGH */
		default:
			return EXIT_FAILURE;
		}
	}
	argc -= optind;
	argv += optind;

	struct stat sb;

	if (stat(argv[1], &sb) < 0) {
		fprintf(stderr, "%s: FATAL: cannot open file: %s\n", progname, argv[1]);
		return EXIT_FAILURE;
	}

	/* Open the disk image, see if it isn't obviously bad. */
	FILE *fp = fopen(argv[1], "rb");
	fseeko(fp, 0, SEEK_END);
	off_t size = ftello(fp);

	if (size <= TOS_ROOT_SECTOR_SIZE) {
		fprintf(stderr, "%s: FATAL: too short to be a valid disk image\n", progname);
		goto err;
	}

	rewind(fp);

	/* Look in the disk image more closely. */
	tos_root_sector *trs = trs_read(fp);
	trs_validate_status status;

	if ((status = trs_validate(trs)) != TRS_VALIDATE_OK) {
		fprintf(stderr, "%s: FATAL: unknown Root Sector interface specification: %d\n", progname, trs->iface);	
		return EXIT_FAILURE;
	}

	report(trs);
	extract(trs, fp);
	fclose(fp);

	return EXIT_SUCCESS;

err:
	fclose(fp);
	return EXIT_FAILURE;
}


static void
report(const tos_root_sector *trs)
{
	printf("Cylinders: %d\n", trs->cylinders);
	printf("Heads: %d\n", trs->heads);

	char iface[8];
	snprintf(iface, sizeof(iface), "%s", "");

	switch (trs->iface) {
	case 0x00:
		snprintf(iface, sizeof(iface), "%s", "SASI");
		break;
	case 0xff:
		snprintf(iface, sizeof(iface), "%s", "SCSI");
		break;
	default:
		; /* NOTREACHED */
	}

	printf("Interface: %s\n", iface);
	printf("Write pre-compensation cylinder: %d\n", trs->write_precomp_cylinder);
	printf("Reduced write current cylinder: %d\n", trs->reduce_write_cylinder);
	printf("Parking cylinder offset: %d\n", trs->parking_cylinder_off);
	printf("Step rate: %d\n", trs->step_rate);
	printf("Interleave: %d\n", trs->interleave);
	printf("Sectors per track: %d\n", trs->sectors_per_track);
	printf("Size: %d sectors => %d MB\n", trs->secsize, sectors_to_mb(trs->secsize));
	printf("\n");

	int i;
	for (i = 0; i < TOS_MAX_PARTITION_COUNT; i++) {
		bool exists = (trs->parts[i].status << 7);
		if (!exists)
			continue;
		bool bootable = (trs->parts[i].status & 0x01);
		printf("Partition #%d\n", i);
		printf("\tStatus: %s %s\n", (exists ? "exists" : ""), (bootable ? "bootable" : ""));
		printf("\tType: %c%c%c\n", trs->parts[i].name[0], trs->parts[i].name[1], trs->parts[i].name[2]);
		printf("\tOffset (in sectors): %d\n", trs->parts[i].offset);
		printf("\tSize: %d sectors => %d MB\n", trs->parts[i].size, sectors_to_mb(trs->parts[i].size));
		printf("\n");
	}

	printf("Bad sectors list offset: %d\n", trs->bad_sectors_list_off);
	printf("Bad sectors count: %d\n", trs->bad_sectors_count);
	printf("Checksum: 0x%04x\n", trs->checksum);
}


static void
extract(const tos_root_sector *trs, FILE *fp)
{
	int partnum;
	unsigned char *this_sector = malloc(512);
	char filename[512];

	for (partnum = 0; partnum < 2; partnum++) {
		snprintf(filename, sizeof(filename), "partition%02d.img", partnum);
	
		uint32_t offset = trs->parts[partnum].offset;
		uint32_t size = trs->parts[partnum].size;
	
		fprintf(stderr, "going to write %d sectors to %s\n", size, filename);

		FILE *out = fopen(filename, "wb");
	
		fseek(fp, 512*offset, SEEK_SET);

		uint32_t i;
		for (i = 0; i < size; i++) {
			fread(this_sector, 512, 1, fp);
			fwrite(this_sector, 512, 1, out);
		}
	
		fclose(out);
	}

	free(this_sector);
}
