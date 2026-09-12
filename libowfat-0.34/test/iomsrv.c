#include <libowfat/socket.h>
#include <libowfat/io.h>
#include <libowfat/errmsg.h>
#include <libowfat/ip6.h>
#include <libowfat/fmt.h>
#include <libowfat/scan.h>
#include <assert.h>
#include <unistd.h>
#include <threads.h>

iomux_t iom;

void read_data_from_socket(int s) {
  char buf[1024];
  ssize_t r=read(s,buf,sizeof buf);
  if (r==-1) {
    r=fmt_str(buf,"I/O error from fd ");
    r+=fmt_ulong(buf+r,s);
    buf[r]=0;
    carpsys(buf);
    close(s);
    return;
  }
  if (r==0) {
    close(s);
    return;
  }
  /* we'd usually have some protocol parsing here but for this
   * simplistic test program we'll just accept any input as valid
   * request */
  if (iom_requeue(&iom,s,IOM_WRITE)!=0) {
    carpsys("iom_requeue");
    close(s);
  }
}

void write_data_to_socket(int s) {
  if (write(s,"hello, world!\n",14) != 14) {
    carpsys("write");
    close(s);
  }
  if (iom_requeue(&iom,s,IOM_READ)!=0) {
    carpsys("iom_requeue");
    close(s);
  }
}

int w(void* a) {
  int servsock=*(int*)a;
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
      if (s==servsock) {
	char ip[16];
	uint16 port;
	uint32 scope_id;
	int conn=socket_accept6(s,ip,&port,&scope_id);
	if (conn==-1)
	  diesys(111,"accept");
	if (iom_add(&iom,conn,IOM_READ)==-1)
	  diesys(111,"iom_add");
	if (iom_requeue(&iom,s,IOM_READ)==-1)
	  diesys(111,"iom_requeue");
      } else if (events&IOM_READ)
	read_data_from_socket(s);
      else
	write_data_to_socket(s);
    }
  }
  return 0;
}

int main(int argc,char* argv[]) {
  int servsock=socket_tcp6();
  errmsg_iam("iomsrv");
  if (servsock==-1)
    diesys(111,"socket");
  if (socket_bind6_reuse(servsock,V6loopback,8000,0)==-1)
    diesys(111,"bind to localhost:8000");
  if (socket_listen(servsock,16)==-1)
    diesys(111,"listen");
  iom_init(&iom);
  if (iom_add(&iom,servsock,IOM_READ)==-1)
    diesys(111,"iom_add");

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
    if (thrd_create(&t[i],w,(void*)&servsock)!=thrd_success)
      die(111,"thread creation failed");
  }

  for (unsigned int i=0; i<nthr; ++i) {
    int retval;
    if (thrd_join(t[i],&retval)!=thrd_success)
      die(111,"thread join failed");
  }
  return 0;
}
