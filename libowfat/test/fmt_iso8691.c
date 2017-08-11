#include "fmt.h"
#include "scan.h"
#include <assert.h>
#include <unistd.h>

int main() {
  char buf[1024];
  buf[fmt_iso8601(buf,1431551843)]=0;	// Wed May 13 14:17:23 PDT 2015
  assert(!strcmp(buf,"2015-05-13T21:17:23Z"));
  struct timespec t;
  assert(scan_iso8601("2015-05-13T21:17:23Z",&t)==20 && t.tv_sec==1431551843);
  assert(scan_iso8601("2015-05-13T14:17:23-0700",&t)==24 && t.tv_sec==1431551843);
  return 0;
}
