/* http://delegate.uec.ac.jp:8081/club/mma/~shimiz98/misc/sendfile.html */
#define _FILE_OFFSET_BITS 64
#include "io_internal.h"
#include "havebsdsf.h"
#include "havesendfile.h"
#include <errno.h>

#if defined(HAVE_BSDSENDFILE)
#define SENDFILE 1
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

int64 io_sendfile(int64 s,int64 fd,uint64 off,uint64 n) {
  off_t sbytes;
  int r=sendfile(fd,s,off,n,0,&sbytes,0);
  if (r==-1) {
    io_eagain_write(s);
    return (errno==EAGAIN?(sbytes?sbytes:-1):-3);
  }
  if (sbytes<n)
    io_eagain_write(s);
  return n;
}

#elif defined(HAVE_SENDFILE)

#ifdef __hpux__

#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>

int64 io_sendfile(int64 out,int64 in,uint64 off,uint64 bytes) {
  long long r=sendfile64(out,in,off,bytes,0,0);
  if (r==-1 && errno!=EAGAIN) r=-3;
  if (r!=bytes)
    io_eagain_write(out);
  }
  return r;
}

#elif defined (__sun__) && defined(__svr4__)

#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

int64 io_sendfile(int64 out,int64 in,uint64 off,uint64 bytes) {
  off64_t o=off;
  long long r=sendfile64(out,in,&o,bytes);
  if (r==-1 && errno!=EAGAIN) r=-3;
  if (r!=bytes) {
    io_eagain_write(out);
  }
  return r;
}

#elif defined(_AIX)

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

int64 io_sendfile(int64 out,int64 in,uint64 off,uint64 bytes) {
  struct sf_parms p;
  int destfd=out;
  p.header_data=0;
  p.header_length=0;
  p.file_descriptor=in;
  p.file_offset=off;
  p.file_bytes=bytes;
  p.trailer_data=0;
  p.trailer_length=0;
  if (send_file(&destfd,&p,0)>=0) {
    if (p.bytes_sent != bytes) {
      io_eagain_write(out);
    }
    return p.bytes_sent;
  } if (errno==EAGAIN)
    return -1;
  else
    return -3;
}

#elif defined(__linux__)

#if defined(__GLIBC__)
#include <sys/sendfile.h>
#elif defined(__dietlibc__)
#include <sys/sendfile.h>
#else
#include <linux/unistd.h>
_syscall4(int,sendfile,int,out,int,in,long *,offset,unsigned long,count)
#endif

int64 io_sendfile(int64 s,int64 fd,uint64 off,uint64 n) {
  off_t o=off;
  off_t i;
  uint64 done=0;
  /* What a spectacularly broken design for sendfile64.
   * The offset is 64-bit for sendfile64, but the count is not. */
  while (n) {
    off_t todo=n>0x7fffffff?0x7fffffff:n;
    i=sendfile(s,fd,&o,todo);
    if (i==todo) {
      done+=todo;
      n-=todo;
      if (n==0) return done;
      continue;
    } else {
      io_eagain_write(s);
      if (i==-1)
	return (errno==EAGAIN?-1:-3);
      else
	return done+i;
    }
  }
  return 0;
}
#endif

#elif defined(__MINGW32__)

#include <windows.h>
#include <mswsock.h>

int64 io_sendfile(int64 out,int64 in,uint64 off,uint64 bytes) {
  io_entry* e=iarray_get(&io_fds,out);
  if (!e) { errno=EBADF; return -3; }
  if (e->sendfilequeued==1) {
    /* we called TransmitFile, and it returned. */
    e->sendfilequeued=2;
    errno=e->errorcode;
    if (e->bytes_written==-1) return -1;
    if (e->bytes_written!=bytes) {	/* we wrote less than caller wanted to write */
      e->sendfilequeued=1;	/* so queue next request */
      off+=e->bytes_written;
      bytes-=e->bytes_written;
      e->os.Offset=off;
      e->os.OffsetHigh=(off>>32);
      TransmitFile(out,(HANDLE)in,bytes>0xffff?0xffff:bytes,0,&e->os,0,TF_USE_KERNEL_APC);
    }
    return e->bytes_written;
  } else {
    e->sendfilequeued=1;
    e->os.Offset=off;
    e->os.OffsetHigh=(off>>32);
    /* we always write at most 64k, so timeout handling is possible */
    if (!TransmitFile(out,(HANDLE)in,bytes>0xffff?0xffff:bytes,0,&e->os,0,TF_USE_KERNEL_APC))
      return -3;
  }
}

#else

#include <iob.h>
#include <unistd.h>

static int64 writecb(int64 s,const void* buf,uint64 n) {
  return write(s,buf,n);
}

int64 io_sendfile(int64 out,int64 in,uint64 off,uint64 bytes) {
  return io_mmapwritefile(out,in,off,bytes,writecb);
}

#endif
