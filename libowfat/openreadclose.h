/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef OPENREADCLOSE_H
#define OPENREADCLOSE_H

#include <compiler.h>
#include <stralloc.h>

#ifdef __cplusplus
extern "C" {
#endif

att_read(1)
int openreadclose(const char *filename,stralloc *buf,size_t initiallength);

#ifdef __cplusplus
}
#endif

#endif
