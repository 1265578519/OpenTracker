#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#include "open.h"
#endif
#include "mmap.h"

extern char* mmap_shared(const char* filename,size_t * filesize) {
#ifdef _WIN32
  HANDLE fd,m;
  char* map;
  fd=CreateFile(filename,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if (fd==INVALID_HANDLE_VALUE)
    return 0;
  m=CreateFileMapping(fd,0,PAGE_READWRITE,0,0,NULL);
  map=0;
  if (m)
    if ((map=MapViewOfFile(m,FILE_MAP_WRITE,0,0,0)))
      *filesize=GetFileSize(fd,NULL);
  CloseHandle(m);
  CloseHandle(fd);
  return map;
#else
  int fd=open_rw(filename);
  char *map;
  if (fd>=0) {
    register off_t o=lseek(fd,0,SEEK_END);
    if (o==0 || (sizeof(off_t)!=sizeof(size_t) && o > (off_t)(size_t)-1)) { close(fd); return 0; }
    *filesize=(size_t)o;
    if (o) {
      map=mmap(0,*filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
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
