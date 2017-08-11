#include "io.h"
#include "buffer.h"
#include <unistd.h>

int main() {
  char buf[2048];
  int64 pfd[2];
  tai6464 t;
  int64 r;

  if (!io_pipe(pfd)) {
    buffer_puts(buffer_2,"io_pipe: ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  taia_now(&t);
  taia_addsec(&t,&t,1);
  if (!io_fd(pfd[0])) return 111;
  io_timeout(pfd[0],t);
  sleep(2);
  if ((r=io_tryreadtimeout(pfd[0],buf,sizeof buf))!=-2) {
    buffer_puts(buffer_2,"io_tryreadtimeout returned ");
    buffer_putlonglong(buffer_2,r);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  return 0;
}
