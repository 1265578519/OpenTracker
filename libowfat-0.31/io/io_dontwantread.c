#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "io_internal.h"
#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#ifdef HAVE_EPOLL
#include <inttypes.h>
#include <sys/epoll.h>
#include <byte.h>
#endif
#ifdef HAVE_DEVPOLL
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/devpoll.h>
#endif

#ifdef DEBUG
#include <assert.h>
#else
#define assert(x)
#endif

void io_dontwantread_really(int64 d, io_entry* e) {
  int newfd;
  (void)d;
  assert(e->kernelwantread);
  newfd=!e->kernelwantwrite;
  io_wanted_fds-=newfd;
#ifdef HAVE_EPOLL
  if (io_waitmode==EPOLL) {
    struct epoll_event x;
    byte_zero(&x,sizeof(x));   // to shut up valgrind
    x.events=0;
    if (e->kernelwantwrite) x.events|=EPOLLOUT;
    x.data.fd=d;
    epoll_ctl(io_master,e->kernelwantwrite?EPOLL_CTL_MOD:EPOLL_CTL_DEL,d,&x);
  }
#endif
#ifdef HAVE_KQUEUE
  if (io_waitmode==KQUEUE) {
    struct kevent kev;
    struct timespec ts;
    EV_SET(&kev, d, EVFILT_READ, EV_DELETE, 0, 0, 0);
    ts.tv_sec=0; ts.tv_nsec=0;
    kevent(io_master,&kev,1,0,0,&ts);
  }
#endif
#ifdef HAVE_DEVPOLL
  if (io_waitmode==DEVPOLL) {
    struct pollfd x;
    x.fd=d;
    x.events=0;
    if (e->kernelwantwrite) x.events|=POLLOUT;
    if (!x.events) x.events=POLLREMOVE;
    write(io_master,&x,sizeof(x));
  }
#endif
  e->wantread=0;
  e->kernelwantread=0;
}

void io_dontwantread(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    if (e->canread)
      io_dontwantread_really(d,e);
    e->wantread=0;
  }
}
