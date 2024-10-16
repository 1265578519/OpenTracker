#include "errmsg.h"

const char* argv0;

void errmsg_iam(const char* who) {
  argv0=who;
}

