#include "io_internal.h"

void io_eagain_read(int64 d) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e) {
    e->canread=0;
#if defined(HAVE_SIGIO)
    if (d==alt_curread) {
#if 0
      debug_printf(("io_eagain: dequeueing %lld from alt read queue (next is %ld)\n",d,e->next_read));
      alt_firstread=e->next_read;
      e->next_read=-1;
#else
      alt_curread=-1;
#endif
    }
#endif
  }
}
