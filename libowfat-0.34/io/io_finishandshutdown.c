#include "io_internal.h"

void io_finishandshutdown(void) {
  iarray_free(&io_fds);
  array_reset(&io_pollfds);
}
