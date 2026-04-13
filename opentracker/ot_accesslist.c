/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

/* System */
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WANT_DYNAMIC_ACCESSLIST
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

/* Libowfat */
#include "byte.h"
#include "fmt.h"
#include "ip6.h"
#include "mmap.h"
#include "scan.h"

/* Opentracker */
#include "ot_accesslist.h"
#include "ot_vector.h"
#include "trackerlogic.h"

/* GLOBAL VARIABLES */
#ifdef WANT_ACCESSLIST
char *g_accesslist_filename = NULL;
#ifdef WANT_DYNAMIC_ACCESSLIST
char *g_accesslist_pipe_add    = NULL;
char *g_accesslist_pipe_delete = NULL;
#endif
static pthread_mutex_t g_accesslist_mutex;

/* Accesslists are lock free linked lists. We can not make them locking, because every announce
   would try to acquire the mutex, making it the most contested mutex in the whole of opentracker,
   basically creating a central performance choke point.

   The idea is that updating the list heads happens under the g_accesslist_mutex guard and is
   done atomically, while consumers might potentially still hold pointers deeper inside the list.

   Consumers (for now only via accesslist_hashisvalid) will always fetch the list head pointer
   that is guaranteed to live for at least five minutes. This should be many orders of magnitudes
   more than how long it will be needed by the bsearch done on the list. */
struct ot_accesslist;
typedef struct ot_accesslist ot_accesslist;
struct ot_accesslist {
  ot_hash       *list;
  size_t         size;
  ot_time        base;
  ot_accesslist *next;
};
static ot_accesslist *_Atomic g_accesslist = NULL;
#ifdef WANT_DYNAMIC_ACCESSLIST
static ot_accesslist *_Atomic g_accesslist_add    = NULL;
static ot_accesslist *_Atomic g_accesslist_delete = NULL;
#endif

/* Helpers to work on access lists */
static int vector_compare_hash(const void *hash1, const void *hash2) { return memcmp(hash1, hash2, OT_HASH_COMPARE_SIZE); }

static ot_accesslist *accesslist_free(ot_accesslist *accesslist) {
  while (accesslist) {
    ot_accesslist *this_accesslist = accesslist;
    accesslist = this_accesslist->next;
    free(this_accesslist->list);
    free(this_accesslist);
  }
  return NULL;
}

static ot_accesslist *accesslist_make(ot_accesslist *next, size_t size) {
  ot_accesslist *accesslist_new = malloc(sizeof(ot_accesslist));
  if (accesslist_new) {
    accesslist_new->list = size ? malloc(sizeof(ot_hash) * size) : NULL;
    accesslist_new->size = size;
    accesslist_new->base = g_now_minutes;
    accesslist_new->next = next;
    if (size && !accesslist_new->list) {
      free(accesslist_new);
      accesslist_new = NULL;
    }
  }
  return accesslist_new;
}

/* This must be called with g_accesslist_mutex held.
   This will never delete head, because that might still be in use. */
static void accesslist_clean(ot_accesslist *accesslist) {
  while (accesslist && accesslist->next) {
    if (accesslist->next->base + 5 < g_now_minutes)
      accesslist->next = accesslist_free(accesslist->next);
    accesslist = accesslist->next;
  }
}

