#include "io_internal.h"
#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#endif
#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

int iom_init(iomux_t* c) {
#ifdef HAVE_EPOLL
  c->ctx = epoll_create1(EPOLL_CLOEXEC);
#elif defined(HAVE_KQUEUE)
  if ((c->ctx = kqueue()) != -1) {
    if (fcntl(c->ctx,F_SETFD,FD_CLOEXEC) == -1) {
      close(c->ctx);
      c->ctx=-1;
    }
  }
#else
#warning "only epoll and kqueue supported for now"
#endif
  unsigned int i;
  c->working=0;
  c->h=c->l=0;	/* no elements in queue */
  for (i=0; i<SLOTS; ++i) {
    c->q[i].fd=-1;
    c->q[i].events=0;
  }
#ifdef __dietlibc__
  mtx_init(&c->mtx, mtx_timed);
  cnd_init(&c->sem);
#else
  sem_init(&c->sem, 0, 1);
#endif
  return (c->ctx!=-1);
}
