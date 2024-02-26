# Warning!  There is no real "make install" support.
# The include files are named after the djb, i.e. pretty generic.  They
# will probably conflict with other headers.  That's why I install them
# in /opt/diet, where they are in the default search path for my diet libc
# work but don't conflict with anything there.  YMMV.

prefix=/opt/diet
LIBDIR=${prefix}/lib
INCLUDEDIR=${prefix}/include
MAN3DIR=${prefix}/man/man3

LIBS=byte.a fmt.a scan.a str.a uint.a open.a stralloc.a unix.a socket.a \
buffer.a mmap.a taia.a tai.a dns.a case.a mult.a array.a io.a \
textcode.a cdb.a critbit.a

all: headers ent $(LIBS) libowfat.a libsocket t

pic pie:
	$(MAKE) CC="gcc -fPIC" LDFLAGS="-fpie"

picx32 piex32:
	$(MAKE) CC="gcc -mx32 -fPIC" LDFLAGS="-fpie"

CROSS=
#CROSS=i686-mingw-
CC?=gcc
AR?=ar
RANLIB?=ranlib
CCC=$(CROSS)$(CC)
WERROR=
WARN=-W -Wall -Wextra $(WERROR)

# Use the second version if you are building for a binary that is only
# supposed to run on this machine. It tells gcc to use CPU instructions
# that are specific to the CPU the code is compiled on.
NATIVE=
#NATIVE=-march=native -mtune=native

OPT_REG=-O2
OPT_PLUS=-O3 $(NATIVE)

DEFINE=-D_REENTRANT

CFLAGS=-pipe $(WARN) $(DEFINE) $(OPT_REG)
CFLAGS_OPT=-pipe $(WARN) $(DEFINE) $(OPT_PLUS)

#CFLAGS=-pipe -Os -march=pentiumpro -mcpu=pentiumpro -fomit-frame-pointer -fschedule-insns2 -Wall

ent: ent.c haveuint128.h
	$(CC) -g -o ent ent.c -I.

# CFLAGS += -fstrict-aliasing -Wstrict-aliasing=2

# startrip
ifneq ($(DEBUG),)
CFLAGS=-pipe -Wall -g -Og
endif
path = $(subst :, ,$(PATH))
diet_path = $(foreach dir,$(path),$(wildcard $(dir)/diet))
ifeq ($(strip $(diet_path)),)
ifneq ($(wildcard /opt/diet/bin/diet),)
DIET=/opt/diet/bin/diet
else
DIET=
endif
else
DIET:=$(strip $(diet_path))
endif
gcc_path = $(foreach dir,$(path),$(wildcard $(dir)/gcc))
ifeq ($(strip $(gcc_path)),)
CC=clang
else
CC=gcc
endif

