#include "iarray.h"

void* iarray_get(iarray* ia,size_t pos) {
  size_t index;
  iarray_page* p=ia->pages[pos%(sizeof(ia->pages)/sizeof(ia->pages[0]))];
  pos /= sizeof(ia->pages)/sizeof(ia->pages[0]);
  for (index=0; p; p=p->next, index+=ia->elemperpage) {
    if (pos>=index && pos<index+ia->elemperpage)
      return &p->data[(pos-index)*ia->elemsize];
  }
  return 0;
}
