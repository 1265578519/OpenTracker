#ifdef __MINGW32__

#include <windows.h>
#include <mswsock.h>
#include <errno.h>
#include "io_internal.h"
#include "iob_internal.h"
#include <stdio.h>

int64 iob_send(int64 s,io_batch* b) {
  /* Windows has a sendfile called TransmitFile, which can send one
   * header and one trailer buffer. */
  iob_entry* x,* last;
  io_entry* e;
  int64 sent;
  int i;

  if (b->bytesleft==0) return 0;
  sent=-1;
  e=iarray_get(&io_fds,s);
  if (!e) { errno=EBADF; return -3; }
  if (!(x=array_get(&b->b,sizeof(iob_entry),b->next)))
    return -3;		/* can't happen error */
  last=(iob_entry*)(((char*)array_start(&b->b))+array_bytes(&b->b));

  fprintf(stderr,"iob_send() called!\n");
  if (e->canwrite || e->sendfilequeued==1) {
    fprintf(stderr,"...reaping finished WriteFile/TransmitFile.\n");
    /* An overlapping write finished.  Reap the result. */
    if (e->bytes_written==-1) return -3;
    if (e->bytes_written<x->n) {
      sent=e->bytes_written;
      if (x->n < e->bytes_written) {
	e->bytes_written-=x->n;
	x->n=0;
	++x;
      }
      x->n -= e->bytes_written;
      x->offset += e->bytes_written;
      b->bytesleft -= e->bytes_written;
    }
    e->canwrite=0; e->sendfilequeued=0;
  }

  for (i=0; x+i<last; ++i)
    if (x[i].n) break;

  if (x[i].type==FROMBUF) {
    fprintf(stderr,"found non-sent buffer batch entry at %d\n",i);
    if (x+i+1 < last &&
	(x[i+1].type==FROMFILE)) {
      fprintf(stderr,"Next is a file, can use TransmitFile\n",i);
      TRANSMIT_FILE_BUFFERS tfb;
      e->sendfilequeued=1;
      memset(&tfb,0,sizeof(tfb));
      memset(&e[i].os,0,sizeof(e[i].os));
      e[i].os.Offset=x[i].offset;
      e[i].os.OffsetHigh=(x[i].offset>>32);
      fprintf(stderr,"Calling TransmitFile on %p...",s);
      if (!TransmitFile(s,(HANDLE)x[i].fd,
		      x[i].n+tfb.HeadLength>0xffff?0xffff:x[i].n,
		      0,&e[i].os,&tfb,TF_USE_KERNEL_APC)) {
	if (GetLastError()==ERROR_IO_PENDING) {
	  fprintf(stderr," pending.!\n");
	  e->writequeued=1;
	  errno=EAGAIN;
	  e->errorcode=0;
	  return -1;
	} else {
	  fprintf(stderr," failed!\n");
	  e->errorcode=errno;
	  return -3;
	}
      }
      fprintf(stderr," OK!\n");
      return sent;
    } else {
      e->writequeued=1;
      fprintf(stderr,"Queueing WriteFile on %p...",s);
      if (!WriteFile(s,x[i].buf+x[i].offset,x[i].n,0,&e->ow)) {
	if (GetLastError()==ERROR_IO_PENDING) {
	  fprintf(stderr," pending.\n");
	  e->writequeued=1;
	  errno=EAGAIN;
	  e->errorcode=0;
	  return -1;
	} else {
	  fprintf(stderr," failed.\n");
	  e->errorcode=errno;
	  return -3;
	}
      }
      return sent;
    }
  } else {
    fprintf(stderr,"Calling TransmitFile...\n");
    e->sendfilequeued=1;
    memset(&e[i].os,0,sizeof(e[i].os));
    e[i].os.Offset=x[i].offset;
    e[i].os.OffsetHigh=(x[i].offset>>32);
    if (!TransmitFile(s,(HANDLE)x[i].fd,
		      x[i].n>0xffff?0xffff:x[i].n,
		      0,&e[i].os,0,TF_USE_KERNEL_APC))
      return -3;
    return sent;
  }
}

#else

#include "havebsdsf.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "havealloca.h"
#include "iob_internal.h"

