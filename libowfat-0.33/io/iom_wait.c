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
#ifdef __dietlibc__
#include <threads.h>
#else
#include <semaphore.h>
#endif

// return the next event, waiting of none are queued
// wait at most timeout milliseconds
// on success, return 1 and return the fd in *s and the events on it in *revents
// if we waited but ran into a timeout, return 0
// if we run into a system error, return -1
// if another thread aborted this iomux, return -2
int iom_wait(iomux_t* c,int64* s,unsigned int* revents,unsigned long timeout) {
  for (;;) {
    /* If we have an event in the queue, use that one */
    int r;
    if (c->working==-2) return -2;	/* iomux was aborted */

    for (;;) {		// CAS-loop get the first element from the queue
      unsigned int f=c->l;	// c is a ring buffer, c->l is low, c->h is high
      // f is here to prevent double fetches from the volatile low water mark
      if (f == c->h)
	break; /* no elements in queue */
      // We want to grab the first element but other threads might be
      // racing us. So first grab the event from the low water mark in
      // the ring buffer, then increment the low water mark atomically,
      // and if that worked we know we grabbed the right event.
      int n=(f+1)%SLOTS;	// next value for c->l

      *s=c->q[f].fd;		// grab event we think is next in line
      *revents=c->q[f].events;

      // now atomically increment low water mark
      if (__sync_bool_compare_and_swap(&c->l,f,n)) {
	/* Nobody snatched the event from us. Report success */
	return 1;
      }
      /* collided with another thread, try again */
    }

    /* The queue was empty. If someone else is already calling
    * epoll_wait/kevent, then use the semaphore */
    if (__sync_bool_compare_and_swap(&c->working,0,1)) {
      /* If we get here, we got the lock and no other thread is
       * attempting to fill the queue at the same time. However,
       * another thread could have interrupted and refilled the job
       * queue already, so check if that happened. */
      if (c->h != c->l) {
	/* set working back to 0 unless someone set it to -2 in the mean time (iom_abort) */
	if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
	continue;	// this is why we have an outer for loop, so we don't need goto here
      }

      /* We have the job to fill the struct. */

#ifdef HAVE_EPOLL
      struct epoll_event ee[SLOTS];
      int i;
      r=epoll_wait(c->ctx, ee, SLOTS, timeout);
      if (r <= 0) {
	/* epoll_wait returned a timeout or an error! */
	/* relinquish the lock and return 0 / -1 */
	if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
#ifdef __dietlibc__
	// for timeout we want to hand off to one other thread, no need
	// to wake them all up. Error might be transient (EINTR) and the
	// next guy might succeed, so only wake one up. If the error was
	// not transient, then they will also get an error and wake the
	// next up
	cnd_signal(&c->sem);
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
	  /* loop terminates here, but no return statement because we
	   * still need to signal the semaphore below */
	} else {
	  /* The CAS loop on c->working above ensures we are the only one writing to c->h */
	  size_t hcapture = __atomic_load_n(&c->h, __ATOMIC_ACQUIRE);	// c->h is volatile so make copy to perf-avoid double fetch
	  c->q[hcapture].fd=ee[i].data.fd;
	  c->q[hcapture].events=e;
//	  c->h = (hcapture + 1) % SLOTS;
	  // use __atomic_store so ARM hardware writes c->q before c->h
	  __atomic_store_n(&c->h, (hcapture + 1) % SLOTS, __ATOMIC_RELEASE);
	}
      }
#elif defined(HAVE_KQUEUE)
      struct kevent kev[SLOTS];
      struct timespec ts = { .tv_sec=timeout/1000, .tv_nsec=(timeout%1000)*1000000 };
      int r=kevent(c->ctx, 0, 0, kev, SLOTS, &ts);
      int i;
      if (r<=0) {
	/* kevent returned a timeout or an error! */
	/* relinquish the lock and return 0 / -1 */
	if (__sync_val_compare_and_swap(&c->working,1,0)==-2) return -2;
#ifdef __dietlibc__
	// no dietlibc for kqueue based systems yet
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
	  *s=kev[i].ident;
	  *revents=e;
	  /* loop terminates here, but no return statement because we
	   * still need to signal the semaphore below */
	} else {
	  /* The CAS loop on c->working above ensures we are the only one writing to c->h */
	  size_t hcapture = __atomic_load_n(&c->h, __ATOMIC_ACQUIRE);	// c->h is volatile so make copy to perf-avoid double fetch
	  c->q[hcapture].fd=kev[i].ident;
	  c->q[hcapture].events=e;
//	  c->h = (c->h + 1) % SLOTS;
	  // use __atomic_store so ARM hardware writes c->q before c->h
	  __atomic_store_n(&c->h, (hcapture + 1) % SLOTS, __ATOMIC_RELEASE);
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
      if (c->h == (c->l + 1) % SLOTS)
	cnd_signal(&c->sem);
      else
	cnd_broadcast(&c->sem);
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
