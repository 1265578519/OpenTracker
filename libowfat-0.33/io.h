/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IO_H
#define IO_H

#ifdef __MINGW32__
#include_next <io.h>
#endif

/* http://cr.yp.to/lib/io.html */

#include <libowfat/uint64.h>
#include <libowfat/taia.h>
#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* like open(s,O_RDONLY) */
/* return 1 if ok, 0 on error */
/* sandboxing: open(2) (glibc: openat(2)) */
att_read(2)
att_write(1)
int io_readfile(int64* d,const char* filename);

/* like open(s,O_WRONLY|O_CREAT|O_TRUNC,0600) */
/* return 1 if ok, 0 on error */
/* sandboxing: open(2) (glibc: openat(2)) */
att_read(2)
att_write(1)
int io_createfile(int64* d,const char* filename);

/* like open(s,O_RDWR) */
/* return 1 if ok, 0 on error */
/* sandboxing: open(2) (glibc: openat(2)) */
att_read(2)
att_write(1)
int io_readwritefile(int64* d,const char* filename);

/* like open(s,O_WRONLY|O_APPEND|O_CREAT,0600) */
/* return 1 if ok, 0 on error */
/* sandboxing: open(2) (glibc: openat(2)) */
att_read(2)
att_write(1)
int io_appendfile(int64* d,const char* filename);

/* like pipe(d) */
/* return 1 if ok, 0 on error */
/* sandboxing: pipe(2), close(2), mmap(2), munmap(2), linux: epoll_create(2), bsd: kqueue(2), solaris: open(2) on /dev/poll, oldlinux: sigprocmask(2), fcntl(2) */
att_write(1)
int io_pipe(int64* d);

/* like socketpair() */
/* return 1 if ok, 0 on error */
/* sandboxing: socketpair(2), close(2), mmap(2), munmap(2), linux: epoll_create(2), bsd: kqueue(2), solaris: open(2) /dev/poll, oldlinux: sigprocmask(2), fcntl(2) */
att_write(1)
int io_socketpair(int64* d);

/* non-blocking read(), -1 for EAGAIN and -3+errno for other errors */
/* sandboxing: poll(2), setitimer(2), read(2) */
att_writen(2, 3)
int64 io_tryread(int64 d,char* buf,int64 len);

/* blocking read(), with -3 instead of -1 for errors */
/* sandboxing: poll(2), read(2) */
att_writen(2, 3)
int64 io_waitread(int64 d,char* buf,int64 len);

/* non-blocking write(), -1 for EAGAIN and -3+errno for other errors */
/* sandboxing: poll(2), setitimer(2), write(2) */
att_readn(2, 3)
int64 io_trywrite(int64 d,const char* buf,int64 len);

/* blocking write(), with -3 instead of -1 for errors */
/* sandboxing: poll(2), write(2) */
att_readn(2, 3)
int64 io_waitwrite(int64 d,const char* buf,int64 len);

/* modify timeout attribute of file descriptor */
/* sandboxing: calls no syscalls */
void io_timeout(int64 d,tai6464 t);

/* like io_tryread but will return -2,errno=ETIMEDOUT if d has a timeout
 * associated and it is passed without input being there */
/* sandboxing: poll(2), setitimer(2), gettimeofday(2), read(2) */
att_writen(2, 3)
int64 io_tryreadtimeout(int64 d,char* buf,int64 len);

/* like io_trywrite but will return -2,errno=ETIMEDOUT if d has a timeout
 * associated and it is passed without being able to write */
/* sandboxing: poll(2), setitimer(2), gettimeofday(2), write(2) */
att_readn(2, 3)
int64 io_trywritetimeout(int64 d,const char* buf,int64 len);

/* sandboxing: linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
void io_wantread(int64 d);
void io_wantwrite(int64 d);
void io_dontwantread(int64 d);
void io_dontwantwrite(int64 d);

/* sandboxing: close(2), linux: epoll_wait(2), linux: epoll_ctl(2), bsd: kevent(2), solaris: ioctl(2), oldlinux: sigwaitinfo(2), oldlinux: sigtimedwait(2), oldlinux: signal(2), realloc(3): mmap/mremap/munmap/brk/brk2, poll(2) */
void io_wait();
void io_waituntil(tai6464 t);
int64 io_waituntil2(int64 milliseconds);
void io_check();

/* signal that read/accept/whatever returned EAGAIN */
/* needed for SIGIO and epoll */
/* sandboxing: linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
void io_eagain(int64 d);  /* do not use, API was a bad idea */
#define HAVE_EAGAIN_READWRITE
void io_eagain_read(int64 d);	/* use these ones */
void io_eagain_write(int64 d);

