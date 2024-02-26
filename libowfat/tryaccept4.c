#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main() {
  accept4(0, NULL, NULL, 0);	// we just want to know if the symbol exists
  return 0;
}
