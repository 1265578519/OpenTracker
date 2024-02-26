#include "rangecheck.h"
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

void check_minmax() {
  /* make sure the __MIN and __MAX macros work */
  assert(__MIN(signed char)==-128);
  assert(__MAX(signed char)==127);
  assert(__MIN(unsigned char)==0);
  assert(__MAX(unsigned char)==255);

  assert(__MIN(short)==SHRT_MIN);
  assert(__MAX(short)==SHRT_MAX);
  assert(__MIN(unsigned short)==0);
  assert(__MAX(unsigned short)==USHRT_MAX);

  assert(__MIN(int)==INT_MIN);
  assert(__MAX(int)==INT_MAX);
  assert(__MIN(unsigned int)==0);
  assert(__MAX(unsigned int)==UINT_MAX);

  assert(__MIN(long)==LONG_MIN);
  assert(__MAX(long)==LONG_MAX);
  assert(__MIN(unsigned long)==0);
  assert(__MAX(unsigned long)==ULONG_MAX);

#ifdef LLONG_MIN
  assert(__MIN(long long)==LLONG_MIN);
  assert(__MAX(long long)==LLONG_MAX);
  assert(__MIN(unsigned long long)==0);
  assert(__MAX(unsigned long long)==ULLONG_MAX);
#endif

  assert(__MIN(int32_t)==(int32_t)0x80000000);
  assert(__MAX(int32_t)==0x7fffffff);
  assert(__MIN(uint32_t)==0);
  assert(__MAX(uint32_t)==0xffffffff);
}

void check_assign() {
  int a;
  assert(assign(a,5)==0 && a==5);
  a=23; assert(assign(a,(unsigned int)-23)==1);
#ifdef LLONG_MIN
  a=23; assert(assign(a,LLONG_MAX)==1);
#endif
}

void check_rangeptrbuf() {
  char buf[1000];

  /* does range_ptrinbuf check all the incoming pointer cases right? */
  assert(range_ptrinbuf(buf,sizeof(buf),0)==0);
  assert(range_ptrinbuf(buf,sizeof(buf),buf-1)==0);
  assert(range_ptrinbuf(buf,sizeof(buf),buf)==1);
  assert(range_ptrinbuf(buf,sizeof(buf),buf+50)==1);
  assert(range_ptrinbuf(buf,sizeof(buf),buf+sizeof(buf))==0);

  /* what if we give it an invalid buffer? */
  assert(range_ptrinbuf(0,sizeof(buf),0)==0);
  assert(range_ptrinbuf(buf,(unsigned long)-1,buf+1)==0);

  /* see if range_validbuf works */
  assert(range_validbuf(buf,sizeof(buf))==1);
  assert(range_validbuf(0,sizeof(buf))==0);
  assert(range_validbuf(buf,(unsigned long)-1)==0);

  /* range_bufinbuf */
  assert(range_bufinbuf(buf,sizeof(buf),buf,sizeof(buf))==1);
  assert(range_bufinbuf(buf,sizeof(buf),buf,sizeof(buf)+1)==0);
  assert(range_bufinbuf(buf,sizeof(buf),buf-1,sizeof(buf))==0);
  assert(range_bufinbuf(buf-1,sizeof(buf)+1,buf,sizeof(buf))==1);
  assert(range_bufinbuf(buf-1,sizeof(buf)+2,buf,sizeof(buf))==1);
  assert(range_bufinbuf(0,sizeof(buf),(void*)1,1)==0);
  assert(range_bufinbuf(buf,(unsigned long)-1,buf,1)==0);
  /* the quintessential integer overflow exploit scenario */
  assert(range_bufinbuf(buf,sizeof(buf),buf+10,(unsigned long)-5)==0);
  assert(range_bufinbuf(buf,sizeof(buf),buf+10,sizeof(buf))==0);
  assert(range_bufinbuf(buf,sizeof(buf),buf+10,sizeof(buf)-10)==1);

  /* range_arrayinbuf */
  assert(range_arrayinbuf(buf,sizeof(buf),buf,1,10)==1);
  assert(range_arrayinbuf(buf,sizeof(buf),buf+sizeof(buf)-10,1,10)==1);
  assert(range_arrayinbuf(buf,sizeof(buf),buf-1,1,10)==0);
  assert(range_arrayinbuf(buf,sizeof(buf),buf+1,1,1000)==0);
  assert(range_arrayinbuf(buf,sizeof(buf),buf-1,1,1002)==0);
  /* now overflow the array parameters */
  assert(range_arrayinbuf(buf,sizeof(buf),buf+10,0x10000,0x10000)==0);
  assert(range_arrayinbuf(buf,sizeof(buf),buf+10,0x80000000,2)==0);
  assert(range_arrayinbuf(buf,sizeof(buf),buf+10,0xffffffff,1)==0);

  /* range_strinbuf */
  assert(range_strinbuf(buf,sizeof(buf),0)==0);
  assert(range_strinbuf(buf,sizeof(buf),buf+sizeof(buf))==0);
  {
    char* x="fnord";
    assert(range_strinbuf(x,5,x)==0);
    assert(range_strinbuf(x,6,x)==1);
    assert(range_strinbuf(x,6,x+5)==1);
    assert(range_strinbuf(x,6,x+6)==0);
  }

  /* range_str2inbuf */
  assert(range_str2inbuf(buf,sizeof(buf),0)==0);
  assert(range_str2inbuf(buf,sizeof(buf),buf+sizeof(buf))==0);
  {
    uint16_t y[6];
    int i;
    for (i=0; i<6; ++i) y[i]="fnord"[i];
    assert(range_str2inbuf(y,5*2,y)==0);
    assert(range_str2inbuf(y,5*2+1,y)==0);
    assert(range_str2inbuf(y,sizeof(y),y)==1);
    assert(range_str2inbuf(y,sizeof(y),y+5)==1);
    assert(range_str2inbuf(y,sizeof(y),y+6)==0);
  }

  /* range_str4inbuf */
  assert(range_str4inbuf(buf,sizeof(buf),0)==0);
  assert(range_str4inbuf(buf,sizeof(buf),buf+sizeof(buf))==0);
  {
    uint32_t y[6];
    int i;
    for (i=0; i<6; ++i) y[i]="fnord"[i];
    assert(range_str4inbuf(y,5*4,y)==0);
    assert(range_str4inbuf(y,5*4+3,y)==0);
    assert(range_str4inbuf(y,sizeof(y),y)==1);
    assert(range_str4inbuf(y,sizeof(y),y+5)==1);
    assert(range_str4inbuf(y,sizeof(y),y+6)==0);
  }

  assert(range_ptrinbuf2(buf,buf+sizeof(buf),buf));
  assert(range_ptrinbuf2(buf+sizeof(buf),buf,buf)==0);
  assert(range_ptrinbuf2(buf,buf+sizeof(buf),buf+sizeof(buf)-1));
  assert(range_ptrinbuf2(buf,buf+sizeof(buf),buf+sizeof(buf))==0);
  assert(range_ptrinbuf2(buf,buf,buf)==0);
  assert(range_ptrinbuf2(0,buf+100,buf)==0);

  assert(range_validbuf2(buf,buf+100));
  assert(range_validbuf2(buf,buf-1)==0);
  assert(range_validbuf2(buf,buf));
  assert(range_validbuf2(NULL,buf+100)==0);
}

