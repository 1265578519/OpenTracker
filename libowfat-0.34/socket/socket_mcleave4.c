#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "socket.h"
#include "byte.h"

int socket_mcleave4(int s,const char ip[4])
{
  struct ip_mreq opt;
  byte_copy(&opt.imr_multiaddr.s_addr,4,ip);
  byte_zero(&opt.imr_interface.s_addr,4);
  return winsock2errno(setsockopt(s,IPPROTO_IP,IP_DROP_MEMBERSHIP,&opt,sizeof opt));
}

