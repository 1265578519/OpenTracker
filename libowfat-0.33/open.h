/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef OPEN_H
#define OPEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libowfat/compiler.h>

/* open filename for reading and return the file handle or -1 on error */
att_read(1)
int open_read(const char* filename);

/* create filename for exclusive write only use (mode 0600) and return
 * the file handle or -1 on error */
att_read(1)
int open_excl(const char* filename);

/* open filename for appending  write only use (mode 0600)
 * and return the file handle or -1 on error.
 * All write operation will append after the last byte, regardless of
 * seeking or other processes also appending to the file.  The file will
 * be created if it does not exist. */
att_read(1)
int open_append(const char* filename);

/* open filename for writing (mode 0644).  Create the file if it does
 * not exist, truncate it to zero length otherwise.  Return the file
 * handle or -1 on error. */
att_read(1)
int open_trunc(const char* filename);

/* open filename for writing.  Create the file if it does not exist.
 * Return the file handle or -1 on error. */
att_read(1)
int open_write(const char* filename);

/* open filename for reading and writing.  Create file if not there.
 * Return file handle or -1 on error. */
att_read(1)
int open_rw(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
