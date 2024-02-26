#include <iob_internal.h>

int64 iob_write2(int64 s,io_batch* b,io_write_callback cb,io_sendfile_callback sfcb) {
  iob_entry* e,* last;
  uint64 total;
  int64 sent;
  long i;

  if (b->bytesleft==0) return 0;
  last=(iob_entry*)(((char*)array_start(&b->b))+array_bytes(&b->b));
  total=0;
  if (!(e=array_get(&b->b,sizeof(iob_entry),b->next)))
    return -3;		/* can't happen error */
  for (i=0; e+i<last; ++i) {
    int thatsit;
    if (!e[i].n) continue;
    if (e[i].type==FROMFILE)
      sent=sfcb(s,e[i].fd,e[i].offset,e[i].n,cb);
    else
      sent=cb(s,e[i].buf+e[i].offset,e[i].n);
    if (sent>0 && (uint64)sent>e[i].n) sent=e[i].n; /* can't happen */
    thatsit=((uint64)sent != e[i].n);
    if (sent<=0)
      return total?total:(uint64)sent;
    e[i].offset+=sent;
    e[i].n-=sent;
    total+=sent;
    b->bytesleft-=sent;
    if (thatsit) break;
  }
  if (total == b->bytesleft)
    iob_reset(b);
  return total;
}

int64 iob_write(int64 s,io_batch* b,io_write_callback cb) {
  return iob_write2(s,b,cb,io_mmapwritefile);
}
