#include <cdb.h>
#include "buffer.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "errmsg.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int main(int argc,char* argv[]) {
  int fd;
  static struct cdb c;
  errmsg_iam("cdbget");
  if (argc<3)
    die(1,"usage: cdbget data.cdb key");
  fd=open(argv[1],O_RDONLY|O_BINARY);
  if (fd==-1)
    diesys(1,"open");
  cdb_init(&c,fd);
  if (cdb_find(&c,argv[2],strlen(argv[2]))>0) {
    do {
      char* x=malloc(cdb_datalen(&c));
      if (!x)
	die(1,"out of memory");
      if (cdb_read(&c,x,cdb_datalen(&c),cdb_datapos(&c))==-1)
	diesys(1,"cdb_read");
      buffer_put(buffer_1,x,cdb_datalen(&c));
      buffer_put(buffer_1,"\n",1);
      free(x);
    } while (cdb_findnext(&c,argv[2],strlen(argv[2]))>0);
  }
  buffer_flush(buffer_1);
}