void check_intof() {
  /* since the macros are type independent, we only check one signed and
   * one unsigned type */

  /* some checks are redundant, but we want to cover off-by-one cases */
  {
    int a;
    a=0; assert(add_of(a,INT_MAX-3,1)==0 && a==INT_MAX-2);
    a=0; assert(add_of(a,INT_MAX-3,5)==1);
    a=0; assert(add_of(a,INT_MAX-3,3)==0 && a==INT_MAX);
    a=0; assert(add_of(a,INT_MAX-3,4)==1);
    a=0; assert(add_of(a,0,INT_MIN)==0 && a==INT_MIN);
    a=0; assert(add_of(a,-1,INT_MIN)==1);
    a=0; assert(add_of(a,1,INT_MAX)==1);

    /* now let's see what happens if the addition has a wider data
     * type than the destionation */
    a=0; assert(add_of(a,0x100000000ll,-0x80000000ll)==1);
    a=0; assert(add_of(a,0x100000000ll,-0x80000001ll)==0 && a==0x7fffffff);
    a=0; assert(add_of(a,0x100000000ll,-10)==1);
    a=0; assert(add_of(a,-0x90000000ll,0x10000000)==0 && a==0x80000000);

    /* what if we add two ints, the result does not overflow but is
     * still negative, but we assign it to an unsigned int? */
    a=0; assert(add_of(a,0x7ffffff0,0x10)==1);
  }

  {
    unsigned int a;
    a=0; assert(add_of(a,UINT_MAX-3,1)==0 && a==UINT_MAX-2);
    a=0; assert(add_of(a,UINT_MAX-3,5)==1);
    a=0; assert(add_of(a,UINT_MAX-3,4)==1);
    a=0; assert(add_of(a,2,-3)==1);
    a=23; assert(add_of(a,2,-2)==0 && a==0);
    a=23; assert(sub_of(a,(int)0x80000000,(int)-2147483648)==0 && a==0);
    a=0; assert(add_of(a,(int)0x7fffffff,(int)-2147483648)==1);
    a=0; assert(add_of(a,1,UINT_MAX)==1);

    /* now let's see what happens if the addition has a wider data
     * type than the destionation */
    a=0; assert(add_of(a,0x100000000ll,-5)==0 && a==4294967291);
    a=0; assert(add_of(a,-0x100000010ll,5)==1);
    a=0; assert(add_of(a,0x100000010ll,-5)==1);

    /* what if we add two ints, the result does not overflow but is
     * still negative, but we assign it to an unsigned int? */
    a=0; assert(add_of(a,-10,-5)==1);
  }

  {
    int a;
    a=0; assert(sub_of(a,10,5)==0 && a==5);
    a=0; assert(sub_of(a,5,10)==0 && a==-5);
    a=0; assert(sub_of(a,INT_MIN,10)==1);
    a=0; assert(sub_of(a,INT_MIN,-10)==0 && a==INT_MIN+10);
    a=0; assert(sub_of(a,INT_MAX,-10)==1);
    a=0; assert(sub_of(a,INT_MAX,10)==0 && a==INT_MAX-10);
  }

  {
    unsigned long long a;
    a=0; assert(add_of(a,0xfffffff0ul,0x10)==0 && a==0x100000000);
  }

}

int main() {

  check_minmax();
  check_assign();
  check_intof();
  check_rangeptrbuf();

  puts("all tests ok");
  return 0;
}
