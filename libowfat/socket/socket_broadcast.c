#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "socket.h"
#include "windoze.h"

int socket_broadcast(int s)
{
  int opt = 1;
  return winsock2errno(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&opt,sizeof opt));
}