/* Read initial access list */
static void accesslist_readfile(void) {
  ot_accesslist *accesslist_new;
  ot_hash       *info_hash;
  const char    *map, *map_end, *read_offs;
  size_t         maplen;

  if ((map = mmap_read(g_accesslist_filename, &maplen)) == NULL) {
    char *wd = getcwd(NULL, 0);
    fprintf(stderr, "Warning: Can't open accesslist file: %s (but will try to create it later, if necessary and possible).\nPWD: %s\n", g_accesslist_filename, wd);
    free(wd);
    return;
  }

  /* You need at least 41 bytes to pass an info_hash, make enough room
     for the maximum amount of them */
  accesslist_new = accesslist_make(g_accesslist, maplen / 41);
  if (!accesslist_new) {
    fprintf(stderr, "Warning: Not enough memory to allocate %zd bytes for accesslist buffer. May succeed later.\n", (maplen / 41) * 20);
    mmap_unmap(map, maplen);
    return;
  }
  info_hash = accesslist_new->list;

  /* No use to scan if there's not enough room for another full info_hash */
  map_end   = map + maplen - 40;
  read_offs = map;

  /* We do ignore anything that is not of the form "^[:xdigit:]{40}[^:xdigit:].*" */
  while (read_offs <= map_end) {
    int i;
    for (i = 0; i < (int)sizeof(ot_hash); ++i) {
      int eger1 = scan_fromhex((unsigned char)read_offs[2 * i]);
      int eger2 = scan_fromhex((unsigned char)read_offs[1 + 2 * i]);
      if (eger1 < 0 || eger2 < 0)
        break;
      (*info_hash)[i] = (uint8_t)(eger1 * 16 + eger2);
    }

    if (i == sizeof(ot_hash)) {
      read_offs += 40;

      /* Append accesslist to accesslist vector */
      if (read_offs == map_end || scan_fromhex((unsigned char)*read_offs) < 0)
        ++info_hash;
    }

    /* Find start of next line */
    while (read_offs <= map_end && *(read_offs++) != '\n')
      ;
  }
#ifdef _DEBUG
  fprintf(stderr, "Added %zd info_hashes to accesslist\n", (size_t)(info_hash - accesslist_new->list));
#endif

  mmap_unmap(map, maplen);

  qsort(accesslist_new->list, info_hash - accesslist_new->list, sizeof(*info_hash), vector_compare_hash);
  accesslist_new->size = info_hash - accesslist_new->list;

  /* Now exchange the accesslist vector in the least race condition prone way */
  pthread_mutex_lock(&g_accesslist_mutex);
  accesslist_new->next = g_accesslist;
  g_accesslist         = accesslist_new; /* Only now set a new list */

#ifdef WANT_DYNAMIC_ACCESSLIST
  /* If we have dynamic accesslists, reloading a new one will always void the add/delete lists.
     Insert empty ones at the list head */
  if (g_accesslist_add && (accesslist_new = accesslist_make(g_accesslist_add, 0)) != NULL)
    g_accesslist_add = accesslist_new;
  if (g_accesslist_delete && (accesslist_new = accesslist_make(g_accesslist_delete, 0)) != NULL)
    g_accesslist_delete = accesslist_new;
#endif

  accesslist_clean(g_accesslist);

  pthread_mutex_unlock(&g_accesslist_mutex);
}

int accesslist_hashisvalid(ot_hash hash) {
  /* Get working copy of current access list */
  ot_accesslist *accesslist = g_accesslist;
#ifdef WANT_DYNAMIC_ACCESSLIST
  ot_accesslist *accesslist_add, *accesslist_delete;
#endif
  void *exactmatch = NULL;

  if (accesslist)
    exactmatch = bsearch(hash, accesslist->list, accesslist->size, OT_HASH_COMPARE_SIZE, vector_compare_hash);

#ifdef WANT_DYNAMIC_ACCESSLIST
  /* If we had no match on the main list, scan the list of dynamically added hashes */
  accesslist_add = g_accesslist_add;
  if ((exactmatch == NULL) && accesslist_add)
    exactmatch = bsearch(hash, accesslist_add->list, accesslist_add->size, OT_HASH_COMPARE_SIZE, vector_compare_hash);

  /* If we found a matching hash on the main list, scan the list of dynamically deleted hashes */
  accesslist_delete = g_accesslist_delete;
  if ((exactmatch != NULL) && accesslist_delete && bsearch(hash, accesslist_add->list, accesslist_add->size, OT_HASH_COMPARE_SIZE, vector_compare_hash))
    exactmatch = NULL;
#endif

#ifdef WANT_ACCESSLIST_BLACK
  return exactmatch == NULL;
#else
  return exactmatch != NULL;
#endif
}

static void *accesslist_worker(void *args) {
  int      sig;
  sigset_t signal_mask;

  sigemptyset(&signal_mask);
  sigaddset(&signal_mask, SIGHUP);

  (void)args;

  while (1) {
    if (!g_opentracker_running)
      return NULL;

    /* Initial attempt to read accesslist */
    accesslist_readfile();

    /* Wait for signals */
    while (sigwait(&signal_mask, &sig) != 0 && sig != SIGHUP)
      ;
  }
  return NULL;
}

