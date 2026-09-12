#include "parse.h"

int bs_nomoredataassert(struct bytestream* bs) {
  if (bs->cur == bs->max)
    return 1;
  bs->cur = 1;	/* otherwise set error state */
  bs->max = 0;
  return 0;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_err.c"

int main() {
  struct bytestream bs;

  bs.cur=0; bs.max=100;	// full buffer
  assert(bs_nomoredataassert(&bs) == 0);
  assert(bs_err(&bs));

  bs.cur=0; bs.max=0;	// empty buffer
  assert(bs_nomoredataassert(&bs) == 1);
  assert(bs_err(&bs) == 0);

  bs.cur=1; bs.max=0;	// error state buffer
  assert(bs_nomoredataassert(&bs) == 0);
  assert(bs_err(&bs));

  return 0;
}
#endif
