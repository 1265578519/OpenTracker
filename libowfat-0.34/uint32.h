/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT32_H
#define UINT32_H

#include <inttypes.h>	/* uint32_t, int32_t */
#include <stddef.h>	/* size_t */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t uint32;
typedef int32_t int32;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT32_MACROS)

/* On x86 and x64 unaligned access are OK and the compiler knows it and
 * will collapse this long-looking code into a single load or store
 * instruction, so turn it into inline functions */

static inline void uint32_pack(char* out,uint32 in) {
  out[0]=in;
  out[1]=in>>8;
  out[2]=in>>16;
  out[3]=in>>24;
}

static inline void uint32_unpack(const char *in,uint32* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

static inline uint32 uint32_read(const char* in) {
  const unsigned char* i = (const unsigned char*)in;
  return i[0] | (i[1]<<8) | (i[2]<<16) | (i[3]<<24);
}

static inline void uint32_pack_big(char *out,uint32 in) {
  out[0]=in>>24;
  out[1]=in>>16;
  out[2]=in>>8;
  out[3]=in;
}

static inline void uint32_unpack_big(const char *in,uint32* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=(i[0]<<24) | (i[1]<<16) | (i[2]<<8) | i[3];
}

static inline uint32 uint32_read_big(const char *in) {
  const unsigned char* i = (const unsigned char*)in;
  return (i[0]<<24) | (i[1]<<16) | (i[2]<<8) | i[3];
}

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

inline void uint32_pack(unsigned char* out,uint32 in) {
  uint32_pack(reinterpret_cast<char*>(out), in);
}

inline void uint32_pack_big(unsigned char* out,uint32 in) {
  uint32_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint32_unpack(const unsigned char *in,uint32* out) {
  uint32_unpack(reinterpret_cast<const char*>(in), out);
}

inline void uint32_unpack_big(const unsigned char *in,uint32* out) {
  uint32_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline void uint32_pack(uint32* out,uint32 in) {
  uint32_pack(reinterpret_cast<char*>(out), in);
}

inline void uint32_pack_big(uint32* out,uint32 in) {
  uint32_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint32_unpack(uint32* *in,uint32* out) {
  uint32_unpack(reinterpret_cast<const char*>(in), out);
}

inline void uint32_unpack_big(uint32* *in,uint32* out) {
  uint32_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline uint32 uint32_read(const unsigned char *in) {
  return uint32_read(reinterpret_cast<const char*>(in));
}

inline uint32 uint32_read_big(const unsigned char *in) {
  return uint32_read_big(reinterpret_cast<const char*>(in));
}

inline uint32 uint32_read(const uint32 *in) {
  return uint32_read(reinterpret_cast<const char*>(in));
}

inline uint32 uint32_read_big(const uint32 *in) {
  return uint32_read_big(reinterpret_cast<const char*>(in));
}

inline uint32 uint_read(const uint32* in) {
  return uint32_read(reinterpret_cast<const char*>(in));
}

inline uint32 uint_read_big(const uint32* in) {
  return uint32_read_big(reinterpret_cast<const char*>(in));
}
#endif

#endif
