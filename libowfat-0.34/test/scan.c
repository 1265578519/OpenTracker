#include <assert.h>
#include <limits.h>
#include "scan.h"

int main() {
  unsigned long l;
  assert(scan_ulong("12345",&l)==5 && l==12345);
  assert(scan_ulong("-12345",&l)==0);
  assert(scan_ulong("4294967295",&l)==10 && l==4294967295ul);
  if (sizeof(unsigned long)==4) {
    assert(scan_ulong("4294967296",&l)==9 && l==429496729);
    assert(scan_ulong("42949672950",&l)==10 && l==4294967295ul);
    assert(scan_ulong("5294967295",&l)==9 && l==529496729);
    assert(scan_ulong("4295067295",&l)==9 && l==429506729);
  }
  return 0;
}
