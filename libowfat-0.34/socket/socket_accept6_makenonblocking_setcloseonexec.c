#ifndef __MINGW32__
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <string.h>
#include "windoze.h"
#include "socket.h"

int socket_accept6_makenonblocking(int s,char ip[16],uint16 *port,uint32* scope_id) {
  return socket_accept6_flags(s,ip,port,scope_id,SOCKET_NONBLOCK | SOCKET_CLOEXEC);
}
