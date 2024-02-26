#include "socket.h"
#include "io.h"
#include "buffer.h"
#include "ip6.h"
#include <errno.h>

int main() {
  int s=socket_tcp6();
  uint32 scope_id;
  char ip[16];
  uint16 port;
  if (socket_bind6_reuse(s,V6any,1234,0)==-1) {
    buffer_puts(buffer_2,"socket_bind6_reuse: ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  if (!io_fd(s)) {
    buffer_puts(buffer_2,"io_fd: ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  if (socket_listen(s,16)==-1) {
    buffer_puts(buffer_2,"socket_listen: ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  io_wantread(s);
  buffer_puts(buffer_2,"listening on port 1234 (fd #");
  buffer_putulong(buffer_2,s);
  buffer_putsflush(buffer_2,")\n");
  for (;;) {
    int64 i;
    io_wait();
    while ((i=io_canread())!=-1) {
      if (i==s) {
	int n;
	while ((n=socket_accept6(s,ip,&port,&scope_id))!=-1) {
	  char buf[IP6_FMT];
	  buffer_puts(buffer_2,"accepted new connection from ");
	  buffer_put(buffer_2,buf,fmt_ip6(buf,ip));
	  buffer_puts(buffer_2,":");
	  buffer_putulong(buffer_2,port);
	  buffer_puts(buffer_2," (fd ");
	  buffer_putulong(buffer_2,n);
	  buffer_puts(buffer_2,")");
	  if (io_fd(n)) {
	    io_nonblock(n);
	    io_wantread(n);
	  } else {
	    buffer_puts(buffer_2,", but io_fd failed.");
	    io_close(n);
	  }
	  buffer_putnlflush(buffer_2);
	}
	if (errno!=EAGAIN) {
	  buffer_puts(buffer_2,"socket_accept6: ");
	  buffer_puterror(buffer_2);
	  buffer_putnlflush(buffer_2);
	}
      } else {
	char buf[1024];

	int l=io_tryread(i,buf,sizeof buf);
	if (l==-1) {
	  buffer_puts(buffer_2,"io_tryread(");
	  buffer_putulong(buffer_2,i);
	  buffer_puts(buffer_2,"): ");
	  buffer_puterror(buffer_2);
	  buffer_putnlflush(buffer_2);
	  io_close(i);
	} else if (l==0) {
	  buffer_puts(buffer_2,"eof on fd #");
	  buffer_putulong(buffer_2,i);
	  buffer_putnlflush(buffer_2);
	  io_close(i);
	} else {
	  int r;
	  switch (r=io_trywrite(i,buf,l)) {
	  case -1:
	    buffer_puts(buffer_2,"io_tryread(");
	    buffer_putulong(buffer_2,i);
	    buffer_puts(buffer_2,"): ");
	    buffer_puterror(buffer_2);
	    buffer_putnlflush(buffer_2);
	    io_close(i);
	    break;
	  case 0:
	    buffer_puts(buffer_2,"write eof on fd #");
	    buffer_putulong(buffer_2,i);
	    buffer_putnlflush(buffer_2);
	    io_close(i);
	  default:
	    if (r!=l) {
	      buffer_puts(buffer_2,"short write on fd #");
	      buffer_putulong(buffer_2,i);
	      buffer_puts(buffer_2,": wrote ");
	      buffer_putulong(buffer_2,r);
	      buffer_puts(buffer_2,", wanted to write ");
	      buffer_putulong(buffer_2,l);
	      buffer_putsflush(buffer_2,").\n");
	    }
	  }
	}
      }
    }
  }
  return 0;
}
