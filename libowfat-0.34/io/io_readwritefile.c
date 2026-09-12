#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <fcntl.h>
#include "io_internal.h"

int io_readwritefile(int64* d,const char* s) {
  long fd=open(s,O_RDWR);
  if (fd != -1) {	// gcc -fanalyze false positive
    *d=fd;	// no leak, we return the fd in *d
    return 1;
  }
  return 0;
}
