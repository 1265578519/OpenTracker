#include "fmt.h"

size_t fmt_human(char* dest,unsigned long long l) {
  char unit;
  size_t i;
  if (l<1000) return fmt_ulong(dest,l);
  if (l>1000000000000ull) {
    /* dang!  overflow! */
    l/=1000;
    l=(l+50000000)/100000000;
    unit='T';
  } else if (l>1000000000) {
    l=(l+50000000)/100000000;
    unit='G';
  } else if (l>1000000) {
    l=(l+50000)/100000;
    unit='M';
  } else {
    l=(l+50)/100;
    unit='k';
  }
  if (!dest) return fmt_ulong(0,l)+2;
  i=fmt_ulong(dest,l/10);
  dest[i]='.';
  dest[i+1]=(char)((l%10)+'0');
  dest[i+2]=unit;
  return i+3;
}
