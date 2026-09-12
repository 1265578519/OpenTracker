#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#endif
#include "windoze.h"
#include "socket.h"

void socket_tryreservein(int s,int size)
{
  while (size >= 1024) {
    if (winsock2errno(setsockopt(s,SOL_SOCKET,SO_RCVBUF,&size,sizeof size)) == 0) return;
    size -= (size >> 5);
  }
}
