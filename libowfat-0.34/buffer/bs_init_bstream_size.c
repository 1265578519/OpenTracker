#include "parse.h"

void bs_init_bstream_size(struct bytestream* bs,struct bytestream* other,size_t maxlen) {
  bs->type = BSTREAM;
  // check if we have enough capacity in the parent bytestream
  if (bs_capacityassert(other, maxlen)) {
    bs->cur = 0;
    bs->max = maxlen;
  } else {
    // nope, so set the new stream to error state right out of the box
    bs->cur = 1;
    bs->max = 0;
  }
  bs->u.bs=other;
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"
#include "buffer/bs_capacityassert.c"
#include "buffer/bs_init_membuf.c"

// we use membuf here, mock buffer stuff away
ssize_t buffer_getc(buffer* b,char* x) { return 0; }

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("\x12\x34\x56\x78",4);
  struct bytestream sub;
  assert(bs_get(&bs) == 0x12);		// eat one byte
  bs_init_bstream_size(&sub, &bs, 2);	// 2 byte substream
  assert(bs_get(&sub) == 0x34);		// make sure we got the offset right
  assert(bs_get(&sub) == 0x56);		// read second (and last) byte

  assert(bs_err(&sub) == 0);		// should have no errors yet
  assert(bs_err(&bs) == 0);

  assert(bs_get(&sub) == 0);		// next byte should error out
  assert(bs_err(&sub));			// substream should have error state
  assert(bs_err(&bs) == 0);		// parent stream not
  assert(bs_get(&bs) == 0x78);		// and we should not have overstepped

  // now check the range checking in bs_init_bstream_size itself
  bs_init_membuf(&bs, (const unsigned char*)"\x12\x34\x56\x78", 4);

  bs_init_bstream_size(&sub, &bs, 4);	// tight fit
  assert(bs_err(&sub) == 0);
  assert(bs_err(&bs) == 0);

  bs_init_bstream_size(&sub, &bs, 5);
  assert(bs_err(&sub));
  assert(bs_err(&bs));
}
#endif
