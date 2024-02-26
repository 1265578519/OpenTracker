/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef CASE_H
#define CASE_H

#include <stddef.h>
#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* turn upper case letters to lower case letters, ASCIIZ */
att_mangle(1)
void case_lowers(char *s);

/* turn upper case letters to lower case letters, binary */
att_manglen(1,2)
void case_lowerb(void *buf,size_t len);

/* like str_diff, ignoring case */
att_pure
att_read(1)
att_read(2)
int case_diffs(const char *,const char *);

/* like byte_diff, ignoring case */
att_pure
att_readn(1,2)
att_readn(3,2)
int case_diffb(const void *,size_t ,const void *);

/* like str_start, ignoring case */
att_pure
att_read(1)
att_read(2)
int case_starts(const char *,const char *);

#define case_equals(s,t) (!case_diffs((s),(t)))
#define case_equalb(s,n,t) (!case_diffb((s),(n),(t)))

#ifdef __cplusplus
}
#endif

#endif
