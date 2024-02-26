#include "parse.h"

void bs_seterrorstate(struct bytestream* b) {
  b->cur = 1;
  b->max = 0;
  if (b->type == BSTREAM)
    bs_seterrorstate(b->u.bs);
}
