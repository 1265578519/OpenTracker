/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef TAI_H
#define TAI_H

/* Times with 1 second precision */

#include <libowfat/uint64.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A struct tai value is an integer between 0 inclusive and 2^64
 * exclusive. The format of struct tai is designed to speed up common
 * operations; applications should not look inside struct tai.
 *
 * A struct tai variable is commonly used to store a TAI64 label. Each
 * TAI64 label refers to one second of real time. TAI64 labels span a
 * range of hundreds of billions of years.
 *
 * A struct tai variable may also be used to store the numerical
 * difference between two TAI64 labels.
 * See http://cr.yp.to/libtai/tai64.html */

typedef struct tai {
  uint64 x;
} tai64;


#define tai_unix(t,u) ((void) ((t)->x = 4611686018427387914ULL + (uint64) (u)))

/* tai_now puts the current time into t. More precisely: tai_now puts
 * into t its best guess as to the TAI64 label for the 1-second interval
 * that contains the current time.
 *
 * This implementation of tai_now assumes that the time_t returned from
 * the time function represents the number of TAI seconds since
 * 1970-01-01 00:00:10 TAI. This matches the convention used by the
 * Olson tz library in ``right'' mode. */
void tai_now(struct tai *);

/* tai_approx returns a double-precision approximation to t. The result
 * of tai_approx is always nonnegative. */
#define tai_approx(t) ((double) ((t)->x))

/* tai_add adds a to b modulo 2^64 and puts the result into t. The
 * inputs and output may overlap. */
void tai_add(struct tai *,const struct tai *,const struct tai *);
/* tai_sub subtracts b from a modulo 2^64 and puts the result into t.
 * The inputs and output may overlap. */
void tai_sub(struct tai *,const struct tai *,const struct tai *);
/* tai_less returns 1 if a is less than b, 0 otherwise. */
#define tai_less(t,u) ((t)->x < (u)->x)

#define TAI_PACK 8
/* tai_pack converts a TAI64 label from internal format in t to external
 * TAI64 format in buf. */
void tai_pack(char *,const struct tai *);
/* tai_unpack converts a TAI64 label from external TAI64 format in buf
 * to internal format in t. */
void tai_unpack(const char *,struct tai *);

void tai_uint(struct tai *,unsigned int);

#ifdef __cplusplus
}
#endif

#endif
