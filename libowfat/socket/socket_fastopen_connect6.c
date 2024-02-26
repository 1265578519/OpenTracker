#include "socket.h"
#ifdef __MINGW32__
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#endif
#include <errno.h>

ssize_t socket_fastopen_connect6(int s,const char* ip,uint16 port,uint32_t scope_id,const char* buf,size_t len) {
#ifndef MSG_FASTOPEN
  int r;
  {
#else
  int r=socket_send6_flag(s,buf,len,ip,port,scope_id,MSG_FASTOPEN);
  if (r==-1 && errno==ENOTCONN) {
#endif
    /* apparently the kernel does not support TCP fast open */
    r=socket_connect6(s,ip,port,scope_id);
    if (r==0) return write(s,buf,len);
  }
  return r;
}
