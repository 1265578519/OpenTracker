#include "fmt.h"
#include "byte.h"
#include <time.h>

static unsigned int fmt_2digits(char* dest,int i) {
  dest[0]=(char)((i/10)+'0');
  dest[1]=(char)((i%10)+'0');
  return 2;
}

size_t fmt_httpdate(char* dest,time_t t) {
  static const char days[] = "SunMonTueWedThuFriSat";
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  struct tm* x=gmtime(&t);
  size_t i;

  if (dest==0) return 29;
  /* "Sun, 06 Nov 1994 08:49:37 GMT" */
  byte_copy(dest,3,days+3*x->tm_wday); i=3;
  i+=fmt_str(dest+i,", ");
  i+=fmt_2digits(dest+i,x->tm_mday);
  i+=fmt_str(dest+i," ");
  byte_copy(dest+i,3,months+3*x->tm_mon); i+=3;
  i+=fmt_str(dest+i," ");
  i+=fmt_2digits(dest+i,(x->tm_year+1900)/100);
  i+=fmt_2digits(dest+i,(x->tm_year+1900)%100);
  i+=fmt_str(dest+i," ");
  i+=fmt_2digits(dest+i,x->tm_hour);
  i+=fmt_str(dest+i,":");
  i+=fmt_2digits(dest+i,x->tm_min);
  i+=fmt_str(dest+i,":");
  i+=fmt_2digits(dest+i,x->tm_sec);
  i+=fmt_str(dest+i," GMT");
  return i;
}
