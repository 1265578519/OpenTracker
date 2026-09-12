/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef PARSE_H
#define PARSE_H

/* for size_t: */
#include <stddef.h>
/* for uint32_t: */
#include <stdint.h>

#include <libowfat/compiler.h>

#include <libowfat/buffer.h>

#include <libowfat/uint16.h>
#include <libowfat/uint32.h>
#include <libowfat/uint64.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This file declares an API for decoding binary messages.
   Goals:

     - You say in advance if there is a message size limit

     - The object can be bound to a memory buffer of an I/O buffer

     - After having set up the object, you get two APIs:

       1. read bytes
       2. was there an error?

       If you parse a memory buffer manually, you need to do range
       checking for every byte. If you parse from an I/O buffer
       manually, you need to check for end of file or I/O error after
       every byte.

       This API will return 0 bytes and set the error flag when you read
       past the limit. That way you don't have to check after every
       byte, but only once at the end of each sub-message.

     - Many binary protocols have a message length and then sub-packets.
       For example, an IPv4 packet has a header with a length and the IP
       options. With this abstraction here, you would have one
       bytestream for the packet (with size limit set to how many bytes
       came in from the network) and then you would make a new
       bytestream for the IP header and another one for the option
       headers. Each would only let you read bytes from that subregion,
       and would do bounds checking at instantiation time to make sure
       it physically fits into to upper layer space.
*/

struct bytestream {
  enum {
    MEMBUF,
    IOBUF,
    BSTREAM
  } type;
  size_t cur, max;
  union {
    const unsigned char* base;
    struct buffer* b;
    struct bytestream* bs;
  } u;
};

att_write(1)
att_read(2)
void bs_init_membuf(struct bytestream* bs,const unsigned char* membuf,size_t len);

att_write(1)
att_mangle(2)
void bs_init_iobuf(struct bytestream* bs,struct buffer* b);

att_write(1)
att_mangle(2)
void bs_init_iobuf_size(struct bytestream* bs,struct buffer* b,size_t maxlen);

att_write(1)
att_mangle(2)
void bs_init_bstream_size(struct bytestream* bs,struct bytestream* parent,size_t maxlen);

#define BS_FROM_MEMBUF(buf,len) { .type=MEMBUF, .max=(len), .u.base=(const unsigned char*)(buf) }
#define BS_FROM_BUFFER(buffer) { .type=IOBUF, .max=(size_t)-1, .u.b=(buffer) }
#define BS_FROM_BUFFER_SIZE(buffer,len) { .type=IOBUF, .max=(len), u.b=(buffer) }

/* return next byte from stream or 0 if EOF or read error. */
att_mangle(1)
unsigned char bs_get(struct bytestream* bs);

/* like bs_get but do not advance position in stream. */
// This is NOT read-only because it sets bs to error state if it hits EOF
att_mangle(1)
unsigned char bs_peek(struct bytestream* bs);

/* was there a read error or did we attempt to read more than maxlen bytes? */
att_pure
att_read(1)
int bs_err(const struct bytestream* bs);

/* mark bytestream as erroneous */
att_mangle(1)
void bs_seterrorstate(struct bytestream* b);

/* Can we read this much more bytes from the bytestream? */
/* returns 1 for yes, 0 for no */
att_pure
att_read(1)
int bs_capacitycheck(struct bytestream* bs,size_t capacity);

/* Like bs_capacitycheck but will set stream to error state on fail */
att_pure
att_mangle(1)
int bs_capacityassert(struct bytestream* bs,size_t capacity);

/* Return number of bytes left before limit, or 0 on error. */
/* If the backend is an iobuf without limit, return max size_t value. */
att_pure
att_read(1)
size_t bs_capacityleft(const struct bytestream* bs);

/* Assert there are no more bytes left in a bytestream. */
/* Useful to make sure that you parsed the whole packet
 * and there were no slack bytes in the end.
 * Return 1 if there really were no more bytes in the stream.
 * If there ARE bytes left, will set error flag in stream and return 0 */
att_mangle(1)
int bs_nomoredataassert(struct bytestream* bs);

/* Consume all bytes left before limit */
/* Useful for nested structs or when the backing store is an iobuf */
/* Return number of bytes consumed */
att_mangle(1)
size_t bs_consumeleftovers(struct bytestream* bs);

/* Skip n bytes, return number skipped (or 0 if error) */
att_mangle(1)
size_t bs_skip(struct bytestream* bs, size_t n);

/* Read n bytes from stream. Return n.
 * Set stream to error state if not enough space or I/O error. */
att_mangle(1)
att_writen(2,3)
ssize_t prs_readblob(struct bytestream* bs,unsigned char* dest,size_t destlen);

att_mangle(1)
uint16_t prs_u16(struct bytestream* bs);

att_mangle(1)
uint16_t prs_u16_big(struct bytestream* bs);

att_mangle(1)
uint32_t prs_u32(struct bytestream* bs);

att_mangle(1)
uint32_t prs_u32_big(struct bytestream* bs);

att_mangle(1)
uint64_t prs_u64(struct bytestream* bs);

att_mangle(1)
uint64_t prs_u64_big(struct bytestream* bs);

/* Read an asciiz string from the byte stream, up to len bytes (including the 0 terminator). */
/* Return number of bytes consumed (excluding the 0 terminator), i.e. strlen(dest) */
/* If there is no 0 byte in these len bytes, set error flag in stream and return -1. */
/* Calling this function with destsize==0 is an error. */
/* destsize will be clamped to the maximum number representable in ssize_t */
att_mangle(1)
att_writen(2, 3)
ssize_t prs_asciiz(struct bytestream* bs, char* dest, size_t destsize);

/* Some protocols have a fixed field length for a string,
 * If the string is shorter than the field, the rest is filled with 0
 * bytes. But it is not an error if there are no 0 bytes.
 * This function is for those cases (the filename field in the tar file
 * header is an example of this).
 * For a field of length 8, you need to pass destsize as 9 so we can add
 * a 0 terminator. This function will consume the 8 bytes and add a 0 byte.
 * The return value is strlen(dest). */
att_mangle(1)
att_writen(2, 3)
ssize_t prs_asciiz_fixedlen(struct bytestream* bs, char* dest, size_t destsize);

#ifdef __cplusplus
}
#endif

#endif
