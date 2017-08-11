#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "io_internal.h"

#ifdef __MINGW32__
#include <stdio.h>
#endif

void io_wantread_really(int64 d, io_entry* e);

int64 io_canread() {
  io_entry* e;

#if defined(HAVE_SIGIO)
  /* We promise that the user can call io_canread() and read, and
    * the user uses io_tryread or calls io_eagain_read to signal if
    * there is no more data to read. That means if the user does not
    * call io_eagain_read, we need to know which fd it was so we can
    * keep it in the alternative queue. */
  if (alt_curread!=-1) {
    e=iarray_get(&io_fds,alt_curread);
    e->next_read=alt_firstread;
    alt_firstread=alt_curread;
    alt_curread=-1;
  }
#endif

  if (first_readable==-1)
#if defined(HAVE_SIGIO)
  {
    if (alt_firstread>=0 && (e=iarray_get(&io_fds,alt_firstread)) && e->canread) {
      debug_printf(("io_canread: normal read queue is empty, swapping in alt read queue (starting with %ld)\n",alt_firstread));
      first_readable=alt_firstread;
      alt_firstread=-1;
    } else
      return -1;
  }
#else
    return -1;
#endif
  for (;;) {
    int64 r;
    e=iarray_get(&io_fds,first_readable);
    if (!e) break;
    r=first_readable;
    first_readable=e->next_read;
    e->next_read=-1;
    debug_printf(("io_canread: dequeue %lld from normal read queue (next is %ld)\n",r,first_readable));

    if (e->closed) {
      /* The fd was previously closed, but there were still open events on it.
       * To prevent race conditions, we did not actually close the fd
       * but only marked it as closed, so we can skip this event here
       * and really closed it now. */
      io_close(r);
      continue;
    }

#ifdef __MINGW32__
//    printf("event on %d: wr %d rq %d aq %d\n",(int)r,e->wantread,e->readqueued,e->acceptqueued);
#endif

    if (e->wantread &&
#ifdef __MINGW32__
		       (e->canread || e->acceptqueued==1 || e->readqueued==1)
#else
			e->canread
#endif
				  ) {
#if defined(HAVE_SIGIO)
#if 0
      /* this code violates an invariant that the other code has, namely
       * that e->next_read is -1 once the fd is dequeued. */
      e->next_read=alt_firstread;
      alt_firstread=r;
      debug_printf(("io_canread: enqueue %ld in alt read queue (next is %ld)\n",alt_firstread,e->next_read));
#else
      alt_curread=r;
#endif
      if (io_waitmode!=_SIGIO)
#endif
	e->canread=0;
      if (!e->kernelwantread)
	io_wantread_really(r,e);
      return r;
    }
  }
  return -1;
}
