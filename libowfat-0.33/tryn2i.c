#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

int main() {
  static char ifname[IFNAMSIZ];
  char *tmp=if_indextoname(0,ifname);
  (void)tmp;
  return 0;
}
