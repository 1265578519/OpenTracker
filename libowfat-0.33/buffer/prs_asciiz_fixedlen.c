#include "parse.h"

static const size_t max_ssize_t = (((size_t)1) << (sizeof(size_t)*8-1))-1;

/* Some protocols have a fixed field length for a string,
 * If the string is shorter than the field, the rest is filled with 0
 * bytes. But it is not an error if there are no 0 bytes.
 * This function is for those cases (the filename field in the tar file
 * header is an example of this).
 * For a field of length 8, you need to pass destsize as 9 so we can add
 * a 0 terminator. This function will consume the 8 bytes and add a 0 byte.
 * The return value is strlen(dest). */
ssize_t prs_asciiz_fixedlen(struct bytestream* bs, char* dest, size_t len) {
  size_t i;
  ssize_t r=0;
  /* The maximum value of ssize_t is half that of size_t.
   * So we arbitrarily decide to limit len to it here. */
  if (len>max_ssize_t) len=max_ssize_t;
  if (len==0) {
    bs->cur = 1;	// mark bytestream state as erroneous
    bs->max = 0;
    return -1;
  }

  for (i=0; i+1<len; ++i)
    if ((dest[i] = bs_get(bs)) == 0 && r==0)
      r=i;	// note the position of the first 0 byte
  dest[i] = 0;	// add 0 terminator
  if (dest[r]) r=i;
  return bs_err(bs) ? -1 : r;
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
  assert(prs_asciiz_fixedlen(&bs, buf, 8) == 6);	// return value should be strlen("fnord\n")
  assert(!memcmp(buf,"fnord\n\0\0",8));	// returned string should be "fnord\n" and the rest filled with 0 bytes
  assert(!bs_err(&bs));

  bs_init_membuf(&bs, (const unsigned char*)"fnord\n\0x", 8);
  assert(prs_asciiz_fixedlen(&bs, buf, 5) == 4);	// no 0 terminator in first 4 bytes
  assert(!memcmp(buf,"fnor",5));	// expect 4 bytes + 0 terminator in dest buf
  assert(bs_get(&bs) == 'd');		// 0 terminator in buf was artificial, 'd' was not consumed
  assert(!bs_err(&bs));			// bytestream should be ok
}
#endif
