#include "ip6.h"
#include "byte.h"
#include <ctype.h>
#include "socket.h"
#include "havealloca.h"

size_t scan_ip6if(const char* src,char* ip,uint32* scope_id) {
  size_t i=scan_ip6(src,ip);
  *scope_id=0;
  if (src[i]=='%') {
    size_t j;
    char* tmp;
    for (j=i+1; isalnum(src[j]); ++j) ;
    if (!src[j])
      tmp=(char*)src+i+1;
    else {
      tmp=alloca(j-i);
      byte_copy(tmp,j-(i+1),src+i+1);
      tmp[j-(i+1)]=0;
    }
    if (*tmp) {
      *scope_id=socket_getifidx(tmp);
      return j;
    }
  }
  return i;
}
