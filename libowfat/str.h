/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef STR_H
#define STR_H

#include <stddef.h>

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* str_copy copies leading bytes from in to out until \0.
 * return number of copied bytes. */
att_write(1) att_read(2)
size_t str_copy(char *out,const char *in) noexcept;

/* str_diff returns negative, 0, or positive, depending on whether the
 * string a[0], a[1], ..., a[n]=='\0' is lexicographically smaller than,
 * equal to, or greater than the string b[0], b[1], ..., b[m-1]=='\0'.
 * If the strings are different, str_diff does not read bytes past the
 * first difference. */
att_pure att_read(1) att_read(2)
int str_diff(const char *a,const char *b) noexcept;

/* str_diffn returns negative, 0, or positive, depending on whether the
 * string a[0], a[1], ..., a[n]=='\0' is lexicographically smaller than,
 * equal to, or greater than the string b[0], b[1], ..., b[m-1]=='\0'.
 * If the strings are different, str_diffn does not read bytes past the
 * first difference. The strings will be considered equal if the first
 * limit characters match. */
att_pure att_read(1) att_read(2)
int str_diffn(const char *a,const char *b,size_t limit) noexcept;

#ifdef __dietlibc__
#include <string.h>
#define str_len(foo) strlen(foo)
#else
/* str_len returns the index of \0 in s */
att_pure att_read(1)
size_t str_len(const char *s) noexcept;
#endif

/* str_chr returns the index of the first occurance of needle or \0 in haystack */
att_pure att_read(1)
size_t str_chr(const char *haystack,char needle) noexcept;

/* str_rchr returns the index of the last occurance of needle or \0 in haystack */
att_pure att_read(1)
size_t str_rchr(const char *haystack,char needle) noexcept;

/* str_start returns 1 if the b is a prefix of a, 0 otherwise */
att_pure att_read(1) att_read(2)
int str_start(const char *a,const char *b) noexcept;

/* convenience shortcut to test for string equality */
#define str_equal(s,t) (!str_diff((s),(t)))

#ifdef __cplusplus
}
#endif

#endif
