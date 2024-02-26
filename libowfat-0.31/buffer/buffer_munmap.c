#include "buffer.h"
#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/mman.h>
#endif

void buffer_munmap(void* buf) {
  buffer* b=(buffer*)buf;
#ifdef __MINGW32__
  UnmapViewOfFile(b->x);
#else
  munmap(b->x,b->a);
#endif
}