int64 iob_send(int64 s,io_batch* b) {
  iob_entry* e,* last;
  struct iovec* v;
  uint64 total;
  int64 sent;
  long i;
  long headers;
#ifdef MSG_MORE
  int docork;
#endif
#ifdef HAVE_BSDSENDFILE
  long trailers;
#endif
#ifdef TCP_CORK
  int corked=0;
#endif

  if (b->bytesleft==0) return 0;
  last=(iob_entry*)(((char*)array_start(&b->b))+array_bytes(&b->b));
  v=alloca(b->bufs*sizeof(struct iovec));
  total=0;
  for (;;) {
    if (!(e=array_get(&b->b,sizeof(iob_entry),b->next)))
      return -3;		/* can't happen error */
#ifdef HAVE_BSDSENDFILE
    /* BSD sendfile can send headers and trailers.  If we run on BSD, we
    * should try to exploit this. */
    headers=trailers=0;
#endif
    for (i=0; e+i<last; ++i) {
      if (e[i].type==FROMFILE) break;
      v[i].iov_base=(char*)(e[i].buf+e[i].offset);
      v[i].iov_len=e[i].n;
    }
    headers=i;
#ifdef HAVE_BSDSENDFILE
    if (e[i].type==FROMFILE) {
      off_t sbytes;
      struct sf_hdtr hdr;
      int r;
      for (++i; e+i<last; ++i) {
	if (e[i].type==FROMFILE) break;
	v[i-1].iov_base=(char*)(e[i].buf+e[i].offset);
	v[i-1].iov_len=e[i].n;
	++trailers;
      }
      hdr.headers=v; hdr.hdr_cnt=headers;
      hdr.trailers=v+headers; hdr.trl_cnt=trailers;
      r=sendfile(e[headers].fd,s,e[headers].offset,e[headers].n,&hdr,&sbytes,0);
      if (r==0)
	sent=b->bytesleft;
      else if (r==-1 && errno==EAGAIN) {
	if (!(sent=sbytes)) {
	  sent=-1;
	  goto eagain;
	}
      } else
	sent=-3;
    } else {
      if (headers==1)	/* cosmetics for strace */
	sent=write(s,v[0].iov_base,v[0].iov_len);
      else
	sent=writev(s,v,headers);
      if (sent==-1) {
	if (errno!=EAGAIN)
	  sent=-3;
	else {
eagain:
	  io_eagain_write(s);
	  return -1;
	}
      }
    }
#else
    /* Linux has two ways to coalesce sent data; either setsockopt
     * TCP_CORK or sendto/sendmsg with MSG_MORE. MSG_MORE saves syscalls
     * in one scenario: when there is n buffers and then possibly one
     * file to send.  If there is more buffers after the file, then we
     * need to use TCP_CORK to prevent the TCP push after the file. */
#ifdef MSG_MORE
    if (e+i==last)
      docork=-1;	/* no files, only buffer, so no need for TCP_CORK or MSG_MORE */
    else
      docork=!(e+i+1==last);
    if (docork>0) {
      setsockopt(s,IPPROTO_TCP,TCP_CORK,(int[]){ 1 },sizeof(int));
      corked=1;
    }
    if (headers) {
      if (docork<0) {	/* write+writev */
	if (headers==1)	/* cosmetics for strace */
	  sent=write(s,v[0].iov_base,v[0].iov_len);
	else
	  sent=writev(s,v,headers);
      } else {
	if (headers==1)	/* cosmetics for strace */
	  sent=sendto(s,v[0].iov_base,v[0].iov_len,MSG_MORE, NULL, 0);
	else {
	  struct msghdr msg;
	  memset(&msg,0,sizeof(msg));
	  msg.msg_iov=v;
	  msg.msg_iovlen=headers;
	  sent=sendmsg(s,&msg,MSG_MORE);
	}
      }
      if (sent==-1) {
	if (errno==EAGAIN) {
	  io_eagain_write(s);
	  return -1;
	}
	sent=-3;
      }
    } else
      sent=io_sendfile(s,e->fd,e->offset,e->n);
#else	/* !MSG_MORE */
#ifdef TCP_CORK
    if (b->bufs && b->files && !b->next) {
      setsockopt(s,IPPROTO_TCP,TCP_CORK,(int[]){ 1 },sizeof(int));
      corked=1;
    }
#endif
    if (headers) {
      if (headers==1)	/* cosmetics for strace */
	sent=write(s,v[0].iov_base,v[0].iov_len);
      else
	sent=writev(s,v,headers);
      if (sent==-1) {
	if (errno==EAGAIN) {
	  io_eagain_write(s);
	  return -1;
	}
	sent=-3;
      }
    } else {
      sent=io_sendfile(s,e->fd,e->offset,e->n);
    }
#endif	/* !MSG_MORE */
#endif
    if (sent>0)
      total+=sent;
    else
      return total?total:(uint64)sent;
    if ((uint64)sent==b->bytesleft) {
      iob_reset(b);
      break;
    } else if (sent>0) {
      uint64 rest=sent;

      b->bytesleft-=rest;
      for (i=0; e+i<last; ++i) {
	if (e[i].n<=rest) {
	  rest-=e[i].n;
	  ++b->next;
	  if (!rest) break;
	} else {
	  e[i].offset+=rest;
	  e[i].n-=rest;
	  goto abort;
	}
      }
      io_eagain_write(s);
    } else break;
  }
abort:
#ifdef TCP_CORK
  if (corked)
    setsockopt(s,IPPROTO_TCP,TCP_CORK,(int[]){ 0 },sizeof(int));
#endif
  return total;
}

#endif
