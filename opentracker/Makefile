# $Id$

# Linux flavour
# PREFIX?=/opt/diet
# LIBOWFAT_HEADERS=$(PREFIX)/include
# LIBOWFAT_LIBRARY=$(PREFIX)/lib

# BSD flavour
# PREFIX?=/usr/local
# LIBOWFAT_HEADERS=$(PREFIX)/include/libowfat
# LIBOWFAT_LIBRARY=$(PREFIX)/lib

# Debug flavour
PREFIX?=..
LIBOWFAT_HEADERS=$(PREFIX)/libowfat
LIBOWFAT_LIBRARY=$(PREFIX)/libowfat

BINDIR?=$(PREFIX)/bin
STRIP?=strip

#FEATURES+=-DWANT_V4_ONLY
#FEATURES+=-DWANT_ACCESSLIST_BLACK
#FEATURES+=-DWANT_ACCESSLIST_WHITE
#FEATURES+=-DWANT_DYNAMIC_ACCESSLIST

#FEATURES+=-DWANT_SYNC_LIVE
#FEATURES+=-DWANT_IP_FROM_QUERY_STRING

# If you want gzip support to be compiled in, uncomment the next include.
# You can further modify the behaviour by setting DWANT_COMPRESSION_GZIP_ALWAYS
# in Makefile.gzip
#include Makefile.gzip

# If you want zstd support to be compiled in, uncomment the next include.
# You can further modify the behaviour by setting DWANT_COMPRESSION_ZSTD_ALWAYS
# in Makefile.zstd
#include Makefile.zstd

#FEATURES+=-DWANT_LOG_NETWORKS
#FEATURES+=-DWANT_RESTRICT_STATS
FEATURES+=-DWANT_IP_FROM_PROXY
#FEATURES+=-DWANT_FULLLOG_NETWORKS
#FEATURES+=-DWANT_LOG_NUMWANT
#FEATURES+=-DWANT_MODEST_FULLSCRAPES
#FEATURES+=-DWANT_SPOT_WOODPECKER
#FEATURES+=-DWANT_SYSLOGS
#FEATURES+=-DWANT_DEV_RANDOM
#FEATURES+=-DWANT_FULLSCRAPE

# You need libowfat version 0.34 to allow for automatic release of chunks during
# full scrape transfer, if you rely on an older versions, enable this flag
FEATURES+=-DWANT_NO_AUTO_FREE

# Is enabled on BSD systems by default in trackerlogic.h
# on Linux systems the include Makefile adds -lbsd
#include Makefile.arc4random

#FEATURES+=-D_DEBUG_HTTPERROR
#FEATURES+=-D_DEBUG_RANDOMTORRENTS

GIT_VERSION=$(shell sh -c 'command -v git >/dev/null && test -d .git && git rev-parse HEAD || echo _git_or_commit_not_found_')

OPTS_debug=-D_DEBUG -g -ggdb # -pg -fprofile-arcs -ftest-coverage
OPTS_production=-O3

CFLAGS+=-I$(LIBOWFAT_HEADERS) -DGIT_VERSION=$(GIT_VERSION) -Wall -pipe -pthread -Wextra #-ansi -pedantic
LDFLAGS+=-L$(LIBOWFAT_LIBRARY) -lowfat -pthread

BINARY =opentracker
HEADERS=trackerlogic.h scan_urlencoded_query.h ot_mutex.h ot_stats.h ot_vector.h ot_clean.h ot_udp.h ot_iovec.h ot_fullscrape.h ot_accesslist.h ot_http.h ot_livesync.h ot_rijndael.h
SOURCES=opentracker.c trackerlogic.c scan_urlencoded_query.c ot_mutex.c ot_stats.c ot_vector.c ot_clean.c ot_udp.c ot_iovec.c ot_fullscrape.c ot_accesslist.c ot_http.c ot_livesync.c ot_rijndael.c
SOURCES_proxy=proxy.c ot_vector.c ot_mutex.c

OBJECTS = $(SOURCES:%.c=%.o)
OBJECTS_debug = $(SOURCES:%.c=%.debug.o)
OBJECTS_proxy = $(SOURCES_proxy:%.c=%.o)
OBJECTS_proxy_debug = $(SOURCES_proxy:%.c=%.debug.o)

.SUFFIXES: .debug.o .o .c

all: $(BINARY) $(BINARY).debug

CFLAGS_production = $(CFLAGS) $(OPTS_production) $(FEATURES)
CFLAGS_debug = $(CFLAGS) $(OPTS_debug) $(FEATURES)

$(BINARY): $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)
	$(STRIP) $@
$(BINARY).debug: $(OBJECTS_debug) $(HEADERS)
	$(CC) -o $@ $(OBJECTS_debug) $(LDFLAGS)
proxy: $(OBJECTS_proxy) $(HEADERS)
	$(CC) -o $@ $(OBJECTS_proxy) $(CFLAGS_production) $(LDFLAGS)
proxy.debug: $(OBJECTS_proxy_debug) $(HEADERS)
	$(CC) -o $@ $(OBJECTS_proxy_debug) $(LDFLAGS)

.c.debug.o : $(HEADERS)
	$(CC) -c -o $@ $(CFLAGS_debug) $(<:.debug.o=.c)

.c.o : $(HEADERS)
	$(CC) -c -o $@ $(CFLAGS_production) $<

clean:
	rm -rf opentracker opentracker.debug *.o *~

install:
	install -m 755 opentracker $(DESTDIR)$(BINDIR)
