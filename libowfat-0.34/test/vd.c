#include "socket.h"
#include "buffer.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/uio.h>
#include "ndelay.h"

int main(int argc,char* argv[]) {
  int s=socket_tcp4b();
  char line[1024];
  char buf[4096];
  int l;
  int header=1;
  struct iovec x[2];
  buffer filein;

  (void)argc;
  buffer_init(&filein,read,s,buf,sizeof buf);
  if (socket_connect4(s,"\x7f\x00\x00\x01",4000)) {
    perror("connect");
    return 1;
  }
  {
    char* c;
    if ((c=strrchr(argv[0],'/')))
      x[0].iov_base=c+1;
    else
      x[0].iov_base=argv[0];
  }
  x[0].iov_len=strlen(x[0].iov_base);
  x[1].iov_base="\nq\n";
  x[1].iov_len=3;
  writev(s,x,2);
  for (;;) {
    line[0]=0;
    if ((l=buffer_getline(&filein,line,(sizeof line)-1))==0 && line[l]!='\n')
      break;
    else {
      line[l+1]=0;
      if (!header) {
	if (strcmp(line,"\e[7mMLdonkey command-line:\e[2;37;0m\n") &&
	    strcmp(line,"\e[2;37;0m\e[7mMLdonkey command-line:\e[2;37;0m\n") &&
	    strncmp(line,"> ",2))
	  buffer_put(buffer_1,line,l+1);
      }
      if (!strcmp(line,"Use \e[31m?\e[2;37;0m for help\n")) header=0;
      if (!strcmp(line,"Use ? for help\n")) header=0;
    }
  }
  buffer_flush(buffer_1);
  return 0;
}
