#include <unistd.h>
#include <sys/types.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include <stdio.h>
#include "io_internal.h"

extern void io_dontwantread_really(int64 d,io_entry* e);
extern void io_dontwantwrite_really(int64 d,io_entry* e);

long first_deferred=-1;

void io_close(int64 d) {
  io_entry* e;
  if ((e=iarray_get(&io_fds,d))) {
    e->inuse=0;
    e->cookie=0;
    if (e->kernelwantread) { io_dontwantread_really(d,e); e->kernelwantread=0; }
    if (e->kernelwantwrite) { io_dontwantwrite_really(d,e); e->kernelwantwrite=0; }
    if (e->mmapped) {
#ifdef __MINGW32__
      UnmapViewOfFile(e->mmapped);
      CloseHandle(e->mh);
#else
      munmap(e->mmapped,e->maplen);
#endif
      e->mmapped=0;
    }
#if 1
    if (e->next_read!=-1 || e->next_write!=-1) {
      /* There are still outstanding events. If we close the fd, between
       * now and when those events are handled, another accept() or
       * open() could return this descriptor, and cause bad things to
       * happen for everybody.
       * So we don't actually close the fd now, but we will mark it as
       * closed. */
//      fprintf(stderr,"io_close(%d) DEFERRED!\n",d);
      if (e->closed==0) {
	e->closed=1;
	e->next_defer=first_deferred;
	first_deferred=d;
      }
      return;
    } else
      e->closed=0;
#endif
  }
//  fprintf(stderr,"io_close(%d)\n",d);
  close(d);
}
