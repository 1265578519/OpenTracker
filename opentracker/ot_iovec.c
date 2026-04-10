/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

/* System */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

/* Libowfat */

/* Opentracker */
#include "ot_iovec.h"

void *iovec_increase(size_t *iovec_entries, struct iovec **iovector, size_t new_alloc) {
  void         *new_data;
  size_t        new_entries = 1 + *iovec_entries;
  struct iovec *new_vec     = realloc(*iovector, new_entries * sizeof(struct iovec));

  if (!new_vec)
    return NULL;

  /* Only allocate after we have a place to store the pointer */
  new_data = malloc(new_alloc);
  if (!new_data)
    return NULL;

  new_vec[new_entries - 1].iov_base = new_data;
  new_vec[new_entries - 1].iov_len  = new_alloc;

  *iovector                         = new_vec;
  ++*iovec_entries;
  return new_data;
}

void *iovec_append(size_t *iovec_entries, struct iovec **iovector, struct iovec *append_iovector) {
  size_t        new_entries = *iovec_entries + 1;
  struct iovec *new_vec     = realloc(*iovector, new_entries * sizeof(struct iovec));
  if (!new_vec)
    return NULL;

  /* Take over data from appended iovec */
  new_vec[*iovec_entries].iov_base = append_iovector->iov_base;
  new_vec[*iovec_entries].iov_len  = append_iovector->iov_len;

  append_iovector->iov_base        = NULL;
  append_iovector->iov_len         = 0;

  *iovector                        = new_vec;
  *iovec_entries                   = new_entries;

  return new_vec;
}

void iovec_free(size_t *iovec_entries, struct iovec **iovector) {
  size_t i;
  for (i = 0; i < *iovec_entries; ++i)
    free(((*iovector)[i]).iov_base);
  *iovector      = NULL;
  *iovec_entries = 0;
}

void iovec_fixlast(size_t *iovec_entries, struct iovec **iovector, void *last_ptr) {
  if (*iovec_entries) {
    char  *base      = (char *)((*iovector)[*iovec_entries - 1]).iov_base;
    size_t new_alloc = ((char *)last_ptr) - base;

    ((*iovector)[*iovec_entries - 1]).iov_base = realloc(base, new_alloc);
    ((*iovector)[*iovec_entries - 1]).iov_len  = new_alloc;
  }
}

void *iovec_fix_increase_or_free(size_t *iovec_entries, struct iovec **iovector, void *last_ptr, size_t new_alloc) {
  void *new_data;

  iovec_fixlast(iovec_entries, iovector, last_ptr);

  if (!(new_data = iovec_increase(iovec_entries, iovector, new_alloc)))
    iovec_free(iovec_entries, iovector);

  return new_data;
}

size_t iovec_length(const size_t *iovec_entries, const struct iovec **iovector) {
  size_t i, length = 0;
  for (i = 0; i < *iovec_entries; ++i)
    length += ((*iovector)[i]).iov_len;
  return length;
}
