#include <unistd.h>
#include <sys/time.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <poll.h>
#endif
#include <errno.h>
#include "io_internal.h"

#ifdef __MINGW32__
#include <stdio.h>

/* All the Unix trickery is unsupported on Windows.  Instead, one is
 * supposed to do the whole write in overlapping mode and then get
 * notified via an I/O completion port when it's done. */

/* So we assume io_trywrite is not used so much and do the overlapping
 * stuff on I/O batches. */

int64 io_trywrite(int64 d,const char* buf,int64 len) {
  io_entry* e=iarray_get(&io_fds,d);
  int r;
  if (!e) { errno=EBADF; return -3; }
  if (!e->nonblock) {
    DWORD written;
    fprintf(stderr,"Socket is in blocking mode, just calling WriteFile...");
    if (WriteFile((HANDLE)d,buf,len,&written,0)) {
      fprintf(stderr," OK, got %u bytes.\n",written);
      return written;
    } else {
      fprintf(stderr," failed.\n",written);
      return winsock2errno(-3);
    }
  } else {
    if (e->writequeued && !e->canwrite) {
      fprintf(stderr,"io_trywrite: write already queued, returning EAGAIN\n");
      errno=EAGAIN;
      return -1;
    }
    if (e->canwrite) {
      e->canwrite=0;
      e->next_write=-1;
      if (e->errorcode) {
	fprintf(stderr,"io_trywrite: e->canwrite was set, returning error %d\n",e->errorcode);
	errno=winsock2errno(e->errorcode);
	return -3;
      }
      fprintf(stderr,"io_trywrite: e->canwrite was set, had written %u bytes\n",e->bytes_written);
      return e->bytes_written;
    } else {
      fprintf(stderr,"io_trywrite: queueing write...");
      if (WriteFile((HANDLE)d,buf,len,&e->errorcode,&e->ow)) {
	fprintf(stderr," worked unexpectedly, error %d\n",e->errorcode);
	return e->errorcode; /* should not happen */
      } else if (GetLastError()==ERROR_IO_PENDING) {
	fprintf(stderr," pending.\n");
	e->writequeued=1;
	errno=EAGAIN;
	e->errorcode=0;
	return -1;
      } else {
	fprintf(stderr," failed, error %d\n",e->errorcode);
	winsock2errno(-1);
	e->errorcode=errno;
	return -3;
      }
    }
  }
}

#else

int64 io_trywrite(int64 d,const char* buf,int64 len) {
  long r;
  struct itimerval old,new;
  struct pollfd p;
  io_entry* e=iarray_get(&io_fds,d);
  io_sigpipe();
  if (!e) { errno=EBADF; return -3; }
  if (!e->nonblock) {
    p.fd=d;
    if (p.fd != d) { errno=EBADF; return -3; }	/* catch overflow */
    p.events=POLLOUT;
    switch (poll(&p,1,0)) {
    case -1: return -3;
    case 0: errno=EAGAIN;
	    e->canwrite=0;
	    e->next_write=-1;
	    return -1;
    }
    new.it_interval.tv_usec=10000;
    new.it_interval.tv_sec=0;
    new.it_value.tv_usec=10000;
    new.it_value.tv_sec=0;
    setitimer(ITIMER_REAL,&new,&old);
  }
  r=write(d,buf,len);
  if (!e->nonblock) {
    setitimer(ITIMER_REAL,&old,0);
  }
  if (r==-1) {
    if (errno==EINTR) errno=EAGAIN;
    if (errno!=EAGAIN)
      r=-3;
  }
  if (r!=len) {
    e->canwrite=0;
#if defined(HAVE_SIGIO)
    if (d==alt_firstwrite) {
      debug_printf(("io_trywrite: dequeueing %ld from alt write queue (next is %ld)\n",d,e->next_write));
      alt_firstwrite=e->next_write;
      e->next_write=-1;
    }
#endif
  }
  return r;
}

#endif