ifneq ($(DIET),)
DIETLIBPATH=$(shell $(DIET) -L $(CCC))
ifneq ($(wildcard $(DIETLIBPATH)/*ibc.a),)
LIBDIR=$(DIETLIBPATH)
endif
ifneq ($(DEBUG),1)
DIET+=-Os
endif
endif

# to build without diet libc support, use $ make DIET=
# see http://www.fefe.de/dietlibc/ for details about the diet libc

VPATH=str:byte:fmt:scan:uint:open:stralloc:unix:socket:buffer:mmap:textcode:taia:tai:dns:case:array:mult:io:cdb:critbit

BYTE_OBJS=$(patsubst byte/%.c,%.o,$(wildcard byte/*.c))
FMT_OBJS=$(patsubst fmt/%.c,%.o,$(wildcard fmt/*.c))
SCAN_OBJS=$(patsubst scan/%.c,%.o,$(wildcard scan/*.c))
STR_OBJS=$(patsubst str/%.c,%.o,$(wildcard str/*.c))
UINT_OBJS=$(patsubst uint/%.c,%.o,$(wildcard uint/*.c))
OPEN_OBJS=$(patsubst open/%.c,%.o,$(wildcard open/*.c))
STRALLOC_OBJS=$(patsubst stralloc/%.c,%.o,$(wildcard stralloc/*.c))
UNIX_OBJS=$(patsubst unix/%.c,%.o,$(wildcard unix/*.c))
SOCKET_OBJS=$(patsubst socket/%.c,%.o,$(wildcard socket/*.c))
BUFFER_OBJS=$(patsubst buffer/%.c,%.o,$(wildcard buffer/*.c))
MMAP_OBJS=$(patsubst mmap/%.c,%.o,$(wildcard mmap/*.c))
TEXTCODE_OBJS=$(patsubst textcode/%.c,%.o,$(wildcard textcode/*.c))
TAI_OBJS=$(patsubst tai/%.c,%.o,$(wildcard tai/*.c))
TAIA_OBJS=$(patsubst taia/%.c,%.o,$(wildcard taia/*.c))
DNS_OBJS=$(patsubst dns/%.c,%.o,$(wildcard dns/*.c))
CASE_OBJS=$(patsubst case/%.c,%.o,$(wildcard case/*.c))
ARRAY_OBJS=$(patsubst array/%.c,%.o,$(wildcard array/*.c))
MULT_OBJS=$(patsubst mult/%.c,%.o,$(wildcard mult/*.c))
IO_OBJS=$(patsubst io/%.c,%.o,$(wildcard io/*.c))
CDB_OBJS=$(patsubst cdb/%.c,%.o,$(wildcard cdb/*.c))
CRITBIT_OBJS=$(patsubst critbit/%.c,%.o,$(wildcard critbit/*.c))

$(BYTE_OBJS): byte.h
$(FMT_OBJS): fmt.h
$(SCAN_OBJS): scan.h haveuint128.h
$(STR_OBJS): str.h
$(UINT_OBJS): uint16.h uint32.h
$(STRALLOC_OBJS): stralloc.h
$(SOCKET_OBJS): socket.h
$(BUFFER_OBJS): buffer.h
$(MMAP_OBJS): mmap.h open.h
$(TEXTCODE_OBJS): textcode.h
$(TAI_OBJS): tai.h uint64.h
$(TAIA_OBJS): taia.h tai.h uint64.h
$(DNS_OBJS): dns.h stralloc.h taia.h tai.h uint64.h iopause.h
$(CASE_OBJS): case.h
$(ARRAY_OBJS): uint64.h array.h
$(MULT_OBJS): uint64.h uint32.h uint16.h safemult.h
$(IO_OBJS): uint64.h array.h io.h io_internal.h taia.h tai.h haveepoll.h havekqueue.h havesigio.h havebsdsf.h havedevpoll.h havesendfile.h
$(CDB_OBJS): cdb.h uint32.h
$(CRITBIT_OBJS): critbit.h

mult64.o: haveuint128.h


iob_addbuf.o iob_addfile.o iob_new.o iob_reset.o iob_send.o: iob_internal.h iob.h

iopause.o: iopause.h select.h
openreadclose.o readclose.o: readclose.h
dns_rcip.o dns_rcrw.o openreadclose.o: openreadclose.h

iob_send.o scan_ip6if.o: havealloca.h

cdb.o io_mmapwritefile.o: havepread.h

# stoprip

byte.a: $(BYTE_OBJS)
fmt.a: $(FMT_OBJS)
scan.a: $(SCAN_OBJS)
str.a: $(STR_OBJS)
uint.a: $(UINT_OBJS)
open.a: $(OPEN_OBJS)
stralloc.a: $(STRALLOC_OBJS)
unix.a: $(UNIX_OBJS)
socket.a: $(SOCKET_OBJS)
buffer.a: $(BUFFER_OBJS)
mmap.a: $(MMAP_OBJS)
textcode.a: $(TEXTCODE_OBJS)
taia.a: $(TAIA_OBJS)
tai.a: $(TAI_OBJS)
dns.a: $(DNS_OBJS)
case.a: $(CASE_OBJS)
array.a: $(ARRAY_OBJS)
mult.a: $(MULT_OBJS)
io.a: $(IO_OBJS)
cdb.a: $(CDB_OBJS)
critbit.a: $(CRITBIT_OBJS)

ALL_OBJS=$(DNS_OBJS) $(BYTE_OBJS) $(FMT_OBJS) $(SCAN_OBJS) \
$(STR_OBJS) $(UINT_OBJS) $(OPEN_OBJS) $(STRALLOC_OBJS) $(UNIX_OBJS) \
$(SOCKET_OBJS) $(BUFFER_OBJS) $(MMAP_OBJS) $(TEXTCODE_OBJS) \
$(TAIA_OBJS) $(TAI_OBJS) $(CASE_OBJS) $(ARRAY_OBJS) $(MULT_OBJS) \
$(IO_OBJS) $(CDB_OBJS) $(CRITBIT_OBJS)

libowfat.a: $(ALL_OBJS)
	$(CROSS)$(AR) cru $@ $(ALL_OBJS)
	-$(CROSS)$(RANLIB) $@

CFLAGS+=-I.
CFLAGS_OPT+=-I.

%.o: byte/%.c
	$(DIET) $(CCC) -c $< $(CFLAGS_OPT)

%.o: %.c
	$(DIET) $(CCC) -c $< $(CFLAGS)

%.a:
	$(CROSS)$(AR) cru $@ $^
	-$(CROSS)$(RANLIB) $@

t.o: t.c fmt.h scan.h str.h uint16.h uint32.h stralloc.h socket.h \
  buffer.h ip4.h ip6.h byte.h mmap.h open.h textcode.h dns.h iopause.h \
  taia.h tai.h uint64.h case.h errmsg.h iob.h io.h array.h safemult.h \
  iarray.h io_internal.h haveepoll.h havekqueue.h havedevpoll.h \
  havesigio.h CAS.h

t: t.o libowfat.a libsocket
	$(DIET) $(CCC) -g -o $@ t.o libowfat.a `cat libsocket` -lpthread $(LDFLAGS)

.PHONY: all clean tar install rename
clean:
	rm -f *.o *.obj *.a *.lib *.da *.bbg *.bb core t haveip6.h haven2i.h \
havesl.h haveinline.h iopause.h select.h havekqueue.h haveepoll.h \
libepoll havesigio.h havebsdsf.h havesendfile.h havescope.h havedevpoll.h \
dep libsocket havealloca.h haveuint128.h entities.h ent havepread.h haveaccept4.h \
*.gcda *.gcno

INCLUDES=buffer.h byte.h fmt.h ip4.h ip6.h mmap.h scan.h socket.h str.h stralloc.h \
uint16.h uint32.h uint64.h open.h textcode.h tai.h taia.h dns.h iopause.h case.h \
openreadclose.h readclose.h ndelay.h array.h io.h safemult.h iob.h havealloca.h \
errmsg.h cdb.h cdb_make.h rangecheck.h iarray.h va_narg.h isset.h \
compiletimeassert.h critbit.h parse.h compiler.h

libowfat:
	-mkdir libowfat

.PHONY: headers
headers: libowfat $(INCLUDES)
	ln -f $(INCLUDES) libowfat/

install-inc:
	install -d $(DESTDIR)$(INCLUDEDIR)/libowfat
	install -m 644 $(INCLUDES) $(DESTDIR)$(INCLUDEDIR)/libowfat

install-lib: libowfat.a
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 libowfat.a $(DESTDIR)$(LIBDIR)

install-man:
	install -d $(DESTDIR)$(MAN3DIR)
	install -m 644 $(wildcard */*.3) $(DESTDIR)$(MAN3DIR)

install: headers install-inc install-man install-lib

uninstall:
	rm -f $(patsubst %.h,$(INCLUDEDIR)/%.h,$(INCLUDES))
	rm -f $(INCLUDEDIR)/libowfat/*.h
	test -d $(INCLUDEDIR)/libowfat && rmdir $(INCLUDEDIR)/libowfat
	rm -f $(patsubst %.3,$(MAN3DIR)/%.3,$(notdir $(wildcard */*.3)))
	rm -f $(LIBDIR)/libowfat.a

