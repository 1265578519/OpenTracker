/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

#ifdef WANT_FULLSCRAPE

/* System */
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#ifdef WANT_COMPRESSION_GZIP
#include <zlib.h>
#endif
#ifdef WANT_COMPRESSION_ZSTD
#include <zstd.h>
#endif


/* Libowfat */
#include "byte.h"
#include "io.h"
#include "textcode.h"

/* Opentracker */
#include "ot_fullscrape.h"
#include "ot_iovec.h"
#include "ot_mutex.h"
#include "trackerlogic.h"

/* Fetch full scrape info for all torrents
   Full scrapes usually are huge and one does not want to
   allocate more memory. So lets get them in 512k units
*/
#define OT_SCRAPE_CHUNK_SIZE  (1024 * 1024)

/* "d8:completei%zde10:downloadedi%zde10:incompletei%zdee" */
#define OT_SCRAPE_MAXENTRYLEN 256

/* Forward declaration */
static void fullscrape_make(int taskid, ot_tasktype mode);
#ifdef WANT_COMPRESSION_GZIP
static void fullscrape_make_gzip(int taskid, ot_tasktype mode);
#endif
#ifdef WANT_COMPRESSION_ZSTD
static void fullscrape_make_zstd(int taskid, ot_tasktype mode);
#endif

/* Converter function from memory to human readable hex strings
   XXX - Duplicated from ot_stats. Needs fix. */
static char *to_hex(char *d, uint8_t *s) {
  char *m = "0123456789ABCDEF";
  char *t = d;
  char *e = d + 40;
  while (d < e) {
    *d++ = m[*s >> 4];
    *d++ = m[*s++ & 15];
  }
  *d = 0;
  return t;
}

/* This is the entry point into this worker thread
   It grabs tasks from mutex_tasklist and delivers results back
*/
static void *fullscrape_worker(void *args) {
  (void)args;

  while (g_opentracker_running) {
    ot_tasktype tasktype = TASK_FULLSCRAPE;
    ot_taskid   taskid   = mutex_workqueue_poptask(&tasktype);
#ifdef WANT_COMPRESSION_ZSTD
    if (tasktype & TASK_FLAG_ZSTD)
      fullscrape_make_zstd(taskid, tasktype);
    else
#endif
#ifdef WANT_COMPRESSION_GZIP
    if (tasktype & TASK_FLAG_GZIP)
      fullscrape_make_gzip(taskid, tasktype);
    else
#endif
      fullscrape_make(taskid, tasktype);
    mutex_workqueue_pushchunked(taskid, NULL);
  }
  return NULL;
}

static pthread_t thread_id;
void fullscrape_init( ) {
  pthread_create( &thread_id, NULL, fullscrape_worker, NULL );
}

void fullscrape_deinit( ) {
  pthread_cancel( thread_id );
}

void fullscrape_deliver( int64 sock, ot_tasktype tasktype ) {
  mutex_workqueue_pushtask( sock, tasktype );
}

static char * fullscrape_write_one( ot_tasktype mode, char *r, ot_torrent *torrent, ot_hash *hash ) {
  size_t seed_count = torrent->peer_list6->seed_count + torrent->peer_list4->seed_count;
  size_t peer_count = torrent->peer_list6->peer_count + torrent->peer_list4->peer_count;
  size_t down_count = torrent->peer_list6->down_count + torrent->peer_list4->down_count;

  switch (mode & TASK_TASK_MASK) {
  case TASK_FULLSCRAPE:
  default:
    /* push hash as bencoded string */
    *r++ = '2';
    *r++ = '0';
    *r++ = ':';
    memcpy(r, hash, sizeof(ot_hash));
    r += sizeof(ot_hash);
    /* push rest of the scrape string */
    r += sprintf(r, "d8:completei%zde10:downloadedi%zde10:incompletei%zdee", seed_count, down_count, peer_count - seed_count);

    break;
  case TASK_FULLSCRAPE_TPB_ASCII:
    to_hex(r, *hash);
    r += 2 * sizeof(ot_hash);
    r += sprintf(r, ":%zd:%zd\n", seed_count, peer_count - seed_count);
    break;
  case TASK_FULLSCRAPE_TPB_ASCII_PLUS:
    to_hex(r, *hash);
    r += 2 * sizeof(ot_hash);
    r += sprintf(r, ":%zd:%zd:%zd\n", seed_count, peer_count - seed_count, down_count);
    break;
  case TASK_FULLSCRAPE_TPB_BINARY:
    memcpy(r, *hash, sizeof(ot_hash));
    r                    += sizeof(ot_hash);
    *(uint32_t *)(r + 0)  = htonl((uint32_t)seed_count);
    *(uint32_t *)(r + 4)  = htonl((uint32_t)(peer_count - seed_count));
    r                    += 8;
    break;
  case TASK_FULLSCRAPE_TPB_URLENCODED:
    r += fmt_urlencoded(r, (char *)*hash, 20);
    r += sprintf(r, ":%zd:%zd\n", seed_count, peer_count - seed_count);
    break;
  case TASK_FULLSCRAPE_TRACKERSTATE:
    to_hex(r, *hash);
    r += 2 * sizeof(ot_hash);
    r += sprintf(r, ":%zd:%zd\n", torrent->peer_list6->base, down_count);
    break;
  }
  return r;
}

