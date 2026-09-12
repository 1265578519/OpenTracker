#include <stdlib.h>
#include "byte.h"
#include "iob_internal.h"

void iob_reset(io_batch* b) {
  unsigned long i,l;
  iob_entry* x=array_start(&b->b);
  l=array_length(&b->b,sizeof(iob_entry));
  for (i=0; i<l; ++i) {
    if (x[i].cleanup)
      x[i].cleanup(x+i);
  }
  array_reset(&b->b);
  byte_zero(b,sizeof(*b));
}
