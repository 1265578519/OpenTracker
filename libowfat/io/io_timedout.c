#include "io_internal.h"

int io_timedout(int64 d) {
  tai6464 now;
  io_entry* e=iarray_get(&io_fds,d);
  taia_now(&now);
  return (e && e->timeout.sec.x && taia_less(&e->timeout,&now));
}
