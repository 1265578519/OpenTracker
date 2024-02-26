#ifndef __MINGW32__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "windoze.h"
#include "socket.h"
#include "havesl.h"

#ifdef __MINGW32__
#include <windows.h>
#include <mswsock.h>
#include <errno.h>
#include <stdio.h>
#include "io_internal.h"
#endif

int socket_accept4(int s,char *ip,uint16 *port) {
  struct sockaddr_in si;
  socklen_t len = sizeof si;
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
	GetAcceptExSockaddrs(e->inbuf,0,200,200,&x,&sa2len,&y,&len);
	if (len>sizeof(si)) len=sizeof(si);
	memcpy(&si,y,len);
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

    if ((fd=accept(s,(void*) &si,&len))==-1)
      return winsock2errno(-1);

#ifdef __MINGW32__
  }
#endif
  if (ip) *(uint32*)ip = *(uint32*)&si.sin_addr;
  if (port) uint16_unpack_big((char *) &si.sin_port,port);
  return fd;
}
