#include <sys/types.h>
#include <sys/param.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "byte.h"
#include "socket.h"
#include "ip6.h"
#include "haveip6.h"
#include "uint32.h"
#include "havesl.h"
#include "havescope.h"

int socket_local6(int s,char ip[16],uint16 *port,uint32 *scope_id)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 si;
#else
  struct sockaddr_in si;
#endif
  socklen_t len = sizeof si;

  if (getsockname(s,(void*) &si,&len) == -1) return winsock2errno(-1);
#ifdef LIBC_HAS_IP6
  if (noipv6 || si.sin6_family==AF_INET || si.sin6_family==PF_INET) {
    struct sockaddr_in *si4=(void*)&si;
    if (ip) {
      byte_copy(ip,12,V4mappedprefix);
      byte_copy(ip+12,4,(char *) &si4->sin_addr);
    }
    if (port) uint16_unpack_big((char *) &si4->sin_port,port);
    return 0;
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
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *) &si.sin_addr);
  }
  if (port) uint16_unpack_big((char *) &si.sin_port,port);
  if (scope_id) *scope_id=0;
#endif
  return 0;
}
