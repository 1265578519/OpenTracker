/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef FMT_H
#define FMT_H

/* sandboxing note: None of these functions call any syscalls */

/* for size_t: */
#include <stddef.h>
/* for uint32_t */
#include <stdint.h>
/* for time_t: */
#ifdef _WIN32
#include <time.h>
#else
#include <sys/types.h>
#endif
/* for byte_copy */
#include <libowfat/byte.h>
#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FMT_LONG  41 /* enough space to hold -2^127 in decimal, plus \0 */
#define FMT_ULONG 40 /* enough space to hold 2^128 - 1 in decimal, plus \0 */
#define FMT_8LONG 44 /* enough space to hold 2^128 - 1 in octal, plus \0 */
#define FMT_XLONG 33 /* enough space to hold 2^128 - 1 in hexadecimal, plus \0 */
#define FMT_LEN ((char *) 0) /* convenient abbreviation */

/* This file declares routines for formatting strings and numbers in
 * various forms or encodings, either binary or text based.
 *
 * The first argument is always the destination buffer, the second
 * argument is always the value to be formatted/encoded. The return
 * value is always the number of bytes written in the output buffer.
 *
 * If you don't know how large the output will be, call the function
 * first with NULL as destination buffer and use the length it returns
 * to allocate the destination buffer.
 *
 * Note that none of these routines append a 0 terminator.  If you need
 * 0 termination, call the functions like this:

     buf[fmt_ulong(buf,number)]=0;

 */

/* convert signed src integer -23 to ASCII '-','2','3', return number of
 * bytes of value in output format (3 in this example).
 * If dest is not NULL, write result to dest */
att_write(1)
size_t fmt_long(char *dest,signed long src) noexcept;

/* convert unsigned src integer 23 to ASCII '2','3', return number of
 * bytes of value in output format (2 in this example).
 * If dest is not NULL, write result to dest */
att_write(1)
size_t fmt_ulong(char *dest,unsigned long src) noexcept;

/* convert unsigned src integer 0x23 to ASCII '2','3', return number of
 * bytes of value in output format (2 in this example).
 * If dest is not NULL, write result to dest */
att_write(1)
size_t fmt_xlong(char *dest,unsigned long src) noexcept;

/* convert unsigned src integer 023 to ASCII '2','3', return number of
 * bytes of value in output format (2 in this example).
 * If dest is not NULL, write result to dest */
att_write(1)
size_t fmt_8long(char *dest,unsigned long src) noexcept;

/* like fmt_long but for long long */
att_write(1)
size_t fmt_longlong(char *dest,signed long long src) noexcept;

/* like fmt_ulong but for unsigned long long */
att_write(1)
size_t fmt_ulonglong(char *dest,unsigned long long src) noexcept;

/* like fmt_xlong but for unsigned long long */
att_write(1)
size_t fmt_xlonglong(char *dest,unsigned long long src) noexcept;

#define fmt_uint(dest,src) fmt_ulong(dest,src)
#define fmt_int(dest,src) fmt_long(dest,src)
#define fmt_xint(dest,src) fmt_xlong(dest,src)
#define fmt_8int(dest,src) fmt_8long(dest,src)

/* Like fmt_ulong, but prepend '0' while length is smaller than padto.
 * Does not truncate! */
/* fmt_ulong0(buf,23,4) -> '0','0','2','3' return 4 */
/* fmt_ulong0(buf,234,2) -> '2','3','4', return 3 */
att_write(1)
size_t fmt_ulong0(char *,unsigned long src,size_t padto) noexcept;

#define fmt_uint0(buf,src,padto) fmt_ulong0(buf,src,padto)

/* convert src double 1.7 to ASCII '1','.','7', return length.
 * If dest is not NULL, write result to dest */
att_write(1)
size_t fmt_double(char *dest, double d,int max,int prec) noexcept;

/* if src is negative, write '-' and return 1.
 * if src is positive, write '+' and return 1.
 * otherwise return 0 */
att_write(1)
size_t fmt_plusminus(char *dest,int src) noexcept;

/* if src is negative, write '-' and return 1.
 * otherwise return 0. */
att_write(1)
size_t fmt_minus(char *dest,int src) noexcept;

/* copy str to dest until \0 byte, return number of copied bytes. */
/* fmt_str(NULL,str) == strlen(str) */
/* fmt_str(buf,str) == strcpy(buf,str), return strlen(str) */
/* strcat(strcpy(buf,"foo"),"bar") can be written as
 *   i=fmt_str(buf,"foo");
 *   i+=fmt_str(buf+i,"bar");
 *   buf[i]=0;
 * This is more efficient because strcat needs to scan the string to
 * find the end and append.
 */
