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

/* IDEA: if someone calls io_dontwantwrite, do not do the syscall to
 * tell the kernel about it.  Only when a write event comes in and the
 * user has told us he does not want them, THEN tell the kernel we are
 * not interested.  In the typical protocol case of "write request, read
 * reply", this should save a lot of syscalls. */

void io_dontwantwrite_really(int64 d,io_entry* e) {
  int newfd;
  (void)d;
  assert(e->kernelwantwrite);
  newfd=!e->kernelwantread;
  io_wanted_fds-=newfd;
#ifdef HAVE_EPOLL
  if (io_waitmode==EPOLL) {
    struct epoll_event x;
    byte_zero(&x,sizeof(x));   // to shut up valgrind
    x.events=0;
    if (e->wantread) x.events|=EPOLLIN;
    x.data.fd=d;
    epoll_ctl(io_master,e->kernelwantread?EPOLL_CTL_MOD:EPOLL_CTL_DEL,d,&x);
  }
#endif
#ifdef HAVE_KQUEUE
  if (io_waitmode==KQUEUE) {
    struct kevent kev;
    struct timespec ts;
    EV_SET(&kev, d, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    ts.tv_sec=0; ts.tv_nsec=0;
    kevent(io_master,&kev,1,0,0,&ts);
  }
#endif
#ifdef HAVE_DEVPOLL
  if (io_waitmode==DEVPOLL) {
    struct pollfd x;
    x.fd=d;
    x.events=0;
    if (e->kernelwantread) x.events|=POLLIN;
    if (!x.events) x.events=POLLREMOVE;
    write(io_master,&x,sizeof(x));
  }
#endif
  e->wantwrite=0;
  e->kernelwantwrite=0;
}

void io_dontwantwrite(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    if (e->canwrite)
      io_dontwantwrite_really(d,e);
    e->wantwrite=0;
  }
}
