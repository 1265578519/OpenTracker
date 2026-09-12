#include "parse.h"

uint64_t prs_u64_big(struct bytestream* bs) {
  unsigned int i;
  uint64_t x = bs_get(bs);
  for (i=1; i<8; ++i)
    x = (x << 8) | bs_get(bs);
  return x;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"

// mock this
ssize_t buffer_getc(buffer* b,char* c) { return 0; }

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("\xde\xad\xbe\xef\x12\x34\x56\x78",8);
  assert(prs_u64_big(&bs) == 0xdeadbeef12345678);
  assert(bs_err(&bs) == 0);
  assert(prs_u64_big(&bs) == 0);
  assert(bs_err(&bs));
}
#endif
