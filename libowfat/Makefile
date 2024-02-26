# do not edit!  edit GNUmakefile instead
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

array_allocate.o: array/array_allocate.c likely.h safemult.h array.h \
 byte.h
array_bytes.o: array/array_bytes.c array.h
array_cat.o: array/array_cat.c array.h byte.h
array_cat0.o: array/array_cat0.c array.h
array_catb.o: array/array_catb.c array.h byte.h
array_cate.o: array/array_cate.c array.h
array_cats.o: array/array_cats.c array.h str.h
array_cats0.o: array/array_cats0.c array.h str.h
array_equal.o: array/array_equal.c byte.h array.h
array_fail.o: array/array_fail.c array.h
array_get.o: array/array_get.c likely.h safemult.h array.h
array_length.o: array/array_length.c array.h
array_reset.o: array/array_reset.c array.h
array_start.o: array/array_start.c array.h
array_trunc.o: array/array_trunc.c array.h
array_truncate.o: array/array_truncate.c likely.h safemult.h array.h
iarray_allocate.o: array/iarray_allocate.c likely.h iarray.h
iarray_free.o: array/iarray_free.c iarray.h
iarray_get.o: array/iarray_get.c iarray.h
iarray_init.o: array/iarray_init.c iarray.h
iarray_length.o: array/iarray_length.c iarray.h
bs_capacityassert.o: buffer/bs_capacityassert.c parse.h
bs_capacitycheck.o: buffer/bs_capacitycheck.c parse.h
bs_capacityleft.o: buffer/bs_capacityleft.c parse.h
bs_consumeleftovers.o: buffer/bs_consumeleftovers.c parse.h
bs_err.o: buffer/bs_err.c parse.h
bs_get.o: buffer/bs_get.c parse.h
bs_init_bstream_size.o: buffer/bs_init_bstream_size.c parse.h
bs_init_iobuf.o: buffer/bs_init_iobuf.c parse.h
bs_init_iobuf_size.o: buffer/bs_init_iobuf_size.c parse.h
bs_init_membuf.o: buffer/bs_init_membuf.c parse.h
bs_nomoredataassert.o: buffer/bs_nomoredataassert.c parse.h
bs_peek.o: buffer/bs_peek.c parse.h
bs_seterrorstate.o: buffer/bs_seterrorstate.c parse.h
bs_skip.o: buffer/bs_skip.c parse.h
buffer_0.o: buffer/buffer_0.c buffer.h
buffer_0small.o: buffer/buffer_0small.c buffer.h
buffer_1.o: buffer/buffer_1.c buffer.h
buffer_1small.o: buffer/buffer_1small.c buffer.h
buffer_2.o: buffer/buffer_2.c buffer.h
buffer_close.o: buffer/buffer_close.c buffer.h
buffer_feed.o: buffer/buffer_feed.c buffer.h
buffer_flush.o: buffer/buffer_flush.c buffer.h
buffer_free.o: buffer/buffer_free.c buffer.h
buffer_fromarray.o: buffer/buffer_fromarray.c array.h buffer.h
buffer_frombuf.o: buffer/buffer_frombuf.c stralloc.h buffer.h
buffer_fromsa.o: buffer/buffer_fromsa.c stralloc.h buffer.h
buffer_get.o: buffer/buffer_get.c byte.h buffer.h
buffer_get_new_token_sa.o: buffer/buffer_get_new_token_sa.c stralloc.h \
 buffer.h
buffer_get_new_token_sa_pred.o: buffer/buffer_get_new_token_sa_pred.c \
 stralloc.h buffer.h
buffer_get_token.o: buffer/buffer_get_token.c byte.h buffer.h scan.h
buffer_get_token_pred.o: buffer/buffer_get_token_pred.c byte.h buffer.h \
 scan.h
buffer_get_token_sa.o: buffer/buffer_get_token_sa.c byte.h stralloc.h \
 buffer.h
buffer_get_token_sa_pred.o: buffer/buffer_get_token_sa_pred.c byte.h \
 stralloc.h buffer.h
buffer_getc.o: buffer/buffer_getc.c byte.h buffer.h
buffer_getline.o: buffer/buffer_getline.c buffer.h
buffer_getline_sa.o: buffer/buffer_getline_sa.c stralloc.h buffer.h
buffer_getn.o: buffer/buffer_getn.c byte.h buffer.h
buffer_getnewline_sa.o: buffer/buffer_getnewline_sa.c stralloc.h buffer.h
buffer_init.o: buffer/buffer_init.c buffer.h
buffer_init_allocbuf.o: buffer/buffer_init_allocbuf.c buffer.h
buffer_init_free.o: buffer/buffer_init_free.c buffer.h
buffer_init_read.o: buffer/buffer_init_read.c buffer.h
buffer_init_read_allocbuf.o: buffer/buffer_init_read_allocbuf.c buffer.h
buffer_init_staticcontents.o: buffer/buffer_init_staticcontents.c \
 buffer.h mmap.h
buffer_init_staticcontents_free.o: \
 buffer/buffer_init_staticcontents_free.c buffer.h mmap.h
buffer_init_write.o: buffer/buffer_init_write.c buffer.h
buffer_init_write_allocbuf.o: buffer/buffer_init_write_allocbuf.c \
 buffer.h
buffer_mmapread.o: buffer/buffer_mmapread.c buffer.h mmap.h
buffer_munmap.o: buffer/buffer_munmap.c buffer.h
buffer_peek.o: buffer/buffer_peek.c buffer.h
buffer_peekc.o: buffer/buffer_peekc.c byte.h buffer.h
buffer_put.o: buffer/buffer_put.c buffer.h
buffer_put8long.o: buffer/buffer_put8long.c buffer.h fmt.h
buffer_putalign.o: buffer/buffer_putalign.c byte.h buffer.h
buffer_puterror.o: buffer/buffer_puterror.c buffer.h
buffer_puterror2.o: buffer/buffer_puterror2.c buffer.h
buffer_putflush.o: buffer/buffer_putflush.c buffer.h
buffer_putlong.o: buffer/buffer_putlong.c buffer.h fmt.h
buffer_putlonglong.o: buffer/buffer_putlonglong.c buffer.h fmt.h
buffer_putm_internal.o: buffer/buffer_putm_internal.c buffer.h
buffer_putm_internal_flush.o: buffer/buffer_putm_internal_flush.c \
 buffer.h
