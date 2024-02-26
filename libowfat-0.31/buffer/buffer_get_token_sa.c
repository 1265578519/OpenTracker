#include "byte.h"
#include "stralloc.h"
#include "buffer.h"
#include <errno.h>

int buffer_get_token_sa(buffer* b,stralloc* sa,
			const char* charset,
			size_t setlen) {
  for (;;) {
    char x;
    if (!stralloc_readyplus(sa,1)) goto nomem;
    switch  (buffer_getc(b,&x)) {
    case -1: return -1;
    case 0: return 0;
    }
    stralloc_append(sa,&x);
    if (byte_chr(charset,setlen,x)<setlen) break;
  }
  return 1;
nomem:
  errno=ENOMEM;
  return -1;
}
