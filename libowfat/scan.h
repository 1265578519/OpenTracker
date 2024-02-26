/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef SCAN_H
#define SCAN_H

/* for size_t: */
#include <stddef.h>
/* for uint32_t: */
#include <stdint.h>
/* for time_t: */
#include <sys/types.h>
/* for struct timespec: */
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __pure__
#define __pure__
#endif

/* This file declared functions used to decode / scan / unmarshal
 * integer or string values from a buffer.
 * The first argument is always the source buffer, the second argument
 * is a pointer to the destination (where to store the result). The
 * return value is number of bytes scanned successfully. */

/* Interpret src as ASCII decimal number, write number to dest and
 * return the number of bytes that were parsed.
 * scan_ulong("23",&i) -> i=23, return 2
 * NB: leading + or - or space not accepted:
 * scan_ulong("+23",&i) -> return 0
 * scan_ulong("-23",&i) -> return 0
 * scan_ulong(" 23",&i) -> return 0
 * scan_ulong("23,42",&i) -> i=23, return 2
 * NB: 023 for octal or 0x23 for hex are not supported!
 * scan_ulong("0x23",&i) -> i=0, return 1
 * NB: will detect integer overflow and abort on excessively large
 * values, i.e. on a 32-bit system:
 * scan_ulong("4294967296",&i" -> i=429496729, return 9 */
size_t scan_ulong(const char *src,unsigned long *dest);
size_t scan_ulongn(const char* src,size_t n,unsigned long* dest);

/* Interpret src as ASCII hexadecimal number, write number to dest and
 * return the number of bytes that were parsed.
 * Note: leading '+' or '-' not accepted! */
size_t scan_xlong(const char *src,unsigned long *dest);
size_t scan_xlongn(const char *src,size_t n,unsigned long *dest);

/* interpret src as ASCII octal number, write number to dest and
 * return the number of bytes that were parsed.
 * Note: leading '+' or '-' not accepted! */
size_t scan_8long(const char *src,unsigned long *dest);
size_t scan_8longn(const char *src,size_t n,unsigned long *dest);

/* interpret src as signed ASCII decimal number, write number to dest
 * and return the number of bytes that were parsed.
 * Note: leading spaces not accepted! */
size_t scan_long(const char *src,signed long *dest);
size_t scan_longn(const char *src,size_t n,signed long *dest);

size_t scan_longlong(const char *src,signed long long *dest);
size_t scan_ulonglong(const char *src,unsigned long long *dest);
size_t scan_xlonglong(const char *src,unsigned long long *dest);
size_t scan_8longlong(const char *src,unsigned long long *dest);

size_t scan_uint(const char *src,unsigned int *dest);
size_t scan_xint(const char *src,unsigned int *dest);
size_t scan_8int(const char *src,unsigned int *dest);
size_t scan_int(const char *src,signed int *dest);

size_t scan_ushort(const char *src,unsigned short *dest);
size_t scan_xshort(const char *src,unsigned short *dest);
size_t scan_8short(const char *src,unsigned short *dest);
size_t scan_short(const char *src,signed short *dest);

/* interpret src as double precision floating point number,
 * write number to dest and return the number of bytes that were parsed */
size_t scan_double(const char *in, double *dest);

/* if *src=='-', set *dest to -1 and return 1.
 * if *src=='+', set *dest to 1 and return 1.
 * otherwise set *dest to 1 return 0. */
size_t scan_plusminus(const char *src,signed int *dest);

/* return the highest integer n<=limit so that isspace(in[i]) for all 0<=i<=n */
size_t scan_whitenskip(const char *in,size_t limit) __pure__;

/* return the highest integer n<=limit so that !isspace(in[i]) for all 0<=i<=n */
size_t scan_nonwhitenskip(const char *in,size_t limit) __pure__;

/* return the highest integer n<=limit so that in[i] is element of
 * charset (ASCIIZ string) for all 0<=i<=n */
size_t scan_charsetnskip(const char *in,const char *charset,size_t limit) __pure__;

/* return the highest integer n<=limit so that in[i] is not element of
 * charset (ASCIIZ string) for all 0<=i<=n */
size_t scan_noncharsetnskip(const char *in,const char *charset,size_t limit) __pure__;

/* try to parse ASCII GMT date; does not understand time zones. */
/* example dates:
 *   "Sun, 06 Nov 1994 08:49:37 GMT"
 *   "Sunday, 06-Nov-94 08:49:37 GMT"
 *   "Sun Nov  6 08:49:37 1994"
 */
size_t scan_httpdate(const char *in,time_t *t) __pure__;

/* try to parse ASCII ISO-8601 date; does not understand time zones. */
/* example date: "2014-05-27T19:22:16Z" */
size_t scan_iso8601(const char* in,struct timespec* t) __pure__;

/* some variable length encodings for integers */
size_t scan_utf8(const char* in,size_t len,uint32_t* n) __pure__;
size_t scan_asn1derlength(const char* in,size_t len,unsigned long long* n) __pure__;
size_t scan_asn1dertag(const char* in,size_t len,unsigned long long* n) __pure__;

/* Google protocol buffers */
/* A protocol buffer is a sequence of (tag,value).
 * Parse each tag with scan_pb_tag, then look at the field number to see
 *   which field in your struct is being sent. Integers must have type
 *   0, double type 1, strings type 2 and floats type 5. However, you
 *   have to check this yourself.
 */ 
size_t scan_varint(const char* in,size_t len, unsigned long long* n) __pure__;	/* internal */
size_t scan_pb_tag(const char* in,size_t len, size_t* fieldno,unsigned char* type) __pure__;

/* Then, depending on the field number, validate the type and call the
 * corresponding of these functions to parse the value */
size_t scan_pb_type0_int(const char* in,size_t len,unsigned long long* l) __pure__;
size_t scan_pb_type0_sint(const char* in,size_t len,signed long long* l) __pure__;
size_t scan_pb_type1_double(const char* in,size_t len,double* d) __pure__;
size_t scan_pb_type1_fixed64(const char* in,size_t len,uint64_t* b) __pure__;
/* NOTE: scan_pb_type2_stringlen only parses the length of the string,
 * not the string itself. It will return the number of bytes parsed in
 * the length, then set slen to the value of the length integer it just
 * read, and let string point to the next byte (where the actual string
 * starts). To advance in the protocol buffer, you'll have to skip the
 * return value of this function + slen bytes.
 * This is done so you can detect too large strings and abort the
 * parsing early without having to read and allocate memory for the rest
 * (potentially gigabytes) of the data announced by one unreasonable
 * string length value. */
size_t scan_pb_type2_stringlen(const char* in,size_t len,const char** string, size_t* slen) __pure__;
size_t scan_pb_type5_float(const char* in,size_t len,float* f) __pure__;
size_t scan_pb_type5_fixed32(const char* in,size_t len,uint32_t* b) __pure__;

/* parse a netstring, input buffer is in (len bytes).
 * if parsing is successful:
 *   *dest points to string and *slen is size of string
 *   return number of bytes parsed
 * else
 *   return 0
 * Note: *dest will point inside the input buffer!
 */
size_t scan_netstring(const char* in,size_t len,char** dest,size_t* slen) __pure__;

/* internal function that might be useful independently */
/* convert from hex ASCII, return 0 to 15 for success or -1 for failure */
int scan_fromhex(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif
