#include "socket.h"
#ifndef __MINGW32__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#include <errno.h>

#if defined(TCP_FASTOPEN) && !defined(SOL_TCP) && defined(IPPROTO_TCP)
#define SOL_TCP IPPROTO_TCP
#endif

int socket_fastopen(int s) {
#if defined(SOL_TCP) && defined(TCP_FASTOPEN)
  return setsockopt(s,SOL_TCP,TCP_FASTOPEN,(int[]){ 5 }, sizeof(int));
#else
#ifdef ENOPROTOOPT
  errno=ENOPROTOOPT;
#else
  errno=ENOSYS;
#endif
  return -1;
#endif
}
