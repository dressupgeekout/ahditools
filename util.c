/*
 * AHDI Tools
 *
 * util.c
 * Charlotte Koch <dressupgeekout@gmail.com>
 */

#include "tos_root_sector.h"
#include "util.h"

int
sectors_to_mb(int sectors)
{
	return TOS_ROOT_SECTOR_SIZE * sectors / 1024 / 1024;
}
