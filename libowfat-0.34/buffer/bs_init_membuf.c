#include "parse.h"

void bs_init_membuf(struct bytestream* bs,const unsigned char* membuf,size_t len) {
  bs->type = MEMBUF;
  bs->cur = 0;
  bs->max = len;
  bs->u.base=membuf;
  if ((const unsigned char*)(((uintptr_t)membuf) + len) < membuf) {
    bs->cur = 1;	// if the buffer + length is implausible, set error immediately
    bs->max = 0;
  }
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"

ssize_t buffer_getc(buffer* b,char* c) { return -1; }

int main() {
  static struct bytestream bs;
  bs_init_membuf(&bs, (const unsigned char*)"fnord\n", 6);
  char buf[7];
  int i;
  for (i=0; i<7; ++i) buf[i]=bs_get(&bs);
  assert(!memcmp(buf,"fnord\n",7));	// we should have gotten everything and then a 0 byte
  assert(bs_err(&bs));		// that should have set the error flag

  /* see if we catch an impossible buffer size */
  bs_init_membuf(&bs, (const unsigned char*)"fnord\n", (size_t)-1);
  assert(bs_err(&bs));
}
#endif
