#include "io_internal.h"

void io_eagain_write(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    e->canwrite=0;
#if defined(HAVE_SIGIO)
    if (d==alt_firstwrite) {
      debug_printf(("io_eagain: dequeueing %lld from alt write queue (next is %ld)\n",d,e->next_write));
      alt_firstwrite=e->next_write;
      e->next_write=-1;
    }
#endif
  }
}