buffer_putnlflush.o: buffer/buffer_putnlflush.c str.h buffer.h
buffer_puts.o: buffer/buffer_puts.c str.h buffer.h
buffer_putsa.o: buffer/buffer_putsa.c stralloc.h buffer.h
buffer_putsaflush.o: buffer/buffer_putsaflush.c stralloc.h buffer.h
buffer_putsalign.o: buffer/buffer_putsalign.c str.h buffer.h
buffer_putsflush.o: buffer/buffer_putsflush.c str.h buffer.h
buffer_putspace.o: buffer/buffer_putspace.c str.h buffer.h
buffer_putulong.o: buffer/buffer_putulong.c buffer.h fmt.h
buffer_putulonglong.o: buffer/buffer_putulonglong.c buffer.h fmt.h
buffer_putxlong.o: buffer/buffer_putxlong.c buffer.h fmt.h
buffer_seek.o: buffer/buffer_seek.c buffer.h
buffer_stubborn.o: buffer/buffer_stubborn.c buffer.h
buffer_stubborn2.o: buffer/buffer_stubborn2.c buffer.h
buffer_tosa.o: buffer/buffer_tosa.c stralloc.h buffer.h
errmsg_iam.o: buffer/errmsg_iam.c errmsg.h
errmsg_info.o: buffer/errmsg_info.c errmsg.h errmsg_int.h str.h
errmsg_infosys.o: buffer/errmsg_infosys.c errmsg.h errmsg_int.h str.h
errmsg_puts.o: buffer/errmsg_puts.c errmsg.h errmsg_int.h str.h
errmsg_warn.o: buffer/errmsg_warn.c errmsg.h errmsg_int.h str.h
errmsg_warnsys.o: buffer/errmsg_warnsys.c errmsg.h errmsg_int.h str.h
errmsg_write.o: buffer/errmsg_write.c errmsg.h errmsg_int.h
prs_asciiz.o: buffer/prs_asciiz.c parse.h
prs_asciiz_fixedlen.o: buffer/prs_asciiz_fixedlen.c parse.h
prs_readblob.o: buffer/prs_readblob.c parse.h
prs_u16.o: buffer/prs_u16.c parse.h
prs_u16_big.o: buffer/prs_u16_big.c parse.h
prs_u32.o: buffer/prs_u32.c parse.h
prs_u32_big.o: buffer/prs_u32_big.c parse.h
prs_u64.o: buffer/prs_u64.c parse.h
prs_u64_big.o: buffer/prs_u64_big.c parse.h
byte_chr.o: byte/byte_chr.c byte.h
byte_copy.o: byte/byte_copy.c byte.h
byte_copyr.o: byte/byte_copyr.c byte.h
byte_diff.o: byte/byte_diff.c byte.h
byte_equal_notimingattack.o: byte/byte_equal_notimingattack.c byte.h
byte_rchr.o: byte/byte_rchr.c byte.h
byte_start.o: byte/byte_start.c byte.h
byte_starts.o: byte/byte_starts.c byte.h
byte_zero.o: byte/byte_zero.c byte.h
case_diffb.o: case/case_diffb.c case.h
case_diffs.o: case/case_diffs.c case.h
case_lowerb.o: case/case_lowerb.c case.h
case_lowers.o: case/case_lowers.c case.h
case_starts.o: case/case_starts.c case.h
cdb.o: cdb/cdb.c byte.h cdb.h havepread.h
cdb_hash.o: cdb/cdb_hash.c cdb.h
cdb_make.o: cdb/cdb_make.c cdb.h cdb_make.h
cdb_traverse.o: cdb/cdb_traverse.c cdb.h
critbit.o: critbit/critbit.c critbit.h
dns_dfd.o: dns/dns_dfd.c byte.h dns.h
dns_domain.o: dns/dns_domain.c case.h byte.h dns.h
dns_dtda.o: dns/dns_dtda.c stralloc.h dns.h
dns_ip.o: dns/dns_ip.c stralloc.h uint16.h byte.h dns.h
dns_ip6.o: dns/dns_ip6.c stralloc.h uint16.h byte.h dns.h ip4.h ip6.h
dns_ipq.o: dns/dns_ipq.c stralloc.h case.h byte.h str.h dns.h
dns_ipq6.o: dns/dns_ipq6.c stralloc.h case.h byte.h str.h dns.h
dns_mx.o: dns/dns_mx.c stralloc.h byte.h uint16.h dns.h
dns_name.o: dns/dns_name.c stralloc.h uint16.h byte.h dns.h ip6.h
dns_nd.o: dns/dns_nd.c byte.h fmt.h dns.h
dns_nd6.o: dns/dns_nd6.c byte.h fmt.h dns.h haveinline.h
dns_packet.o: dns/dns_packet.c dns.h
dns_random.o: dns/dns_random.c dns.h taia.h uint32.h
dns_rcip.o: dns/dns_rcip.c taia.h openreadclose.h byte.h ip4.h ip6.h \
 dns.h
dns_rcrw.o: dns/dns_rcrw.c taia.h byte.h str.h openreadclose.h dns.h
dns_resolve.o: dns/dns_resolve.c iopause.h taia.h taia.h byte.h dns.h \
 ip6.h
dns_sortip.o: dns/dns_sortip.c byte.h dns.h
dns_sortip6.o: dns/dns_sortip6.c byte.h dns.h
dns_transmit.o: dns/dns_transmit.c socket.h byte.h uint16.h dns.h ip6.h
dns_txt.o: dns/dns_txt.c stralloc.h uint16.h byte.h dns.h
buffer_getline.o: examples/buffer_getline.c stralloc.h buffer.h io.h \
 errmsg.h
byte.o: examples/byte.c byte.h buffer.h
str.o: examples/str.c str.h buffer.h
fmt_8long.o: fmt/fmt_8long.c fmt.h
fmt_8longlong.o: fmt/fmt_8longlong.c fmt.h
fmt_asn1derlength.o: fmt/fmt_asn1derlength.c fmt.h
fmt_asn1dertag.o: fmt/fmt_asn1dertag.c fmt.h
fmt_double.o: fmt/fmt_double.c fmt.h
fmt_escapecharc.o: fmt/fmt_escapecharc.c fmt.h
fmt_escapecharhtml.o: fmt/fmt_escapecharhtml.c
fmt_escapecharjson.o: fmt/fmt_escapecharjson.c fmt.h
fmt_escapecharquotedprintable.o: fmt/fmt_escapecharquotedprintable.c \
 fmt.h
fmt_escapecharquotedprintableutf8.o: \
 fmt/fmt_escapecharquotedprintableutf8.c fmt.h
