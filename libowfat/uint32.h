/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT32_H
#define UINT32_H

#include <inttypes.h>	// uint32_t, int32_t
#include <stddef.h>	// size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t uint32;
typedef int32_t int32;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT32_MACROS)

static inline void uint32_pack(char* out,uint32 in) {
  *(uint32*)out=in;
}

static inline void uint32_unpack(const char *in,uint32* out) {
  *out=*(const uint32*)in;
}

static inline uint32 uint32_read(const char* in) {
  return *(const uint32*)in;
}

#if defined(__x86_64__) && defined(__GNUC__)

static inline void uint32_pack_big(char *out,uint32 in) {
  *(uint32*)out=__builtin_bswap32(in);
}

static inline void uint32_unpack_big(const char *in,uint32* out) {
  *out=__builtin_bswap32(*(const uint32*)in);
}

static inline uint32 uint32_read_big(const char *in) {
  return __builtin_bswap32(*(const uint32*)in);
}

#else

void uint32_pack_big(char *out,uint32 in);
void uint32_unpack_big(const char *in,uint32* out);
uint32 uint32_read_big(const char *in);

#endif

#else

void uint32_pack(char *out,uint32 in);
void uint32_pack_big(char *out,uint32 in);
void uint32_unpack(const char *in,uint32* out);
void uint32_unpack_big(const char *in,uint32* out);
uint32 uint32_read(const char *in);
uint32 uint32_read_big(const char *in);

#endif

static inline size_t fmt_uint32(char* out, uint32 in) {
  if (out) uint32_pack(out,in);
  return 4;
}

static inline size_t fmt_uint32_big(char* out, uint32 in) {
  if (out) uint32_pack_big(out,in);
  return 4;
}

static inline size_t scan_uint32(const char* in, size_t len, uint32_t* b) {
  if (len<4) return 0;
  *b = uint32_read(in);
  return 4;
}

static inline size_t scan_uint32_big(const char* in, size_t len, uint32_t* b) {
  if (len<4) return 0;
  *b = uint32_read_big(in);
  return 4;
}

#ifdef __cplusplus
}
#endif

#endif
