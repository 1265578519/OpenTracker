/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef IARRAY_H
#define IARRAY_H

/* This header defines an indirect array for use with the io_* routines.
 * Assumptions:
 * - the elements are small (many fit on one page),
 * - the platform has an atomic compare-and-swap instruction
 * - the compiler supports it via __sync_val_compare_and_swap
 *
 * Implementation parameters:
 * - once an element is allocated, it stays allocated and at the same place
 * - there is no deleting elements or moving elements around
 * - we are not coordinating access to elements, only providing pointers to them
 * - new elements are allocated upwards, not arbitrarily (like file descriptors on Unix)
 * - meant for long-running processes, destruction is provided only as an afterthought for completeness
 */

#include <uint64.h>
#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/* The basic data structure is a static array of pointers to pages.
 * Each page also contains a next pointer to form a linked list.
 * To get to element n, you take n % the number of elements in the
 * static array (iarray->pages) to get to the list of pages that
 * contains it. Then keep going to the next page until you are on the
 * right page.
 * Note: The elements on each page are not contiguous. If the fanout is
 * 16, the indices on page 0 are 0, 16, 32, ...
 * To get to element 0, you'd go to iarray->pages[0].data,
 * to get to element 1, you'd go to iarray->pages[1].data,
 * to get to element 16, you'd go to iarray->pages[0].data+iarray->elemsize.
 */
typedef struct _iarray_page {
  struct _iarray_page* next;
  char data[];
} iarray_page;

typedef struct {
  iarray_page* pages[16];
  size_t elemsize,elemperpage,bytesperpage,len;
} iarray;

void iarray_init(iarray* ia,size_t elemsize);
void* iarray_get(iarray* ia,size_t pos);
void* iarray_allocate(iarray* ia,size_t pos);
size_t iarray_length(iarray* ia);

/* WARNING: do not use the array during or after iarray_free, make sure
 * no threads are potentially doing anything with the iarray while it is
 * being freed! */
void iarray_free(iarray* ia);

#endif