fmt_escapecharxml.o: fmt/fmt_escapecharxml.c fmt.h
fmt_fill.o: fmt/fmt_fill.c fmt.h
fmt_httpdate.o: fmt/fmt_httpdate.c fmt.h byte.h
fmt_human.o: fmt/fmt_human.c fmt.h
fmt_humank.o: fmt/fmt_humank.c fmt.h
fmt_iso8601.o: fmt/fmt_iso8601.c fmt.h byte.h
fmt_long.o: fmt/fmt_long.c fmt.h
fmt_longlong.o: fmt/fmt_longlong.c fmt.h
fmt_minus.o: fmt/fmt_minus.c fmt.h
fmt_netstring.o: fmt/fmt_netstring.c fmt.h rangecheck.h
fmt_pad.o: fmt/fmt_pad.c fmt.h
fmt_pb_double.o: fmt/fmt_pb_double.c fmt.h
fmt_pb_float.o: fmt/fmt_pb_float.c fmt.h
fmt_pb_int.o: fmt/fmt_pb_int.c fmt.h
fmt_pb_sint.o: fmt/fmt_pb_sint.c fmt.h
fmt_pb_string.o: fmt/fmt_pb_string.c fmt.h rangecheck.h
fmt_pb_tag.o: fmt/fmt_pb_tag.c fmt.h
fmt_pb_type0_sint.o: fmt/fmt_pb_type0_sint.c fmt.h
fmt_pb_type1_double.o: fmt/fmt_pb_type1_double.c fmt.h \
 compiletimeassert.h
fmt_pb_type1_fixed64.o: fmt/fmt_pb_type1_fixed64.c fmt.h uint64.h
fmt_pb_type2_string.o: fmt/fmt_pb_type2_string.c fmt.h rangecheck.h
fmt_pb_type5_fixed32.o: fmt/fmt_pb_type5_fixed32.c fmt.h uint32.h
fmt_pb_type5_float.o: fmt/fmt_pb_type5_float.c fmt.h compiletimeassert.h
fmt_plusminus.o: fmt/fmt_plusminus.c fmt.h
fmt_str.o: fmt/fmt_str.c fmt.h
fmt_strm_internal.o: fmt/fmt_strm_internal.c fmt.h
fmt_strn.o: fmt/fmt_strn.c fmt.h
fmt_tohex.o: fmt/fmt_tohex.c fmt.h
fmt_ulong.o: fmt/fmt_ulong.c fmt.h
fmt_ulong0.o: fmt/fmt_ulong0.c fmt.h
fmt_ulonglong.o: fmt/fmt_ulonglong.c fmt.h
fmt_utf8.o: fmt/fmt_utf8.c fmt.h
fmt_varint.o: fmt/fmt_varint.c fmt.h
fmt_xlong.o: fmt/fmt_xlong.c fmt.h haveinline.h
fmt_xlonglong.o: fmt/fmt_xlonglong.c fmt.h
fmt_xmlescape.o: fmt/fmt_xmlescape.c fmt.h
io_appendfile.o: io/io_appendfile.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_block.o: io/io_block.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_canread.o: io/io_canread.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_canwrite.o: io/io_canwrite.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_check.o: io/io_check.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_close.o: io/io_close.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_closeonexec.o: io/io_closeonexec.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_createfile.o: io/io_createfile.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_debugstring.o: io/io_debugstring.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h iarray.h fmt.h
io_dontwantread.o: io/io_dontwantread.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h byte.h
io_dontwantwrite.o: io/io_dontwantwrite.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h byte.h
io_eagain.o: io/io_eagain.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_eagain_read.o: io/io_eagain_read.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_eagain_write.o: io/io_eagain_write.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_fd.o: io/io_fd.c io_internal.h haveepoll.h havekqueue.h havedevpoll.h \
 havesigio.h byte.h
io_finishandshutdown.o: io/io_finishandshutdown.c io_internal.h \
 haveepoll.h havekqueue.h havedevpoll.h havesigio.h
io_getcookie.o: io/io_getcookie.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_mmapwritefile.o: io/io_mmapwritefile.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h iob.h havepread.h
io_nonblock.o: io/io_nonblock.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_passfd.o: io/io_passfd.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_pipe.o: io/io_pipe.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_readfile.o: io/io_readfile.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_readwritefile.o: io/io_readwritefile.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_receivefd.o: io/io_receivefd.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_sendfile.o: io/io_sendfile.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h havebsdsf.h havesendfile.h
io_setcookie.o: io/io_setcookie.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_sigpipe.o: io/io_sigpipe.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_socketpair.o: io/io_socketpair.c windoze.h io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_timedout.o: io/io_timedout.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_timeout.o: io/io_timeout.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_timeouted.o: io/io_timeouted.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_tryread.o: io/io_tryread.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h byte.h
io_tryreadtimeout.o: io/io_tryreadtimeout.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_trywrite.o: io/io_trywrite.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_trywritetimeout.o: io/io_trywritetimeout.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_wait.o: io/io_wait.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_waitread.o: io/io_waitread.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_waituntil.o: io/io_waituntil.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h safemult.h
io_waituntil2.o: io/io_waituntil2.c io_internal.h haveepoll.h \
 havekqueue.h havedevpoll.h havesigio.h
io_waitwrite.o: io/io_waitwrite.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
io_wantread.o: io/io_wantread.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h byte.h
io_wantwrite.o: io/io_wantwrite.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h byte.h
iob_addbuf.o: io/iob_addbuf.c iob_internal.h iob.h array.h
iob_addbuf_free.o: io/iob_addbuf_free.c iob_internal.h iob.h array.h
iob_addbuf_internal.o: io/iob_addbuf_internal.c iob_internal.h iob.h \
 array.h
iob_addbuf_munmap.o: io/iob_addbuf_munmap.c mmap.h iob_internal.h iob.h \
 array.h
iob_addfile.o: io/iob_addfile.c iob_internal.h iob.h array.h
iob_addfile_close.o: io/iob_addfile_close.c iob_internal.h iob.h array.h
iob_adds.o: io/iob_adds.c str.h iob.h
iob_adds_free.o: io/iob_adds_free.c str.h iob.h
iob_bytesleft.o: io/iob_bytesleft.c iob.h
iob_free.o: io/iob_free.c iob_internal.h iob.h array.h
iob_new.o: io/iob_new.c byte.h iob_internal.h iob.h array.h
iob_prefetch.o: io/iob_prefetch.c iob_internal.h iob.h array.h
iob_reset.o: io/iob_reset.c byte.h iob_internal.h iob.h array.h
iob_send.o: io/iob_send.c havebsdsf.h havealloca.h io_internal.h \
 haveepoll.h havekqueue.h havedevpoll.h havesigio.h iob_internal.h iob.h \
 array.h
