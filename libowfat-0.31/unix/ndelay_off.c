#include <sys/types.h>
#include <fcntl.h>

#include "ndelay.h"

#ifdef __MINGW32__
#include <winsock2.h>
#include "windoze.h"
#endif

#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif

int ndelay_off(int fd) {
#ifdef __MINGW32__
  unsigned long i=1;
  return winsock2errno(ioctlsocket( fd, FIONBIO, &i));
#else
  return fcntl(fd,F_SETFL,fcntl(fd,F_GETFL,0) & ~O_NDELAY);
#endif
}
