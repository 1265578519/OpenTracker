#include "io_internal.h"

void io_timeout(int64 d,tai6464 t) {
  io_entry* e=iarray_get(&io_fds,d);
  if (e)
    e->timeout=t;
}
