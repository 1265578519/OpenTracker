#include <sys/types.h>
#ifndef __MINGW32__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"

#include "socket.h"
#include "ndelay.h"

int socket_udp4b(void) {
  int s;
  __winsock_init();
  s = winsock2errno(socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP));
  return s;
}

int socket_udp4(void) {
  int s;
  s=socket_udp4b();
  if (s!=-1 && ndelay_on(s) == -1) { close(s); return -1; }
  return s;
}
