#include <stdarg.h>
#include <sys/types.h>
#include "errmsg.h"
#include "errmsg_int.h"
#include "str.h"
#include <string.h>
#include <errno.h>

void errmsg_warnsys(const char* message, ...) {
  va_list a;
  va_start(a,message);
  errmsg_write(2,strerror(errno),message,a);
}