VERSION=libowfat-$(shell head -n 1 CHANGES|sed 's/://')
CURNAME=$(notdir $(shell pwd))

tar: Makefile clean rename
	rm -f dep libdep
	cd ..; tar cvvf $(VERSION).tar.xz --use=xz --exclude CVS $(VERSION)

rename:
	if test $(CURNAME) != $(VERSION); then cd .. && mv $(CURNAME) $(VERSION); fi

haveip6.h: tryip6.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryip6.c >/dev/null 2>&1; then echo "#define LIBC_HAS_IP6"; fi > $@
	-rm -f tryip6.o

havescope.h: tryscope.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryscope.c >/dev/null 2>&1; then echo "#define LIBC_HAS_SCOPE_ID"; fi > $@
	-rm -f tryscope.o

haven2i.h: tryn2i.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -o t tryn2i.c >/dev/null 2>&1; then echo "#define HAVE_N2I"; fi > $@
	-rm -f t

havesl.h: trysl.c
	-rm -f $@
	if ! $(DIET) $(CCC) $(CFLAGS) -o t trysl.c >/dev/null 2>&1; then echo "typedef int socklen_t;"; fi > $@
	-rm -f t

haveinline.h: tryinline.c
	-rm -f $@
	if ! $(DIET) $(CCC) $(CFLAGS) -c tryinline.c >/dev/null 2>&1; then echo "#define inline"; fi > $@
	-rm -f tryinline.o

