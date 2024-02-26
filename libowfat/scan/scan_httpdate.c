#define _GNU_SOURCE
#define __deprecated__
#include "scan.h"
#include "byte.h"
#include "case.h"
#include <time.h>
#include <stdlib.h>

#ifdef sgi
extern char** environ;
#endif

static int parsetime(const char*c,struct tm* x) {
  unsigned long tmp;
  c+=scan_ulong(c,&tmp); x->tm_hour=(int)tmp;
  if (*c!=':') return -1;
  ++c;
  c+=scan_ulong(c,&tmp); x->tm_min=(int)tmp;
  if (*c!=':') return -1;
  ++c;
  c+=scan_ulong(c,&tmp); x->tm_sec=(int)tmp;
  if (*c!=' ') return -1;
  return 0;
}

size_t scan_httpdate(const char *in,time_t *t) {
  struct tm x;
  int i;
  unsigned long tmp;
  const char* c;
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  if (!(c=in)) return 0;
  if (c[3]==',') c+=5; else
  if (c[6]==',') c+=8; else {
    c+=4;
    for (i=0; i<12; ++i) {
      if (case_equalb(c,3,months+i*3)) {
	x.tm_mon=i; break;
      }
    }
    c+=4; if (*c==' ') ++c;
    c+=scan_ulong(c,&tmp); x.tm_mday=(int)tmp;
    ++c;
    if (parsetime(c,&x)) return 0;
    c+=9;
    c+=scan_ulong(c,&tmp); x.tm_year=(int)(tmp-1900);
    goto done;
  }
  c+=scan_ulong(c,&tmp); x.tm_mday=(int)tmp;
  while (*c==' ') ++c; // work around crappy sqlite download httpd
//  ++c;
  for (i=0; i<12; ++i)
    if (case_equalb(c,3,months+i*3)) {
      x.tm_mon=i; break;
    }
  c+=4;
  c+=scan_ulong(c,&tmp);
  if (tmp>1000) x.tm_year=(int)(tmp-1900); else
    if (tmp<70) x.tm_year=(int)(tmp+100); else
                x.tm_year=(int)tmp;
  ++c;
  if (parsetime(c,&x)) return 0;
  c+=9;
  if (byte_equal(c,3,"GMT")) c+=3;
  if (*c=='+' || *c=='-') {
    ++c;
    while (*c>='0' && *c<='9') ++c;
  }
done:
  x.tm_wday=x.tm_yday=x.tm_isdst=0;
#if defined(__dietlibc__) || defined(__GLIBC__)
  *t=timegm(&x);
#elif defined(__MINGW32__)
  *t=mktime(&x);
#else
  {
#ifdef sgi
    char** old=environ;
    char** newenv={0};
    environ=newenv;
    *t=mktime(&x);
    environ=old;
#else
    char* old=getenv("TZ");
    unsetenv("TZ");
    *t=mktime(&x);
    if (old) setenv("TZ",old,1);
#endif
  }
#endif
  return (size_t)(c-in);
}
