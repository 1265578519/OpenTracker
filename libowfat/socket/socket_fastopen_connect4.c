#include "socket.h"
#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#endif
#include <errno.h>

ssize_t socket_fastopen_connect4(int s,const char* ip,uint16 port,const char* buf,size_t len) {
#ifndef MSG_FASTOPEN
  int r;
  {
#else
  int r;
  if (len)
    r=socket_send4_flag(s,buf,len,ip,port,MSG_FASTOPEN);
  else
    r=socket_connect4(s,ip,port);
  if (r==-1 && errno==ENOTCONN) {
#endif
    /* apparently the kernel does not support TCP fast open */
    r=socket_connect4(s,ip,port);
    if (r==0) return write(s,buf,len);
  }
  return r;
}