#ifdef WANT_DYNAMIC_ACCESSLIST
static pthread_t thread_adder_id, thread_deleter_id;
static void     *accesslist_adddel_worker(char *fifoname, ot_accesslist *_Atomic *adding_to, ot_accesslist *_Atomic *removing_from) {
  struct stat st;

  if (!stat(fifoname, &st)) {
    if (!S_ISFIFO(st.st_mode)) {
      fprintf(stderr, "Error when starting dynamic accesslists: Found Non-FIFO file at %s.\nPlease remove it and restart opentracker.\n", fifoname);
      return NULL;
    }
  } else {
    int error = mkfifo(fifoname, 0755);
    if (error && error != EEXIST) {
      fprintf(stderr, "Error when starting dynamic accesslists: Couldn't create FIFO at %s, error: %s\n", fifoname, strerror(errno));
      return NULL;
    }
  }

  while (g_opentracker_running) {
    FILE   *fifo    = fopen(fifoname, "r");
    char   *line    = NULL;
    size_t  linecap = 0;
    ssize_t linelen;

    if (!fifo) {
      fprintf(stderr, "Error when reading dynamic accesslists: Couldn't open FIFO at %s, error: %s\n", fifoname, strerror(errno));
      return NULL;
    }

    while ((linelen = getline(&line, &linecap, fifo)) > 0) {
      ot_hash info_hash;
      int     i;

      printf("Got line %*s", (int)linelen, line);
      /* We do ignore anything that is not of the form "^[:xdigit:]{40}[^:xdigit:].*"
        If there's not enough characters for an info_hash in the line, skip it. */
      if (linelen < 41)
        continue;

      for (i = 0; i < (int)sizeof(ot_hash); ++i) {
        int eger1 = scan_fromhex((unsigned char)line[2 * i]);
        int eger2 = scan_fromhex((unsigned char)line[1 + 2 * i]);
        if (eger1 < 0 || eger2 < 0)
          break;
        ((uint8_t *)info_hash)[i] = (uint8_t)(eger1 * 16 + eger2);
      }
      printf("parsed info_hash %20s\n", info_hash);
      if (i != sizeof(ot_hash))
        continue;

      /* From now on we modify g_accesslist_add and g_accesslist_delete, so prevent the
         other worker threads from doing the same */
      pthread_mutex_lock(&g_accesslist_mutex);

      /* If the info hash is in the removing_from list, create a new head without that entry */
      if (*removing_from && (*removing_from)->list) {
        ot_hash *exactmatch = bsearch(info_hash, (*removing_from)->list, (*removing_from)->size, OT_HASH_COMPARE_SIZE, vector_compare_hash);
        if (exactmatch) {
          ptrdiff_t      off            = exactmatch - (*removing_from)->list;
          ot_accesslist *accesslist_new = accesslist_make(*removing_from, (*removing_from)->size - 1);
          if (accesslist_new) {
            memcpy(accesslist_new->list, (*removing_from)->list, sizeof(ot_hash) * off);
            memcpy(accesslist_new->list + off, (*removing_from)->list + off + 1, (*removing_from)->size - off - 1);
            *removing_from = accesslist_new;
          }
        }
      }

      /* Simple case: there's no adding_to list yet, create one with one member */
      if (!*adding_to) {
        ot_accesslist *accesslist_new = accesslist_make(NULL, 1);
        if (accesslist_new) {
          memcpy(accesslist_new->list, info_hash, sizeof(ot_hash));
          *adding_to = accesslist_new;
        }
      } else {
        int exactmatch   = 0;
        ot_hash *insert_point = binary_search(info_hash, (*adding_to)->list, (*adding_to)->size, OT_HASH_COMPARE_SIZE, sizeof(ot_hash), &exactmatch);

        /* Only if the info hash is not in the adding_to list, create a new head with that entry */
        if (!exactmatch) {
          ot_accesslist *accesslist_new = accesslist_make(*adding_to, (*adding_to)->size + 1);
          ptrdiff_t      off            = insert_point - (*adding_to)->list;
          if (accesslist_new) {
            memcpy(accesslist_new->list, (*adding_to)->list, sizeof(ot_hash) * off);
            memcpy(accesslist_new->list + off, info_hash, sizeof(info_hash));
            memcpy(accesslist_new->list + off + 1, (*adding_to)->list + off, (*adding_to)->size - off);
            *adding_to = accesslist_new;
          }
        }
      }

      pthread_mutex_unlock(&g_accesslist_mutex);
    }

    fclose(fifo);
  }
  return NULL;
}

