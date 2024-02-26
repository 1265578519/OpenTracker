#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include <errno.h>
#include "byte.h"
#include "socket.h"
#include "ip6.h"
#include "haveip6.h"
#include "ip4.h"
#include "havescope.h"

ssize_t socket_send6_flag(int s,const char *buf,size_t len,const char ip[16],uint16 port,uint32 scope_id,int flag)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 si;
#else
  struct sockaddr_in si;
#endif

  byte_zero(&si,sizeof si);
#ifdef LIBC_HAS_IP6
  if (noipv6) {
#endif
    if (ip6_isv4mapped(ip))
      return socket_send4(s,buf,len,ip+12,port);
    if (byte_equal(ip,16,V6loopback))
      return socket_send4(s,buf,len,ip4loopback,port);
#ifdef LIBC_HAS_IP6
    errno=EPROTONOSUPPORT;
    return -1;
  }
  si.sin6_family = AF_INET6;
  uint16_pack_big((char *) &si.sin6_port,port);
  byte_copy((char *) &si.sin6_addr,16,ip);
#ifdef LIBC_HAS_SCOPE_ID
  si.sin6_scope_id=scope_id;
#else
  si.sin6_scope_id=0;
#endif
  return winsock2errno(sendto(s,buf,len,flag,(void*) &si,sizeof si));
#else
  errno=EPROTONOSUPPORT;
  return -1;
#endif
}

ssize_t socket_send6(int s,const char *buf,size_t len,const char ip[16],uint16 port,uint32 scope_id) {
  return socket_send6_flag(s,buf,len,ip,port,scope_id,0);
}
