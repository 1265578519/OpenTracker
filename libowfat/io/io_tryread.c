#include <unistd.h>
#include <sys/time.h>
#ifdef __MINGW32__
#include <windows.h>
#include <stdio.h>
#else
#include <poll.h>
#endif
#include <errno.h>
#include "io_internal.h"
#include "byte.h"

#ifdef __MINGW32__
/* In Windows, I/O works differently. */
/* Instead of calling read until it says EAGAIN, you call read in
 * overlapping mode, and then wait for it to finish.
 * We map this to our API by having the first call to io_tryread always
 * return EAGAIN, wait for the I/O completion port to tell us the read
 * is finished, and then return the data we actually read the next time
 * we are called. */

int64 io_tryread(int64 d,char* buf,int64 len) {
  io_entry* e=iarray_get(&io_fds,d);
  if (!e) { errno=EBADF; return -3; }
  if (len<0) { errno=EINVAL; return -3; }
  if (e->readqueued==2) {
    int x=e->bytes_read;
    if (e->errorcode) {
      errno=e->errorcode;
      e->canread=0;
      return -3;
    }
    if (x>len) x=len;
    if (x) {
      byte_copy(buf,x,e->inbuf);
      byte_copy(e->inbuf,e->bytes_read-x,e->inbuf+x);
      e->bytes_read-=x;
    }
    if (!e->bytes_read) {
      e->canread=0;
      if (len>x) {
	/* queue next read */
	if (len>sizeof(e->inbuf)) len=sizeof(e->inbuf);
	fprintf(stderr,"Queueing ReadFile on handle %p...",d);
	if (ReadFile((HANDLE)d,e->inbuf,len,0,&e->or)) {
	  fprintf(stderr," got immediate answer\n");
	  e->canread=1;
	  e->readqueued=2;
	  e->next_write=first_writeable;
	  first_writeable=d;
	} else if ((e->errorcode=GetLastError())==ERROR_IO_PENDING) {
	  fprintf(stderr," OK\n");
	  e->readqueued=1;
	  e->errorcode=0;
	} else {
	  fprintf(stderr," error!\n");
	  e->canread=1;
	  e->readqueued=2;
	  e->next_write=first_writeable;
	  first_writeable=d;
	}
      }
    }
    return x;
  }
  if (!e->readqueued) {
    fprintf(stderr,"!e->readqueued\n");
    if (len>sizeof(e->inbuf)) len=sizeof(e->inbuf);
    if (ReadFile((HANDLE)d,e->inbuf,len,0,&e->or)) {
      e->readqueued=1;
      fprintf(stderr,"ReadFile returned nonzero\n");
    } else
      fprintf(stderr,"ReadFile returned zero\n");
  }
  errno=EAGAIN;
  return -1;
}

#else

int64 io_tryread(int64 d,char* buf,int64 len) {
  long r;
  struct itimerval old,new;
  struct pollfd p;
  io_entry* e=iarray_get(&io_fds,d);
  if (!e) { errno=EBADF; return -3; }
  if (!e->nonblock) {
    p.fd=d;
    if (p.fd!=d) { errno=EBADF; return -3; }	/* catch integer truncation */
    p.events=POLLIN;
    switch (poll(&p,1,0)) {
    case -1: return -3;
    case 0: errno=EAGAIN;
	    e->canread=0;
	    e->next_read=-1;
	    return -1;
    }
    new.it_interval.tv_usec=10000;
    new.it_interval.tv_sec=0;
    new.it_value.tv_usec=10000;
    new.it_value.tv_sec=0;
    setitimer(ITIMER_REAL,&new,&old);
  }
  r=read(d,buf,len);
  if (!e->nonblock) {
    setitimer(ITIMER_REAL,&old,0);
  }
  if (r==-1 && errno==EAGAIN)
    io_eagain_read(d);
  if (r==-1) {
    if (errno==EINTR) errno=EAGAIN;
    if (errno!=EAGAIN)
      r=-3;
  }
  if (r!=len) {
    e->canread=0;
    io_eagain_read(d);
#if defined(HAVE_SIGIO)
#if 0
    debug_printf(("io_tryread: dequeueing %ld from alt read queue (next is %ld)\n",d,alt_firstread));
    alt_firstread=e->next_read;
    e->next_read=-1;
#else
  } else {
    debug_printf(("io_tryread: enqueueing %ld into alt read queue (next is %ld)\n",d,alt_firstread));
    e->next_read=alt_firstread;
    alt_firstread=d;
#endif
#endif
  }
  return r;
}

#endif
