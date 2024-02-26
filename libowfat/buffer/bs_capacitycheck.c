#include "parse.h"

// This function is supposed to tell the caller if there is more data to
// read. However, we have several limits we could run into. We have our
// own limit, which we check first, but then, if the bytestream is bound
// to an I/O stream we should also try to find out if the I/O stream has
// hit EOF.
int bs_capacitycheck(struct bytestream* bs,size_t capacity) {
  if (bs->cur>=bs->max) return 0;	// if EOF or error, return 0
  if (bs->max - bs->cur < capacity) return 0;	// not EOF but less than that many bytes left
  return 1;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_err.c"

int main() {
  struct bytestream bs;
  // test basic functionality
  bs.cur=0; bs.max=100;
  assert(bs_capacitycheck(&bs, 100) == 1);
  assert(bs_err(&bs) == 0);

  bs.cur=50; bs.max=100;
  assert(bs_capacitycheck(&bs, 50) == 1);
  assert(bs_err(&bs) == 0);

  assert(bs_capacitycheck(&bs, 51) == 0);
  assert(bs_err(&bs) == 0);	// capacitycheck should not set error. capacityassert should

  // try to provoke a numeric overflow
  bs.cur=100; bs.max=(size_t)-1;
  assert(bs_capacitycheck(&bs, (size_t)-50) == 0);
  assert(bs_err(&bs) == 0);	// should still not set an error
}

#endif
