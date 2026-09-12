#include "parse.h"

static const size_t max_ssize_t = (((size_t)1) << (sizeof(size_t)*8-1))-1;

/* Read an asciiz string from the byte stream, up to len bytes (including the 0 terminator). */
/* Return number of bytes consumed (excluding the 0 terminator), i.e. strlen(dest) */
/* If there is no 0 byte in these len bytes, set error flag in stream and return -1. */
/* Calling this function with len==0 is an error. */
/* destsize will be clamped to the maximum number representable in ssize_t */
ssize_t prs_asciiz(struct bytestream* bs, char* dest, size_t len) {
  size_t i;
  /* The maximum value of ssize_t is half that of size_t.
   * So we arbitrarily decide to limit len to it here. */
  if (len>max_ssize_t) len=max_ssize_t;
  if (len==0) {
    bs->cur = 1;	// mark bytestream state as erroneous
    bs->max = 0;
    return -1;
  }

  for (i=0; i+1<len; ++i)
    if ((dest[i] = bs_get(bs)) == 0) {
      /* we might get here either because there actually was a 0 byte in
       * the stream, or because we hit EOF. So check if we hit EOF here
       * and return -1 then. */
      return (bs->cur<=bs->max) ? (ssize_t)i : -1;
    }
  /* if we get here, we read len-1 bytes and there was no 0 byte. */
  if ((dest[i] = bs_peek(bs))) {	// the loop went till i+1<len and we checked that len!=0
    // the next byte was not 0, so signal error
    bs->cur = 1;
    bs->max = 0;
    // but still write 0 terminator to dest
    dest[i] = 0;
    return -1;
  } else {
    bs_get(bs);	// the next byte was 0, so consume it
    return i;
  }
}

#ifdef UNITTEST
#include <assert.h>

#undef UNITTEST
#include "buffer/bs_init_membuf.c"
#include "buffer/bs_get.c"
#include "buffer/bs_err.c"
#include "buffer/bs_peek.c"

// we use membuf here, mock buffer stuff away
ssize_t buffer_getc(buffer* b,char* x) { return 0; }
ssize_t buffer_peekc(buffer* b,char* x) { return 0; }

int main() {
  struct bytestream bs = BS_FROM_MEMBUF("fnord\n\0x",8);
  char buf[100];
  assert(prs_asciiz(&bs, buf, sizeof buf) == 6);	// return value should be strlen("fnord\n")
  assert(!memcmp(buf,"fnord\n",7));	// returned string should be "fnord\n" with 0 terminator
  assert(bs_get(&bs) == 'x');		// should have consumed the 0 terminator from bytestream

  bs_init_membuf(&bs, (const unsigned char*)"fnord\n\0x", 8);
  assert(prs_asciiz(&bs, buf, 5) == -1);	// no 0 terminator in first 5 bytes, expect error
  assert(!memcmp(buf,"fnor",5));		// expect 4 bytes + 0 terminator in dest buf
  assert(bs_err(&bs));			// bytestream should be in error state now
}
#endif
