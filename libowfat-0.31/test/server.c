#include "scan.h"
#include "ip6.h"
#include "str.h"
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include "socket.h"
#include "ndelay.h"
#include <sys/socket.h>
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
    if (i==-1) panic("server: error: read() failed");
    if (i==0) exit(0);
    for (j=0; j<i; ) {
      k=write(dest,buf+j,i-j);
      if (k==-1) panic("server: error: write() failed");
      if (k==0) panic("server: error: short write\n");
      j+=k;
    }
  }
}

int main(int argc,char* argv[]) {
  int s,t;
  uint16 port=2342;
  uint32 scope_id=0;
  char ip[16];
  char buf[8192];
  int i;
  struct pollfd p[2];

  if (argc<3) {
usage:
    __write2("usage: server ip port\n\nexample: server 0 8000 | restore x -f -\n");
    return 0;
  }
  if (!strcmp(argv[1],"0"))
    byte_zero(ip,16);
  else
    if (argv[1][i=scan_ip6if(argv[1],ip,&scope_id)]) {
      __write2("server: error: invalid ip address!\n");
      goto usage;
    }
  if (argv[2][scan_ushort(argv[2],&port)]) {
    __write2("server: error: invalid port number!\n");
    goto usage;
  }
  if (byte_equal(ip,12,V4mappedprefix)) {
    s=socket_tcp4b();
    if (s==-1) panic("server: error: socket() failed");
    if (socket_bind4_reuse(s,ip+12,port)==-1) panic("server: error: bind() failed");
    if (socket_listen(s,1)==-1) panic("server: error: listen() failed");
    ndelay_off(s);
    if ((t=socket_accept4(s,0,0))==-1) panic("server: error: accept() failed");
  } else {
    s=socket_tcp6b();
    if (s==-1) panic("server: error: socket() failed");
    if (socket_bind6_reuse(s,ip,port,scope_id)==-1) panic("server: error: bind() failed");
    if (socket_listen(s,1)==-1) panic("server: error: listen() failed");
    ndelay_off(s);
    if ((t=socket_accept6(s,0,0,0))==-1) panic("server: error: accept() failed");
  }
  close(s);
  p[0].fd=0; p[0].events=POLLIN;
  p[1].fd=t; p[1].events=POLLIN;
  while (poll(p,2,5000)) {
    if (p[0].revents) {
      int j;
      if (p[0].revents&POLLERR)
	panic("server: error: poll() signals POLLERR\n");
      i=read(0,buf,sizeof(buf));
      if (i==0) { shutdown(t,SHUT_WR); blockingcopy(t,1); }
      for (j=0; j<i; ) {
	int k=write(t,buf+j,i-j);
	if (k==-1) panic("server: error: write() failed");
	if (k==0) panic("server: error: short write!\n");
	j+=k;
      }
    }
    if (p[1].revents) {
      int j;
      if (p[1].revents&POLLERR)
	panic("server: error: poll() signals POLLERR\n");
      i=read(t,buf,sizeof(buf));
      if (i==0) { shutdown(1,SHUT_WR); blockingcopy(0,t); }
      for (j=0; j<i; ) {
	int k=write(1,buf+j,i-j);
	if (k==-1) panic("server: error: write() failed");
	if (k==0) panic("server: error: short write!\n");
	j+=k;
      }
    }
  }
  return 0;
}