havekqueue.h: trykqueue.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trykqueue.c >/dev/null 2>&1; then echo "#define HAVE_KQUEUE"; fi > $@
	-rm -f trykqueue.o

havebsdsf.h: trybsdsf.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -o trybsdsf trybsdsf.c >/dev/null 2>&1; then echo "#define HAVE_BSDSENDFILE"; fi > $@
	-rm -f trybsdsf.o trybsdsf

havesendfile.h: trysendfile.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trysendfile.c >/dev/null 2>&1; then echo "#define HAVE_SENDFILE"; fi > $@
	-rm -f trysendfile.o

haveepoll.h: tryepoll.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -o tryepoll tryepoll.c >/dev/null 2>&1; then echo "#define HAVE_EPOLL 1"; else \
	if $(DIET) $(CCC) $(CFLAGS) -o tryepoll tryepoll.c -lepoll >/dev/null 2>&1; then echo "#define HAVE_EPOLL 2"; fi; fi > $@
	-rm -f tryepoll

havedevpoll.h: trydevpoll.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trydevpoll.c >/dev/null 2>&1; then echo "#define HAVE_DEVPOLL"; fi > $@
	-rm -f trydevpoll.o

libepoll: haveepoll.h
	if test "z`cat haveepoll.h`" = "z#define HAVE_EPOLL 2"; then echo -lepoll; fi > $@

havesigio.h: trysigio.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trysigio.c >/dev/null 2>&1; then echo "#define HAVE_SIGIO"; fi > $@
	-rm -f trysigio.o

havealloca.h: tryalloca.c
	-rm -f $@
	echo "#include <stdlib.h>" > $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryalloca.c -DA >/dev/null 2>&1; then echo "#include <alloca.h>"; fi >> $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryalloca.c -DB >/dev/null 2>&1; then echo "#include <malloc.h>"; fi >> $@
	-rm -f tryalloca.o

haveuint128.h: tryuint128.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryuint128.c >/dev/null 2>&1; then echo "#define HAVE_UINT128"; fi > $@
	-rm -f tryuint128.o

havepread.h: trypread.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trypread.c >/dev/null 2>&1; then echo "#define HAVE_PREAD"; fi > $@
	-rm -f trypread.o

iopause.h: iopause.h1 iopause.h2 trypoll.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -o t trypoll.c >/dev/null 2>&1; then cp iopause.h2 iopause.h; else cp iopause.h1 iopause.h; fi
	-rm -f t

select.h: select.h1 select.h2 trysysel.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c trysysel.c >/dev/null 2>&1; then cp select.h2 select.h; else cp select.h1 select.h; fi
	-rm -f trysysel.o

haveaccept4.h: tryaccept4.c
	-rm -f $@
	if $(DIET) $(CCC) $(CFLAGS) -c tryaccept4.c >/dev/null 2>&1; then echo "#define HAVE_ACCEPT4"; fi > $@
	-rm -f tryaccept4.o

