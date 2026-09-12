#include "fmt.h"
#include "buffer.h"

int main() {
  char buf[1024];
  buffer_put(buffer_1,buf,fmt_human(buf,1)); buffer_putnlflush(buffer_1);
  buffer_put(buffer_1,buf,fmt_human(buf,1400)); buffer_putnlflush(buffer_1);
  buffer_put(buffer_1,buf,fmt_human(buf,2300000)); buffer_putnlflush(buffer_1);
  buffer_put(buffer_1,buf,fmt_human(buf,(unsigned long long)(-1ll))); buffer_putnlflush(buffer_1);
  return 0;
}
