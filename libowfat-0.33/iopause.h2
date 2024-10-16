/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IOPAUSE_H
#define IOPAUSE_H

/* sysdep: +poll */
#define IOPAUSE_POLL

#include <sys/types.h>
#include <sys/poll.h>

typedef struct pollfd iopause_fd;
#define IOPAUSE_READ POLLIN
#define IOPAUSE_WRITE POLLOUT

#include "taia.h"

extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
