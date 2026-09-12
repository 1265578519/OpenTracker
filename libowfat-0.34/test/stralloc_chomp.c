#include <assert.h>
#include "stralloc.h"
#include "buffer.h"

int main() {
  static stralloc sa;
  stralloc_copys(&sa,"foo\r\n");
  assert(sa.len==5);
  assert(stralloc_chomp(&sa)==2);
  assert(stralloc_chop(&sa)=='o');
  assert(stralloc_chop(&sa)=='o');
  assert(stralloc_chop(&sa)=='f');
  assert(stralloc_chop(&sa)==-1);
  return 0;
}
