/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef CDB_MAKE_H
#define CDB_MAKE_H

#include <buffer.h>
#include <uint64.h>
#include <uint32.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CDB_HPLIST 1000

struct cdb_hp { uint32 h; uint32 p; } ;

struct cdb_hplist {
  struct cdb_hp hp[CDB_HPLIST];
  struct cdb_hplist *next;
  int num;
} ;

struct cdb_make {
  char bspace[8192];
  char final[2048];
  uint32 count[256];
  uint32 start[256];
  struct cdb_hplist *head;
  struct cdb_hp *split; /* includes space for hash */
  struct cdb_hp *hash;
  uint32 numentries;
  buffer b;
  uint32 pos;
  int64 fd;
} ;

extern int cdb_make_start(struct cdb_make *,int64);
extern int cdb_make_addbegin(struct cdb_make *, size_t, size_t);
extern int cdb_make_addend(struct cdb_make *, size_t, size_t, uint32);
extern int cdb_make_add(struct cdb_make *, const unsigned char *, size_t, const unsigned char *, size_t);
extern int cdb_make_finish(struct cdb_make *);

#ifdef __cplusplus
}
#endif

#endif
