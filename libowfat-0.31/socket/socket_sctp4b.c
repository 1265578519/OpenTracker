#include <sys/types.h>
#ifndef __MINGW32__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "socket.h"
#include "ndelay.h"
#include <errno.h>

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT EINVAL
#endif

int socket_sctp4b(void) {
#ifdef IPPROTO_SCTP
  int s;
  __winsock_init();
  s = winsock2errno(socket(PF_INET,SOCK_STREAM,IPPROTO_SCTP));
  if (s == -1) return -1;
  return s;
#else
  errno=EPROTONOSUPPORT;
  return -1;
#endif
}
