#include "parse.h"

uint32_t prs_u32(struct bytestream* bs) {
  return bs_get(bs) | (bs_get(bs) << 8) | (bs_get(bs) << 16) | (bs_get(bs) << 24);
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"

// mock this
ssize_t buffer_getc(buffer* b,char* c) { return 0; }

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("\x78\x56\x34\x12",4);
  assert(prs_u32(&bs) == 0x12345678);
  assert(bs_err(&bs) == 0);
  assert(prs_u32(&bs) == 0);
  assert(bs_err(&bs));
}
#endif
