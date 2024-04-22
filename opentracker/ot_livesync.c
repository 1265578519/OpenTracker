/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
 It is considered beerware. Prost. Skol. Cheers or whatever.

 $id$ */

/* System */
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

/* Libowfat */
#include "byte.h"
#include "ip6.h"
#include "ndelay.h"
#include "socket.h"

/* Opentracker */
#include "ot_accesslist.h"
#include "ot_livesync.h"
#include "ot_mutex.h"
#include "ot_stats.h"
#include "trackerlogic.h"

#ifdef WANT_SYNC_LIVE

char groupip_1[4] = {224, 0, 23, 5};

#define LIVESYNC_INCOMING_BUFFSIZE        (256 * 256)

#define LIVESYNC_OUTGOING_BUFFSIZE_PEERS  1480
#define LIVESYNC_OUTGOING_WATERMARK_PEERS (sizeof(ot_peer) + sizeof(ot_hash))

#define LIVESYNC_MAXDELAY                 15 /* seconds */

enum { OT_SYNC_PEER4, OT_SYNC_PEER6 };

/* Forward declaration */
static void           *livesync_worker(void *args);

/* For outgoing packets */
static int64           g_socket_in    = -1;

/* For incoming packets */
static int64           g_socket_out   = -1;

static pthread_mutex_t g_outbuf_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
  uint8_t data[LIVESYNC_OUTGOING_BUFFSIZE_PEERS];
  size_t  fill;
  ot_time next_packet_time;
} sync_buffer;

static sync_buffer g_v6_buf;
static sync_buffer g_v4_buf;

static pthread_t   thread_id;
void livesync_init() {

  if (g_socket_in == -1)
    exerr("No socket address for live sync specified.");

  /* Prepare outgoing peers buffer */
  memcpy(g_v6_buf.data, &g_tracker_id, sizeof(g_tracker_id));
  memcpy(g_v4_buf.data, &g_tracker_id, sizeof(g_tracker_id));

  uint32_pack_big((char *)g_v6_buf.data + sizeof(g_tracker_id), OT_SYNC_PEER6);
  uint32_pack_big((char *)g_v4_buf.data + sizeof(g_tracker_id), OT_SYNC_PEER4);

  g_v6_buf.fill             = sizeof(g_tracker_id) + sizeof(uint32_t);
  g_v4_buf.fill             = sizeof(g_tracker_id) + sizeof(uint32_t);

  g_v6_buf.next_packet_time = g_now_seconds + LIVESYNC_MAXDELAY;
  g_v4_buf.next_packet_time = g_now_seconds + LIVESYNC_MAXDELAY;

  pthread_create(&thread_id, NULL, livesync_worker, NULL);
}

void livesync_deinit() {
  if (g_socket_in != -1)
    close(g_socket_in);
  if (g_socket_out != -1)
    close(g_socket_out);

  pthread_cancel(thread_id);
}

void livesync_bind_mcast(ot_ip6 ip, uint16_t port) {
  char  tmpip[4] = {0, 0, 0, 0};
  char *v4ip;

  if (!ip6_isv4mapped(ip))
    exerr("v6 mcast support not yet available.");
  v4ip = ip + 12;

  if (g_socket_in != -1)
    exerr("Error: Livesync listen ip specified twice.");

  if ((g_socket_in = socket_udp4()) < 0)
    exerr("Error: Cant create live sync incoming socket.");
  ndelay_off(g_socket_in);

  if (socket_bind4_reuse(g_socket_in, tmpip, port) == -1)
    exerr("Error: Cant bind live sync incoming socket.");

  if (socket_mcjoin4(g_socket_in, groupip_1, v4ip))
    exerr("Error: Cant make live sync incoming socket join mcast group.");

  if ((g_socket_out = socket_udp4()) < 0)
    exerr("Error: Cant create live sync outgoing socket.");
  if (socket_bind4_reuse(g_socket_out, v4ip, port) == -1)
    exerr("Error: Cant bind live sync outgoing socket.");

  socket_mcttl4(g_socket_out, 1);
  socket_mcloop4(g_socket_out, 0);
}

/* Caller MUST hold g_outbuf_mutex. Returns with g_outbuf_mutex unlocked */
static void livesync_issue_peersync(sync_buffer *buf) {
  char   mycopy[LIVESYNC_OUTGOING_BUFFSIZE_PEERS];
  size_t fill = buf->fill;

  memcpy(mycopy, buf->data, fill);
  buf->fill             = sizeof(g_tracker_id) + sizeof(uint32_t);
  buf->next_packet_time = g_now_seconds + LIVESYNC_MAXDELAY;

  /* From now this thread has a local copy of the buffer and
     has modified the protected element */
  pthread_mutex_unlock(&g_outbuf_mutex);

  socket_send4(g_socket_out, mycopy, fill, groupip_1, LIVESYNC_PORT);
}

