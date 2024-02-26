#include "parse.h"

uint64_t prs_u64(struct bytestream* bs) {
  unsigned int i;
  uint64_t x = bs_get(bs);
  for (i=1; i<8; ++i)
    x |= ((uint64_t)bs_get(bs) << (i*8));
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
  struct bytestream bs = BS_FROM_MEMBUF("\x78\x56\x34\x12\xef\xbe\xad\xde",8);
  assert(prs_u64(&bs) == 0xdeadbeef12345678);
  assert(bs_err(&bs) == 0);
  assert(prs_u64(&bs) == 0);
  assert(bs_err(&bs));
}
#endif
