/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef CDB_H
#define CDB_H

#include <stddef.h>
#include <libowfat/uint32.h>
#include <libowfat/uint64.h>
#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CDB_HASHSTART 5381
att_const
uint32 cdb_hashadd(uint32 h, unsigned char c);

att_pure
att_read(1)
uint32 cdb_hash(const unsigned char *buf, size_t len);

struct cdb {
  char *map; /* 0 if no map is available */
  int64 fd;
  uint32 size; /* initialized if map is nonzero */
  uint32 loop; /* number of hash slots searched under this key */
  uint32 khash; /* initialized if loop is nonzero */
  uint32 kpos; /* initialized if loop is nonzero */
  uint32 hpos; /* initialized if loop is nonzero */
  uint32 hslots; /* initialized if loop is nonzero */
  uint32 dpos; /* initialized if cdb_findnext() returns 1 */
  uint32 dlen; /* initialized if cdb_findnext() returns 1 */
} ;

void cdb_free(struct cdb *);
void cdb_init(struct cdb *, int64 fd);

int cdb_read(struct cdb * restrict, unsigned char * restrict, size_t, uint32);

extern void cdb_findstart(struct cdb *);
extern int cdb_findnext(struct cdb *, const unsigned char *, size_t);
extern int cdb_find(struct cdb *, const unsigned char *, size_t);

extern int cdb_firstkey(struct cdb *c,uint32 *kpos);
extern int cdb_nextkey(struct cdb *c,uint32 *kpos);

extern int cdb_successor(struct cdb *c, const unsigned char *, size_t);

#define cdb_datapos(c) ((c)->dpos)
#define cdb_datalen(c) ((c)->dlen)
#define cdb_keypos(c) ((c)->kpos)
#define cdb_keylen(c) ((c)->dpos-(c)->kpos)

#ifdef __cplusplus
}
#endif

#endif
