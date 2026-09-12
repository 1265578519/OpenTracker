#include <unistd.h>
#include "buffer.h"
#include "io.h"

int main() {
  int64 pfd[2];
  char buf[20480];
  unsigned int i;
  if (!io_pipe(pfd)) return 111;
  io_nonblock(pfd[1]);
  if (!io_fd(pfd[1])) return 111;
  switch (fork()) {
  case -1: return 111;
  case 0: /* child */
	   io_close(pfd[1]);
	   sleep(1);
	   for (;;) {
	    switch (io_waitread(pfd[0],buf,sizeof buf)) {
	    case -1: buffer_putsflush(buffer_2,"io_waitread returned -1!\n"); return 111;
	    case -3: buffer_puts(buffer_2,"io_waitread: ");
		      buffer_puterror(buffer_2);
		      buffer_putnlflush(buffer_2);
		      return 111;
	    case 0: io_close(pfd[0]);
		    return 0;
	    }
	   }
  }
  io_close(pfd[0]);
  for (i=0; i<sizeof(buf); ++i) buf[i]="abcdefghihjklmnopqrstuvwxyz"[i%26];
  for (i=0; i<1000; ++i) {
    int64 r;
    if ((r=io_waitwrite(pfd[1],buf,sizeof buf))!=sizeof buf) {
      buffer_puts(buffer_2,"io_waitwrite returned ");
      buffer_putlonglong(buffer_2,r);
      buffer_putnlflush(buffer_2);
    }
  }
  return 0;
}