static void *accesslist_adder_worker(void *args) {
  (void)args;
  return accesslist_adddel_worker(g_accesslist_pipe_add, &g_accesslist_add, &g_accesslist_delete);
}
static void *accesslist_deleter_worker(void *args) {
  (void)args;
  return accesslist_adddel_worker(g_accesslist_pipe_delete, &g_accesslist_delete, &g_accesslist_add);
}
#endif

static pthread_t thread_id;
void accesslist_init() {
  pthread_mutex_init(&g_accesslist_mutex, NULL);
  pthread_create(&thread_id, NULL, accesslist_worker, NULL);
#ifdef WANT_DYNAMIC_ACCESSLIST
  if (g_accesslist_pipe_add)
    pthread_create(&thread_adder_id, NULL, accesslist_adder_worker, NULL);
  if (g_accesslist_pipe_delete)
    pthread_create(&thread_deleter_id, NULL, accesslist_deleter_worker, NULL);
#endif
}

void accesslist_deinit(void) {
  /* Wake up sleeping worker */
  pthread_kill(thread_id, SIGHUP);

  pthread_mutex_lock(&g_accesslist_mutex);

  g_accesslist = accesslist_free(g_accesslist);

#ifdef WANT_DYNAMIC_ACCESSLIST
  g_accesslist_add    = accesslist_free(g_accesslist_add);
  g_accesslist_delete = accesslist_free(g_accesslist_delete);
#endif

  pthread_mutex_unlock(&g_accesslist_mutex);
  pthread_cancel(thread_id);
  pthread_mutex_destroy(&g_accesslist_mutex);
}

void accesslist_cleanup(void) {
  pthread_mutex_lock(&g_accesslist_mutex);

  accesslist_clean(g_accesslist);
#if WANT_DYNAMIC_ACCESSLIST
  accesslist_clean(g_accesslist_add);
  accesslist_clean(g_accesslist_delete);
#endif

  pthread_mutex_unlock(&g_accesslist_mutex);
}
#endif

int address_in_net(const ot_ip6 address, const ot_net *net) {
  int bits = net->bits, checkbits = (0x7f00 >> (bits & 7));
  int result = memcmp(address, &net->address, bits >> 3);
  if (!result && (bits & 7))
    result = (checkbits & address[bits >> 3]) - (checkbits & net->address[bits >> 3]);
  return result == 0;
}

void *set_value_for_net(const ot_net *net, ot_vector *vector, const void *value, const size_t member_size) {
  size_t i;
  int    exactmatch;

  /* Caller must have a concept of ot_net in it's member */
  if (member_size < sizeof(ot_net))
    return 0;

  /* Check each net in vector for overlap */
  uint8_t *member = ((uint8_t *)vector->data);
  for (i = 0; i < vector->size; ++i) {
    if (address_in_net(*(ot_ip6 *)member, net) || address_in_net(net->address, (ot_net *)member))
      return 0;
    member += member_size;
  }

  member = vector_find_or_insert(vector, (void *)net, member_size, sizeof(ot_net), &exactmatch);
  if (member) {
    memcpy(member, net, sizeof(ot_net));
    memcpy(member + sizeof(ot_net), value, member_size - sizeof(ot_net));
  }

  return member;
}

/* Takes a vector filled with { ot_net net, uint8_t[x] value };
   Returns value associated with the net, or NULL if not found */
void *get_value_for_net(const ot_ip6 address, const ot_vector *vector, const size_t member_size) {
  int     exactmatch;
  /* This binary search will return a pointer to the first non-containing network... */
  ot_net *net = binary_search(address, vector->data, vector->size, member_size, sizeof(ot_ip6), &exactmatch);
  if (!net)
    return NULL;
  /* ... so we'll need to move back one step unless we've exactly hit the first address in network */
  if (!exactmatch && ((void *)net > vector->data))
    --net;
  if (!address_in_net(address, net))
    return NULL;
  return (void *)net;
}

#ifdef WANT_FULLLOG_NETWORKS
static ot_vector       g_lognets_list;
ot_log                *g_logchain_first, *g_logchain_last;
static pthread_mutex_t g_lognets_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void loglist_add_network(const ot_net *net) {
  pthread_mutex_lock(&g_lognets_list_mutex);
  set_value_for_net(net, &g_lognets_list, NULL, sizeof(ot_net));
  pthread_mutex_unlock(&g_lognets_list_mutex);
}

