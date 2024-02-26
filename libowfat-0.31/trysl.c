#include <sys/types.h>
#ifdef __MINGW32__
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

int main() {
  socklen_t t;
  (void)t;
  return 0;
}
