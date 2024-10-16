#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <net/if.h>
#endif
#include "socket.h"
#include "haven2i.h"

uint32 socket_getifidx(const char* ifname) {
#ifdef HAVE_N2I
  return if_nametoindex(ifname);
#else
  return 0;
#endif
}
