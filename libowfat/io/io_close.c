#include <unistd.h>
#include <sys/types.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include "io_internal.h"

extern void io_dontwantread_really(int64 d,io_entry* e);
extern void io_dontwantwrite_really(int64 d,io_entry* e);

void io_close(int64 d) {
  io_entry* e;
  if ((e=iarray_get(&io_fds,d))) {
    e->inuse=0;
    e->cookie=0;
    if (e->kernelwantread) io_dontwantread_really(d,e);
    if (e->kernelwantwrite) io_dontwantwrite_really(d,e);
    if (e->mmapped) {
#ifdef __MINGW32__
      UnmapViewOfFile(e->mmapped);
      CloseHandle(e->mh);
#else
      munmap(e->mmapped,e->maplen);
#endif
      e->mmapped=0;
    }
  }
  close(d);
}
