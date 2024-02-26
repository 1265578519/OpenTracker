#include <time.h>
#include "scan.h"
#include "buffer.h"
#include "fmt.h"

int main() {
  char buf[100];
  time_t s,t;
  buffer_put(buffer_1,buf,fmt_httpdate(buf,time(&s)));
  buffer_putnlflush(buffer_1);
  buffer_puts(buffer_1,buf+scan_httpdate(buf,&t));
  buffer_putnlflush(buffer_1);
  buffer_putulong(buffer_1,s);
  buffer_puts(buffer_1," vs. ");
  buffer_putulong(buffer_1,t);
  buffer_putnlflush(buffer_1);
  return 0;
}
