#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
  int fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  (int)fd;
  return 0;
}
