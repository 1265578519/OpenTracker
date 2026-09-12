#include "socket.h"
#ifndef __MINGW32__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#endif
#include <errno.h>

int socket_quickack(int s,int value) {
#ifdef TCP_QUICKACK
  return setsockopt(s, SOL_TCP, TCP_QUICKACK, &value, sizeof(int));
#else
  (void)s;	// shut up unused variable warnings
  (void)value;
#ifdef ENOPROTOOPT
  errno=ENOPROTOOPT;
#else
  errno=ENOSYS;
#endif
  return -1;
#endif
}
