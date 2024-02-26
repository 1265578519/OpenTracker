/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef TAIA_H
#define TAIA_H

/* Times with 1 attosecond precision */

#include <tai.h>
#include <uint32.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A struct taia value is a number between 0 inclusive and 2^64
 * exclusive. The number is a multiple of 10^-18. The format of struct
 * taia is designed to speed up common operations; applications should
 * not look inside struct taia. */
typedef struct taia {
  struct tai sec;
  uint32 nano; /* 0...999999999 */
  uint32 atto; /* 0...999999999 */
} tai6464;

/* extract seconds */
void taia_tai(const tai6464 *source,tai64 *dest);

/* get current time */
void taia_now(struct taia *);

/* return double-precision approximation; always nonnegative */
double taia_approx(const tai6464 *);
/* return double-precision approximation of the fraction part;
 * always nonnegative */
double taia_frac(const tai6464 *);

/* add source1 to source2 modulo 2^64 and put the result in dest.
 * The inputs and output may overlap */
void taia_add(tai6464 *dest,const tai6464 *source1,const tai6464 *source2);
/* add secs seconds to source modulo 2^64 and put the result in dest. */
void taia_addsec(tai6464 *dest,const tai6464 *source,long secs);
/* subtract source2 from source1 modulo 2^64 and put the result in dest.
 * The inputs and output may overlap */
void taia_sub(tai6464 *,const tai6464 *,const tai6464 *);
/* divide source by 2, rouding down to a multiple of 10^-18, and put the
 * result into dest.  The input and output may overlap */
void taia_half(tai6464 *dest,const tai6464 *source);
/* return 1 if a is less than b, 0 otherwise */
int taia_less(const tai6464 *a,const tai6464 *b);

#define TAIA_PACK 16
/* char buf[TAIA_PACK] can be used to store a TAI64NA label in external
 * representation, which can then be used to transmit the binary
 * representation over a network or store it on disk in a byte order
 * independent fashion */

/* convert a TAI64NA label from internal format in src to external
 * TAI64NA format in buf. */
void taia_pack(char *buf,const tai6464 *src);
/* convert a TAI64NA label from external TAI64NA format in buf to
 * internal format in dest. */
void taia_unpack(const char *buf,tai6464 *dest);

#define TAIA_FMTFRAC 19
/* print the 18-digit fraction part of t in decimal, without a decimal
 * point but with leading zeros, into the character buffer s, without a
 * terminating \0. It returns 18, the number of characters written. s
 * may be zero; then taia_fmtfrac returns 18 without printing anything.
 * */
unsigned int taia_fmtfrac(char *s,const tai6464 *t);

/* initialize t to secs seconds. */
void taia_uint(tai6464 *t,unsigned int secs);

#ifdef __cplusplus
}
#endif

#endif
