#ifdef __hpux__
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>

int main() {
/*
      sbsize_t sendfile(int s, int fd, off_t offset, bsize_t nbytes,
                    const struct iovec *hdtrl, int flags);
*/
  struct iovec x[2];
  int fd=open("havesendfile.c",0);
  x[0].iov_base="header";
  x[0].iov_len=6;
  x[1].iov_base="footer";
  x[1].iov_len=6;
  sendfile(1 /* dest socket */,fd /* src file */,
           0 /* offset */, 23 /* nbytes */,
           x, 0);
  perror("sendfile");
  return 0;
}
#elif defined (__sun__) && defined(__svr4__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <stdio.h>

int main() {
  off_t o;
  o=0;
  sendfile(1 /* dest */, 0 /* src */,&o,23 /* nbytes */);
  perror("sendfile");
  return 0;
}
#elif defined (_AIX)

#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

int main() {
  int fd=open("havesendfile.c",0);
  struct sf_parms p;
  int destfd=1;
  p.header_data="header";
  p.header_length=6;
  p.file_descriptor=fd;
  p.file_offset=0;
  p.file_bytes=23;
  p.trailer_data="footer";
  p.trailer_length=6;
  if (send_file(&destfd,&p,0)>=0)
    printf("sent %lu bytes.\n",p.bytes_sent);
  return 0;
}
#elif defined(__linux__)

#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <unistd.h>
#if defined(__GLIBC__)
#include <sys/sendfile.h>
#elif defined(__dietlibc__)
#include <sys/sendfile.h>
#else
#include <linux/unistd.h>
_syscall4(int,sendfile,int,out,int,in,long *,offset,unsigned long,count)
#endif
#include <stdio.h>

int main() {
  int fd=open("havesendfile.c",0);
  off_t o=0;
  off_t r=sendfile(1,fd,&o,23);
  if (r!=-1)
    printf("sent %llu bytes.\n",r);
  return 0;
}

#else
#error unsupported architecture
/* stupid fucking IRIX c99 does not signal #error via a non-zero exit
 * code!  ARGH!  So insert a parse error */
#include rumpelstilzchen
) 
#endif