void loglist_reset() {
  pthread_mutex_lock(&g_lognets_list_mutex);
  free(g_lognets_list.data);
  g_lognets_list.data = 0;
  g_lognets_list.size = g_lognets_list.space = 0;
  pthread_mutex_unlock(&g_lognets_list_mutex);
}

int loglist_check_address(const ot_ip6 address) {
  int result;
  pthread_mutex_lock(&g_lognets_list_mutex);
  result = (NULL != get_value_for_net(address, &g_lognets_list, sizeof(ot_net)));
  pthread_mutex_unlock(&g_lognets_list_mutex);
  return result;
}
#endif

#ifdef WANT_IP_FROM_PROXY
typedef struct {
  ot_net   *proxy;
  ot_vector networks;
} ot_proxymap;

static ot_vector       g_proxies_list;
static pthread_mutex_t g_proxies_list_mutex = PTHREAD_MUTEX_INITIALIZER;

int proxylist_add_network(const ot_net *proxy, const ot_net *net) {
  ot_proxymap *map;
  int exactmatch, result = 1;
  pthread_mutex_lock(&g_proxies_list_mutex);

  /* If we have a direct hit, use and extend the vector there */
  map = binary_search(proxy, g_proxies_list.data, g_proxies_list.size, sizeof(ot_proxymap), sizeof(ot_net), &exactmatch);

  if (!map || !exactmatch) {
    /* else see, if we've got overlapping networks
       and get a new empty vector if not */
    ot_vector empty;
    memset(&empty, 0, sizeof(ot_vector));
    map = set_value_for_net(proxy, &g_proxies_list, &empty, sizeof(ot_proxymap));
  }

  if (map && set_value_for_net(net, &map->networks, NULL, sizeof(ot_net)))
    result = 1;

  pthread_mutex_unlock(&g_proxies_list_mutex);
  return result;
}

int proxylist_check_proxy(const ot_ip6 proxy, const ot_ip6 address) {
  int result = 0;
  ot_proxymap *map;

  pthread_mutex_lock(&g_proxies_list_mutex);

  if ((map = get_value_for_net(proxy, &g_proxies_list, sizeof(ot_proxymap))))
    if (!address || get_value_for_net(address, &map->networks, sizeof(ot_net)))
      result = 1;

  pthread_mutex_unlock(&g_proxies_list_mutex);
  return result;
}

#endif

static ot_net         g_admin_nets[OT_ADMINIP_MAX];
static ot_permissions g_admin_nets_permissions[OT_ADMINIP_MAX];
static unsigned int   g_admin_nets_count = 0;

int accesslist_bless_net(ot_net *net, ot_permissions permissions) {
  if (g_admin_nets_count >= OT_ADMINIP_MAX)
    return -1;

  memcpy(g_admin_nets + g_admin_nets_count, net, sizeof(ot_net));
  g_admin_nets_permissions[g_admin_nets_count++] = permissions;

#ifdef _DEBUG
  {
    char _debug[512];
    int  off  = snprintf(_debug, sizeof(_debug), "Blessing ip net ");
    off      += fmt_ip6c(_debug + off, net->address);
    if (net->bits < 128) {
      _debug[off++] = '/';
      if (ip6_isv4mapped(net->address))
        off += fmt_long(_debug + off, net->bits - 96);
      else
        off += fmt_long(_debug + off, net->bits);
    }

    if (permissions & OT_PERMISSION_MAY_STAT)
      off += snprintf(_debug + off, 512 - off, " may_fetch_stats");
    if (permissions & OT_PERMISSION_MAY_LIVESYNC)
      off += snprintf(_debug + off, 512 - off, " may_sync_live");
    if (permissions & OT_PERMISSION_MAY_FULLSCRAPE)
      off += snprintf(_debug + off, 512 - off, " may_fetch_fullscrapes");
    if (permissions & OT_PERMISSION_MAY_PROXY)
      off += snprintf(_debug + off, 512 - off, " may_proxy");
    if (!permissions)
      off += snprintf(_debug + off, sizeof(_debug) - off, " nothing");
    _debug[off++] = '.';
    _debug[off++] = '\n';
    (void)write(2, _debug, off);
  }
#endif

  return 0;
}

int accesslist_is_blessed(ot_ip6 ip, ot_permissions permissions) {
  unsigned int i;
  for (i = 0; i < g_admin_nets_count; ++i)
    if (address_in_net(ip, g_admin_nets + i) && (g_admin_nets_permissions[i] & permissions))
      return 1;
  return 0;
}
