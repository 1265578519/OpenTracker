/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef BYTE_H
#define BYTE_H

/* sandboxing note: None of these functions call any syscalls */

/* for size_t: */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>

/* byte_chr returns the smallest integer i between 0 and len-1
 * inclusive such that one[i] equals needle, or len if not found. */
att_readn(1,2)
size_t byte_chr(const void* haystack, size_t len, char needle) noexcept;

/* byte_rchr returns the largest integer i between 0 and len-1 inclusive
 * such that one[i] equals needle, or len if not found. */
att_pure
att_readn(1,2)
size_t byte_rchr(const void* haystack,size_t len,char needle) noexcept;

/* byte_copy copies in[0] to out[0], in[1] to out[1], ... and in[len-1]
 * to out[len-1]. */
att_writen(1,2)
att_readn(3,2)
void byte_copy(void* out, size_t len, const void* in) noexcept;

/* byte_copyr copies in[len-1] to out[len-1], in[len-2] to out[len-2],
 * ... and in[0] to out[0] */
att_writen(1,2)
att_readn(3,2)
void byte_copyr(void* out, size_t len, const void* in) noexcept;

/* byte_diff returns negative, 0, or positive, depending on whether the
 * string a[0], a[1], ..., a[len-1] is lexicographically smaller
 * than, equal to, or greater than the string b[0], b[1], ...,
 * b[len-1]. When the strings are different, byte_diff does not read
 * bytes past the first difference. */
att_pure
att_readn(1,2)
att_readn(3,2)
int byte_diff(const void* a, size_t len, const void* b) noexcept;

/* byte_zero sets the bytes out[0], out[1], ..., out[len-1] to 0 */
att_writen(1,2)
void byte_zero(void* out, size_t len) noexcept;

#define byte_equal(s,n,t) (!byte_diff((s),(n),(t)))

/* Return 1 iff (b,blen) is a prefix of (a,alen), 0 otherwise.
 * Will abort early on mismatch */
att_pure
att_readn(1,2)
att_readn(3,4)
int byte_start(const void* a,size_t alen,const void* b,size_t blen) noexcept;

/* equivalent to byte_start(a,alen,str,strlen(str)) */
att_pure
att_readn(1,2)
att_read(3)
int byte_starts(const void* a,size_t alen,const char* str) noexcept;

#if defined(__GNUC__) && !defined(__LIBOWFAT_INTERNAL)
/* If str is a string constant, strlen will be done at compile time */
#define byte_starts(a,alen,str) (__builtin_constant_p(str) ? byte_start(a,alen,str,strlen(str)) : byte_starts(a,alen,str))
#endif

att_pure
att_readn(1,2)
att_readn(3,2)
int byte_equal_notimingattack(const void* a, size_t len,const void* b) noexcept;

#if defined(__i386__) || defined(__x86_64__)
#define UNALIGNED_ACCESS_OK
#endif

#ifdef __cplusplus
}
#endif

#endif
