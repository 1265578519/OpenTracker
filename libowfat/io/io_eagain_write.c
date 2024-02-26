#include "io_internal.h"

void io_wantwrite_really(int64 d, io_entry* e);

void io_eagain_write(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    e->canwrite=0;
    if (e->wantwrite && !e->kernelwantwrite)
      io_wantwrite_really(d, e);
#if defined(HAVE_SIGIO)
    if (d==alt_curwrite) {
#if 0
      debug_printf(("io_eagain: dequeueing %lld from alt write queue (next is %ld)\n",d,e->next_write));
      alt_firstwrite=e->next_write;
      e->next_write=-1;
#else
      alt_curwrite=-1;
#endif
    }
#endif
  }
}
