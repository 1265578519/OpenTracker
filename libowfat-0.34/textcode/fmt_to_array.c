#include "array.h"
#include "textcode.h"

void fmt_to_array(size_t (*func)(char*,const char*,size_t),
		  array* a,const char* src,size_t len) {
  size_t needed=func(0,src,len);
  if (array_bytes(a)+needed>=needed &&
      array_allocate(a,1,array_bytes(a)+needed-1)) {
    char* x=((char*)array_start(a))+array_bytes(a)-needed;
    func(x,src,len);
  } else
    array_fail(a);
}
