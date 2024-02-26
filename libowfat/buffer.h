/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef BUFFER_H
#define BUFFER_H

/* for size_t: */
#include <stddef.h>
/* for ssize_t: */
#include <sys/types.h>
/* for strlen */
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct buffer {
  char *x;		/* actual buffer space */
  size_t p;		/* current position */
  size_t n;		/* current size of string in buffer */
  size_t a;		/* allocated buffer size */
  ssize_t (*op)();	/* use read(2) or write(2) */
  void* cookie;			/* used internally by the to-stralloc buffers, and for buffer chaining */
  void (*deinit)(void*);	/* called to munmap/free cleanup, with a pointer to the buffer as argument */
  int fd;		/* passed as first argument to op */
} buffer;

#define BUFFER_INIT(op,fd,buf,len) { (buf), 0, 0, (len), (op), NULL, NULL, (fd) }
#define BUFFER_INIT_FREE(op,fd,buf,len) { (buf), 0, 0, (len), (op), NULL, buffer_free, (fd) }
#define BUFFER_INIT_READ(op,fd,buf,len) BUFFER_INIT(op,fd,buf,len) /*obsolete*/
#define BUFFER_INSIZE 8192
#define BUFFER_OUTSIZE 8192

void buffer_init(buffer* b,ssize_t (*op)(),int fd,char* y,size_t ylen);
void buffer_init_free(buffer* b,ssize_t (*op)(),int fd,char* y,size_t ylen);
void buffer_free(void* buf);
void buffer_munmap(void* buf);
int buffer_mmapread(buffer* b,const char* filename);
void buffer_close(buffer* b);

int buffer_flush(buffer* b);
int buffer_put(buffer* b,const char* x,size_t len);
int buffer_putalign(buffer* b,const char* x,size_t len);
int buffer_putflush(buffer* b,const char* x,size_t len);
int buffer_puts(buffer* b,const char* x);
int buffer_putsalign(buffer* b,const char* x);
int buffer_putsflush(buffer* b,const char* x);

#if defined(__GNUC__) && !defined(__LIBOWFAT_INTERNAL)
/* as a little gcc-specific hack, if somebody calls buffer_puts with a
 * constant string, where we know its length at compile-time, call
 * buffer_put with the known length instead */
#define buffer_puts(b,s) (__builtin_constant_p(s) ? buffer_put(b,s,strlen(s)) : buffer_puts(b,s))
#define buffer_putsflush(b,s) (__builtin_constant_p(s) ? buffer_putflush(b,s,strlen(s)) : buffer_putsflush(b,s))
#endif

int buffer_putm_internal(buffer*b,...);
int buffer_putm_internal_flush(buffer*b,...);
#define buffer_putm(b,...) buffer_putm_internal(b,__VA_ARGS__,(char*)0)
#define buffer_putmflush(b,...) buffer_putm_internal_flush(b,__VA_ARGS__,(char*)0)

int buffer_putspace(buffer* b);
int buffer_putnlflush(buffer* b); /* put \n and flush */

#define buffer_PUTC(s,c) \
  ( ((s)->a != (s)->p) \
    ? ( (s)->x[(s)->p++] = (c), 0 ) \
    : buffer_put((s),&(c),1) \
  )

ssize_t buffer_get(buffer* b,char* x,size_t len);
ssize_t buffer_feed(buffer* b);
ssize_t buffer_getc(buffer* b,char* x);
ssize_t buffer_getn(buffer* b,char* x,size_t len);

/* read bytes until the destination buffer is full (len bytes), end of
 * file is reached or the read char is in charset (setlen bytes).  An
 * empty line when looking for \n will write '\n' to x and return 0.  If
 * EOF is reached, \0 is written to the buffer */
ssize_t buffer_get_token(buffer* b,char* x,size_t len,const char* charset,size_t setlen);
ssize_t buffer_getline(buffer* b,char* x,size_t len);

/* this predicate is given the string as currently read from the buffer
 * and is supposed to return 1 if the token is complete, 0 if not. */
typedef int (*string_predicate)(const char* x,size_t len);

/* like buffer_get_token but the token ends when your predicate says so */
ssize_t buffer_get_token_pred(buffer* b,char* x,size_t len,string_predicate p);

char *buffer_peek(buffer* b);
void buffer_seek(buffer* b,size_t len);

#define buffer_PEEK(s) ( (s)->x + (s)->p )
#define buffer_SEEK(s,len) ( (s)->p += (len) )

#define buffer_GETC(s,c) \
  ( ((s)->p < (s)->n) \
    ? ( *(c) = *buffer_PEEK(s), buffer_SEEK((s),1), 1 ) \
    : buffer_get((s),(c),1) \
  )

int buffer_putulong(buffer *b,unsigned long int l);
int buffer_put8long(buffer *b,unsigned long int l);
int buffer_putxlong(buffer *b,unsigned long int l);
int buffer_putlong(buffer *b,signed long int l);

int buffer_putlonglong(buffer* b,signed long long int l);
int buffer_putulonglong(buffer* b,unsigned long long int l);

int buffer_puterror(buffer* b);
int buffer_puterror2(buffer* b, int errnum);

extern buffer *buffer_0;
extern buffer *buffer_0small;
extern buffer *buffer_1;
extern buffer *buffer_1small;
extern buffer *buffer_2;

#ifdef STRALLOC_H
/* write stralloc to buffer */
int buffer_putsa(buffer* b,const stralloc* sa);
/* write stralloc to buffer and flush */
int buffer_putsaflush(buffer* b,const stralloc* sa);

/* these "read token" functions return 0 if the token was complete or
 * EOF was hit or -1 on error.  In contrast to the non-stralloc token
 * functions, the separator is also put in the stralloc; use
 * stralloc_chop or stralloc_chomp to get rid of it. */

/* WARNING!  These token reading functions will not clear the stralloc!
 * They _append_ the token to the contents of the stralloc.  The idea is
 * that this way these functions can be used on non-blocking sockets;
 * when you get signalled EAGAIN, just call the functions again when new
 * data is available. */

/* read token from buffer to stralloc */
int buffer_get_token_sa(buffer* b,stralloc* sa,const char* charset,size_t setlen);
/* read line from buffer to stralloc */
int buffer_getline_sa(buffer* b,stralloc* sa);

/* same as buffer_get_token_sa but empty sa first */
int buffer_get_new_token_sa(buffer* b,stralloc* sa,const char* charset,size_t setlen);
/* same as buffer_getline_sa but empty sa first */
int buffer_getnewline_sa(buffer* b,stralloc* sa);

typedef int (*sa_predicate)(stralloc* sa);

/* like buffer_get_token_sa but the token ends when your predicate says so */
int buffer_get_token_sa_pred(buffer* b,stralloc* sa,sa_predicate p);
/* same, but clear sa first */
int buffer_get_new_token_sa_pred(buffer* b,stralloc* sa,sa_predicate p);

/* make a buffer from a stralloc.
 * Do not change the stralloc after this! */
void buffer_fromsa(buffer* b,const stralloc* sa);	/* read from sa */
int buffer_tosa(buffer*b,stralloc* sa);		/* write to sa, auto-growing it */
#endif

void buffer_frombuf(buffer* b,const char* x,size_t l);	/* buffer reads from static buffer */

#ifdef ARRAY_H
void buffer_fromarray(buffer* b,array* a);	/* buffer reads from array */
#endif

#ifdef __cplusplus
}
#endif

#endif
