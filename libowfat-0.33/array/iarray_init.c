#include "iarray.h"

void iarray_init(iarray* ia,size_t elemsize) {
  size_t i;
  ia->elemsize=elemsize;
  for (i=0; i<sizeof(ia->pages)/sizeof(ia->pages[0]); ++i)
    ia->pages[i]=0;
  if (elemsize<1024)
    ia->bytesperpage=4096;
  else if (elemsize<8192)
    ia->bytesperpage=65536;
  else
    ia->bytesperpage=elemsize+sizeof(void*);
  ia->elemperpage=(ia->bytesperpage-sizeof(void*))/elemsize;
}

