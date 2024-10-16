#include "fmt.h"
#include "byte.h"
#include <time.h>

static unsigned int fmt_2digits(char* dest,int i) {
  dest[0]=(char)((i/10)+'0');
  dest[1]=(char)((i%10)+'0');
  return 2;
}

size_t fmt_iso8601(char* dest,time_t t) {
#ifdef _WIN32
  struct tm tmp;
  struct tm* x=&tmp;
  gmtime_s(&tmp,&t);	// can't recover from when this fails
#else
  struct tm* x=gmtime(&t);
#endif
  size_t i;

  if (dest==0) return sizeof("2014-05-27T19:22:16Z")-1;
  /* "2014-05-27T19:22:16Z" */
  i=fmt_ulong(dest,(x->tm_year+1900));
  dest[i++]='-';
  i+=fmt_2digits(dest+i,x->tm_mon+1);
  dest[i++]='-';
  i+=fmt_2digits(dest+i,x->tm_mday);
  dest[i++]='T';
  i+=fmt_2digits(dest+i,x->tm_hour);
  i+=fmt_str(dest+i,":");
  i+=fmt_2digits(dest+i,x->tm_min);
  i+=fmt_str(dest+i,":");
  i+=fmt_2digits(dest+i,x->tm_sec);
  i+=fmt_str(dest+i,"Z");
  return i;
}
