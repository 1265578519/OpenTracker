/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT16_H
#define UINT16_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t uint16;
typedef int16_t int16;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT16_MACROS)

static inline void uint16_pack(char* out,uint16 in) {
  *(uint16*)out=in;
}

static inline void uint16_unpack(const char *in,uint16* out) {
  *out=*(uint16*)in;
}

static inline uint16 uint16_read(const char* in) {
  return *(uint16*)in;
}

void uint16_pack_big(char *out,uint16 in);
void uint16_unpack_big(const char *in,uint16* out);
uint16 uint16_read_big(const char *in);
#else

void uint16_pack(char *out,uint16 in);
void uint16_pack_big(char *out,uint16 in);
void uint16_unpack(const char *in,uint16* out);
void uint16_unpack_big(const char *in,uint16* out);
uint16 uint16_read(const char *in);
uint16 uint16_read_big(const char *in);

#endif

#ifdef __cplusplus
}
#endif

#endif
