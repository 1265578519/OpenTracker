#include <sys/param.h>
#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "byte.h"
#include "socket.h"
#include "ip6.h"
#include "haveip6.h"
#include "havesl.h"
#include "havescope.h"

ssize_t socket_recv6(int s,char *buf,size_t len,char ip[16],uint16 *port,uint32 *scope_id)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 si;
#else
  struct sockaddr_in si;
#endif
  socklen_t Len = sizeof si;
  ssize_t r;

  byte_zero(&si,Len);
  if ((r = recvfrom(s,buf,len,0,(struct sockaddr *) &si,&Len))<0) return winsock2errno(-1);

#ifdef LIBC_HAS_IP6
  if (noipv6 || si.sin6_family==AF_INET || si.sin6_family==PF_INET) {
    struct sockaddr_in *si4=(struct sockaddr_in *)&si;
    if (ip) {
      byte_copy(ip,12,V4mappedprefix);
      byte_copy(ip+12,4,(char *) &si4->sin_addr);
    }
    if (port) uint16_unpack_big((char *) &si4->sin_port,port);
    return r;
  }
  if (ip) byte_copy(ip,16,(char *) &si.sin6_addr);
  if (port) uint16_unpack_big((char *) &si.sin6_port,port);
#ifdef LIBC_HAS_SCOPE_ID
  if (scope_id) *scope_id=si.sin6_scope_id;
#else
  if (scope_id) *scope_id=0;
#endif
#else
  if (ip) {
    byte_copy(ip,12,(char *)V4mappedprefix);
    byte_copy(ip+12,4,(char *) &si.sin_addr);
  }
  if (port) uint16_unpack_big((char *) &si.sin_port,port);
  if (scope_id) *scope_id=0;
#endif

  return r;
}
