/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT16_H
#define UINT16_H

#include <inttypes.h>	// uint16_t, int16_t
#include <stddef.h>	// size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t uint16;
typedef int16_t int16;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT16_MACROS)

/* On x86 and x64 unaligned access are OK and the compiler knows it and
 * will collapse this long-looking code into a single load or store
 * instruction, so turn it into inline functions */

static inline void uint16_pack(char* out,uint16 in) {
  out[0]=in;
  out[1]=in>>8;
}

static inline void uint16_unpack(const char *in,uint16* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=i[0] | (i[1]<<8);
}

static inline uint16 uint16_read(const char* in) {
  const unsigned char* i = (const unsigned char*)in;
  return i[0] | (i[1]<<8);
}

static inline void uint16_pack_big(char* out,uint16 in) {
  out[0]=in>>8;
  out[1]=in;
}

static inline void uint16_unpack_big(const char *in,uint16* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=(i[0]<<8) | i[1];
}

static inline uint16 uint16_read_big(const char* in) {
  const unsigned char* i = (const unsigned char*)in;
  return (i[0]<<8) | i[1];
}

#else

void uint16_pack(char *out,uint16 in);
void uint16_pack_big(char *out,uint16 in);
void uint16_unpack(const char *in,uint16* out);
void uint16_unpack_big(const char *in,uint16* out);
uint16 uint16_read(const char *in);
uint16 uint16_read_big(const char *in);

#endif

static inline size_t fmt_uint16(char* out, uint16 in) {
  if (out) uint16_pack(out,in);
  return 2;
}

static inline size_t fmt_uint16_big(char* out, uint16 in) {
  if (out) uint16_pack_big(out,in);
  return 2;
}

static inline size_t scan_uint16(const char* in, size_t len, uint16_t* b) {
  if (len<2) return 0;
  *b = uint16_read(in);
  return 2;
}

static inline size_t scan_uint16_big(const char* in, size_t len, uint16_t* b) {
  if (len<2) return 0;
  *b = uint16_read_big(in);
  return 2;
}

#ifdef __cplusplus
}

inline void uint16_pack(unsigned char* out,uint16 in) {
  uint16_pack(reinterpret_cast<char*>(out), in);
}

inline void uint16_pack_big(unsigned char* out,uint16 in) {
  uint16_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint16_unpack(const unsigned char *in,uint16* out) {
  uint16_unpack(reinterpret_cast<const char*>(in), out);
}

inline void uint16_unpack_big(const unsigned char *in,uint16* out) {
  uint16_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline void uint16_pack(uint16* out,uint16 in) {
  uint16_pack(reinterpret_cast<char*>(out), in);
}

inline void uint16_pack_big(uint16* out,uint16 in) {
  uint16_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint16_unpack(uint16* *in,uint16* out) {
  uint16_unpack(reinterpret_cast<const char*>(in), out);
}

inline void uint16_unpack_big(uint16* *in,uint16* out) {
  uint16_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline uint16 uint16_read(const unsigned char *in) {
  return uint16_read(reinterpret_cast<const char*>(in));
}

inline uint16 uint16_read_big(const unsigned char *in) {
  return uint16_read_big(reinterpret_cast<const char*>(in));
}

inline uint16 uint16_read(const uint16 *in) {
  return uint16_read(reinterpret_cast<const char*>(in));
}

inline uint16 uint16_read_big(const uint16 *in) {
  return uint16_read_big(reinterpret_cast<const char*>(in));
}

inline uint16 uint_read(const uint16* in) {
  return uint16_read(reinterpret_cast<const char*>(in));
}

inline uint16 uint_read_big(const uint16* in) {
  return uint16_read_big(reinterpret_cast<const char*>(in));
}

#endif


#endif
