#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "byte.h"
#include "cdb.h"
#include "havepread.h"
#ifdef __MINGW32__
#include "windows.h"
#else
#include <sys/mman.h>
#endif

void cdb_free(struct cdb *c) {
  if (c->map) {
#ifdef __MINGW32__
    UnmapViewOfFile(c->map);
#else
    munmap(c->map,c->size);
#endif
    c->map = 0;
  }
}

void cdb_findstart(struct cdb *c) {
  c->loop = 0;
}

void cdb_init(struct cdb *c,int64 fd) {
#ifndef __MINGW32__
  struct stat st;
  char *x;
#endif

  cdb_free(c);
  cdb_findstart(c);
  c->fd = fd;

#ifdef __MINGW32__
  {
    HANDLE m=CreateFileMapping((HANDLE)(uintptr_t)fd,0,PAGE_READONLY,0,0,NULL);
    if (m)
      if ((c->map=MapViewOfFile(m,FILE_MAP_READ,0,0,0)))
	c->size=GetFileSize((HANDLE)(uintptr_t)fd,NULL);
    CloseHandle(m);
  }
#else
  if (fstat(fd,&st) == 0)
    if (st.st_size <= 0xffffffff) {
      x = mmap(0,st.st_size,PROT_READ,MAP_SHARED,fd,0);
      if (x != MAP_FAILED) {
	c->size = st.st_size;
	c->map = x;
      }
    }
#endif
}

int cdb_read(struct cdb *c,unsigned char *buf,unsigned long len,uint32 pos) {
  if (c->map) {
    if ((pos > c->size) || (c->size - pos < len)) goto FORMAT;
    byte_copy(buf,len,c->map + pos);
  }
  else {
#ifndef HAVE_PREAD
    if (lseek(c->fd,pos,SEEK_SET) == -1) return -1;
#endif
    while (len > 0) {
      ssize_t r;
      do
#ifdef HAVE_PREAD
	r = pread(c->fd,buf,len,pos);
#else
        r = read(c->fd,buf,len);
#endif
      while ((r == -1) && (errno == EINTR));
      if (r == -1) return -1;
      if (r == 0) goto FORMAT;
      buf += r;
      len -= r;
#ifdef HAVE_PREAD
      pos += r;
#endif
    }
  }
  return 0;

  FORMAT:
#ifdef EPROTO
  errno = EPROTO;
#else
  errno = EINVAL;
#endif
  return -1;
}

static int match(struct cdb *c,const unsigned char *key,unsigned long int len,uint32 pos) {
  unsigned char buf[32];
  unsigned long n;

  while (len > 0) {
    n = sizeof buf;
    if (n > len) n = len;
    if (cdb_read(c,buf,n,pos) == -1) return -1;
    if (byte_diff(buf,n,key)) return 0;
    pos += n;
    key += n;
    len -= n;
  }
  return 1;
}

int cdb_findnext(struct cdb *c,const unsigned char *key,unsigned long int len) {
  unsigned char buf[8];
  uint32 pos;
  uint32 u;

  if (!c->loop) {
    u = cdb_hash(key,len);
    if (cdb_read(c,buf,8,(u << 3) & 2047) == -1) return -1;
    uint32_unpack((char*)buf + 4,&c->hslots);
    if (!c->hslots) return 0;
    uint32_unpack((char*)buf,&c->hpos);
    c->khash = u;
    u >>= 8;
    u %= c->hslots;
    u <<= 3;
    c->kpos = c->hpos + u;
  }

  while (c->loop < c->hslots) {
    if (cdb_read(c,buf,8,c->kpos) == -1) return -1;
    uint32_unpack((char*)buf + 4,&pos);
    if (!pos) return 0;
    c->loop += 1;
    c->kpos += 8;
    if (c->kpos == c->hpos + (c->hslots << 3)) c->kpos = c->hpos;
    uint32_unpack((char*)buf,&u);
    if (u == c->khash) {
      if (cdb_read(c,buf,8,pos) == -1) return -1;
      uint32_unpack((char*)buf,&u);
      if (u == len)
	switch(match(c,key,len,pos + 8)) {
	  case -1:
	    return -1;
	  case 1:
	    uint32_unpack((char*)buf + 4,&c->dlen);
	    c->dpos = pos + 8 + len;
	    return 1;
	}
    }
  }

  return 0;
}

int cdb_find(struct cdb *c,const unsigned char *key,unsigned long int len) {
  cdb_findstart(c);
  return cdb_findnext(c,key,len);
}
