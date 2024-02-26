#include "fmt.h"
#include "scan.h"
#include "str.h"
#include "uint16.h"
#include "uint32.h"
#include "stralloc.h"
#include "socket.h"
#include "buffer.h"
#include "ip4.h"
#include "ip6.h"
#include "mmap.h"
#include "open.h"
#include "byte.h"
#include "textcode.h"
#include "dns.h"
#include "case.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "errmsg.h"
#include "iob.h"
#include "safemult.h"
#include "iarray.h"
#include "critbit.h"
#include <assert.h>

#include "CAS.h"

#include "io_internal.h"

#define rdtscl(low) \
     __asm__ __volatile__ ("lfence\nrdtsc" : "=a" (low) : : "edx")

// #define atomic_add(mem,val) asm volatile ("lock; add%z0 %1, %0": "+m" (mem): "ir" (val))

static int64 writecb(int64 fd,const void* buf,uint64 n) {
  (void)fd;
  (void)buf;
  (void)n;
#if 0
  int r;
  int todo=n>=65536?65536:n;
  r=write(fd,buf,todo);
  if (r==-1 && r!=EAGAIN) r=-3;
  return r;
#endif
  return -1;
}

static int ret0(const char* s,void* foo) {
  (void)foo;
  assert(strcmp(s,"fnord")==0);
  return 0;
}

static int ret1(const char* s,void* foo) {
  static int i;
  (void)foo;
  switch (i) {
  case 0: assert(strcmp(s,"fnord")==0); break;
  case 1: assert(strcmp(s,"fnord2")==0); break;
  default: return -1;
  }
  ++i;
  return 1;
}