static void livesync_handle_peersync(struct ot_workstruct *ws, size_t peer_size) {
  size_t off = sizeof(g_tracker_id) + sizeof(uint32_t);

  /* Now basic sanity checks have been done on the live sync packet
     We might add more testing and logging. */
  while ((ssize_t)(off + sizeof(ot_hash) + peer_size) <= ws->request_size) {
    memcpy(&ws->peer, ws->request + off + sizeof(ot_hash), peer_size);
    ws->hash = (ot_hash *)(ws->request + off);

    if (!g_opentracker_running)
      return;

    if (OT_PEERFLAG(ws->peer) & PEER_FLAG_STOPPED)
      remove_peer_from_torrent(FLAG_MCA, ws);
    else
      add_peer_to_torrent_and_return_peers(FLAG_MCA, ws, /* amount = */ 0);

    off += sizeof(ot_hash) + peer_size;
  }

  stats_issue_event(EVENT_SYNC, 0, (ws->request_size - sizeof(g_tracker_id) - sizeof(uint32_t)) / ((ssize_t)sizeof(ot_hash) + peer_size));
}

/* Tickle the live sync module from time to time, so no events get
   stuck when there's not enough traffic to fill udp packets fast
   enough */
void livesync_ticker() {
  /* livesync_issue_peersync sets g_next_packet_time */
  pthread_mutex_lock(&g_outbuf_mutex);
  if (g_now_seconds > g_v6_buf.next_packet_time && g_v6_buf.fill > sizeof(g_tracker_id) + sizeof(uint32_t))
    livesync_issue_peersync(&g_v6_buf);
  else
    pthread_mutex_unlock(&g_outbuf_mutex);

  pthread_mutex_lock(&g_outbuf_mutex);
  if (g_now_seconds > g_v4_buf.next_packet_time && g_v4_buf.fill > sizeof(g_tracker_id) + sizeof(uint32_t))
    livesync_issue_peersync(&g_v4_buf);
  else
    pthread_mutex_unlock(&g_outbuf_mutex);
}

/* Inform live sync about whats going on. */
void livesync_tell(struct ot_workstruct *ws) {
  size_t       peer_size; /* initialized in next line */
  ot_peer     *peer_src = peer_from_peer6(&ws->peer, &peer_size);
  sync_buffer *dest_buf = peer_size == OT_PEER_SIZE6 ? &g_v6_buf : &g_v4_buf;

  pthread_mutex_lock(&g_outbuf_mutex);

  memcpy(dest_buf->data + dest_buf->fill, ws->hash, sizeof(ot_hash));
  dest_buf->fill += sizeof(ot_hash);

  memcpy(dest_buf->data + dest_buf->fill, peer_src, peer_size);
  dest_buf->fill += peer_size;

  if (dest_buf->fill >= LIVESYNC_OUTGOING_BUFFSIZE_PEERS - LIVESYNC_OUTGOING_WATERMARK_PEERS)
    livesync_issue_peersync(dest_buf);
  else
    pthread_mutex_unlock(&g_outbuf_mutex);
}

static void *livesync_worker(void *args) {
  struct ot_workstruct ws;
  ot_ip6               in_ip;
  uint16_t             in_port;

  (void)args;

  /* Initialize our "thread local storage" */
  ws.inbuf = ws.request = malloc(LIVESYNC_INCOMING_BUFFSIZE);
  ws.outbuf = ws.reply = 0;

  memcpy(in_ip, V4mappedprefix, sizeof(V4mappedprefix));

  while (1) {
    ws.request_size = socket_recv4(g_socket_in, (char *)ws.inbuf, LIVESYNC_INCOMING_BUFFSIZE, 12 + (char *)in_ip, &in_port);

    /* Expect at least tracker id and packet type */
    if (ws.request_size <= (ssize_t)(sizeof(g_tracker_id) + sizeof(uint32_t)))
      continue;
    if (!accesslist_is_blessed(in_ip, OT_PERMISSION_MAY_LIVESYNC))
      continue;
    if (!memcmp(ws.inbuf, &g_tracker_id, sizeof(g_tracker_id))) {
      /* TODO: log packet coming from ourselves */
      continue;
    }

    switch (uint32_read_big(sizeof(g_tracker_id) + (char *)ws.inbuf)) {
    case OT_SYNC_PEER6:
      livesync_handle_peersync(&ws, OT_PEER_SIZE6);
      break;
    case OT_SYNC_PEER4:
      livesync_handle_peersync(&ws, OT_PEER_SIZE4);
      break;
    default:
      break;
    }
  }

  /* Never returns. */
  return NULL;
}

#endif