att_write(1)
size_t fmt_str(char *dest,const char *src) noexcept;

/* copy str to dest until \0 byte or limit bytes copied.
 * return number of copied bytes. */
att_writen(1,3)
att_readn(2,3)
size_t fmt_strn(char *dest,const char *src,size_t limit) noexcept;

/* copy n bytes from src to dest, return n */
att_writen(1,3)
static inline size_t fmt_copybytes(char* dest,const char* src,size_t n) {
  if (dest) byte_copy(dest,n,src);
  return n;
}

/* copy sizeof(src)-1 bytes from src to dest, return sizeof(src)-1 */
/* this is for
 *   fmt_copybytes_sizeof_minus1(dest, "\x01\x02\x03\x04");
 * since we are technically passing a string, sizeof will include the
 * finishing 0 byte which we neither need nor want */
#define fmt_copybytes_sizeof_minus1(dest,src) fmt_copybytes(dest,src,sizeof(src)-1)

/* "foo" -> "  foo"
 * write padlen-srclen spaces, if that is >= 0.  Then copy srclen
 * characters from src.  Truncate only if total length is larger than
 * maxlen.  Return number of characters written. */
att_writen(1,5)
att_readn(2,3)
size_t fmt_pad(char* dest,const char* src,size_t srclen,size_t padlen,size_t maxlen) noexcept;

/* "foo" -> "foo  "
 * append padlen-srclen spaces after dest, if that is >= 0.  Truncate
 * only if total length is larger than maxlen.  Return number of
 * characters written. */
att_writen(1,4)
size_t fmt_fill(char* dest,size_t srclen,size_t padlen,size_t maxlen) noexcept;

/* 1 -> "1", 4900 -> "4.9k", 2300000 -> "2.3M" */
att_write(1)
size_t fmt_human(char* dest,unsigned long long l) noexcept;

/* 1 -> "1", 4900 -> "4.8k", 2300000 -> "2.2M" */
att_write(1)
size_t fmt_humank(char* dest,unsigned long long l) noexcept;

/* "Sun, 06 Nov 1994 08:49:37 GMT" */
att_write(1)
size_t fmt_httpdate(char* dest,time_t t) noexcept;	/* not marked pure because it calls gmtime */

/* "2014-05-27T19:22:16.247Z" */
att_write(1)
size_t fmt_iso8601(char* dest,time_t t) noexcept;

#define FMT_UTF8 5
#define FMT_ASN1LENGTH 17 /* enough space to hold 2^128-1 */
#define FMT_ASN1TAG 19 /* enough space to hold 2^128-1 */

/* some variable length encodings for integers */
att_write(1)
size_t fmt_utf8(char* dest,uint32_t n) noexcept;	/* can store 0-0x7fffffff */

att_write(1)
size_t fmt_utf8_scratch(char* dest,uint32_t n) noexcept;	/* same, but is allowed to overwrite up to 7 additional bytes */

att_write(1)
size_t fmt_asn1derlength(char* dest,unsigned long long l) noexcept;	/* 0-0x7f: 1 byte, above that 1+bytes_needed bytes */

att_write(1)
size_t fmt_asn1derlength_scratch(char* dest,unsigned long long l) noexcept;	/* same, but is allowed to overwrite up to 7 additional bytes */

att_write(1)
size_t fmt_asn1dertag(char* dest,unsigned long long l) noexcept;	/* 1 byte for each 7 bits; upper bit = more bytes coming */

att_write(1)
size_t fmt_asn1dertag_scratch(char* dest,unsigned long long l) noexcept;/* same, but is allowed to overwrite up to 7 additional bytes */

/* Google Protocol Buffers, https://developers.google.com/protocol-buffers/docs/encoding */
att_write(1)
size_t fmt_varint(char* dest,unsigned long long l) noexcept;	/* protocol buffers encoding; like asn1dertag but little endian */

att_write(1)
size_t fmt_pb_tag(char* dest,size_t fieldno,unsigned char type) noexcept;	/* protocol buffer tag */

att_write(1)
size_t fmt_pb_type0_int(char* dest,unsigned long long l) noexcept;	/* protocol buffers encoding: type 0 bool/enum/int32/uint32/int64/uint64 */

