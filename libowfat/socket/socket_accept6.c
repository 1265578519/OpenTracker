#include <sys/param.h>
#include <sys/types.h>
#include <string.h>
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

#ifdef __MINGW32__
#include <windows.h>
#include <mswsock.h>
#include <errno.h>
#include <stdio.h>
#include "io_internal.h"
#endif

int socket_accept6(int s,char* ip,uint16* port,uint32* scope_id)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 sa;
#else
  struct sockaddr_in sa;
#endif
  socklen_t dummy = sizeof sa;
  int fd;

#ifdef __MINGW32__
  io_entry* e=array_get(&io_fds,sizeof(io_entry),s);
  if (e && e->inuse) {
    int sa2len;
    fd=-1;
    if (e->acceptqueued==1) {
      errno=EAGAIN;
      return -1;
    }
    if (e->acceptqueued==2) {
incoming:
      /* incoming! */
      {
	struct sockaddr* x,* y;
	GetAcceptExSockaddrs(e->inbuf,0,200,200,&x,&sa2len,&y,&dummy);
	if (dummy>sizeof(sa)) dummy=sizeof(sa);
	memcpy(&sa,y,dummy);
      }
      fd=e->next_accept;
      e->next_accept=0;
      if (e->nonblock) {
	if (io_fd(fd)) {
	  io_entry* f=array_get(&io_fds,sizeof(io_entry),fd);
	  if (f) {
	    f->nonblock=1;
//	    printf("setting fd %lu to non-blocking\n",(int)fd);
	  }
	}
      }
    }

    /* no accept queued, queue one now. */
    if (e->next_accept==0) {
      e->next_accept=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if (e==-1)
	return winsock2errno(-1);
    }
    if (AcceptEx(s,e->next_accept,e->inbuf,0,200,200,&e->errorcode,&e->or))
      goto incoming;
    if (WSAGetLastError() != ERROR_IO_PENDING)
      return winsock2errno(-1);
    e->acceptqueued=1;
    if (fd==-1) {
      errno=EAGAIN;
      return fd;
    }

  } else {
#endif
    fd = accept(s,(struct sockaddr *) &sa,&dummy);
    if (fd == -1)
      return winsock2errno(-1);
#ifdef __MINGW32__
  }
#endif

#ifdef LIBC_HAS_IP6
  if (noipv6 || sa.sin6_family==AF_INET || sa.sin6_family==PF_INET) {
    struct sockaddr_in *sa4=(struct sockaddr_in*)&sa;
    if (ip) {
      byte_copy(ip,12,V4mappedprefix);
      byte_copy(ip+12,4,(char *) &sa4->sin_addr);
    }
    if (port) uint16_unpack_big((char *) &sa4->sin_port,port);
    return fd;
  }
  if (ip) byte_copy(ip,16,(char *) &sa.sin6_addr);
  if (port) uint16_unpack_big((char *) &sa.sin6_port,port);
#ifdef LIBC_HAS_SCOPE_ID
  if (scope_id) *scope_id=sa.sin6_scope_id;
#else
  if (scope_id) *scope_id=0;
#endif

  return fd;
#else
  if (ip) {
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *) &sa.sin_addr);
  }
  if (port) uint16_unpack_big((char *) &sa.sin_port,port);
  if (scope_id) *scope_id=0;
  return fd;
#endif
}
