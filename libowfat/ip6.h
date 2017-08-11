/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IP6_H
#define IP6_H

#include "byte.h"
#include "uint32.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t scan_ip6(const char* src,char* ip);
size_t fmt_ip6(char* dest,const char* ip);
size_t fmt_ip6c(char* dest,const char* ip);

size_t scan_ip6if(const char* src,char* ip,uint32* scope_id);
size_t fmt_ip6if(char* dest,const char* ip,uint32 scope_id);
size_t fmt_ip6ifc(char* dest,const char* ip,uint32 scope_id);

size_t scan_ip6_flat(const char *src,char *);
size_t fmt_ip6_flat(char *dest,const char *);

/*
 ip6 address syntax: (h = hex digit), no leading '0' required
   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
   2. any number of 0000 may be abbreviated as "::", but only once
 flat ip6 address syntax:
   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
 */

#define IP6_FMT 40
#define FMT_IP6 40

extern const char V4mappedprefix[12]; /*={0,0,0,0,0,0,0,0,0,0,0xff,0xff}; */
extern const char V6loopback[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}; */
extern const char V6any[16]; /*={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; */

#define ip6_isv4mapped(ip) (byte_equal(ip,12,V4mappedprefix))

#ifdef __cplusplus
}
#endif

#endif
