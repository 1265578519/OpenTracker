#include "array.h"

void array_cate(array* to,const array* const from,int64 pos,int64 stop) {
  if (pos<0 || stop<pos) {
    array_fail(to);
    return;
  }
  array_catb(to,from->p+pos,stop-pos);
}