/* return next descriptor from io_wait that can be read from */
/* sandboxing: linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
int64 io_canread();
/* return next descriptor from io_wait that can be written to */
/* sandboxing: linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
int64 io_canwrite();

/* return next descriptor with expired timeout */
/* sandboxing: calls no syscalls */
int64 io_timeouted();

/* is this fd over its timeout? */
/* sandboxing: calls no syscalls */
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
/* sandboxing: fcntl(2), mmap(2), munmap(2), linux: epoll_create(2), bsd: kqueue(2), solaris: open(2) on /dev/poll, oldlinux: sigprocmask(2), fcntl(2) */
int io_fd(int64 d);		/* use this for sockets before you called connect() or accept() */
int io_fd_canwrite(int64 d);	/* use this for connected sockets (assumes socket is writable) */
int io_fd_flags(int64 d,int flags);	/* can be used to tell io_fd to skip one syscall */

/* sandboxing: calls no syscalls */
att_write(2)
void io_setcookie(int64 d,void* cookie);

/* sandboxing: calls no syscalls */
void* io_getcookie(int64 d);

/* put descriptor in non-blocking mode */
/* sandboxing: fcntl(2) */
void io_nonblock(int64 d);
/* put descriptor in blocking mode */
/* sandboxing: fcntl(2) */
void io_block(int64 d);
/* put descriptor in close-on-exec mode */
/* sandboxing: fcntl(2) */
void io_closeonexec(int64 d);

/* sandboxing: munmap(2), close(2) */
void io_close(int64 d);

/* Free the internal data structures from libio.
 * This only makes sense if you run your program in a malloc checker and
 * these produce false alarms.  Your OS will free these automatically on
 * process termination. */
/* sandboxing: free(3): munmap(2) */
void io_finishandshutdown(void);

/* send n bytes from file fd starting at offset off to socket s */
/* return number of bytes written */
/* sandboxing: sendfile(2), fallback: write(2), fallback: mmap(2), fallback: munmap(2), fallback: lseek(2), fallback: pread(2), fallback: read(2), linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
int64 io_sendfile(int64 s,int64 fd,uint64 off,uint64 n);

/* Pass fd over sock (must be a unix domain socket) to other process.
 * Return 0 if ok, -1 on error, setting errno. */
/* sandboxing: sendmsg(2) */
int io_passfd(int64 sock,int64 fd);

/* Receive fd over sock (must be a unix domain socket) from other
 * process.  Return sock if ok, -1 on error, setting errno. */
/* sandboxing: recvmsg(2) */
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
typedef int64 (*io_sendfile_callback)(int64 out,int64 in,uint64 off,uint64 bytes,io_write_callback writecb);

/* used internally, but hey, who knows */
/* sandboxing: write(2), mmap(2), munmap(2), lseek(2), pread(2), read(2), linux: epoll_ctl(2), bsd: kevent(2), solaris: write(2), oldlinux: poll(2) */
int64 io_mmapwritefile(int64 out,int64 in,uint64 off,uint64 bytes,io_write_callback writecb);

/* only needed for debugging, will print some stats into the buffer to
 * aid in debugging the state machine if a descriptor loops or so */
/* sandboxing: calls no syscalls */
unsigned int io_debugstring(int64 s,char* buf,unsigned int bufsize);

#ifdef __MINGW32__
#include <mcfgthread/c11thread.h>
#elif defined(__dietlibc__)
#include <threads.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

enum { SLOTS=128 };
typedef struct iomux {
  int ctx;
  volatile int working;		/* used to synchronize who is filling the queue */
  volatile unsigned int h,l;	/* high, low */
  struct {
    int fd, events;
  } q[SLOTS];
#if defined(__MINGW32__) || defined(__dietlibc__)
  mtx_t mtx;
  cnd_t sem;
#else
  sem_t sem;
#endif
} iomux_t;


/* Init master context */
int iom_init(iomux_t* c);

/* Add socket to iomux */
enum {
  IOM_READ=1,
  IOM_WRITE=2,
  IOM_ERROR=4
};
/* return -1 if error, events can be IOM_READ or IOM_WRITE */
int iom_add(iomux_t* c,int64 s,unsigned int events);

/* Blocking wait for single event, timeout in milliseconds */
/* return -1 if error, 0 if ok; s set to fd, revents set to known events on that fd */
/* when done with the fd, call iom_add on it again! */
/* This can be called by multiple threads in parallel */
int iom_wait(iomux_t* c,int64* s,unsigned int* revents,unsigned long timeout);

/* Call this to terminate all threads waiting in iom_wait */
int iom_abort(iomux_t* c);

#ifdef __cplusplus
}
#endif

#endif
