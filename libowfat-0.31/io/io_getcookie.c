#include <unistd.h>
#include "io_internal.h"

void* io_getcookie(int64 d) {
  io_entry* e;
  e=iarray_get(&io_fds,d);
  return e?e->cookie:0;
}
