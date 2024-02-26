#include "fmt.h"
#include <string.h>
#include <assert.h>

int main() {
  char buf[100];
  buf[5]='x';
  assert(fmt_str(buf,"fnord")==5);
  assert(buf[5]=='x');
  buf[5]=0;
  assert(!strcmp(buf,"fnord"));
  assert(fmt_strm(buf,"Fn","0rd")==5);
  assert(!strcmp(buf,"Fn0rd"));
}
