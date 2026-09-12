#ifdef __MINGW32__
#include "socket.h"
#include "windoze.h"

void __winsock_init(void) {
  static int inited;
  if (!inited) {
    WSADATA wsaData;
    if (WSAStartup( MAKEWORD(2,2), &wsaData ) ||
	LOBYTE( wsaData.wVersion ) != 2 ||
	HIBYTE( wsaData.wVersion ) != 2)
      exit(111);
    inited=1;
  }
}
#endif
