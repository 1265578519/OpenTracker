#include <libowfat/socket.h>
#include <libowfat/io.h>
#include <libowfat/errmsg.h>
#include <libowfat/ip6.h>
#include <libowfat/fmt.h>
#include <libowfat/scan.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <threads.h>
#include <stdatomic.h>

iomux_t iom;
int s[32];
atomic_int n;

#define TRANSACTIONS 100000

int w(void* a) {
  (void)a;
  for (;;) {
    int64 s;
    unsigned int events;
    switch (iom_wait(&iom,&s,&events,1000)) {
    case -1:
      diesys(111,"iom_wait");
      break;
    case 0:
      carp("iom_wait: timeout");
      break;
    case 1:
      if (events&IOM_WRITE) {
	if (write(s,"salut!\n",7) != 7)
	  diesys(111,"write");
	if (iom_requeue(&iom,s,IOM_READ) == -1)
	  diesys(111,"iom_requeue");
      } else {
	char buf[100];
	ssize_t r=read(s,buf,sizeof buf);
	if (r<=0 || atomic_fetch_add(&n,1) >= TRANSACTIONS) {
	  close(s);
	  return 0;
	}
	if (iom_requeue(&iom,s,IOM_WRITE) == -1)
	  diesys(111,"iom_requeue");
      }
    }
  }
  return 0;
}

int main(int argc,char* argv[]) {
  iom_init(&iom);
  for (unsigned int i=0; i<sizeof(s)/sizeof(s[0]); ++i) {
    s[i]=socket_tcp6b();
    errmsg_iam("iomcli");
    if (s[i]==-1)
      diesys(111,"socket");
    if (socket_connect6(s[i],V6loopback,8000,0)==-1 && errno!=EINPROGRESS)
      diesys(111,"connect");
    if (iom_add(&iom,s[i],IOM_WRITE)==-1)
      diesys(111,"iom_add");
  }

  thrd_t t[32];
  unsigned int nthr=1;
  if (argc>1 && scan_uint(argv[1],&nthr)>0) {
    if (nthr>sizeof(t)/sizeof(t[0])) {
      char buf[FMT_LONG];
      buf[fmt_ulong(buf,sizeof(t)/sizeof(t[0]))]=0;
      die(111,"max threads > ",buf);
    }
  }
  for (unsigned int i=0; i<nthr; ++i) {
    if (thrd_create(&t[i],w,0)!=thrd_success)
      die(111,"thread creation failed");
  }

  for (unsigned int i=0; i<nthr; ++i) {
    int retval;
    if (thrd_join(t[i],&retval)!=thrd_success)
      die(111,"thread join failed");
  }
  return 0;
}
