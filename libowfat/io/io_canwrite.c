#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "io_internal.h"

void io_wantwrite_really(int64 d, io_entry* e);

int64 io_canwrite() {
  io_entry* e;

#if defined(HAVE_SIGIO)
  /* We promise that the user can call io_canread() and read, and
    * the user uses io_tryread or calls io_eagain_read to signal if
    * there is no more data to read. That means if the user does not
    * call io_eagain_read, we need to know which fd it was so we can
    * keep it in the alternative queue. */
  if (alt_curwrite!=-1) {
    e=iarray_get(&io_fds,alt_curwrite);
    e->next_write=alt_firstwrite;
    alt_firstwrite=alt_curwrite;
    alt_curwrite=-1;
  }
#endif

  if (first_writeable==-1)
#if defined(HAVE_SIGIO)
  {
    if (alt_firstwrite>=0 && (e=iarray_get(&io_fds,alt_firstwrite)) && e->canwrite) {
      debug_printf(("io_canwrite: normal write queue is empty, swapping in alt write queue (starting with %ld)\n",alt_firstwrite));
      first_writeable=alt_firstwrite;
      alt_firstwrite=-1;
    } else
      return -1;
  }
#else
    return -1;
#endif
  for (;;) {
    int64 r;
    e=iarray_get(&io_fds,first_writeable);
    if (!e) break;
    r=first_writeable;
    first_writeable=e->next_write;
    e->next_write=-1;

    if (e->closed) {
      /* The fd was previously closed, but there were still open events on it.
       * To prevent race conditions, we did not actually close the fd
       * but only marked it as closed, so we can skip this event here
       * and really closed it now. */
      io_close(r);
      continue;
    }

    debug_printf(("io_canwrite: dequeue %lld from normal write queue (next is %ld)\n",r,first_writeable));
    if (e->wantwrite &&
#ifdef __MINGW32__
                        (e->canwrite || e->sendfilequeued==1)
#else
                        e->canwrite
#endif
      				   ) {
#if defined(HAVE_SIGIO)
#if 0
      /* this code violates an invariant that the other code has, namely
       * that e->next_read is -1 once the fd is dequeued. */
      e->next_write=alt_firstwrite;
      alt_firstwrite=r;
      debug_printf(("io_canwrite: enqueue %ld in alt write queue (next is %ld)\n",alt_firstwrite,e->next_write));
#else
      alt_curwrite=r;
#endif
      if (io_waitmode!=_SIGIO)
#endif
	e->canwrite=0;
      if (!e->kernelwantwrite)
	io_wantwrite_really(r,e);
      return r;
    }
  }
  return -1;
}
