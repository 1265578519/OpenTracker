#ifndef my_extern
#define my_extern extern
#endif

#include "libowfat/io.h"
#include "libowfat/array.h"
#include "libowfat/iarray.h"
#ifdef __MINGW32__
#include <winsock2.h>
#include "socket.h"
my_extern HANDLE io_comport;
#else
#include "haveepoll.h"
#include "havekqueue.h"
#include "havedevpoll.h"
#include "havesigio.h"
#ifdef HAVE_SIGIO
#define _GNU_SOURCE
#include <signal.h>
#endif
#endif

/* We simulate a level-triggered API on top of an event signalling
 * mechanism that can be level-triggered (epoll/kqueue/poll) or
 * edge-triggered (SIGIO).
 * Difficulty: we want to avoid unnecessary syscalls, so we keep state
 * internally.  If the user says he does not want to read/write anymore,
 * we don't tell the kernel straight away.  The rationale is that the
 * typical protocol consists of interleaved reading and writing, so
 * after each read you'd call io_dontwantread, io_wantwrite, io_wait,
 * io_dontwantwrite, io_wantread, and in the regular case there is no
 * incoming data between io_dontwantread and io_wantread, so we might as
 * well optimistically not do those syscalls and then handle the
 * complexity if there is more incoming data. */

/* Basically, we tell the kernel that we want to read if !canread,
 * and we tell the kernel that we want to write if !canwrite. */

typedef struct {
  tai6464 timeout;
  unsigned int wantread:1;	/* does the app want to read/write? */
  unsigned int wantwrite:1;
  unsigned int canread:1;	/* do we know we can read/write? */
  unsigned int canwrite:1;
  unsigned int nonblock:1;	/* is this socket non-blocking? */
  unsigned int inuse:1;		/* internal consistency checking */
  unsigned int kernelwantread:1;	/* did we tell the kernel we want to read/write? */
  unsigned int kernelwantwrite:1;
  unsigned int epolladded:1;
  unsigned int closed:1;	/* io_close called, but close deferred because of outstanding events */
  unsigned int zerocopy:1;	/* linux: setsockopt SO_ZEROCOPY done */
  unsigned int goterror:1;	/* got POLLERR|POLLHUP */
#ifdef __MINGW32__
  unsigned int readqueued:2;
  unsigned int writequeued:2;
  unsigned int acceptqueued:2;
  unsigned int connectqueued:2;
  unsigned int sendfilequeued:2;
  unsigned int listened:1;
#endif
  long next_read;
  long next_write;
  long next_defer;
  void* cookie;
  void* mmapped;
  long maplen;
  uint64 mapofs;
#ifdef __MINGW32__
  OVERLAPPED or,ow,os;	/* overlapped for read+accept, write+connect, sendfile */
  HANDLE /* fd, */ mh;
  char inbuf[8192];
  int bytes_read,bytes_written;
  DWORD errorcode;
  SOCKET next_accept;
#endif
} io_entry;

extern int io_multithreaded;
extern int io_sockets[2];

my_extern iarray io_fds;
my_extern uint64 io_wanted_fds;
my_extern array io_pollfds;

my_extern long first_readable;
my_extern long first_writeable;

extern long first_deferred;

my_extern enum __io_waitmode {
  UNDECIDED,
  POLL
#ifdef HAVE_KQUEUE
  ,KQUEUE
#endif
#ifdef HAVE_EPOLL
  ,EPOLL
#endif
#ifdef HAVE_SIGIO
  ,_SIGIO
#endif
#ifdef HAVE_DEVPOLL
  ,DEVPOLL
#endif
#ifdef __MINGW32__
  ,COMPLETIONPORT
#endif
} io_waitmode;

#if defined(HAVE_KQUEUE) || defined(HAVE_EPOLL) || defined(HAVE_DEVPOLL)
my_extern int io_master;
#endif
#if defined(HAVE_SIGIO)
my_extern int io_signum;
my_extern sigset_t io_ss;

my_extern long alt_firstread, alt_firstwrite;
my_extern long alt_curread, alt_curwrite;
#endif

int64 io_waituntil2(int64 milliseconds);

void io_sigpipe(void);

/* return next descriptor from io_wait that can be read from */
int64 io_canread_unlocked();
/* return next descriptor from io_wait that can be written to */
int64 io_canwrite_unlocked();
/* return next descriptor with expired timeout */
int64 io_timeouted_unlocked();

struct eventpacket {
  int fd;
  enum { CANREAD, CANWRITE, TIMEOUT } what;
};

#define debug_printf(x)

struct iom_entry {
  void* cookie;
};