libsocket: trysocket.c
	if $(DIET) $(CCC) $(CFLAGS) -o trysocket trysocket.c >/dev/null 2>&1; then echo ""; else \
	if $(DIET) $(CCC) $(CFLAGS) -o trysocket trysocket.c -lsocket >/dev/null 2>&1; then echo "-lsocket"; else \
	if $(DIET) $(CCC) $(CFLAGS) -o trysocket trysocket.c -lsocket -lnsl >/dev/null 2>&1; then echo "-lsocket -lnsl"; \
	fi; fi; fi > blah
	if $(DIET) $(CCC) $(CFLAGS) -o trysocket trysendfile.c `cat blah`>/dev/null 2>&1; then cat blah; else \
	if $(DIET) $(CCC) $(CFLAGS) -o trysocket trysendfile.c -lsendfile `cat blah` >/dev/null 2>&1; then echo "-lsendfile"; cat blah; \
	fi; fi > libsocket
	rm -f blah trysocket

socket_accept6.o socket_bind6.o socket_connect6.o socket_local6.o \
socket_mchopcount6.o socket_mcjoin6.o socket_mcleave6.o socket_mcloop6.o \
socket_recv6.o socket_remote6.o socket_sctp6b.o socket_send6.o \
socket_tcp6b.o socket_tcp6.o socket_udp6.o socket_accept6_flags.o: haveip6.h

socket_accept4_flags.o socket_accept6_flags.o: haveaccept4.h

socket_accept6.o socket_connect6.o socket_local6.o socket_recv6.o \
socket_remote6.o socket_send6.o socket_accept6_flags.o: havescope.h

socket_getifidx.o socket_getifname.o: haven2i.h

socket_accept4.o socket_accept6.o socket_connected.o socket_local4.o \
socket_local6.o socket_recv4.o socket_recv6.o socket_remote4.o \
socket_remote6.o socket_accept4_flags.o socket_accept6_flags.o: havesl.h

dns_nd6.o fmt_xlong.o scan_xlong.o fmt_ip6_flat.o $(TEXTCODE_OBJS): haveinline.h

iob_send.o scan_ip6if.o: havealloca.h

dep: haveip6.h haven2i.h havesl.h haveinline.h iopause.h select.h haveepoll.h havekqueue.h havedevpoll.h havescope.h havesigio.h havebsdsf.h havesendfile.h havealloca.h haveuint128.h entities.h havepread.h haveaccept4.h
	$(CC) -I. -MM `ls */*.c | grep -v test` t.c | sed -e 's@ \./@ @g' > dep

libdep:
	for i in $(LIBS); do (echo -n $$i|tr a-z A-Z|sed 's/.A$$/_OBJS=/'; echo $${i%.a}/*.c|sed -e 's@[^/]*/\([^.]*\)\.c@\1.o @g'); done > libdep

Makefile: GNUmakefile dep libdep
	echo "# do not edit!  edit GNUmakefile instead" > $@
	sed '/startrip/,$$d' < GNUmakefile >> $@
	cat dep libdep >> $@
	sed -e '1,/stoprip/d' -e 's/ %.c$$//' \
	    -e 's/^VERSION=.*/'VERSION=$(VERSION)/ \
	    -e 's/^CURNAME=.*/'CURNAME=$(CURNAME)/ \
	    -e 's/ Makefile//' < GNUmakefile >> $@

windoze:
	$(MAKE) DIET= CROSS=i686-mingw32-

windoze64:
	$(MAKE) DIET= CROSS=x86_64-w64-mingw32-

update:
	dl -n http://www.w3.org/TR/html5/entities.json

entities.h: entities.json ent
	./ent

scan_html.o: entities.h

sa1:
	$(MAKE) clean
	scan-build $(MAKE) DIET= -j8

sa2:
	$(MAKE) clean
	$(MAKE) DIET= CC="gcc -fanalyzer" -j8

UNITTESTS=$(shell grep -l UNITTEST */*.c)
TESTS=$(patsubst %.c,test_%,$(notdir $(UNITTESTS)))

test_%: %.c
	$(CC) -g -o $@ $< -DUNITTEST -I. -fprofile-arcs -ftest-coverage $(LDFLAGS)
	./$@
	rm -f $@

test_scan_html: entities.h

check2: haveuint128.h haveinline.h entities.h
	for i in $(UNITTESTS); do $(CC) -Wall -fprofile-arcs -ftest-coverage -g -o t -DUNITTEST $$i -I. $(LDFLAGS) && ./t || echo FAIL $$i ; done

check: haveuint128.h haveinline.h $(TESTS)
