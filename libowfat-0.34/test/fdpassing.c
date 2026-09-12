#include <unistd.h>
#include "io.h"
#include "buffer.h"
#include <stdlib.h>

void child(int64 fd) {
  int64 x=io_receivefd(fd);
  char buf[8192];
  int i;
  if (x==-1) {
    buffer_putsflush(buffer_2,"fd passing failed!\n");
    exit(1);
  }
  i=read(x,buf,sizeof(buf));
  write(1,buf,i);
  io_close(x);
}

void father(int64 fd) {
  int64 x;
  if (io_readfile(&x,"/etc/resolv.conf"))
    io_passfd(fd,x);
}

int main() {
  int64 sp[2];
  if (io_socketpair(sp)==-1) return 1;
  switch (fork()) {
  case -1: return 1;
  case 0:	/* child */
    io_close(sp[0]);
    child(sp[1]);
    break;
  default:
    io_close(sp[1]);
    father(sp[0]);
    break;
  }
  return 0;
}
