#include "io_internal.h"
#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#endif
#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#include <errno.h>

int iom_wait(iomux_t* c,int64* s,unsigned int* revents,unsigned long timeout) {
  for (;;) {
    /* If we have an event in the queue, use that one */
    int r;
    if (c->working==-2) return -2;	/* iomux was aborted */
    for (;;) {
      unsigned int f=c->l;
      if (f == c->h)
	break; /* no elements in queue */
      int n=(f+1)%SLOTS;
      if (__sync_bool_compare_and_swap(&c->l,f,n)) {
	/* we got one, and its index is in f */
	*s=c->q[f].fd;
	*revents=c->q[f].events;
      }
      /* collided with another thread, try again */
    }
    /* The queue was empty. If someone else is already calling
    * epoll_wait/kevent, then use the semaphore */
    if (__sync_bool_compare_and_swap(&c->working,0,1)) {
      /* we have the job to fill the struct. */
      int freeslots = (c->h - c->l);
      if (!freeslots) freeslots=SLOTS;

#ifdef HAVE_EPOLL
      struct epoll_event ee[SLOTS];
      int i;
      r=epoll_wait(c->ctx, ee, freeslots, timeout);
      if (r<=0) {
	/* we ran into a timeout, so let someone else take over */
	if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
#ifdef __dietlibc__
	cnd_broadcast(&c->sem);
#else
	sem_post(&c->sem);
#endif
	return r;
      }
      for (i=0; i<r; ++i) {
	/* convert events */
	int e = ((ee[i].events & (EPOLLIN|EPOLLHUP|EPOLLERR)) ? IOM_READ : 0) |
	        ((ee[i].events & (EPOLLOUT|EPOLLHUP|EPOLLERR)) ? IOM_WRITE : 0) |
	        ((ee[i].events & EPOLLERR) ? IOM_ERROR : 0);
	if (i+1==r) {
	  /* return last event instead of enqueueing it */
	  *s=ee[i].data.fd;
	  *revents=e;
	} else {
	  c->q[c->h].fd=ee[i].data.fd;
	  c->q[c->h].events=e;
	  c->h = (c->h + 1) % SLOTS;
	}
      }
#elif defined(HAVE_KQUEUE)
      struct kevent kev[SLOTS];
      struct timespec ts = { .tv_sec=timeout/1000, .tv_nsec=(timeout%1000)*1000000 };
      int r=kevent(c->ctx, 0, 0, &kev, freeslots, &ts);
      if (r<=0) {
	/* we ran into a timeout, so let someone else take over */
	if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
#ifdef __dietlibc__
	cnd_broadcast(&c->sem);
#else
	sem_post(&c->sem);
#endif
	return r;
      }
      for (i=0; i<r; ++i) {
	/* convert events */
	int e = (kev[i].filter == EVFILT_READ ? IOM_READ : 0) |
	        (kev[i].filter == EVFILT_WRITE ? IOM_WRITE : 0);
	if (i+1==r) {
	  /* return last event instead of enqueueing it */
	  *s=kev.ident;
	  *revents=e;
	} else {
	  c->q[c->h].fd=kev[i].ident;
	  c->q[c->h].events=e;
	  c->h = (c->h + 1) % SLOTS;
	}
      }
#else
#warning "only epoll and kqueue supported for now"
#endif
      /* We need to signal the other threads.
	 Either there are other events left, or we need one of them to
	 wake up and call epoll_wait/kevent next, because we aren't
	 doing it anymore */
      if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
#ifdef __dietlibc__
      cnd_signal(&c->sem);
#else
      sem_post(&c->sem);
#endif
      return 1;
    } else {
      /* somebody else has the job to fill the queue */
      struct timespec ts;
      ts.tv_sec = timeout / 1000;
      ts.tv_nsec = (timeout % 1000) * 1000000;
#ifdef __dietlibc__
      r=cnd_timedwait(&c->sem,&c->mtx,&ts);
#else
      r=sem_timedwait(&c->sem,&ts);
#endif
      if (r==-1) {
	if (errno==ETIMEDOUT) return 0;
	return -1;
      }
      /* fall through into next loop iteration */
    }
  }
}
