#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "socket.h"

int socket_mcloop4(int s,char loop)
{
  return winsock2errno(setsockopt(s,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof loop));
}