iob_write.o: io/iob_write.c iob_internal.h iob.h array.h
iom_abort.o: io/iom_abort.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
iom_add.o: io/iom_add.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
iom_init.o: io/iom_init.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
iom_wait.o: io/iom_wait.c io_internal.h haveepoll.h havekqueue.h \
 havedevpoll.h havesigio.h
mmap_private.o: mmap/mmap_private.c open.h mmap.h
mmap_read.o: mmap/mmap_read.c open.h mmap.h
mmap_readat.o: mmap/mmap_readat.c open.h mmap.h
mmap_shared.o: mmap/mmap_shared.c open.h mmap.h
mmap_unmap.o: mmap/mmap_unmap.c open.h mmap.h
imult16.o: mult/imult16.c uint16.h
imult32.o: mult/imult32.c uint32.h
imult64.o: mult/imult64.c uint64.h
range_arrayinbuf.o: mult/range_arrayinbuf.c rangecheck.h safemult.h
range_str2inbuf.o: mult/range_str2inbuf.c rangecheck.h
range_str4inbuf.o: mult/range_str4inbuf.c rangecheck.h
range_strinbuf.o: mult/range_strinbuf.c rangecheck.h
umult16.o: mult/umult16.c uint16.h
umult32.o: mult/umult32.c uint32.h
umult64.o: mult/umult64.c uint64.h
open_append.o: open/open_append.c open.h
open_excl.o: open/open_excl.c open.h
open_read.o: open/open_read.c open.h
open_rw.o: open/open_rw.c open.h
open_trunc.o: open/open_trunc.c open.h
open_write.o: open/open_write.c open.h
openreadclose.o: open/openreadclose.c open.h readclose.h stralloc.h \
 openreadclose.h
readclose.o: open/readclose.c readclose.h stralloc.h
scan_8int.o: scan/scan_8int.c scan.h
scan_8long.o: scan/scan_8long.c scan.h
scan_8longlong.o: scan/scan_8longlong.c scan.h
scan_8longn.o: scan/scan_8longn.c scan.h
scan_8short.o: scan/scan_8short.c scan.h
scan_asn1derlength.o: scan/scan_asn1derlength.c scan.h
scan_asn1dertag.o: scan/scan_asn1dertag.c scan.h
scan_charsetnskip.o: scan/scan_charsetnskip.c scan.h
scan_double.o: scan/scan_double.c scan.h
scan_fromhex.o: scan/scan_fromhex.c scan.h
scan_httpdate.o: scan/scan_httpdate.c scan.h byte.h case.h
scan_int.o: scan/scan_int.c scan.h
scan_iso8601.o: scan/scan_iso8601.c scan.h byte.h case.h
scan_long.o: scan/scan_long.c scan.h
scan_longlong.o: scan/scan_longlong.c scan.h
scan_longn.o: scan/scan_longn.c scan.h
scan_netstring.o: scan/scan_netstring.c scan.h
scan_noncharsetnskip.o: scan/scan_noncharsetnskip.c scan.h
scan_nonwhitenskip.o: scan/scan_nonwhitenskip.c scan.h
scan_pb_tag.o: scan/scan_pb_tag.c scan.h
scan_pb_type0_sint.o: scan/scan_pb_type0_sint.c scan.h
scan_pb_type1_double.o: scan/scan_pb_type1_double.c scan.h \
 compiletimeassert.h
scan_pb_type1_fixed64.o: scan/scan_pb_type1_fixed64.c scan.h uint64.h
scan_pb_type2_stringlen.o: scan/scan_pb_type2_stringlen.c scan.h \
 rangecheck.h
scan_pb_type5_fixed32.o: scan/scan_pb_type5_fixed32.c scan.h uint32.h
scan_pb_type5_float.o: scan/scan_pb_type5_float.c scan.h \
 compiletimeassert.h
scan_plusminus.o: scan/scan_plusminus.c scan.h
scan_short.o: scan/scan_short.c scan.h
scan_uint.o: scan/scan_uint.c scan.h
scan_ulong.o: scan/scan_ulong.c scan.h
scan_ulonglong.o: scan/scan_ulonglong.c scan.h
scan_ulongn.o: scan/scan_ulongn.c scan.h haveuint128.h
scan_ushort.o: scan/scan_ushort.c scan.h
scan_utf8.o: scan/scan_utf8.c fmt.h
scan_utf8_sem.o: scan/scan_utf8_sem.c scan.h
scan_varint.o: scan/scan_varint.c scan.h
scan_whitenskip.o: scan/scan_whitenskip.c scan.h
scan_xint.o: scan/scan_xint.c scan.h
scan_xlong.o: scan/scan_xlong.c scan.h
scan_xlonglong.o: scan/scan_xlonglong.c scan.h
scan_xlongn.o: scan/scan_xlongn.c scan.h
scan_xshort.o: scan/scan_xshort.c scan.h
fmt_ip4.o: socket/fmt_ip4.c fmt.h ip4.h
fmt_ip6.o: socket/fmt_ip6.c fmt.h byte.h ip4.h ip6.h
fmt_ip6_flat.o: socket/fmt_ip6_flat.c ip6.h haveinline.h fmt.h
fmt_ip6c.o: socket/fmt_ip6c.c fmt.h byte.h ip4.h ip6.h
fmt_ip6if.o: socket/fmt_ip6if.c ip6.h str.h fmt.h socket.h
fmt_ip6ifc.o: socket/fmt_ip6ifc.c fmt.h byte.h ip4.h ip6.h
init.o: socket/init.c
scan_ip4.o: socket/scan_ip4.c scan.h ip4.h
scan_ip6.o: socket/scan_ip6.c scan.h ip4.h ip6.h
scan_ip6_flat.o: socket/scan_ip6_flat.c scan.h
scan_ip6if.o: socket/scan_ip6if.c ip6.h byte.h socket.h havealloca.h
socket_accept4.o: socket/socket_accept4.c windoze.h socket.h havesl.h
socket_accept4_flags.o: socket/socket_accept4_flags.c windoze.h socket.h \
 iarray.h havesl.h haveaccept4.h
socket_accept4_makenonblocking.o: socket/socket_accept4_makenonblocking.c \
 windoze.h socket.h
socket_accept4_makenonblocking_setcloseonexec.o: \
 socket/socket_accept4_makenonblocking_setcloseonexec.c windoze.h \
 socket.h
socket_accept4_setcloseonexec.o: socket/socket_accept4_setcloseonexec.c \
 windoze.h socket.h
