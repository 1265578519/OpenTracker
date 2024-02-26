#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "io_internal.h"

void io_wantwrite_really(int64 d, io_entry* e);

int64 io_canwrite() {
  io_entry* e;
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
    debug_printf(("io_canwrite: dequeue %lld from normal write queue (next is %ld)\n",r,first_writeable));
    if (e->wantwrite &&
#ifdef __MINGW32__
                        (e->canwrite || e->sendfilequeued==1)
#else
                        e->canwrite
#endif
      				   ) {
#if defined(HAVE_SIGIO)
      e->next_write=alt_firstwrite;
      alt_firstwrite=r;
      debug_printf(("io_canwrite: enqueue %ld in alt write queue (next is %ld)\n",alt_firstwrite,e->next_write));
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
