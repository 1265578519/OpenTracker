#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "socket.h"
#include "havesl.h"

int socket_connected(int s) {
  struct sockaddr si;
  socklen_t sl=sizeof si;
  if (getpeername(s,&si,&sl))
    return 0;
  return 1;
}
