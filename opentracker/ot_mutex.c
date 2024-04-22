/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

/* System */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/uio.h>

/* Libowfat */
#include "byte.h"
#include "io.h"
#include "uint32.h"

/* Opentracker */
#include "ot_iovec.h"
#include "ot_mutex.h"
#include "ot_stats.h"
#include "trackerlogic.h"

/* #define MTX_DBG( STRING ) fprintf( stderr, STRING ) */
#define MTX_DBG(STRING)

/* Our global all torrents list */
static ot_vector       all_torrents[OT_BUCKET_COUNT];
static pthread_mutex_t bucket_mutex[OT_BUCKET_COUNT];
static size_t          g_torrent_count;

/* Self pipe from opentracker.c */
extern int             g_self_pipe[2];

ot_vector *mutex_bucket_lock(int bucket) {
  pthread_mutex_lock(bucket_mutex + bucket);
  return all_torrents + bucket;
}

ot_vector *mutex_bucket_lock_by_hash(ot_hash const hash) { return mutex_bucket_lock(uint32_read_big((const char *)hash) >> OT_BUCKET_COUNT_SHIFT); }

void mutex_bucket_unlock(int bucket, int delta_torrentcount) {
  pthread_mutex_unlock(bucket_mutex + bucket);
  g_torrent_count += delta_torrentcount;
}

void mutex_bucket_unlock_by_hash(ot_hash const hash, int delta_torrentcount) {
  mutex_bucket_unlock(uint32_read_big((char *)hash) >> OT_BUCKET_COUNT_SHIFT, delta_torrentcount);
}

size_t mutex_get_torrent_count() { return g_torrent_count; }

/* TaskQueue Magic */

struct ot_task {
  ot_taskid       taskid;
  ot_tasktype     tasktype;
  int64           sock;
  int             iovec_entries;
  struct iovec   *iovec;
  struct ot_task *next;
};

static ot_taskid       next_free_taskid = 1;
static struct ot_task *tasklist;
static pthread_mutex_t tasklist_mutex;
static pthread_cond_t  tasklist_being_filled;

int mutex_workqueue_pushtask(int64 sock, ot_tasktype tasktype) {
  struct ot_task **tmptask, *task;

  task = malloc(sizeof(struct ot_task));
  if (!task)
    return -1;

  task->taskid        = 0;
  task->tasktype      = tasktype;
  task->sock          = sock;
  task->iovec_entries = 0;
  task->iovec         = NULL;
  task->next          = 0;

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  /* Skip to end of list */
  tmptask = &tasklist;
  while (*tmptask)
    tmptask = &(*tmptask)->next;
  *tmptask = task;

  /* Inform waiting workers and release lock */
  pthread_cond_broadcast(&tasklist_being_filled);
  pthread_mutex_unlock(&tasklist_mutex);
  return 0;
}

void mutex_workqueue_canceltask(int64 sock) {
  struct ot_task **task;

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  for (task = &tasklist; *task; task = &((*task)->next))
    if ((*task)->sock == sock) {
      struct iovec   *iovec = (*task)->iovec;
      struct ot_task *ptask = *task;
      int             i;

      /* Free task's iovec */
      for (i = 0; i < (*task)->iovec_entries; ++i)
        free(iovec[i].iov_base);

      *task = (*task)->next;
      free(ptask);
      break;
    }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);
}

ot_taskid mutex_workqueue_poptask(ot_tasktype *tasktype) {
  struct ot_task *task;
  ot_taskid       taskid = 0;

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  while (!taskid) {
    /* Skip to the first unassigned task this worker wants to do */
    for (task = tasklist; task; task = task->next)
      if (!task->taskid && (TASK_CLASS_MASK & task->tasktype) == *tasktype) {
        /* If we found an outstanding task, assign a taskid to it
           and leave the loop */
        task->taskid = taskid = ++next_free_taskid;
        *tasktype             = task->tasktype;
        break;
      }

    /* Wait until the next task is being fed */
    if (!taskid)
      pthread_cond_wait(&tasklist_being_filled, &tasklist_mutex);
  }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);

  return taskid;
}

void mutex_workqueue_pushsuccess(ot_taskid taskid) {
  struct ot_task **task;

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  for (task = &tasklist; *task; task = &((*task)->next))
    if ((*task)->taskid == taskid) {
      struct ot_task *ptask = *task;
      *task = (*task)->next;
      free(ptask);
      break;
    }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);
}

int mutex_workqueue_pushresult(ot_taskid taskid, int iovec_entries, struct iovec *iovec) {
  struct ot_task *task;
  const char      byte = 'o';

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  for (task = tasklist; task; task = task->next)
    if (task->taskid == taskid) {
      task->iovec_entries = iovec_entries;
      task->iovec         = iovec;
      task->tasktype      = TASK_DONE;
      break;
    }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);

  io_trywrite(g_self_pipe[1], &byte, 1);

  /* Indicate whether the worker has to throw away results */
  return task ? 0 : -1;
}

int mutex_workqueue_pushchunked(ot_taskid taskid, struct iovec *iovec) {
  struct ot_task *task;
  const char      byte = 'o';

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  for (task = tasklist; task; task = task->next)
    if (task->taskid == taskid) {
      if (iovec) {
        if (iovec_append(&task->iovec_entries, &task->iovec, iovec))
          task->tasktype = TASK_DONE_PARTIAL;
        else
          task = NULL;
      } else
        task->tasktype = TASK_DONE;
      break;
    }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);

  io_trywrite(g_self_pipe[1], &byte, 1);

  /* Indicate whether the worker has to throw away results */
  return task ? 0 : -1;
}

int64 mutex_workqueue_popresult(int *iovec_entries, struct iovec **iovec, int *is_partial) {
  struct ot_task **task;
  int64            sock = -1;

  *is_partial = 0;

  /* Want exclusive access to tasklist */
  pthread_mutex_lock(&tasklist_mutex);

  for (task = &tasklist; *task; task = &((*task)->next))
    if (((*task)->tasktype & TASK_CLASS_MASK) == TASK_DONE) {
      struct ot_task *ptask = *task;
      *iovec_entries        = ptask->iovec_entries;
      *iovec                = ptask->iovec;
      sock                  = ptask->sock;

      if ((*task)->tasktype == TASK_DONE) {
        *task = ptask->next;
        free(ptask);
      } else {
        ptask->iovec_entries = 0;
        ptask->iovec         = NULL;
        *is_partial          = 1;
        /* Prevent task from showing up immediately again unless new data was added */
        (*task)->tasktype    = TASK_FULLSCRAPE;
      }
      break;
    }

  /* Release lock */
  pthread_mutex_unlock(&tasklist_mutex);
  return sock;
}

void mutex_init() {
  int i;
  pthread_mutex_init(&tasklist_mutex, NULL);
  pthread_cond_init(&tasklist_being_filled, NULL);
  for (i = 0; i < OT_BUCKET_COUNT; ++i)
    pthread_mutex_init(bucket_mutex + i, NULL);
  byte_zero(all_torrents, sizeof(all_torrents));
}

void mutex_deinit() {
  int i;
  for (i = 0; i < OT_BUCKET_COUNT; ++i)
    pthread_mutex_destroy(bucket_mutex + i);
  pthread_mutex_destroy(&tasklist_mutex);
  pthread_cond_destroy(&tasklist_being_filled);
  byte_zero(all_torrents, sizeof(all_torrents));
}
