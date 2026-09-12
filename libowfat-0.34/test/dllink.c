#include <socket.h>

#include "socket.h"
#include "buffer.h"
#include <unistd.h>
#include <stdio.h>
#include "case.h"
#include "ndelay.h"

int main(int argc,char* argv[]) {
  int s=socket_tcp4b();
  char line[1024];
  char buf[4096];
  int l;
  int header=1;
  buffer filein;

  ndelay_off(s);
  if (argc<2 || strlen(argv[1])>900) {
    buffer_putsflush(buffer_2,"usage: dllink ed2k://|file|<filename>|<filesize>|<MD4-sum|\n");
    return 0;
  }
  buffer_init(&filein,read,s,buf,sizeof buf);
  if (socket_connect4(s,"\x7f\x00\x00\x01",4000)) {
    perror("connect");
    return 1;
  }
  strcpy(buf,"dllink ");
  if (case_equalb(argv[1],6,"ed2k:|")) {
    strcat(buf,"ed2k://");
    strcat(buf,argv[1]+5);
  } else
    strcat(buf,argv[1]);
  strcat(buf,"\nq\n");
  write(s,buf,strlen(buf));
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
