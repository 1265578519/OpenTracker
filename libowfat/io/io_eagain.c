#include "io_internal.h"

void io_eagain(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    if (e->wantread) e->canread=0;
    if (e->wantwrite) e->canwrite=0;
#if defined(HAVE_SIGIO)
    if (d==alt_firstread) {
      debug_printf(("io_eagain: dequeueing %lld from alt read queue (next is %ld)\n",d,e->next_read));
      alt_firstread=e->next_read;
      e->next_read=-1;
    }
    if (d==alt_firstwrite) {
      debug_printf(("io_eagain: dequeueing %lld from alt write queue (next is %ld)\n",d,e->next_write));
      alt_firstwrite=e->next_write;
      e->next_write=-1;
    }
#endif
  }
}