socket_accept6.o: socket/socket_accept6.c windoze.h byte.h socket.h ip6.h \
 haveip6.h havesl.h havescope.h
socket_accept6_flags.o: socket/socket_accept6_flags.c iarray.h windoze.h \
 byte.h socket.h ip6.h haveip6.h havesl.h havescope.h haveaccept4.h
socket_accept6_makenonblocking.o: socket/socket_accept6_makenonblocking.c \
 windoze.h socket.h
socket_accept6_makenonblocking_setcloseonexec.o: \
 socket/socket_accept6_makenonblocking_setcloseonexec.c windoze.h \
 socket.h
socket_accept6_setcloseonexec.o: socket/socket_accept6_setcloseonexec.c \
 windoze.h socket.h
socket_bind4.o: socket/socket_bind4.c windoze.h byte.h uint16.h uint32.h \
 socket.h
socket_bind4_reuse.o: socket/socket_bind4_reuse.c socket.h windoze.h
socket_bind6.o: socket/socket_bind6.c haveip6.h windoze.h ip6.h byte.h \
 socket.h
socket_bind6_reuse.o: socket/socket_bind6_reuse.c socket.h windoze.h
socket_broadcast.o: socket/socket_broadcast.c socket.h windoze.h
socket_connect4.o: socket/socket_connect4.c windoze.h byte.h socket.h \
 uint16.h uint32.h
socket_connect6.o: socket/socket_connect6.c windoze.h byte.h socket.h \
 ip6.h haveip6.h uint32.h ip4.h havescope.h
socket_connected.o: socket/socket_connected.c socket.h havesl.h
socket_deferaccept.o: socket/socket_deferaccept.c socket.h windoze.h
socket_fastopen.o: socket/socket_fastopen.c socket.h
socket_fastopen_connect4.o: socket/socket_fastopen_connect4.c socket.h
socket_fastopen_connect6.o: socket/socket_fastopen_connect6.c socket.h
socket_getifidx.o: socket/socket_getifidx.c socket.h haven2i.h
socket_getifname.o: socket/socket_getifname.c socket.h haven2i.h
socket_ip4loopback.o: socket/socket_ip4loopback.c
socket_listen.o: socket/socket_listen.c socket.h windoze.h
socket_local4.o: socket/socket_local4.c windoze.h byte.h socket.h \
 havesl.h
socket_local6.o: socket/socket_local6.c windoze.h byte.h socket.h ip6.h \
 haveip6.h uint32.h havesl.h havescope.h
socket_mchopcount6.o: socket/socket_mchopcount6.c windoze.h socket.h \
 byte.h haveip6.h ip6.h
socket_mcjoin4.o: socket/socket_mcjoin4.c windoze.h socket.h byte.h
socket_mcjoin6.o: socket/socket_mcjoin6.c windoze.h socket.h byte.h \
 haveip6.h ip6.h
socket_mcleave4.o: socket/socket_mcleave4.c windoze.h socket.h byte.h
socket_mcleave6.o: socket/socket_mcleave6.c windoze.h socket.h byte.h \
 haveip6.h ip6.h
socket_mcloop4.o: socket/socket_mcloop4.c windoze.h socket.h
socket_mcloop6.o: socket/socket_mcloop6.c windoze.h socket.h haveip6.h
socket_mcttl4.o: socket/socket_mcttl4.c windoze.h socket.h
socket_noipv6.o: socket/socket_noipv6.c
socket_quickack.o: socket/socket_quickack.c socket.h
socket_recv4.o: socket/socket_recv4.c windoze.h socket.h havesl.h
socket_recv6.o: socket/socket_recv6.c windoze.h byte.h socket.h ip6.h \
 haveip6.h havesl.h havescope.h
socket_remote4.o: socket/socket_remote4.c windoze.h byte.h socket.h \
 havesl.h
socket_remote6.o: socket/socket_remote6.c windoze.h byte.h socket.h ip6.h \
 haveip6.h uint32.h havesl.h havescope.h
socket_sctp4.o: socket/socket_sctp4.c socket.h ndelay.h
socket_sctp4b.o: socket/socket_sctp4b.c windoze.h socket.h ndelay.h
socket_sctp6.o: socket/socket_sctp6.c socket.h ndelay.h
socket_sctp6b.o: socket/socket_sctp6b.c windoze.h haveip6.h socket.h \
 ndelay.h
socket_send4.o: socket/socket_send4.c windoze.h byte.h socket.h
socket_send6.o: socket/socket_send6.c windoze.h byte.h socket.h ip6.h \
 haveip6.h ip4.h havescope.h
socket_tcp4.o: socket/socket_tcp4.c socket.h ndelay.h
socket_tcp4b.o: socket/socket_tcp4b.c windoze.h socket.h ndelay.h
socket_tcp6.o: socket/socket_tcp6.c haveip6.h socket.h ndelay.h
socket_tcp6b.o: socket/socket_tcp6b.c windoze.h haveip6.h socket.h \
 ndelay.h