static void fullscrape_make(int taskid, ot_tasktype mode) {
  int          bucket;
  char        *r, *re;
  struct iovec iovector = {NULL, 0};

  /* Setup return vector... */
  r = iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
  if (!r)
    return;

  /* re points to low watermark */
  re = r + OT_SCRAPE_CHUNK_SIZE - OT_SCRAPE_MAXENTRYLEN;

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE)
    r += sprintf(r, "d5:filesd");

  /* For each bucket... */
  for (bucket = 0; bucket < OT_BUCKET_COUNT; ++bucket) {
    /* Get exclusive access to that bucket */
    ot_vector  *torrents_list = mutex_bucket_lock(bucket);
    ot_torrent *torrents      = (ot_torrent *)(torrents_list->data);
    size_t      i;

    /* For each torrent in this bucket.. */
    for (i = 0; i < torrents_list->size; ++i) {
      r = fullscrape_write_one(mode, r, torrents + i, &torrents[i].hash);

      if (r > re) {
        iovector.iov_len = r - (char *)iovector.iov_base;

        if (mutex_workqueue_pushchunked(taskid, &iovector)) {
          free(iovector.iov_base);
          return mutex_bucket_unlock(bucket, 0);
        }
        /* Allocate a fresh output buffer */
        r = iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
        if (!r)
          return mutex_bucket_unlock(bucket, 0);

        /* re points to low watermark */
        re = r + OT_SCRAPE_CHUNK_SIZE - OT_SCRAPE_MAXENTRYLEN;
      }
    }

    /* All torrents done: release lock on current bucket */
    mutex_bucket_unlock(bucket, 0);

    /* Parent thread died? */
    if (!g_opentracker_running)
      return;
  }

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE)
    r += sprintf(r, "ee");

  /* Send rest of data */
  iovector.iov_len = r - (char *)iovector.iov_base;
  if (mutex_workqueue_pushchunked(taskid, &iovector))
    free(iovector.iov_base);
}

#ifdef WANT_COMPRESSION_GZIP

