#define _POSIX_C_SOURCE 200809
#define _ATFILE_SOURCE

#include <sys/types.h>
#include <unistd.h>
#ifndef __MINGW32__
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif
#include "open.h"
#include "mmap.h"

extern const char* mmap_readat(const char* filename,size_t * filesize,int dirfd) {
#ifdef __MINGW32__
  return 0;
#else
  int fd=openat(dirfd,filename,O_RDONLY);
  char *map;
  if (fd>=0) {
    register off_t o=lseek(fd,0,SEEK_END);
    if (o==0 || (sizeof(off_t)!=sizeof(size_t) && o > (off_t)(size_t)-1)) { close(fd); return 0; }
    *filesize=(size_t)o;
    if (o>0) {
      map=mmap(0,*filesize,PROT_READ,MAP_SHARED,fd,0);
      if (map==(char*)-1)
	map=0;
    } else
      map="";
    close(fd);
    return map;
  }
  return 0;
#endif
}