int main(int argc,char* argv[]) {
  int s=socket_udp6();
#if 0
  char buf[100];
  assert(fmt_varint(buf,1)==1 && buf[0]==1);
  fmt_varint(buf,300);
  assert(fmt_varint(buf,300)==2 && buf[0]==(char)0xac && buf[1]==0x02);
#endif
#if 0
  const char buf[]="fnord\n";
  buffer_puts(buffer_1,buf);
  strcpy((char*)buf,"foo\n");
  buffer_puts(buffer_1,buf);
  buffer_puts(buffer_1,"bar\n");
  buffer_flush(buffer_1);
#endif
#if 0
  static critbit0_tree t;
  assert(critbit0_insert(&t,"fnord")==2);
  assert(critbit0_insert(&t,"fnord2")==2);
  assert(critbit0_insert(&t,"fnord2")==1);
  assert(critbit0_contains(&t,"foo")==0);
  assert(critbit0_contains(&t,"fnord")==1);
  assert(critbit0_allprefixed(&t,"fnord",ret1,NULL)==1);
  assert(critbit0_allprefixed(&t,"fnord",ret0,NULL)==0);
  assert(critbit0_delete(&t,"fnord2")==1);
  assert(critbit0_delete(&t,"foo")==0);
#endif
#if 0
  int s = socket_tcp6();
#endif
#if 0
  iarray i;
  iarray_init(&i,sizeof(size_t));
  printf("%p\n",iarray_get(&i,0));
  printf("%p\n",iarray_allocate(&i,0));
  printf("%p\n",iarray_allocate(&i,0));
  printf("%p\n",iarray_get(&i,0));
#endif
#if 0
  char buf[1024];
  size_t l;
  unsigned char c;
  (void)writecb;
  printf("%d\n",(c=scan_fromhex('.')));
  (void)argc;
  (void)argv;
  assert(fmt_jsonescape(buf,"foo\nbar\\",8)==14 && byte_equal(buf,14,"foo\\u000abar\\\\"));
  memset(buf,0,sizeof(buf));
  assert(scan_jsonescape("foo\\u000abar\\\\",buf,&l)==14 && l==8 && byte_equal(buf,8,"foo\nbar\\"));
  memset(buf,0,sizeof(buf));
  /* example from the json spec: G clef U+1D11E encoded using UTF-16 surrogates*/
  assert(scan_jsonescape("\\uD834\\uDD1Exyz",buf,&l)==15 && l==7 && byte_equal(buf,7,"\xf4\x8d\x84\x9exyz"));

/*
	 1D11E -> 0001 1101 0001 0001 1110
	       -> ______00 __011101 __000100 __011110
	 as utf8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	          11110000 10011101 10000100 10011110
		  f   0    9   d    8   4    9   e
*/

#endif
#if 0
  static size_t x;
  x=23;
  atomic_add(&x,3);
  printf("%u\n",x);
  printf("%u\n",atomic_add_return(&x,-3));
  printf("%u\n",compare_and_swap(&x,26,17));
  printf("%u\n",compare_and_swap(&x,23,17));
#endif

#if 0
  atomic_add(&x,3); printf("%u\n",x);
  x=23;
  atomic_add(&x,3); assert(x==26);
  atomic_or(&x,1); assert(x==27);
  atomic_and(&x,-2); assert(x==26);
#endif

#if 0
  iarray a;
  char* c;
  iarray_init(&a,sizeof(io_entry));
  printf("15 -> %p\n",c=iarray_allocate(&a,15));
  printf("23 -> %p\n",c=iarray_allocate(&a,23));
  printf("1234567 -> %p\n",c=iarray_allocate(&a,1234567));
  printf("23 -> %p\n",iarray_get(&a,23));
#endif
#if 0
  io_batch* b=iob_new(1234);
  int64 fd=open("t.c",0);
  iob_addbuf(b,"fnord",5);
  iob_addfile_close(b,fd,0,7365);
  iob_write(1,b,writecb);
#endif
#if 0
  char dest[1024];
  unsigned long len;
  scan_urlencoded2("libstdc++.tar.gz",dest,&len);
  buffer_putmflush(buffer_1,dest,"\n");
#endif
#if 0
  static stralloc sa;
  stralloc_copym(&sa,"foo ","bar ","baz.\n");
  write(1,sa.s,sa.len);
#endif
#if 0
  buffer_putmflush(buffer_1,"foo ","bar ","baz.\n");
#endif
#if 0
  char* c="fnord";
  int fd=open_read(c);
  errmsg_iam(argv[0]);
  carp("could not open file `",c,"'");
  diesys(23,"could not open file `",c,"'");
#endif
#if 0
  errmsg_warn("could not open file `",c,"'",0);
  errmsg_warnsys("could not open file `",c,"'",0);
#endif
#if 0
  char buf[100]="/usr/bin/sh";
  int len=str_len(buf);
  assert(byte_rchr(buf,len,'/')==8);
  assert(byte_rchr(buf,len,'@')==len);
  assert(byte_rchr(buf,len,'h')==len-1);
  printf("%d\n",byte_rchr("x",1,'x'));
#endif
#if 0
  char buf[IP6_FMT+100];
  int i;
  char ip[16];
  uint32 scope_id;
  char* s="fec0::1:220:e0ff:fe69:ad92%eth0/64";
  char blubip[16]="\0\0\0\0\0\0\0\0\0\0\xff\xff\x7f\0\0\001";
  i=scan_ip6if(s,ip,&scope_id);
  assert(s[i]=='/');
  buffer_put(buffer_1,buf,fmt_ip6if(buf,ip,scope_id));
  buffer_putnlflush(buffer_1);
  buffer_put(buffer_1,buf,fmt_ip6ifc(buf,blubip,scope_id));
  buffer_putnlflush(buffer_1);
  scan_ip6("2001:7d0:0:f015:0:0:0:1",ip);
  buffer_put(buffer_1,buf,fmt_ip6(buf,ip));
  buffer_putnlflush(buffer_1);
#endif
#if 0
  char buf[100];
  int i;
  printf("%d\n",i=fmt_pad(buf,"fnord",5,7,10));
  buf[i]=0;
  puts(buf);
#endif
#if 0
  char ip[16];
  char buf[32];
  printf("%d (expect 2)\n",scan_ip6("::",ip));
  printf("%d (expect 3)\n",scan_ip6("::1",ip));
  printf("%d (expect 16)\n",scan_ip6("fec0:0:0:ffff::1/0",ip));
  printf("%.*s\n",fmt_ip6(buf,ip),buf);
#endif
#if 0
  static stralloc s,t;
  stralloc_copys(&s,"fnord");
  stralloc_copys(&t,"abc"); printf("%d\n",stralloc_diff(&s,&t));
  stralloc_copys(&t,"fnor"); printf("%d\n",stralloc_diff(&s,&t));
  stralloc_copys(&t,"fnord"); printf("%d\n",stralloc_diff(&s,&t));
  stralloc_copys(&t,"fnordh"); printf("%d\n",stralloc_diff(&s,&t));
  stralloc_copys(&t,"hausen"); printf("%d\n",stralloc_diff(&s,&t));
#endif
#if 0
  static stralloc s;
  stralloc_copys(&s,"fnord");
  printf("%d\n",stralloc_diffs(&s,"abc"));
  printf("%d\n",stralloc_diffs(&s,"fnor"));
  printf("%d\n",stralloc_diffs(&s,"fnord"));
  printf("%d\n",stralloc_diffs(&s,"fnordh"));
  printf("%d\n",stralloc_diffs(&s,"hausen"));
#endif
#if 0
  printf("%d\n",case_starts("fnordhausen","FnOrD"));
  printf("%d\n",case_starts("fnordhausen","blah"));
#endif
#if 0
  char buf[]="FnOrD";
  case_lowers(buf);
  puts(buf);
#endif
#if 0
  char buf[100]="foo bar baz";
  printf("%d (expect 7)\n",byte_rchr(buf,11,' '));
#endif
#if 0
  unsigned long size;
  char* buf=mmap_read(argv[1],&size);
  if (buf) {
    unsigned int x=fmt_yenc(0,buf,size);
    unsigned int y;
    char* tmp=malloc(x+1);
    y=fmt_yenc(tmp,buf,size);
    write(1,tmp,x);
  }
#endif
#if 0
  char buf[100];
  char buf2[100];
  unsigned int len,len2;
  buf[fmt_yenc(buf,"http://localhost/~fefe",22)]=0;
  buffer_puts(buffer_1,buf);
  buffer_putsflush(buffer_1,"\n");
  if ((buf[len2=scan_yenc(buf,buf2,&len)])!='\n') {
    buffer_putsflush(buffer_2,"parse error!\n");
    return 1;
  }
  buffer_put(buffer_1,buf2,len2);
  buffer_putsflush(buffer_1,"\n");
  return 0;
#endif
#if 0
  char buf[100];
  char buf2[100];
  unsigned int len,len2;
  buf[fmt_base64(buf,"foo:bar",7)]=0;
  buffer_puts(buffer_1,buf);
  buffer_putsflush(buffer_1,"\n");
  if ((buf[len2=scan_base64(buf,buf2,&len)])!=0) {
    buffer_putsflush(buffer_2,"parse error!\n");
    return 1;
  }
  buffer_put(buffer_1,buf2,len2);
  buffer_putsflush(buffer_1,"\n");
  return 0;
#endif
#if 0
  unsigned long size;
  char* buf=mmap_read(argv[1],&size);
  if (buf) {
    unsigned int x=fmt_uuencoded(0,buf,size);
    unsigned int y;
    char* tmp=malloc(x+1);
    y=fmt_uuencoded(tmp,buf,size);
    write(1,tmp,x);
  }
#endif
#if 0
  char buf[]="00000000000000000000000000000001";
  char ip[16];
  if (scan_ip6_flat(buf,ip) != str_len(buf))
    buffer_putsflush(buffer_2,"parse error!\n");
#endif
#if 0
  int fd=open_read("t.c");
  buffer b;
  char buf[1024];
  char line[20];
  int i;
  buffer_init(&b,read,fd,buf,1024);
  i=buffer_getline(&b,line,19);
  buffer_puts(buffer_1,"getline returned ");
  buffer_putulong(buffer_1,i);
  buffer_puts(buffer_1,"\n");
  buffer_puts(buffer_1,line);
  buffer_flush(buffer_1);
#endif
#if 0
  buffer_putulong(buffer_1,23);
//  buffer_putspace(buffer_1);
  buffer_putsflush(buffer_1,"\n");
//  buffer_flush(buffer_1);
#endif
#if 0
  long a,b,c;
  char buf[4096];
  char buf2[4096];
  memcpy(buf,buf2,4096);
  byte_copy(buf,4096,buf2);
  rdtscl(a);
  memcpy(buf,buf2,4096);
  rdtscl(b);
  byte_copy(buf,4096,buf2);
  rdtscl(c);
  printf("memcpy: %d - byte_copy: %d\n",b-a,c-b);
#endif
#if 0
  char ip[16];
  int i;
  if ((i=scan_ip6(argv[1],ip))) {
    char buf[128];
    buf[fmt_ip6(buf,ip)]=0;
    puts(buf);
  }
#endif
#if 0
  char buf[100];
  strcpy(buf,"foobarbaz");
  buf[fmt_fill(buf,3,5,100)]=0;
  printf("\"%s\"\n",buf);
#endif
#if 0
  unsigned long len;
  char *c=mmap_read("/etc/passwd",&len);
  printf("got map %p of len %lu\n",c,len);
#endif
#if 0
  char c;
  printf("%d\n",buffer_getc(buffer_0,&c));
  printf("%c\n",c);
#endif
#if 0
  char buf[100]="01234567890123456789012345678901234567890123456789";
  long a,b,c;
#endif
#if 0
  buf[ip4_fmt(buf,ip4loopback)]=0;
  buffer_puts(buffer_1small,buf);
  buffer_flush(buffer_1small);
#endif

#if 0
  buf[0]=0;
  buf[fmt_8long(buf,0)]=0;
  puts(buf);
  rdtscl(a);
  c=str_len(buf);
  rdtscl(b);
  /*byte_zero_djb(buf,j); */
//  printf("\n%lu %d\n",b-a,c);
#endif
#if 0
  buffer_puts(buffer_1small,"hello, world\n");
  buffer_flush(buffer_1small);
#endif
#if 0
  int s=socket_tcp4();
  char ip[4]={127,0,0,1};
  int t=socket_connect4(s,ip,80);
#endif
#if 0
  char buf[100]="foo bar baz fnord   ";
  char buf2[100]="foo braz fnord";
  long a,b,c;
  long i=0,j=0,k=0;
  double d;
  uint32 l,m,n;
  stralloc sa={0};
  stralloc_copys(&sa,"fnord");
  stralloc_catlong0(&sa,-23,5);
  stralloc_append(&sa,"\n");
  printf("%d %d\n",str_equal("fnord","fnord1"),str_equal("fnord1","fnord"));
  write(1,sa.s,sa.len);
  printf("%d %d\n",stralloc_starts(&sa,"fnord"),stralloc_starts(&sa,"fnord\na"));

  l=0xdeadbeef;
  uint32_pack_big((char*)&m,l);
  uint32_unpack_big((char*)&m,&n);
  printf("%x %x %x\n",l,m,n);

  rdtscl(a);
/*  i=scan_double("3.1415",&d); */
  rdtscl(b);
  /*byte_zero_djb(buf,j); */
  rdtscl(c);
  printf("%lu %lu\n",b-a,c-b);
#endif
#if 0
  size_t size;
  char* buf=mmap_read(argv[1],&size);
  if (buf) {
    unsigned int x=fmt_urlencoded2(0,buf,size,"x");
    unsigned int y;
    char* tmp=malloc(x+1);
    y=fmt_urlencoded2(tmp,buf,size,"x");
    write(1,tmp,x);
  }
#endif
#if 0
  printf("%d %d\n",strcmp("foo","bar"),str_diff("foo","bar"));
  printf("%d %d\n",strcmp("foo","\xfcar"),str_diff("foo","\xfcar"));
#endif
#if 0
  {
    int16 a;
    int32 b;
    int64 c;
    assert(imult16(4,10000,&a)==0);
    assert(imult16(-4,10000,&a)==0);
    assert(imult16(5,10,&a)==1 && a==50);
    assert(imult16(-3,10000,&a)==1 && a==-30000);

    assert(imult32(0x40000000,2,&b)==0);
    assert(imult32(0x3fffffff,2,&b)==1 && b==0x7ffffffe);

    assert(imult64(0x4000000000000000ll,2,&c)==0);
    assert(imult64(0x3fffffffffffffffll,2,&c)==1 && c==0x7ffffffffffffffell);
  }
#endif
#if 0
  stralloc a;
  printf("%d\n",stralloc_copym(&a,"fnord",", ","foo"));
#endif

  return 0;
}