static void fullscrape_make_gzip(int taskid, ot_tasktype mode) {
  int          bucket;
  char        *r;
  struct iovec iovector = {NULL, 0};
  int          zres;
  z_stream     strm;
  /* Setup return vector... */
  iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
  if (!iovector.iov_base)
    return;

  byte_zero(&strm, sizeof(strm));
  strm.next_out  = (uint8_t *)iovector.iov_base;
  strm.avail_out = OT_SCRAPE_CHUNK_SIZE;
  if (deflateInit2(&strm, 7, Z_DEFLATED, 31, 9, Z_DEFAULT_STRATEGY) != Z_OK)
    fprintf(stderr, "not ok.\n");

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE) {
    strm.next_in  = (uint8_t *)"d5:filesd";
    strm.avail_in = strlen("d5:filesd");
    zres          = deflate(&strm, Z_NO_FLUSH);
  }

  /* For each bucket... */
  for (bucket = 0; bucket < OT_BUCKET_COUNT; ++bucket) {
    /* Get exclusive access to that bucket */
    ot_vector  *torrents_list = mutex_bucket_lock(bucket);
    ot_torrent *torrents      = (ot_torrent *)(torrents_list->data);
    size_t      i;

    /* For each torrent in this bucket.. */
    for (i = 0; i < torrents_list->size; ++i) {
      char compress_buffer[OT_SCRAPE_MAXENTRYLEN];
      r             = fullscrape_write_one(mode, compress_buffer, torrents + i, &torrents[i].hash);
      strm.next_in  = (uint8_t *)compress_buffer;
      strm.avail_in = r - compress_buffer;
      zres          = deflate(&strm, Z_NO_FLUSH);
      if ((zres < Z_OK) && (zres != Z_BUF_ERROR))
        fprintf(stderr, "deflate() failed while in fullscrape_make().\n");

      /* Check if there still is enough buffer left */
      while (!strm.avail_out) {
        iovector.iov_len = (char *)strm.next_out - (char *)iovector.iov_base;

        if (mutex_workqueue_pushchunked(taskid, &iovector)) {
          free(iovector.iov_base);
          return mutex_bucket_unlock(bucket, 0);
        }
        /* Allocate a fresh output buffer */
        iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
        if (!iovector.iov_base) {
          fprintf(stderr, "Out of memory trying to claim ouput buffer\n");
          deflateEnd(&strm);
          return mutex_bucket_unlock(bucket, 0);
        }
        strm.next_out  = (uint8_t *)iovector.iov_base;
        strm.avail_out = OT_SCRAPE_CHUNK_SIZE;
        zres           = deflate(&strm, Z_NO_FLUSH);
        if ((zres < Z_OK) && (zres != Z_BUF_ERROR))
          fprintf(stderr, "deflate() failed while in fullscrape_make().\n");
      }
    }

    /* All torrents done: release lock on current bucket */
    mutex_bucket_unlock(bucket, 0);

    /* Parent thread died? */
    if (!g_opentracker_running) {
      deflateEnd(&strm);
      return;
    }
  }

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE) {
    strm.next_in  = (uint8_t *)"ee";
    strm.avail_in = strlen("ee");
  }

  if (deflate(&strm, Z_FINISH) < Z_OK)
    fprintf(stderr, "deflate() failed while in fullscrape_make()'s endgame.\n");

  iovector.iov_len = (char *)strm.next_out - (char *)iovector.iov_base;
  if (mutex_workqueue_pushchunked(taskid, &iovector)) {
    free(iovector.iov_base);
    deflateEnd(&strm);
    return;
  }

  /* Check if there's a last batch of data in the zlib buffer */
  if (!strm.avail_out) {
      /* Allocate a fresh output buffer */
      iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);

      if (!iovector.iov_base) {
        fprintf(stderr, "Problem with iovec_fix_increase_or_free\n");
        deflateEnd(&strm);
        return;
      }
      strm.next_out  = iovector.iov_base;
      strm.avail_out = OT_SCRAPE_CHUNK_SIZE;
      if (deflate(&strm, Z_FINISH) < Z_OK)
        fprintf(stderr, "deflate() failed while in fullscrape_make()'s endgame.\n");

      /* Only pass the new buffer if there actually was some data left in the buffer */
      iovector.iov_len = (char *)strm.next_out - (char *)iovector.iov_base;
      if (!iovector.iov_len || mutex_workqueue_pushchunked(taskid, &iovector))
        free(iovector.iov_base);
  }

  deflateEnd(&strm);
}
/* WANT_COMPRESSION_GZIP */
#endif

#ifdef WANT_COMPRESSION_ZSTD

