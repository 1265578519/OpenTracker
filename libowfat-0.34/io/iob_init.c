#include <stdlib.h>
#include "byte.h"
#include "iob_internal.h"

int iob_init(io_batch* b,size_t hint_entries) {
  byte_zero(b,sizeof(io_batch));
  if (hint_entries) {
    if (!array_allocate(&b->b,sizeof(iob_entry),hint_entries))
      return -1;
    array_trunc(&b->b);
  }
  return 0;
}