socket_tryreservein.o: socket/socket_tryreservein.c windoze.h socket.h
socket_udp4.o: socket/socket_udp4.c windoze.h socket.h ndelay.h
socket_udp6.o: socket/socket_udp6.c windoze.h haveip6.h socket.h ndelay.h
socket_v4mappedprefix.o: socket/socket_v4mappedprefix.c
socket_v6any.o: socket/socket_v6any.c
socket_v6loopback.o: socket/socket_v6loopback.c
str_chr.o: str/str_chr.c str.h
str_copy.o: str/str_copy.c str.h
str_diff.o: str/str_diff.c byte.h
str_diffn.o: str/str_diffn.c byte.h
str_len.o: str/str_len.c str.h
str_rchr.o: str/str_rchr.c str.h
str_start.o: str/str_start.c str.h
stralloc_append.o: stralloc/stralloc_append.c stralloc.h
stralloc_cat.o: stralloc/stralloc_cat.c stralloc.h str.h
stralloc_catb.o: stralloc/stralloc_catb.c byte.h stralloc.h
stralloc_catlong0.o: stralloc/stralloc_catlong0.c stralloc.h fmt.h
stralloc_catm_internal.o: stralloc/stralloc_catm_internal.c stralloc.h
stralloc_cats.o: stralloc/stralloc_cats.c stralloc.h str.h
stralloc_catulong0.o: stralloc/stralloc_catulong0.c stralloc.h fmt.h
stralloc_chomp.o: stralloc/stralloc_chomp.c stralloc.h
stralloc_chop.o: stralloc/stralloc_chop.c stralloc.h
stralloc_copy.o: stralloc/stralloc_copy.c stralloc.h str.h
stralloc_copyb.o: stralloc/stralloc_copyb.c byte.h stralloc.h
stralloc_copys.o: stralloc/stralloc_copys.c stralloc.h str.h
stralloc_diff.o: stralloc/stralloc_diff.c stralloc.h byte.h str.h
stralloc_diffs.o: stralloc/stralloc_diffs.c stralloc.h byte.h str.h
stralloc_free.o: stralloc/stralloc_free.c stralloc.h
stralloc_init.o: stralloc/stralloc_init.c stralloc.h
stralloc_ready.o: stralloc/stralloc_ready.c stralloc.h
stralloc_readyplus.o: stralloc/stralloc_readyplus.c stralloc.h
stralloc_starts.o: stralloc/stralloc_starts.c stralloc.h byte.h str.h
stralloc_zero.o: stralloc/stralloc_zero.c stralloc.h
tai_add.o: tai/tai_add.c tai.h
tai_now.o: tai/tai_now.c tai.h
tai_pack.o: tai/tai_pack.c tai.h
tai_sub.o: tai/tai_sub.c tai.h
tai_uint.o: tai/tai_uint.c tai.h
tai_unpack.o: tai/tai_unpack.c tai.h
taia_add.o: taia/taia_add.c taia.h
taia_addsec.o: taia/taia_addsec.c taia.h
taia_approx.o: taia/taia_approx.c taia.h
taia_frac.o: taia/taia_frac.c taia.h
taia_half.o: taia/taia_half.c taia.h
taia_less.o: taia/taia_less.c taia.h
taia_now.o: taia/taia_now.c taia.h
taia_pack.o: taia/taia_pack.c taia.h
taia_sub.o: taia/taia_sub.c taia.h
taia_tai.o: taia/taia_tai.c taia.h
taia_uint.o: taia/taia_uint.c taia.h
taia_unpack.o: taia/taia_unpack.c taia.h
base64.o: textcode/base64.c
base64url.o: textcode/base64url.c
fmt_base64.o: textcode/fmt_base64.c fmt.h textcode.h haveinline.h
fmt_base64url.o: textcode/fmt_base64url.c fmt.h textcode.h haveinline.h
fmt_cescape.o: textcode/fmt_cescape.c fmt.h textcode.h str.h haveinline.h
fmt_foldwhitespace.o: textcode/fmt_foldwhitespace.c fmt.h textcode.h \
 str.h haveinline.h
fmt_hexdump.o: textcode/fmt_hexdump.c fmt.h textcode.h str.h haveinline.h
fmt_html.o: textcode/fmt_html.c fmt.h textcode.h str.h haveinline.h
fmt_html_tagarg.o: textcode/fmt_html_tagarg.c fmt.h textcode.h str.h \
 haveinline.h
fmt_jsonescape.o: textcode/fmt_jsonescape.c fmt.h textcode.h str.h scan.h \
 case.h haveinline.h
fmt_ldapescape.o: textcode/fmt_ldapescape.c fmt.h textcode.h haveinline.h \
 str.h
fmt_ldapescape2.o: textcode/fmt_ldapescape2.c fmt.h textcode.h \
 haveinline.h
fmt_quotedprintable.o: textcode/fmt_quotedprintable.c fmt.h textcode.h \
 haveinline.h str.h
fmt_to_array.o: textcode/fmt_to_array.c array.h textcode.h
fmt_to_sa.o: textcode/fmt_to_sa.c stralloc.h textcode.h
fmt_tofrom_array.o: textcode/fmt_tofrom_array.c array.h textcode.h
fmt_urlencoded.o: textcode/fmt_urlencoded.c fmt.h textcode.h str.h \
 haveinline.h
fmt_uuencoded.o: textcode/fmt_uuencoded.c fmt.h textcode.h haveinline.h
fmt_xml.o: textcode/fmt_xml.c fmt.h textcode.h str.h haveinline.h
fmt_yenc.o: textcode/fmt_yenc.c fmt.h textcode.h
scan_base64.o: textcode/scan_base64.c textcode.h haveinline.h
scan_base64url.o: textcode/scan_base64url.c textcode.h haveinline.h
scan_cescape.o: textcode/scan_cescape.c fmt.h textcode.h scan.h
scan_hexdump.o: textcode/scan_hexdump.c fmt.h textcode.h scan.h
scan_html.o: textcode/scan_html.c entities.h fmt.h textcode.h \
 haveinline.h scan.h case.h str.h
scan_jsonescape.o: textcode/scan_jsonescape.c fmt.h textcode.h scan.h
scan_ldapescape.o: textcode/scan_ldapescape.c fmt.h textcode.h scan.h
scan_quotedprintable.o: textcode/scan_quotedprintable.c fmt.h textcode.h \
 scan.h
scan_to_array.o: textcode/scan_to_array.c str.h array.h textcode.h
scan_to_sa.o: textcode/scan_to_sa.c str.h stralloc.h textcode.h
scan_tofrom_array.o: textcode/scan_tofrom_array.c str.h array.h \
 textcode.h
scan_urlencoded.o: textcode/scan_urlencoded.c fmt.h textcode.h scan.h
scan_uuencoded.o: textcode/scan_uuencoded.c textcode.h
scan_yenc.o: textcode/scan_yenc.c fmt.h textcode.h
uint16_pack.o: uint/uint16_pack.c uint16.h
uint16_pack_big.o: uint/uint16_pack_big.c uint16.h
uint16_read.o: uint/uint16_read.c uint16.h
uint16_read_big.o: uint/uint16_read_big.c uint16.h
uint16_unpack.o: uint/uint16_unpack.c uint16.h
uint16_unpack_big.o: uint/uint16_unpack_big.c uint16.h
uint32_pack.o: uint/uint32_pack.c uint32.h
uint32_pack_big.o: uint/uint32_pack_big.c uint32.h
uint32_read.o: uint/uint32_read.c uint32.h
uint32_read_big.o: uint/uint32_read_big.c uint32.h
uint32_unpack.o: uint/uint32_unpack.c uint32.h
uint32_unpack_big.o: uint/uint32_unpack_big.c uint32.h
uint64_pack.o: uint/uint64_pack.c uint64.h uint32.h
uint64_pack_big.o: uint/uint64_pack_big.c uint64.h uint32.h
uint64_read.o: uint/uint64_read.c uint64.h uint32.h
uint64_read_big.o: uint/uint64_read_big.c uint64.h uint32.h
uint64_unpack.o: uint/uint64_unpack.c uint64.h uint32.h
uint64_unpack_big.o: uint/uint64_unpack_big.c uint64.h uint32.h
iopause.o: unix/iopause.c taia.h iopause.h taia.h select.h
ndelay_off.o: unix/ndelay_off.c ndelay.h
ndelay_on.o: unix/ndelay_on.c ndelay.h
winsock2errno.o: unix/winsock2errno.c
t.o: t.c fmt.h scan.h str.h uint16.h uint32.h stralloc.h socket.h \
 buffer.h ip4.h ip6.h mmap.h open.h byte.h textcode.h dns.h case.h \
 errmsg.h iob.h safemult.h iarray.h critbit.h compiletimeassert.h parse.h \
 CAS.h io_internal.h haveepoll.h havekqueue.h havedevpoll.h havesigio.h