static void fullscrape_make_zstd(int taskid, ot_tasktype mode) {
  int            bucket;
  char          *r;
  struct iovec   iovector = {NULL, 0};
  ZSTD_CCtx     *zstream = ZSTD_createCCtx();
  ZSTD_inBuffer  inbuf;
  ZSTD_outBuffer outbuf;
  size_t         more_bytes;

  if (!zstream)
    return;

  /* Setup return vector... */
  iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
  if (!iovector.iov_base) {
    ZSTD_freeCCtx(zstream);
    return;
  }

  /* Working with a compression level 6 is half as fast as level 3, but
     seems to be the last reasonable bump that's worth extra cpu */
  ZSTD_CCtx_setParameter(zstream, ZSTD_c_compressionLevel, 6);

  outbuf.dst  = iovector.iov_base;
  outbuf.size = OT_SCRAPE_CHUNK_SIZE;
  outbuf.pos  = 0;

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE) {
    inbuf.src  = (const void *)"d5:filesd";
    inbuf.size = strlen("d5:filesd");
    inbuf.pos  = 0;
    ZSTD_compressStream2(zstream, &outbuf, &inbuf, ZSTD_e_continue);
  }

  /* For each bucket... */
  for (bucket = 0; bucket < OT_BUCKET_COUNT; ++bucket) {
    /* Get exclusive access to that bucket */
    ot_vector  *torrents_list = mutex_bucket_lock(bucket);
    ot_torrent *torrents      = (ot_torrent *)(torrents_list->data);
    size_t      i;

    /* For each torrent in this bucket.. */
    for (i = 0; i < torrents_list->size; ++i) {
      char compress_buffer[OT_SCRAPE_MAXENTRYLEN];
      r             = fullscrape_write_one(mode, compress_buffer, torrents + i, &torrents[i].hash);
      inbuf.src     = compress_buffer;
      inbuf.size    = r - compress_buffer;
      inbuf.pos     = 0;
      ZSTD_compressStream2(zstream, &outbuf, &inbuf, ZSTD_e_continue);

      /* Check if there still is enough buffer left */
      while (outbuf.pos + OT_SCRAPE_MAXENTRYLEN > outbuf.size) {
        iovector.iov_len = outbuf.size;

        if (mutex_workqueue_pushchunked(taskid, &iovector)) {
          free(iovector.iov_base);
          ZSTD_freeCCtx(zstream);
          return mutex_bucket_unlock(bucket, 0);
        }
        /* Allocate a fresh output buffer */
        iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);
        if (!iovector.iov_base) {
          fprintf(stderr, "Out of memory trying to claim ouput buffer\n");
          ZSTD_freeCCtx(zstream);
          return mutex_bucket_unlock(bucket, 0);
        }

        outbuf.dst  = iovector.iov_base;
        outbuf.size = OT_SCRAPE_CHUNK_SIZE;
        outbuf.pos  = 0;

        ZSTD_compressStream2(zstream, &outbuf, &inbuf, ZSTD_e_continue);
      }
    }

    /* All torrents done: release lock on current bucket */
    mutex_bucket_unlock(bucket, 0);

    /* Parent thread died? */
    if (!g_opentracker_running)
      return;
  }

  if ((mode & TASK_TASK_MASK) == TASK_FULLSCRAPE) {
    inbuf.src  = (const void *)"ee";
    inbuf.size = strlen("ee");
    inbuf.pos  = 0;
  }

  more_bytes = ZSTD_compressStream2(zstream, &outbuf, &inbuf, ZSTD_e_end);

  iovector.iov_len = outbuf.pos;
  if (mutex_workqueue_pushchunked(taskid, &iovector)) {
    free(iovector.iov_base);
    ZSTD_freeCCtx(zstream);
    return;
  }

  /* Check if there's a last batch of data in the zlib buffer */
  if (more_bytes) {
      /* Allocate a fresh output buffer */
      iovector.iov_base = malloc(OT_SCRAPE_CHUNK_SIZE);

      if (!iovector.iov_base) {
        fprintf(stderr, "Problem with iovec_fix_increase_or_free\n");
        ZSTD_freeCCtx(zstream);
        return;
      }

      outbuf.dst  = iovector.iov_base;
      outbuf.size = OT_SCRAPE_CHUNK_SIZE;
      outbuf.pos  = 0;

      ZSTD_compressStream2(zstream, &outbuf, &inbuf, ZSTD_e_end);

      /* Only pass the new buffer if there actually was some data left in the buffer */
      iovector.iov_len = outbuf.pos;
      if (!iovector.iov_len || mutex_workqueue_pushchunked(taskid, &iovector))
        free(iovector.iov_base);
  }

  ZSTD_freeCCtx(zstream);
}
/* WANT_COMPRESSION_ZSTD */
#endif

/* WANT_FULLSCRAPE */
#endif
