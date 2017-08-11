#include "scan.h"
#include "ip6.h"
#include "str.h"
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include "socket.h"
#include <sys/socket.h>
#include "ndelay.h"
#ifdef __dietlibc__
#include <write12.h>
#else
static inline __write1(const char* s) { write(1,s,str_len(s)); }
static inline __write2(const char* s) { write(2,s,str_len(s)); }
#endif
#include <stdlib.h>

void panic(const char* s) {
  int i=str_len(s);
  __write2(s);
  if (s[i-1]!='\n') {
    __write2(": ");
    __write2(strerror(errno));
    __write2("\n");
  }
  exit(1);
}

void blockingcopy(int src,int dest) {
  char buf[8192];
  int i,j,k;
  for (;;) {
    i=read(src,buf,sizeof buf);
    if (i==-1) panic("client: error: read() failed");
    if (i==0) exit(0);
    for (j=0; j<i; ) {
      k=write(dest,buf+j,i-j);
      if (k==-1) panic("client: error: write() failed");
      if (k==0) panic("client: error: short write\n");
      j+=k;
    }
  }
}

int main(int argc,char* argv[]) {
  int s;
  uint16 port=2342;
  uint32 scope_id=0;
  char ip[16];
  char buf[8192];
  int i;
  struct pollfd p[2];

  if (argc<3) {
usage:
    __write2("usage: client ip port\n\nexample: dump -0 -f - | client 10.0.0.149 8000\n");
    return 0;
  }
  if (argv[1][i=scan_ip6if(argv[1],ip,&scope_id)]) {
    __write2("client: error: invalid ip address!\n");
    goto usage;
  }
  if (argv[2][scan_ushort(argv[2],&port)]) {
    __write2("client: error: invalid port number!\n");
    goto usage;
  }
  if (byte_equal(ip,12,V4mappedprefix)) {
    s=socket_tcp4b();
    if (s==-1) panic("client: error: socket() failed");
    if (socket_connect4(s,ip+12,port)==-1) panic("client: error: connect() failed");
  } else {
    s=socket_tcp6b();
    if (s==-1) panic("client: error: socket() failed");
    if (socket_connect6(s,ip,port,scope_id)==-1) panic("client: error: connect() failed");
  }
  ndelay_off(s);
  p[0].fd=0; p[0].events=POLLIN;
  p[1].fd=s; p[1].events=POLLIN;
  while (poll(p,2,5000)) {
    if (p[0].revents) {
      int j;
      if (p[0].revents&POLLERR)
	panic("client: error: poll() signals POLLERR\n");
      i=read(0,buf,sizeof(buf));
      if (i==0) { shutdown(s,SHUT_WR); blockingcopy(s,1); }
      for (j=0; j<i; ) {
	int k=write(s,buf+j,i-j);
	if (k==-1) panic("client: error: write() failed");
	if (k==0) panic("client: error: short write!\n");
	j+=k;
      }
    }
    if (p[1].revents) {
      int j;
      if (p[1].revents&POLLERR)
	panic("client: error: poll() signals POLLERR\n");
      i=read(s,buf,sizeof(buf));
      if (i==0) { shutdown(1,SHUT_WR); blockingcopy(0,s); }
      for (j=0; j<i; ) {
	int k=write(1,buf+j,i-j);
	if (k==-1) panic("client: error: write() failed");
	if (k==0) panic("client: error: short write!\n");
	j+=k;
      }
    }
  }
  return 0;
}
