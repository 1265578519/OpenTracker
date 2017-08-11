#include "socket.h"
#include "io.h"
#include "buffer.h"
#include "ip6.h"
#include "str.h"
#include "dns.h"
#include <errno.h>
#include <getopt.h>
#include "scan.h"
#include <stdlib.h>

struct state {
  int64 a,b;
  char buf[1024];
  int connected;
  int done,todo;
  enum { UNDECIDED, FORA, FORB } dir;
};

int main(int argc,char* argv[]) {
  int s=socket_tcp6();
  uint32 scope_id;
  char ip[16];
  uint16 port;
  char hisip[16];
  uint16 hisport;
  uint32 hisscope_id;
  static char seed[128];
  static stralloc fqdn;
  static stralloc out;

  if (argc!=4) {
usage:
    buffer_putsflush(buffer_2,"usage: proxy myip myport hisip hisport\n"
		              "\n"
			      "e.g.: proxy 0 119 news.fu-berlin.de 119\n");
    return 0;
  }

  if (argv[1][scan_ip6if(argv[1],ip,&scope_id)]) {
    if (str_equal(argv[1],"0")) {
      byte_zero(ip,16);
      scope_id=0;
    } else
      goto usage;
  }
  if (argv[2][scan_ushort(argv[2],&port)]) goto usage;
  if (argv[3][scan_ip6if(argv[3],hisip,&hisscope_id)]) {
    dns_random_init(seed);
    if (!stralloc_copys(&fqdn,argv[3])) goto nomem;
    if (dns_ip4(&out,&fqdn) == -1) {
      buffer_puts(buffer_2,"unable to find IP address for ");
      buffer_puts(buffer_2,argv[3]);
      buffer_puts(buffer_2,": ");
      buffer_puterror(buffer_2);
      buffer_putnlflush(buffer_2);
      return 111;
    }
  } else
    if (!stralloc_catb(&out,hisip,16)) {
nomem:
      buffer_putsflush(buffer_2,"out of memory\n");
      return 111;
    }
  if (argv[4][scan_ushort(argv[4],&hisport)]) goto usage;

  if (socket_bind6_reuse(s,ip,port,scope_id)==-1) {
    buffer_puts(buffer_2,"socket_bind6_reuse: ");
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
  if (!io_fd(s)) {
    buffer_puts(buffer_2,"io_fd: ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
    return 111;
  }
  io_wantread(s);
  for (;;) {
    int64 i;
    io_wait();
    while ((i=io_canread())!=-1) {
      if (i==s) {
	/* the read event is on the server socket */
	/* that means it's an incoming connection */
	int n;
	while ((n=socket_accept6(s,ip,&port,&scope_id))!=-1) {
	  int x=socket_tcp6();
	  if (x==-1) {
	    buffer_puts(buffer_2,"socket_tcp6 failed: ");
fail:
	    buffer_puterror(buffer_2);
	    buffer_putnlflush(buffer_2);
	    io_close(n);
	  } else {
	    struct state* s=malloc(sizeof(struct state));
	    if (!s) goto closefail;
	    s->a=n; s->b=x; s->connected=0; s->done=s->todo=0;
	    s->dir=UNDECIDED;
	    io_nonblock(x);
	    socket_connect6(x,out.s,hisport,hisscope_id);
	    if (!io_fd(x) || !io_fd(n)) {
	      buffer_puts(buffer_2,"io_fd failed: ");
closefail:
	      free(s);
	      io_close(x);
	      goto fail;
	    }
	    io_setcookie(x,s);
	    io_setcookie(n,s);
	    io_wantwrite(x);
	  }
	}
	if (errno!=EAGAIN) {
	  buffer_puts(buffer_2,"socket_accept6 failed: ");
	  buffer_puterror(buffer_2);
	  buffer_putnlflush(buffer_2);
	}
      } else {
	/* read event on an established connection */
	struct state* s=io_getcookie(i);
	int l=io_tryread(i,s->buf,sizeof(s->buf));
	if (l==-1) {
	  buffer_puts(buffer_2,"io_tryread(");
	  buffer_putulong(buffer_2,i);
	  buffer_puts(buffer_2,"): ");
	  buffer_puterror(buffer_2);
	  buffer_putnlflush(buffer_2);
	  io_close(s->a);
	  io_close(s->b);
	} else if (l==0) {
	  buffer_puts(buffer_2,"eof on fd #");
	  buffer_putulong(buffer_2,i);
	  buffer_putnlflush(buffer_2);
	  io_close(i);
	} else {
	  int r;
	  switch (r=io_trywrite(i,s->buf,l)) {
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
