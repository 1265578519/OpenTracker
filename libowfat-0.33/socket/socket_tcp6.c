#include <sys/types.h>
#ifndef __MINGW32__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <errno.h>
#include "haveip6.h"
#include "socket.h"
#include "ndelay.h"

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT EINVAL
#endif
#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT EAFNOSUPPORT
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT EAFNOSUPPORT
#endif

int socket_tcp6(void) {
  int s;
#if defined(LIBC_HAS_IP6) && defined(SOCK_NONBLOCK)
  if ((s=socket(noipv6?PF_INET:PF_INET6,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP))>-1 ||
      (errno!=EINVAL && errno!=EAFNOSUPPORT && errno!=EPFNOSUPPORT && errno!=EPROTONOSUPPORT)) return s;
#endif
  s=socket_tcp6b();
  if (s==-1) return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }
  return s;
}