BYTE_OBJS=byte_chr.o byte_copy.o byte_copyr.o byte_diff.o byte_equal_notimingattack.o byte_rchr.o byte_start.o byte_starts.o byte_zero.o 
FMT_OBJS=fmt_8long.o fmt_8longlong.o fmt_asn1derlength.o fmt_asn1dertag.o fmt_double.o fmt_escapecharc.o fmt_escapecharhtml.o fmt_escapecharjson.o fmt_escapecharquotedprintable.o fmt_escapecharquotedprintableutf8.o fmt_escapecharxml.o fmt_fill.o fmt_httpdate.o fmt_human.o fmt_humank.o fmt_iso8601.o fmt_long.o fmt_longlong.o fmt_minus.o fmt_netstring.o fmt_pad.o fmt_pb_double.o fmt_pb_float.o fmt_pb_int.o fmt_pb_sint.o fmt_pb_string.o fmt_pb_tag.o fmt_pb_type0_sint.o fmt_pb_type1_double.o fmt_pb_type1_fixed64.o fmt_pb_type2_string.o fmt_pb_type5_fixed32.o fmt_pb_type5_float.o fmt_plusminus.o fmt_str.o fmt_strm_internal.o fmt_strn.o fmt_tohex.o fmt_ulong.o fmt_ulong0.o fmt_ulonglong.o fmt_utf8.o fmt_varint.o fmt_xlong.o fmt_xlonglong.o fmt_xmlescape.o 
SCAN_OBJS=scan_8int.o scan_8long.o scan_8longlong.o scan_8longn.o scan_8short.o scan_asn1derlength.o scan_asn1dertag.o scan_charsetnskip.o scan_double.o scan_fromhex.o scan_httpdate.o scan_int.o scan_iso8601.o scan_long.o scan_longlong.o scan_longn.o scan_netstring.o scan_noncharsetnskip.o scan_nonwhitenskip.o scan_pb_tag.o scan_pb_type0_sint.o scan_pb_type1_double.o scan_pb_type1_fixed64.o scan_pb_type2_stringlen.o scan_pb_type5_fixed32.o scan_pb_type5_float.o scan_plusminus.o scan_short.o scan_uint.o scan_ulong.o scan_ulonglong.o scan_ulongn.o scan_ushort.o scan_utf8.o scan_utf8_sem.o scan_varint.o scan_whitenskip.o scan_xint.o scan_xlong.o scan_xlonglong.o scan_xlongn.o scan_xshort.o 
STR_OBJS=str_chr.o str_copy.o str_diff.o str_diffn.o str_len.o str_rchr.o str_start.o 
UINT_OBJS=uint16_pack.o uint16_pack_big.o uint16_read.o uint16_read_big.o uint16_unpack.o uint16_unpack_big.o uint32_pack.o uint32_pack_big.o uint32_read.o uint32_read_big.o uint32_unpack.o uint32_unpack_big.o uint64_pack.o uint64_pack_big.o uint64_read.o uint64_read_big.o uint64_unpack.o uint64_unpack_big.o 
OPEN_OBJS=open_append.o open_excl.o open_read.o open_rw.o open_trunc.o open_write.o openreadclose.o readclose.o 
STRALLOC_OBJS=stralloc_append.o stralloc_cat.o stralloc_catb.o stralloc_catlong0.o stralloc_catm_internal.o stralloc_cats.o stralloc_catulong0.o stralloc_chomp.o stralloc_chop.o stralloc_copy.o stralloc_copyb.o stralloc_copys.o stralloc_diff.o stralloc_diffs.o stralloc_free.o stralloc_init.o stralloc_ready.o stralloc_readyplus.o stralloc_starts.o stralloc_zero.o 
UNIX_OBJS=iopause.o ndelay_off.o ndelay_on.o winsock2errno.o 
SOCKET_OBJS=fmt_ip4.o fmt_ip6.o fmt_ip6_flat.o fmt_ip6c.o fmt_ip6if.o fmt_ip6ifc.o init.o scan_ip4.o scan_ip6.o scan_ip6_flat.o scan_ip6if.o socket_accept4.o socket_accept4_flags.o socket_accept4_makenonblocking.o socket_accept4_makenonblocking_setcloseonexec.o socket_accept4_setcloseonexec.o socket_accept6.o socket_accept6_flags.o socket_accept6_makenonblocking.o socket_accept6_makenonblocking_setcloseonexec.o socket_accept6_setcloseonexec.o socket_bind4.o socket_bind4_reuse.o socket_bind6.o socket_bind6_reuse.o socket_broadcast.o socket_connect4.o socket_connect6.o socket_connected.o socket_deferaccept.o socket_fastopen.o socket_fastopen_connect4.o socket_fastopen_connect6.o socket_getifidx.o socket_getifname.o socket_ip4loopback.o socket_listen.o socket_local4.o socket_local6.o socket_mchopcount6.o socket_mcjoin4.o socket_mcjoin6.o socket_mcleave4.o socket_mcleave6.o socket_mcloop4.o socket_mcloop6.o socket_mcttl4.o socket_noipv6.o socket_quickack.o socket_recv4.o socket_recv6.o socket_remote4.o socket_remote6.o socket_sctp4.o socket_sctp4b.o socket_sctp6.o socket_sctp6b.o socket_send4.o socket_send6.o socket_tcp4.o socket_tcp4b.o socket_tcp6.o socket_tcp6b.o socket_tryreservein.o socket_udp4.o socket_udp6.o socket_v4mappedprefix.o socket_v6any.o socket_v6loopback.o 
BUFFER_OBJS=bs_capacityassert.o bs_capacitycheck.o bs_capacityleft.o bs_consumeleftovers.o bs_err.o bs_get.o bs_init_bstream_size.o bs_init_iobuf.o bs_init_iobuf_size.o bs_init_membuf.o bs_nomoredataassert.o bs_peek.o bs_seterrorstate.o bs_skip.o buffer_0.o buffer_0small.o buffer_1.o buffer_1small.o buffer_2.o buffer_close.o buffer_feed.o buffer_flush.o buffer_free.o buffer_fromarray.o buffer_frombuf.o buffer_fromsa.o buffer_get.o buffer_get_new_token_sa.o buffer_get_new_token_sa_pred.o buffer_get_token.o buffer_get_token_pred.o buffer_get_token_sa.o buffer_get_token_sa_pred.o buffer_getc.o buffer_getline.o buffer_getline_sa.o buffer_getn.o buffer_getnewline_sa.o buffer_init.o buffer_init_allocbuf.o buffer_init_free.o buffer_init_read.o buffer_init_read_allocbuf.o buffer_init_staticcontents.o buffer_init_staticcontents_free.o buffer_init_write.o buffer_init_write_allocbuf.o buffer_mmapread.o buffer_munmap.o buffer_peek.o buffer_peekc.o buffer_put.o buffer_put8long.o buffer_putalign.o buffer_puterror.o buffer_puterror2.o buffer_putflush.o buffer_putlong.o buffer_putlonglong.o buffer_putm_internal.o buffer_putm_internal_flush.o buffer_putnlflush.o buffer_puts.o buffer_putsa.o buffer_putsaflush.o buffer_putsalign.o buffer_putsflush.o buffer_putspace.o buffer_putulong.o buffer_putulonglong.o buffer_putxlong.o buffer_seek.o buffer_stubborn.o buffer_stubborn2.o buffer_tosa.o errmsg_iam.o errmsg_info.o errmsg_infosys.o errmsg_puts.o errmsg_warn.o errmsg_warnsys.o errmsg_write.o prs_asciiz.o prs_asciiz_fixedlen.o prs_readblob.o prs_u16.o prs_u16_big.o prs_u32.o prs_u32_big.o prs_u64.o prs_u64_big.o 
MMAP_OBJS=mmap_private.o mmap_read.o mmap_readat.o mmap_shared.o mmap_unmap.o 
TAIA_OBJS=taia_add.o taia_addsec.o taia_approx.o taia_frac.o taia_half.o taia_less.o taia_now.o taia_pack.o taia_sub.o taia_tai.o taia_uint.o taia_unpack.o 
TAI_OBJS=tai_add.o tai_now.o tai_pack.o tai_sub.o tai_uint.o tai_unpack.o 
DNS_OBJS=dns_dfd.o dns_domain.o dns_dtda.o dns_ip.o dns_ip6.o dns_ipq.o dns_ipq6.o dns_mx.o dns_name.o dns_nd.o dns_nd6.o dns_packet.o dns_random.o dns_rcip.o dns_rcrw.o dns_resolve.o dns_sortip.o dns_sortip6.o dns_transmit.o dns_txt.o 
CASE_OBJS=case_diffb.o case_diffs.o case_lowerb.o case_lowers.o case_starts.o 
MULT_OBJS=imult16.o imult32.o imult64.o range_arrayinbuf.o range_str2inbuf.o range_str4inbuf.o range_strinbuf.o umult16.o umult32.o umult64.o 
ARRAY_OBJS=array_allocate.o array_bytes.o array_cat.o array_cat0.o array_catb.o array_cate.o array_cats.o array_cats0.o array_equal.o array_fail.o array_get.o array_length.o array_reset.o array_start.o array_trunc.o array_truncate.o iarray_allocate.o iarray_free.o iarray_get.o iarray_init.o iarray_length.o 
IO_OBJS=io_appendfile.o io_block.o io_canread.o io_canwrite.o io_check.o io_close.o io_closeonexec.o io_createfile.o io_debugstring.o io_dontwantread.o io_dontwantwrite.o io_eagain.o io_eagain_read.o io_eagain_write.o io_fd.o io_finishandshutdown.o io_getcookie.o io_mmapwritefile.o io_nonblock.o io_passfd.o io_pipe.o io_readfile.o io_readwritefile.o io_receivefd.o io_sendfile.o io_setcookie.o io_sigpipe.o io_socketpair.o io_timedout.o io_timeout.o io_timeouted.o io_tryread.o io_tryreadtimeout.o io_trywrite.o io_trywritetimeout.o io_wait.o io_waitread.o io_waituntil.o io_waituntil2.o io_waitwrite.o io_wantread.o io_wantwrite.o iob_addbuf.o iob_addbuf_free.o iob_addbuf_internal.o iob_addbuf_munmap.o iob_addfile.o iob_addfile_close.o iob_adds.o iob_adds_free.o iob_bytesleft.o iob_free.o iob_new.o iob_prefetch.o iob_reset.o iob_send.o iob_write.o iom_abort.o iom_add.o iom_init.o iom_wait.o 
TEXTCODE_OBJS=base64.o base64url.o fmt_base64.o fmt_base64url.o fmt_cescape.o fmt_foldwhitespace.o fmt_hexdump.o fmt_html.o fmt_html_tagarg.o fmt_jsonescape.o fmt_ldapescape.o fmt_ldapescape2.o fmt_quotedprintable.o fmt_to_array.o fmt_to_sa.o fmt_tofrom_array.o fmt_urlencoded.o fmt_uuencoded.o fmt_xml.o fmt_yenc.o scan_base64.o scan_base64url.o scan_cescape.o scan_hexdump.o scan_html.o scan_jsonescape.o scan_ldapescape.o scan_quotedprintable.o scan_to_array.o scan_to_sa.o scan_tofrom_array.o scan_urlencoded.o scan_uuencoded.o scan_yenc.o 
CDB_OBJS=cdb.o cdb_hash.o cdb_make.o cdb_traverse.o 
CRITBIT_OBJS=critbit.o 

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

%.o:
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

VERSION=libowfat-0.33
CURNAME=libowfat-0.33

tar: clean rename
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
	    -e 's///' < GNUmakefile >> $@

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

test_%:
	$(CC) -g -o $@ $< -DUNITTEST -I. -fprofile-arcs -ftest-coverage $(LDFLAGS)
	./$@
	rm -f $@

test_scan_html: entities.h

check2: haveuint128.h haveinline.h entities.h
	for i in $(UNITTESTS); do $(CC) -Wall -fprofile-arcs -ftest-coverage -g -o t -DUNITTEST $$i -I. $(LDFLAGS) && ./t || echo FAIL $$i ; done

check: haveuint128.h haveinline.h $(TESTS)