att_write(1)
size_t fmt_pb_type0_sint(char* dest,signed long long l) noexcept;/* protocol buffers encoding: type 0 sint32/sint64 */

att_write(1)
size_t fmt_pb_type1_double(char* dest,double d) noexcept;	/* protocol buffers encoding: double (64-bit little endian blob) */

att_write(1)
size_t fmt_pb_type1_fixed64(char* dest,uint64_t l) noexcept;	/* protocol buffers encoding: 64-bit little endian blob */

/* fmt_pb_type2_string can return 0 if (s,l) is clearly invalid */
att_write(1)
size_t fmt_pb_type2_string(char* dest,const char* s,size_t l) noexcept;	/* protocol buffers encoding: varint length + blob */

att_write(1)
size_t fmt_pb_type5_float(char* dest,float f) noexcept;		/* protocol buffers encoding: float (32-bit little endian blob) */

att_write(1)
size_t fmt_pb_type5_fixed32(char* dest,uint32_t l) noexcept;	/* protocol buffers encoding: 32-bit little endian blob */

att_write(1)
size_t fmt_pb_int(char* dest,size_t fieldno,unsigned long long l) noexcept;

att_write(1)
size_t fmt_pb_sint(char* dest,size_t fieldno,signed long long l) noexcept;

att_write(1)
size_t fmt_pb_double(char* dest,size_t fieldno,double d) noexcept;

att_write(1)
size_t fmt_pb_float(char* dest,size_t fieldno,float f) noexcept;

att_writen(1,4)
size_t fmt_pb_string(char* dest,size_t fieldno,const char* s,size_t l) noexcept;

/* fmt_netstring can return 0 if (src,len) is clearly invalid */
att_writen(1,3)
size_t fmt_netstring(char* dest,const char* src,size_t len) noexcept;

/* Marshaling helper functions.
 * Escape one character, no matter if it needs escaping or not.
 * The functions will reject characters that cannot be represented but
 * not characters that the standard says should never occur.  The idea
 * is to make these functions useful for creating bad encoding for
 * penetration testing.
 * Depending on the escaping method, the input character (uint32_t, a
 * unicode codepoint) may be limited to 0x7f, 0xff or 0x10ffff. */

/* XML escaping: '&' -> '&amp;', '<' -> '&lt;', 'ö' -> '&#xf6;' */
att_write(1)
size_t fmt_escapecharxml(char* dest,uint32_t ch) noexcept;

/* HTML escaping is the same as XML escaping. */
att_write(1)
size_t fmt_escapecharhtml(char* dest,uint32_t ch) noexcept;

/* JSON escaping: '\' -> '\\', '"' -> '\"', 'ö' -> '\u00f6' */
att_write(1)
size_t fmt_escapecharjson(char* dest,uint32_t ch) noexcept;

/* MIME quoted-printable escaping: 'ö' -> '=f6', characters > 0xff not supported */
att_write(1)
size_t fmt_escapecharquotedprintable(char* dest,uint32_t ch) noexcept;

/* MIME quoted-printable escaping with UTF-8: 'ö' -> '=c3=b6', characters > 0x7fffffff not supported */
att_write(1)
size_t fmt_escapecharquotedprintableutf8(char* dest,uint32_t ch) noexcept;

/* C99 style escaping: '\' -> '\\', newline -> '\n', 0xc2 -> '\302' */
att_write(1)
size_t fmt_escapecharc(char* dest,uint32_t ch) noexcept;

/* internal functions, may be independently useful */
att_const
char fmt_tohex(char c);

#define fmt_strm(b,...) fmt_strm_internal(b,__VA_ARGS__,(char*)0)
att_write(1)
size_t fmt_strm_internal(char* dest,...) noexcept;

#ifndef MAX_ALLOCA
#define MAX_ALLOCA 100000
#endif
#define fmt_strm_alloca(a,...) ({ size_t len=fmt_strm((char*)0,a,__VA_ARGS__)+1; char* c=(len<MAX_ALLOCA?alloca(len):0); if (c) c[fmt_strm(c,a,__VA_ARGS__)]=0; c;})

#define fmt_strm_malloc(a,...) ({ size_t len=fmt_strm((char*)0,a,__VA_ARGS__)+1; char* c=(len<MAX_ALLOCA?malloc(len):0); if (c) c[fmt_strm(c,a,__VA_ARGS__)]=0; c;})

#ifdef __cplusplus
}
#endif

#endif
