/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef DNS_H
#define DNS_H

#include <stralloc.h>
#include <iopause.h>
#include <taia.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DNS_C_IN "\0\1"
#define DNS_C_ANY "\0\377"

#define DNS_T_A "\0\1"
#define DNS_T_NS "\0\2"
#define DNS_T_CNAME "\0\5"
#define DNS_T_SOA "\0\6"
#define DNS_T_PTR "\0\14"
#define DNS_T_HINFO "\0\15"
#define DNS_T_MX "\0\17"
#define DNS_T_TXT "\0\20"
#define DNS_T_RP "\0\21"
#define DNS_T_SIG "\0\30"
#define DNS_T_KEY "\0\31"
#define DNS_T_AAAA "\0\34"
#define DNS_T_AXFR "\0\374"
#define DNS_T_ANY "\0\377"

struct dns_transmit {
  char *query; /* 0, or dynamically allocated */
  unsigned int querylen;
  char *packet; /* 0, or dynamically allocated */
  unsigned int packetlen;
  int s1; /* 0, or 1 + an open file descriptor */
  int tcpstate;
  unsigned int udploop;
  unsigned int curserver;
  struct taia deadline;
  unsigned int pos;
  const char *servers;
  char localip[16];
  unsigned int scope_id;
  char qtype[2];
} ;

void dns_random_init(const char data[128]);
unsigned int dns_random(unsigned int);

void dns_sortip(char *,unsigned int);
void dns_sortip6(char *,unsigned int);

void dns_domain_free(char **);
int dns_domain_copy(char **,const char *);
unsigned int dns_domain_length(const char *);
int dns_domain_equal(const char *,const char *);
int dns_domain_suffix(const char *,const char *);
unsigned int dns_domain_suffixpos(const char *,const char *);
int dns_domain_fromdot(char **,const char *,size_t);
int dns_domain_todot_cat(stralloc *,const char *);

unsigned int dns_packet_copy(const char *,unsigned int,unsigned int,char *,unsigned int);
unsigned int dns_packet_getname(const char *,unsigned int,unsigned int,char **);
unsigned int dns_packet_skipname(const char *,unsigned int,unsigned int);

int dns_transmit_start(struct dns_transmit *,const char servers[256],int,const char *q,const char qtype[2],const char localip[16]);
void dns_transmit_free(struct dns_transmit *);
void dns_transmit_io(struct dns_transmit *,iopause_fd *,struct taia *);
int dns_transmit_get(struct dns_transmit *,const iopause_fd *,const struct taia *);

int dns_resolvconfip(char servers[256]);
int dns_resolve(const char *q,const char qtype[2]);
extern struct dns_transmit dns_resolve_tx;

int dns_ip4_packet(stralloc *,const char *,unsigned int);
int dns_ip4(stralloc *,const stralloc *);
int dns_ip6_packet(stralloc *,const char *,unsigned int);
int dns_ip6(stralloc *,stralloc *);
int dns_name_packet(stralloc *,const char *,unsigned int);
#define DNS_NAME4_DOMAIN 31
void dns_name4_domain(char name[DNS_NAME4_DOMAIN],const char ip[4]);
int dns_name4(stralloc *,const char ip[4]);
int dns_txt_packet(stralloc *,const char *,unsigned int);
int dns_txt(stralloc *,const stralloc *);
int dns_mx_packet(stralloc *,const char *,unsigned int);
int dns_mx(stralloc *,const stralloc *);

int dns_resolvconfrewrite(stralloc *);
int dns_ip4_qualify_rules(stralloc *,stralloc *,const stralloc *,const stralloc *);
int dns_ip4_qualify(stralloc *,stralloc *,const stralloc *);
int dns_ip6_qualify_rules(stralloc *,stralloc *,const stralloc *,const stralloc *);
int dns_ip6_qualify(stralloc *,stralloc *,const stralloc *);

#define DNS_NAME6_DOMAIN (4*16+11)
void dns_name6_domain(char name[DNS_NAME6_DOMAIN],const char ip[16]);
int dns_name6(stralloc *,const char ip[16]);

#ifdef __cplusplus
}
#endif

#endif
