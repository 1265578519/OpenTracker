/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IOB_H
#define IOB_H

/* These functions can be used to create a queue of small (or large)
 * buffers and parts of files to be sent out over a socket.  It is meant
 * for writing HTTP servers or the like. */

/* This API works with non-blocking I/O.  Simply call iob_send until it
 * returns 0 (or -1).  The implementation uses sendfile for zero-copy
 * TCP and it will employ writev (or the built-in sendfile writev on
 * BSD) to make sure the output fragments are coalesced into as few TCP
 * frames as possible.  On Linux it will also use the TCP_CORK socket
 * option. */

#include <libowfat/io.h>
#include <libowfat/array.h>
#include <libowfat/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* you should not have to touch any of the internals */
typedef struct io_batch {
  /* array of iob_entry, defined in iob_internal.h */
  array b;
  /* bytes left in the batch (starts as sum of lengths of all iob_entries */
  uint64 bytesleft;
  /* next is the index of the first unsent batch in the array */
  long next,bufs,files;
  /* Ask iob_send to free entries from the array as we go.
   * Usually when iob_send returns 0 or error, you call iob_reset.
   * You still have to call iob_reset even with this flag set, as
   * autofree operates on the buffers in the array but does not free the
   * array itself */
  int autofree;
} io_batch;

/* Initialize an io_batch. Return 0 on success, -1 on malloc failure for
 * embedded array */
int iob_init(io_batch* b,size_t hint_entries);
/* initialize an io_batch that auto-frees entries as soon as
 * iob_send/iob_write have written them. Return 0 on success, -1 on
 * malloc failure for embedded array */
int iob_init_autofree(io_batch* b,size_t hint_entries);

/* malloc and initialize an io_batch */
io_batch* iob_new(size_t hint_entries);
/* malloc and initialize an io_batch that auto-frees entries as soon as
 * iob_send/iob_write have written them */
io_batch* iob_new_autofree(size_t hint_entries);

/* queue buffer in io_batch */
att_readn(2,3)
int iob_addbuf(io_batch* b,const void* buf,uint64 n);

/* queue buffer in io_batch, and give ownership to batch. */
/* the io_batch functions will take care of freeing it. */
att_readn(2,3)
int iob_addbuf_free(io_batch* b,const void* buf,uint64 n);

/* queue mmapped memory reagion in io_batch, and give ownership to batch. */
/* the io_batch functions will take care of unmapping it. */
att_readn(2,3)
int iob_addbuf_munmap(io_batch* b,const void* buf,uint64 n);

/* queue asciiz string in io_batch. */
att_read(2)
int iob_adds(io_batch* b,const char* s);

/* queue asciiz string in io_batch, and give ownership to batch. */
/* the io_batch functions will take care of freeing it. */
att_read(2)
int iob_adds_free(io_batch* b,const char* s);

/* queue file contents in io_batch. */
int iob_addfile(io_batch* b,int64 fd,uint64 off,uint64 n);

/* queue file contents in io_batch, and give ownership to batch */
/* the io_batch functions will take care of closing it. */
int iob_addfile_close(io_batch* b,int64 fd,uint64 off,uint64 n);

/* send (rest of) io_batch over socket s */
/* return number of bytes written.
 * return 0 means whole batch written successfully.
 * return -1 means couldn't write more on non-blocking socket, try again later
 * return -3 means error (errno set) */
int64 iob_send(int64 s,io_batch* b);
/* same as iob_send but don't use callback instead of write and
 * sendfile. Use this to write via OpenSSL or so */
int64 iob_write(int64 s,io_batch* b,io_write_callback cb);
/* same as iob_write if the TLS library also has a sendfile function */
int64 iob_write2(int64 s,io_batch* b,io_write_callback cb,io_sendfile_callback sfcb);

/* iob_reset closes files and frees buffer in the io_batch, and the
 * array with the buffer, but not the batch itself (in case it's a local
 * variable and not malloced) */
void iob_reset(io_batch* b);
/* iob_free does iob_reset but also frees the batch itself */
void iob_free(io_batch* b);
void iob_prefetch(io_batch* b,uint64 bytes);
uint64 iob_bytesleft(const io_batch* b);

#ifdef __cplusplus
}
#endif

#endif
