#include "fmt.h"
#include "str.h"
#include <assert.h>

int main() {
  char buf[1024];

  assert(fmt_long(0,12345)==5);
  assert(fmt_long(0,-12345)==6);
  assert(fmt_long(buf,12345)==5); buf[5]=0;
  assert(str_equal(buf,"12345"));
  assert(fmt_long(buf,-12345)==6); buf[6]=0;
  assert(str_equal(buf,"-12345"));
  return 0;
}
