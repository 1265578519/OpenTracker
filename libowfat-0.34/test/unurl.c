#include <string.h>
#include "buffer.h"
#include "textcode.h"
#include "havealloca.h"
#include <unistd.h>
#include <stdio.h>

void unurl(const char* s) {
  char* buf;
  unsigned long len;
  buf=alloca(strlen(s)+1);
  if (s[scan_urlencoded(s,buf,&len)]) {
    buffer_putsflush(buffer_2,"parse error!\n");
    return;
  }
  buf[len]=0;
  if (strcmp(s,buf)) {
    buffer_puts(buffer_1,"renaming \"");
    buffer_puts(buffer_1,s);
    buffer_puts(buffer_1,"\" to \"");
    buffer_puts(buffer_1,buf);
    buffer_putsflush(buffer_1,"\"...");
    if (rename(s,buf)==-1)
      buffer_putsflush(buffer_1," failed!\n");
    else
      buffer_putsflush(buffer_1,"\n");
  }
}

int main(int argc,char* argv[]) {
  int i;
  for (i=1; i<argc; ++i)
    unurl(argv[i]);
  return 0;
}
