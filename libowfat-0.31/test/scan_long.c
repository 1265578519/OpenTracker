#include "scan.h"
#include "fmt.h"
#include "buffer.h"
#include <assert.h>

int main() {
  char buf[1024];
  unsigned long long int i;
  unsigned long l;
  if (sizeof(unsigned long)==4) {
    assert(scan_ulong("4294967295",&l) == 10 && l==4294967295ul);
    assert(scan_ulong("4294967296",&l) == 9 && l==429496729);
  } else {
    assert(scan_ulong("18446744073709551615",&l) == 20 && l==18446744073709551615ull);
    assert(scan_ulong("18446744073709551616",&l) == 19 && l==1844674407370955161ull);
  }
  if (sizeof(unsigned long) != 4)
    return 0;
  for (i=1; i<0xfffffffffull; i+=i+1) {
    int k;
    unsigned long test;
    buf[k=fmt_ulonglong(buf,i)]=0;
    buffer_puts(buffer_1,buf); buffer_putnlflush(buffer_1);
    if (buf[scan_ulong(buf,&test)])
      /* scan did not like the whole number */
      assert(i>0xffffffffull);
    else
      assert(i<=0xffffffffull);
  }
  return 0;
}
