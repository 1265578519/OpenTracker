#include "parse.h"

// This function asserts that the caller wants to read n bytes from the stream.
// If there is less space available, set the stream to error state.
int bs_capacityassert(struct bytestream* bs,size_t capacity) {
  if (bs->cur>=bs->max || 	// if EOF or error, return 0
      bs->max - bs->cur < capacity) {	// not EOF but less than that many bytes left
    bs->cur = 1;	// set stream to error state
    bs->max = 0;
    return 0;		// return failure
  }
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
  assert(bs_capacityassert(&bs, 100) == 1);
  assert(bs_err(&bs) == 0);

  bs.cur=50; bs.max=100;
  assert(bs_capacityassert(&bs, 50) == 1);
  assert(bs_err(&bs) == 0);

  assert(bs_capacityassert(&bs, 51) == 0);
  assert(bs_err(&bs));

  // try to provoke a numeric overflow
  bs.cur=100; bs.max=(size_t)-1;
  assert(bs_capacityassert(&bs, (size_t)-50) == 0);
  assert(bs_err(&bs));
}

#endif
