#include "str.h"
#include "buffer.h"

int buffer_putsalign(buffer* b,const char* x) {
  return buffer_putalign(b,x,str_len(x));
}
