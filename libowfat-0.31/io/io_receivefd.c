#ifdef __MINGW32__
#include <errno.h>
#include "io_internal.h"
int64 io_receivefd(int64 sock) {
  errno=EINVAL;
  return -1;
}
#else

#ifdef __NetBSD__
#define _NETBSD_SOURCE
#endif

#ifdef __OpenBSD__
#warning This is a HORRIBLE kludge around yet another thing OpenBSD broke
#warning If they make it possible to do file descriptor passing with
#warning _XOPEN_SOURCE defined, please send me an email so I can remove this.
#warning _XOPEN_SOURCE needs to be defined for this to work on Solaris.
#else
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#endif
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <errno.h>
#include "io_internal.h"

union fdmsg {
  struct cmsghdr h;
  char buf[1000];
};

int64 io_receivefd(int64 sock) {
  struct iovec iov;
  struct msghdr msg;
#ifdef CMSG_FIRSTHDR
  union fdmsg cmsg;
  struct cmsghdr* h;
#else
  int fd;
#endif
  char x;
  char name[100];
  iov.iov_base=&x;
  iov.iov_len=1;
  msg.msg_name=name;
  msg.msg_namelen=100;
#ifdef CMSG_FIRSTHDR
  msg.msg_control=cmsg.buf;
  msg.msg_controllen=sizeof(union fdmsg);
#else
  msg.msg_accrights=(char*)&fd;
  msg.msg_accrightslen=sizeof(fd);
#endif
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
#ifdef CMSG_FIRSTHDR
  msg.msg_flags=0;
  h=CMSG_FIRSTHDR(&msg);
#ifndef CMSG_LEN
#define CMSG_LEN(x) x
#endif
  h->cmsg_len=CMSG_LEN(sizeof(int));
  h->cmsg_level=SOL_SOCKET;
  h->cmsg_type=SCM_RIGHTS;
  *((int*)CMSG_DATA(h))=-1;
#endif
  if (recvmsg(sock,&msg,0)==-1)
    return -1;
#ifdef CMSG_FIRSTHDR
  h=CMSG_FIRSTHDR(&msg);
  if (!h || h->cmsg_len!=CMSG_LEN(sizeof(int)) || h->cmsg_level!=SOL_SOCKET || h->cmsg_type!=SCM_RIGHTS) {
#ifdef EPROTO
    errno=EPROTO;
#else
    errno=EINVAL;
#endif
    return -1;
  }
  return *((int*)CMSG_DATA(h));
#else
  if (msg.msg_accrightslen != sizeof(fd)) return -1;
  return fd;
#endif
}
#endif
