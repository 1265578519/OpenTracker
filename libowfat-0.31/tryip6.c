#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
  struct sockaddr_in6 sa;
  sa.sin6_family = PF_INET6;
  (void)sa;
  return 0;
}
