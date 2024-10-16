#include "iob.h"
#include "array.h"

typedef struct iob_entry {
  enum { FROMBUF, FROMFILE } type;
  int64 fd;
  const char* buf;
  uint64 offset,n;
  void (*cleanup)(struct iob_entry* x);
} iob_entry;

int iob_addbuf_internal(io_batch* b,const void* buf,uint64 n,
			void (*cleanup)(struct iob_entry* x));
