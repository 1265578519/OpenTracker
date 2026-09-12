#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "socket.h"
#include "byte.h"
#include "haveip6.h"
#include "ip6.h"
#include "errno.h"

#ifndef IPV6_ADD_MEMBERSHIP
#ifdef IPV6_JOIN_GROUP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#else
#undef LIBC_HAS_IP6
#endif
#endif

int socket_mcjoin6(int s,const char ip[16],int _interface)
{
#ifdef LIBC_HAS_IP6
  struct ipv6_mreq opt;
#endif
  if (ip6_isv4mapped(ip))
    return socket_mcjoin4(s,ip+12,ip);
#ifdef LIBC_HAS_IP6
  byte_copy(&opt.ipv6mr_multiaddr,16,ip);
  opt.ipv6mr_interface=_interface;
  return winsock2errno(setsockopt(s,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,&opt,sizeof opt));
#else
  errno=EPROTONOSUPPORT;
  return -1;
#endif
}
