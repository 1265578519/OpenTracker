#include "fmt.h"

size_t fmt_str(char *out,const char *in) {
  register char* s=out;
  register const char* t=in;
  for (;;) {
    if (!*t) break;
                    if (s) { *s=*t; ++s; }
                                           ++t;

    if (!*t) break;
                    if (s) { *s=*t; ++s; }
                                           ++t;

    if (!*t) break;
                    if (s) { *s=*t; ++s; }
                                           ++t;

    if (!*t) break;
                    if (s) { *s=*t; ++s; }
                                           ++t;
  }
  return (size_t)(t-in);
}
