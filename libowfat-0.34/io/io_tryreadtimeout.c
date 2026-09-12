#include <errno.h>
#include "io_internal.h"

int64 io_tryreadtimeout(int64 d,char* buf,int64 len) {
  int64 r=io_tryread(d,buf,len);
  if (r==-1) {
    tai6464 x;
    io_entry* e=iarray_get(&io_fds,d);
    taia_now(&x);
    if (!taia_less(&x,&e->timeout)) {
      errno=ETIMEDOUT;
      r=-2;
    }
  }
  return r;
}
