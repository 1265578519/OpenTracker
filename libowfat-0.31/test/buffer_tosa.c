#include "byte.h"
#include "stralloc.h"
#include "buffer.h"
#include <assert.h>

int main() {
  stralloc a;
  buffer b;
  int i;
  stralloc_init(&a);
  buffer_tosa(&b,&a);

  for (i=0; i<100; ++i)
    buffer_puts(&b,"foo bar baz!\n");
  buffer_flush(&b);
  assert(a.len==100*sizeof("foo bar baz!"));
  for (i=0; i<100; ++i)
    assert(byte_equal(a.s+i*sizeof("foo bar baz!"),sizeof("foo bar baz!"),"foo bar baz!\n"));
  return 0;
}
