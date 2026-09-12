#ifndef READCLOSE_H
#define READCLOSE_H

/* for ssize_t */
#include <sys/types.h>

#include "stralloc.h"

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readclose_append(int fd,stralloc *buf,size_t initlen);
ssize_t readclose(int fd,stralloc *buf,size_t initlen);

#ifdef __cplusplus
}
#endif

#endif
