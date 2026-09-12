#include "fmt.h"
#include "byte.h"
#include "ip4.h"
#include "ip6.h"

size_t fmt_ip6c(char *s,const char ip[16])
{
  if (ip6_isv4mapped(ip))
    return fmt_ip4(s,ip+12);
  else
    return fmt_ip6(s,ip);
}

