#include "stralloc.h"
#include "buffer.h"

int main() {
  stralloc sa;
  buffer b;
  stralloc line;

  stralloc_init(&sa);
  stralloc_init(&line);
  stralloc_copys(&sa,"this is a test\nline 2\n");
  buffer_fromsa(&b,&sa);

  while (buffer_getline_sa(&b,&line)==0) {
    buffer_puts(buffer_1,"got line: \"");
    if (stralloc_chop(&line)!='\n') break;
    buffer_putsa(buffer_1,&line);
    buffer_putsflush(buffer_1,"\"\n");
    stralloc_copys(&line,"");
  }
  return 0;
}
