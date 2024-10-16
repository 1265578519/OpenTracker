#define _GNU_SOURCE
#ifndef __MINGW32__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <string.h>
#include "windoze.h"
#include "socket.h"
#include "iarray.h"
#include "havesl.h"

#ifdef __MINGW32__
#include <windows.h>
#include <mswsock.h>
#include <errno.h>
#include <stdio.h>
#include "io_internal.h"
#endif

#include "haveaccept4.h"

int socket_accept4_flags(int s, char ip[4], uint16 *port, int flags) {
  struct sockaddr_in si;
  socklen_t len = sizeof si;
  int fd;

#ifdef __MINGW32__
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
	GetAcceptExSockaddrs(e->inbuf,0,200,200,&x,&sa2len,&y,&len);
	if ((size_t)len>sizeof(si)) len=sizeof(si);
	memcpy(&si,y,len);
      }
      fd=e->next_accept;
      e->next_accept=0;
      if (e->nonblock) {
	if (io_fd_canwrite(fd)) {
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
      if ((fd=accept4(s,(void*) &si, &len, flg))==-1) {
	if (errno != ENOSYS)
	  return -1;
	// if we get here, fd==-1 && errno==ENOSYS
	noaccept4=1;
      }
    }
    if (fd==-1) {
#endif
      int fl = 0;
      /* if we get here, the kernel did not support accept4. */
      if ((fd=accept(s,(void*) &si,&len))==-1)
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
  if (ip) *(uint32*)ip = *(uint32*)&si.sin_addr;
  if (port) uint16_unpack_big((char *) &si.sin_port,port);
  return fd;
}
