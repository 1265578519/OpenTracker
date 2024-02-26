#include "stralloc.h"
#include "buffer.h"

int buffer_get_new_token_sa(buffer* b,stralloc* sa,const char* charset,size_t setlen) {
  stralloc_zero(sa);
  return buffer_get_token_sa(b,sa,charset,setlen);
}
