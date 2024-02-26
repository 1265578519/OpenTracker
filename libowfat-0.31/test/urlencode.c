#include <string.h>
#include "buffer.h"
#include "textcode.h"
#include "havealloca.h"
#include <unistd.h>

void urlencode(const char* c) {
  char* buf=alloca(strlen(c)*3+1);
  buffer_put(buffer_1,buf,fmt_urlencoded(buf,c,strlen(c)));
  buffer_putnlflush(buffer_1);
}

int main(int argc,char* argv[]) {
  int i;
  for (i=1; i<argc; ++i) {
    urlencode(argv[i]);
  }
  if (argc<2) {
    char src[1024];
    int len=read(0,src,sizeof(src)-1);
    if (len==-1) return(1);
    src[len]=0;
    urlencode(src);
  }
  return 0;
}
