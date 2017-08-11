#include <unistd.h>

int main() {
  char buf[100];
  if (pread(0,buf,sizeof(buf),0)==-1) return 111;
  return 0;
}
