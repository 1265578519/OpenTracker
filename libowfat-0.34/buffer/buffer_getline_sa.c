#include "stralloc.h"
#include "buffer.h"

int buffer_getline_sa(buffer* b,stralloc* sa) {
  return buffer_get_token_sa(b,sa,"\n",1);
}
