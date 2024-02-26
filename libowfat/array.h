/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <libowfat/compiler.h>
#include <libowfat/uint64.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char* p;
  int64 allocated;	/* in bytes */
  uint64 initialized;	/* in bytes */

  /* p and allocated nonzero: array is allocated */
  /* p and allocated zero: array is unallocated */
  /* p zero and allocated < 0: array is failed */

  size_t headroom;	/* the actual pointer for free() and realloc() is p-headroom */
} array;

void* array_allocate(array* x,uint64 membersize,int64 pos) noexcept;
void* array_get(const array* const x,uint64 membersize,int64 pos) noexcept;
void* array_start(const array* const x) noexcept;

att_pure
int64 array_length(const array* const x,uint64 membersize) noexcept;

att_pure
int64 array_bytes(const array* const x) noexcept;

void array_truncate(array* x,uint64 membersize,int64 len) noexcept;
void array_trunc(array* x) noexcept;
void array_reset(array* x) noexcept;
void array_fail(array* x) noexcept;
int array_equal(const array* const x,const array* const y) noexcept;
void array_cat(array* to,const array* const from) noexcept;
void array_catb(array* to,const char* from,uint64 len) noexcept;
void array_cats(array* to,const char* from) noexcept;
void array_cats0(array* to,const char* from) noexcept;
void array_cat0(array* to) noexcept;
void array_cate(array* to,const array* const from,int64 pos,int64 stop) noexcept;

void array_shift(array* x,uint64 membersize,uint64 members) noexcept;
void array_chop(array* x,uint64 membersize,uint64 members) noexcept;

#define array_failed(x) (array_bytes(x)==-1)
#define array_unallocated(x) (array_bytes(x)==0)

#ifdef __cplusplus
}
#endif

#endif
