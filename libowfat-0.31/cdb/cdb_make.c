#define _FILE_OFFSET_BITS 64

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "cdb.h"
#include "cdb_make.h"
#ifdef __MINGW32__
#include "windows.h"
#endif

int cdb_make_start(struct cdb_make *c,int64 fd) {
  c->head = 0;
  c->split = 0;
  c->hash = 0;
  c->numentries = 0;
  c->fd = fd;
  c->pos = sizeof c->final;
  buffer_init(&c->b,(void*)write,fd,c->bspace,sizeof c->bspace);
#ifdef __MINGW32__
  return SetFilePointer((HANDLE)(uintptr_t)fd,c->pos,0,FILE_BEGIN);
#else
  return lseek(fd,c->pos,SEEK_SET);
#endif
}

static int posplus(struct cdb_make *c,uint32 len) {
  uint32 newpos = c->pos + len;
  if (newpos < len) { errno = ENOMEM; return -1; }
  c->pos = newpos;
  return 0;
}

int cdb_make_addend(struct cdb_make *c,unsigned long int keylen,unsigned long int datalen,uint32 h) {
  struct cdb_hplist *head;

  head = c->head;
  if (!head || (head->num >= CDB_HPLIST)) {
    head = (struct cdb_hplist *) malloc(sizeof(struct cdb_hplist));
    if (!head) return -1;
    head->num = 0;
    head->next = c->head;
    c->head = head;
  }
  head->hp[head->num].h = h;
  head->hp[head->num].p = c->pos;
  ++head->num;
  ++c->numentries;
  if (posplus(c,8) == -1) return -1;
  if (posplus(c,keylen) == -1) return -1;
  if (posplus(c,datalen) == -1) return -1;
  return 0;
}

int cdb_make_addbegin(struct cdb_make *c,unsigned long int keylen,unsigned long int datalen) {
  char buf[8];

  if (keylen > 0xffffffff) { errno = ENOMEM; return -1; }
  if (datalen > 0xffffffff) { errno = ENOMEM; return -1; }

  uint32_pack(buf,keylen);
  uint32_pack(buf + 4,datalen);
  if (buffer_putalign(&c->b,buf,8) == -1) return -1;
  return 0;
}

int cdb_make_add(struct cdb_make *c,const unsigned char *key,unsigned long int keylen,const unsigned char *data,unsigned long int datalen)
{
  if (cdb_make_addbegin(c,keylen,datalen) == -1) return -1;
  if (buffer_putalign(&c->b,(char*)key,keylen) == -1) return -1;
  if (buffer_putalign(&c->b,(char*)data,datalen) == -1) return -1;
  return cdb_make_addend(c,keylen,datalen,cdb_hash(key,keylen));
}

int cdb_make_finish(struct cdb_make *c)
{
  char buf[8];
  int i;
  uint32 len;
  uint32 u;
  uint32 memsize;
  uint32 count;
  uint32 where;
  struct cdb_hplist *x;
  struct cdb_hp *hp;

  for (i = 0;i < 256;++i)
    c->count[i] = 0;

  for (x = c->head;x;x = x->next) {
    i = x->num;
    while (i--)
      ++c->count[255 & x->hp[i].h];
  }

  memsize = 1;
  for (i = 0;i < 256;++i) {
    u = c->count[i] * 2;
    if (u > memsize)
      memsize = u;
  }

  memsize += c->numentries; /* no overflow possible up to now */
  u = (uint32) 0 - (uint32) 1;
  u /= sizeof(struct cdb_hp);
  if (memsize > u) { errno = ENOMEM; return -1; }

  c->split = (struct cdb_hp *) malloc(memsize * sizeof(struct cdb_hp));
  if (!c->split) return -1;

  c->hash = c->split + c->numentries;

  u = 0;
  for (i = 0;i < 256;++i) {
    u += c->count[i]; /* bounded by numentries, so no overflow */
    c->start[i] = u;
  }

  for (x = c->head;x;x = x->next) {
    i = x->num;
    while (i--)
      c->split[--c->start[255 & x->hp[i].h]] = x->hp[i];
  }

  for (i = 0;i < 256;++i) {
    count = c->count[i];

    len = count + count; /* no overflow possible */
    uint32_pack(c->final + 8 * i,c->pos);
    uint32_pack(c->final + 8 * i + 4,len);

    for (u = 0;u < len;++u)
      c->hash[u].h = c->hash[u].p = 0;

    hp = c->split + c->start[i];
    for (u = 0;u < count;++u) {
      where = (hp->h >> 8) % len;
      while (c->hash[where].p)
	if (++where == len)
	  where = 0;
      c->hash[where] = *hp++;
    }

    for (u = 0;u < len;++u) {
      uint32_pack(buf,c->hash[u].h);
      uint32_pack(buf + 4,c->hash[u].p);
      if (buffer_putalign(&c->b,buf,8) == -1) return -1;
      if (posplus(c,8) == -1) return -1;
    }
  }

  if (buffer_flush(&c->b) == -1) return -1;
  if (lseek(c->fd,0,SEEK_SET) == -1) return -1;
  return buffer_putflush(&c->b,c->final,sizeof c->final);
}
