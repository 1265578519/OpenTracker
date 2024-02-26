/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IO_H
#define IO_H

/* http://cr.yp.to/lib/io.html */

#include "uint64.h"
#include "taia.h"

#ifdef __cplusplus
extern "C" {
#endif

/* like open(s,O_RDONLY) */
/* return 1 if ok, 0 on error */
int io_readfile(int64* d,const char* s);
/* like open(s,O_WRONLY|O_CREAT|O_TRUNC,0600) */
/* return 1 if ok, 0 on error */
int io_createfile(int64* d,const char* s);
/* like open(s,O_RDWR) */
/* return 1 if ok, 0 on error */
int io_readwritefile(int64* d,const char* s);
/* like open(s,O_WRONLY|O_APPEND|O_CREAT,0600) */
/* return 1 if ok, 0 on error */
int io_appendfile(int64* d,const char* s);
/* like pipe(d) */
/* return 1 if ok, 0 on error */
int io_pipe(int64* d);
/* like socketpair() */
/* return 1 if ok, 0 on error */
int io_socketpair(int64* d);

/* non-blocking read(), -1 for EAGAIN and -3+errno for other errors */
int64 io_tryread(int64 d,char* buf,int64 len);

/* blocking read(), with -3 instead of -1 for errors */
int64 io_waitread(int64 d,char* buf,int64 len);

/* non-blocking write(), -1 for EAGAIN and -3+errno for other errors */
int64 io_trywrite(int64 d,const char* buf,int64 len);

/* blocking write(), with -3 instead of -1 for errors */
int64 io_waitwrite(int64 d,const char* buf,int64 len);

/* modify timeout attribute of file descriptor */
void io_timeout(int64 d,tai6464 t);

/* like io_tryread but will return -2,errno=ETIMEDOUT if d has a timeout
 * associated and it is passed without input being there */
int64 io_tryreadtimeout(int64 d,char* buf,int64 len);

/* like io_trywrite but will return -2,errno=ETIMEDOUT if d has a timeout
 * associated and it is passed without being able to write */
int64 io_trywritetimeout(int64 d,const char* buf,int64 len);

void io_wantread(int64 d);
void io_wantwrite(int64 d);
void io_dontwantread(int64 d);
void io_dontwantwrite(int64 d);

void io_wait();
void io_waituntil(tai6464 t);
int64 io_waituntil2(int64 milliseconds);
void io_check();

/* signal that read/accept/whatever returned EAGAIN */
/* needed for SIGIO and epoll */
void io_eagain(int64 d);  /* do not use, API was a bad idea */
#define HAVE_EAGAIN_READWRITE
void io_eagain_read(int64 d);	/* use these ones */
void io_eagain_write(int64 d);

/* return next descriptor from io_wait that can be read from */
int64 io_canread();
/* return next descriptor from io_wait that can be written to */
int64 io_canwrite();

/* return next descriptor with expired timeout */
int64 io_timeouted();

/* is this fd over its timeout? */
int io_timedout(int64 d);

/* 1 means: have IO_FD_CANWRITE, IO_FD_BLOCK and IO_FD_NONBLOCK,
 * will be incremented if API is extended in the future */
#define HAVE_IO_FD_FLAGS 1
enum io_fd_flags {
  IO_FD_CANWRITE=1,	/* new TCP connection, we know it's writable */
  IO_FD_BLOCK=2,	/* skip the fcntl, assume fd is set to blocking */
  IO_FD_NONBLOCK=4,	/* skip the fcntl, assume fd is set to non-blocking */
};

/* put d on internal data structure, return 1 on success, 0 on error */
int io_fd(int64 d);		/* use this for sockets before you called connect() or accept() */
int io_fd_canwrite(int64 d);	/* use this for connected sockets (assumes socket is writable) */
int io_fd_flags(int64 d,int flags);	/* can be used to tell io_fd to skip one syscall */

void io_setcookie(int64 d,void* cookie);
void* io_getcookie(int64 d);

/* put descriptor in non-blocking mode */
void io_nonblock(int64 d);
/* put descriptor in blocking mode */
void io_block(int64 d);
/* put descriptor in close-on-exec mode */
void io_closeonexec(int64 d);

void io_close(int64 d);

/* Free the internal data structures from libio.
 * This only makes sense if you run your program in a malloc checker and
 * these produce false alarms.  Your OS will free these automatically on
 * process termination. */
void io_finishandshutdown(void);

/* send n bytes from file fd starting at offset off to socket s */
/* return number of bytes written */
int64 io_sendfile(int64 s,int64 fd,uint64 off,uint64 n);

/* Pass fd over sock (must be a unix domain socket) to other process.
 * Return 0 if ok, -1 on error, setting errno. */
int io_passfd(int64 sock,int64 fd);

/* Receive fd over sock (must be a unix domain socket) from other
 * process.  Return sock if ok, -1 on error, setting errno. */
int64 io_receivefd(int64 sock);

int io_starteventloopthread(unsigned int threads);

#define HAVE_IO_QUEUEFORREAD
/* Artificially queue a file descriptor as readable.
 * The next call to io_canread will return this descriptor. */
int io_queueforread(int64 d);
/* Artificially queue a file descriptor as writable.
 * The next call to io_canread will return this descriptor. */
int io_queueforwrite(int64 d);

typedef int64 (*io_write_callback)(int64 s,const void* buf,uint64 n);

/* used internally, but hey, who knows */
int64 io_mmapwritefile(int64 out,int64 in,uint64 off,uint64 bytes,io_write_callback writecb);

/* only needed for debugging, will print some stats into the buffer to
 * aid in debugging the state machine if a descriptor loops or so */
unsigned int io_debugstring(int64 s,char* buf,unsigned int bufsize);

#ifdef __MINGW32__
#include_next <io.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
