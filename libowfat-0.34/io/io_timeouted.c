#include "io_internal.h"

int64 io_timeouted() {
  tai6464 now;
  static size_t ptr;
  io_entry* e;
  size_t alen=iarray_length(&io_fds);
  taia_now(&now);
  ++ptr;
  if (ptr>alen) ptr=0;
  e=iarray_get(&io_fds,ptr);
//  if (!e) return -1;
  for (;ptr<=alen; ++ptr,e=iarray_get(&io_fds,ptr)) {
    if (e && e->inuse && e->timeout.sec.x && taia_less(&e->timeout,&now)) {
      /* we have a timeout */
      if ((e->canread&&e->wantread) || (e->canwrite&&e->wantwrite))
	continue;	/* don't count it if we can signal something else */
      return ptr;
    }
  }
  ptr=-1;	/* this is for really pathological cases, where more
		   connections come in all the time and so the timeout
		   handling does not trigger initially */
  return -1;
}
