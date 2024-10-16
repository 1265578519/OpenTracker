/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT64_H
#define UINT64_H

#include <inttypes.h>	// uint64_t, int64_t
#include <stddef.h>	// size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t uint64;
typedef int64_t int64;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT64_MACROS)
#define uint64_pack(out,in) (*(uint64*)(out)=(in))
#define uint64_unpack(in,out) (*(out)=*(uint64*)(in))
#define uint64_read(in) (*(uint64*)(in))
void uint64_pack_big(char *out,uint64 in);
void uint64_unpack_big(const char *in,uint64* out);
uint64 uint64_read_big(const char *in);
#else

void uint64_pack(char *out,uint64 in);
void uint64_pack_big(char *out,uint64 in);
void uint64_unpack(const char *in,uint64* out);
void uint64_unpack_big(const char *in,uint64* out);
uint64 uint64_read(const char *in);
uint64 uint64_read_big(const char *in);

#endif

static inline size_t fmt_uint64(char* out, uint64 in) {
  if (out) uint64_pack(out,in);
  return 8;
}

static inline size_t fmt_uint64_big(char* out, uint64 in) {
  if (out) uint64_pack_big(out,in);
  return 8;
}

static inline size_t scan_uint64(const char* in, size_t len, uint64_t* b) {
  if (len<8) return 0;
  *b = uint64_read(in);
  return 8;
}

static inline size_t scan_uint64_big(const char* in, size_t len, uint64_t* b) {
  if (len<8) return 0;
  *b = uint64_read_big(in);
  return 8;
}

#ifdef __cplusplus
}
#endif

#endif
