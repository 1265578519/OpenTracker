#include "fmt.h"

size_t fmt_double(char *dest, double d,int maxlen,int prec) {
  union {
    double d;
    unsigned long long x;
  } __u = { .d=d, };
  /* step 1: extract sign, mantissa and exponent */
  signed int s=__u.x>>63;
  signed long e=((__u.x>>52)&((1<<11)-1))-1023;
/*  unsigned long long m=*x & ((1ull<<52)-1); */
  /* step 2: exponent is base 2, compute exponent for base 10 */
  signed long e10=1+(long)(e*0.30102999566398119802); /* log10(2) */
  /* step 3: calculate 10^e10 */
  int i;
  double tmp=10.0;
  char *oldbuf=dest;
  int initial=1;
  int writeok=(dest!=0);

  if (s) { d=-d; if (writeok) *dest='-'; --maxlen; dest++; }
  if (d==0.0) { if (writeok) *dest='0'; --maxlen; dest++; return dest-oldbuf; }
  if ((i=e10)>=0) {
    while (i>10) { tmp=tmp*1e10; i-=10; }
    while (i>1) { tmp=tmp*10; --i; }
  } else {
    i=(e10=-e10);
    while (i>10) { tmp=tmp*1e-10; i-=10; }
    while (i>1) { tmp=tmp/10; --i; }
  }
  while (d/tmp<1) {
    --e10;
    tmp/=10.0;
  }
  /* step 4: see if precision is sufficient to display all digits */
  if (e10>prec) {
    /* use scientific notation */
    int len=fmt_double(writeok?dest:0,d/tmp,maxlen,prec);
    if (len==0) return 0;
    maxlen-=len; dest+=len;
    if (--maxlen>=0) {
      if (writeok) *dest='e';
      ++dest;
    }
    for (len=1000; len>0; len/=10) {
      if (e10>=len || !initial) {
	if (--maxlen>=0) {
	  if (writeok) *dest=(e10/len)+'0';
	  ++dest;
	}
	initial=0;
	e10=e10%len;
      }
    }
    if (maxlen>=0) return dest-oldbuf;
    return 0;
  }
  /* step 5: loop through the digits, inserting the decimal point when
   * appropriate */
  for (; prec>0; ) {
    double tmp2=d/tmp;
    char c;
    d-=((int)tmp2*tmp);
    c=((int)tmp2);
    if ((!initial)||c) {
      if (--maxlen>=0) {
	initial=0;
	if (writeok) *dest=c+'0';
	++dest;
      } else
	return 0;
      --prec;
    }
    if (tmp>0.5 && tmp<1.5) {
      tmp=1e-1;
      initial=0;
      if (--maxlen>=0) {
	if (writeok) *dest='.';
	++dest;
      } else
	return 0;
    } else
      tmp/=10.0;
  }
  return dest-oldbuf;
}
