#include "parse.h"

size_t bs_capacityleft(const struct bytestream* bs) {
  if (bs->cur>=bs->max) 	// if EOF or error, return 0
    return 0;
  return bs->max - bs->cur;
}

#ifdef UNITTEST
#include <assert.h>

int main() {
  struct bytestream bs;

  bs.cur=0; bs.max=100;
  assert(bs_capacityleft(&bs) == 100);
  bs.cur=1;
  assert(bs_capacityleft(&bs) == 99);
  bs.max=0;
  assert(bs_capacityleft(&bs) == 0);
}
#endif
