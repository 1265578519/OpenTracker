#define _GNU_SOURCE
#ifdef __MINGW32__
#include <winsock2.h>
#endif
#include <sys/param.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "iarray.h"
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

#include "haveaccept4.h"

int socket_accept6_flags(int s, char ip[16], uint16* port, uint32* scope_id, int flags)
{
#ifdef LIBC_HAS_IP6
  struct sockaddr_in6 sa;
#else
  struct sockaddr_in sa;
#endif
  socklen_t dummy = sizeof sa;
  int fd;

#ifdef __MINGW32__
  // Windows uses overlapped I/O instead of non-blocking I/O
  io_entry* e=iarray_get(&io_fds,s);
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
	if ((size_t)dummy>sizeof(sa)) dummy=sizeof(sa);
	memcpy(&sa,y,dummy);
      }
      fd=e->next_accept;
      e->next_accept=0;
      if (e->nonblock) {
	if (io_fd(fd)) {
	  io_entry* f=iarray_get(&io_fds,fd);
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
      if (e->next_accept==(SOCKET)-1)
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

#ifdef HAVE_ACCEPT4
    static int noaccept4;	// auto initialized to 0
    if (noaccept4)
      fd=-1;
    else {
      int flg=0;
      if (flags & SOCKET_NONBLOCK) flg += SOCK_NONBLOCK;
      if (flags & SOCKET_CLOEXEC) flg += SOCK_CLOEXEC;
      if ((fd=accept4(s,(void*) &sa, &dummy, flg))==-1) {
	if (errno != ENOSYS)
	  return -1;
	noaccept4=1;
      }
    }
    if (fd==-1) {
#endif
      int fl = 0;
      fd = accept(s, (struct sockaddr *) &sa, &dummy);
      if (fd == -1)
	return -1;
#ifndef __MINGW32__
      if (flags & SOCKET_NONBLOCK) fl |= O_NDELAY;
      if (flags & SOCKET_CLOEXEC) fl |= O_CLOEXEC;
      /* On BSD the accepted socket inherits O_NDELAY and O_CLOEXEC, on
       * Linux it doesn't. accept4 makes this explicit. So for the
       * fallback, make it explicit as well */
#ifdef __linux__
      if (fl) {
#endif
	if (fcntl(fd,F_SETFL,(fcntl(fd,F_GETFL,0) | fl) &~ (O_NDELAY | O_CLOEXEC)) == -1) {
	  /* This should never fail! */
	  close(fd);
	  return -1;
	}
#ifdef __linux__
      }
#endif
#endif
#ifdef HAVE_ACCEPT4
    }
#endif
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
