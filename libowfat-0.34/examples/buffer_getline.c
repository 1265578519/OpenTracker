#include "stralloc.h"
#include "buffer.h"
#include "io.h"
#include "errmsg.h"
#include <unistd.h>

int main() {
  static stralloc line;
  buffer in;
  char buf[4096];
  int64 fd;
  char* home;
  long r;

  errmsg_iam("buffer_getline");

  if (!(home=getenv("HOME"))) die(1,"no $HOME");
  if (!stralloc_copys(&line,home) || !stralloc_cats(&line,"/.slrnrc") || !stralloc_0(&line))
    die(1,"out of memory");
  if (!io_readfile(&fd,line.s))
    diesys(1,"could not open ~/.slrnrc");
  buffer_init(&in,read,fd,buf,sizeof buf);

  while ((r=buffer_getnewline_sa(&in,&line))>0) {
    write(1,line.s,line.len);
  }
}
