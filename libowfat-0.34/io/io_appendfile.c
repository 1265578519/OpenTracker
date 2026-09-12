#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <fcntl.h>
#include "io_internal.h"

int io_appendfile(int64* d,const char* s) {
  long fd=open(s,O_WRONLY|O_APPEND|O_CREAT,0600);
  if (fd != -1) {
    *d=fd;
    return 1;
  }
  return 0;
}
