/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef UINT64_H
#define UINT64_H

#include <inttypes.h>

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

#ifdef __cplusplus
}
#endif

#endif
