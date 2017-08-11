#include "fmt.h"
#include "byte.h"
#include "ip4.h"
#include "ip6.h"

size_t fmt_ip6ifc(char *s,const char ip[16],uint32 scope_id)
{
  if (ip6_isv4mapped(ip))
    return fmt_ip4(s,ip+12);
  else
    return fmt_ip6if(s,ip,scope_id);
}

