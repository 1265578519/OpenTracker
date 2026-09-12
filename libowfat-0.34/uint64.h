/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT64_H
#define UINT64_H

#include <inttypes.h>	/* uint64_t, int64_t */
#include <stddef.h>	/* size_t */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t uint64;
typedef int64_t int64;

#if (defined(__i386__) || defined(__x86_64__)) && !defined(NO_UINT64_MACROS)

/* On x86 and x64 unaligned access are OK and the compiler knows it and
 * will collapse this long-looking code into a single load or store
 * instruction, so turn it into inline functions */

static inline void uint64_pack(char* out,uint64 in) {
  out[0]=in;
  out[1]=in>>8;
  out[2]=in>>16;
  out[3]=in>>24;
  out[4]=in>>32;
  out[5]=in>>40;
  out[6]=in>>48;
  out[7]=in>>56;
}

static inline void uint64_unpack(const char *in,uint64* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=i[0] | (i[1]<<8) | (i[2]<<16) | ((uint64)i[3]<<24) |
    ((uint64)i[4]<<32) | ((uint64)i[5]<<40) |
    ((uint64)i[6]<<48) | ((uint64)i[7]<<56);
}

static inline uint64 uint64_read(const char *in) {
  const unsigned char* i = (const unsigned char*)in;
  return i[0] | (i[1]<<8) | (i[2]<<16) | ((uint64)i[3]<<24) |
    ((uint64)i[4]<<32) | ((uint64)i[5]<<40) |
    ((uint64)i[6]<<48) | ((uint64)i[7]<<56);
}

static inline void uint64_pack_big(char *out,uint64 in) {
  out[0]=in>>56;
  out[1]=in>>48;
  out[2]=in>>40;
  out[3]=in>>32;
  out[4]=in>>24;
  out[5]=in>>16;
  out[6]=in>>8;
  out[7]=in;
}

static inline void uint64_unpack_big(const char *in,uint64* out) {
  const unsigned char* i = (const unsigned char*)in;
  *out=(i[7] | (i[6]<<8) | (i[5]<<16) | ((uint64)(i[4])<<24)) |
    ((uint64)(i[3])<<32) | ((uint64)(i[2])<<40) |
    ((uint64)(i[1])<<48) | ((uint64)(i[0])<<56);
}

static inline uint64 uint64_read_big(const char *in) {
  const unsigned char* i = (const unsigned char*)in;
  return (i[7] | (i[6]<<8) | (i[5]<<16) | ((uint64)(i[4])<<24)) |
    ((uint64)(i[3])<<32) | ((uint64)(i[2])<<40) |
    ((uint64)(i[1])<<48) | ((uint64)(i[0])<<56);
}

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

#ifndef uint64_pack
inline void uint64_pack(unsigned char* out,uint64 in) {
  uint64_pack(reinterpret_cast<char*>(out), in);
}

inline void uint64_unpack(const unsigned char *in,uint64* out) {
  uint64_unpack(reinterpret_cast<const char*>(in), out);
}

inline void uint64_pack(uint64* out,uint64 in) {
  uint64_pack(reinterpret_cast<char*>(out), in);
}

inline void uint64_unpack(uint64* *in,uint64* out) {
  uint64_unpack(reinterpret_cast<const char*>(in), out);
}

inline uint64 uint64_read(const unsigned char *in) {
  return uint64_read(reinterpret_cast<const char*>(in));
}

inline uint64 uint64_read(const uint64 *in) {
  return uint64_read(reinterpret_cast<const char*>(in));
}

#endif

inline uint64 uint64_read_big(const unsigned char *in) {
  return uint64_read_big(reinterpret_cast<const char*>(in));
}

inline uint64 uint64_read_big(const uint64 *in) {
  return uint64_read_big(reinterpret_cast<const char*>(in));
}

inline void uint64_pack_big(unsigned char* out,uint64 in) {
  uint64_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint64_unpack_big(const unsigned char *in,uint64* out) {
  uint64_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline void uint64_pack_big(uint64* out,uint64 in) {
  uint64_pack_big(reinterpret_cast<char*>(out), in);
}

inline void uint64_unpack_big(uint64* *in,uint64* out) {
  uint64_unpack_big(reinterpret_cast<const char*>(in), out);
}

inline uint64 uint_read(const uint64* in) {
  return uint64_read(reinterpret_cast<const char*>(in));
}

inline uint64 uint_read_big(const uint64* in) {
  return uint64_read_big(reinterpret_cast<const char*>(in));
}
#endif

#endif
