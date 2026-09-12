#include <assert.h>
#include "array.h"
#include "byte.h"

int main() {
  static array x,y;
  array_cats(&x,"fnord");
  array_cats(&y,"foobar");
  array_cat(&x,&y);
  array_fail(&y);
  array_cat(&y,&x);
  assert(array_failed(&y));
  array_reset(&y);
  array_cats(&y,"fnord");
  assert(byte_equal(x.p,11,"fnordfoobar"));
  array_cate(&x,&y,1,4);
  assert(x.initialized=14 && byte_equal(x.p,14,"fnordfoobarnor"));
  return 0;
}
