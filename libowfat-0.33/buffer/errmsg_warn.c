#include <stdarg.h>
#include <sys/types.h>
#include "errmsg.h"
#include "errmsg_int.h"
#include "str.h"

void errmsg_warn(const char* message, ...) {
  va_list a;
  va_start(a,message);
  errmsg_write(2,0,message,a);
}
