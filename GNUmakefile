#
# AHDI Tools - Makefile
# Charlotte Koch <dressupgeekout@gmail.com>
#

HAVE_GETPROGNAME?=	1

PROG=	tos_harddisk_info

SRCS=	# defined
SRCS+=	main.c
SRCS+=	tos_root_sector.c
SRCS+=	util.c

CFLAGS?=	-g -Wall -Wno-int-in-bool-context -std=c99
CFLAGS+=	-DHAVE_GETPROGNAME=$(HAVE_GETPROGNAME)

.PHONY: all
all: $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tos_harddisk_info: $(patsubst %.c,%.o,$(SRCS))
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(PROG) *.o *.core
