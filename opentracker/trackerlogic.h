/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

#ifndef OT_TRACKERLOGIC_H__
#define OT_TRACKERLOGIC_H__

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__linux__) && defined(WANT_ARC4RANDOM)
#include <bsd/stdlib.h>
#endif
#ifdef __FreeBSD__
#define WANT_ARC4RANDOM
#endif

typedef uint8_t ot_hash[20];
typedef time_t  ot_time;
typedef char    ot_ip6[16];
typedef struct { ot_ip6 address; int bits; }
                ot_net;
/* List of peers should fit in a single UDP packet (around 1200 bytes) */
#define OT_MAX_PEERS_UDP6 66
#define OT_MAX_PEERS_UDP4 200

#define OT_IP_SIZE6 16
#define OT_IP_SIZE4 4
#define OT_PORT_SIZE 2
#define OT_FLAG_SIZE 1
#define OT_TIME_SIZE 1

/* Some tracker behaviour tunable */
#define OT_CLIENT_TIMEOUT 30
#define OT_CLIENT_TIMEOUT_CHECKINTERVAL 10
#define OT_CLIENT_TIMEOUT_SEND (60*30)
#define OT_CLIENT_REQUEST_INTERVAL (60*120)
#define OT_CLIENT_REQUEST_VARIATION (60*6)

#define OT_TORRENT_TIMEOUT_HOURS 24
#define OT_TORRENT_TIMEOUT      (60*OT_TORRENT_TIMEOUT_HOURS)

#define OT_CLIENT_REQUEST_INTERVAL_RANDOM ( OT_CLIENT_REQUEST_INTERVAL - OT_CLIENT_REQUEST_VARIATION/2 + (int)( nrand48(ws->rand48_state) % OT_CLIENT_REQUEST_VARIATION ) )

/* If WANT_MODEST_FULLSCRAPES is on, ip addresses may not
   fullscrape more frequently than this amount in seconds */
#define OT_MODEST_PEER_TIMEOUT (60*5)

/* If peers come back before 10 minutes, don't live sync them */
#define OT_CLIENT_SYNC_RENEW_BOUNDARY 10

/* Number of tracker admin ip addresses allowed */
#define OT_ADMINIP_MAX 64
#define OT_MAX_THREADS 64

/* Number of minutes after announce before peer is removed */
#define OT_PEER_TIMEOUT 144

/* We maintain a list of 1024 pointers to sorted list of ot_torrent structs
 Sort key is, of course, its hash */
#define OT_BUCKET_COUNT_BITS 10

#define OT_BUCKET_COUNT (1<<OT_BUCKET_COUNT_BITS)
#define OT_BUCKET_COUNT_SHIFT (32-OT_BUCKET_COUNT_BITS)

/* From opentracker.c */
extern time_t g_now_seconds;
extern volatile int g_opentracker_running;
#define       g_now_minutes (g_now_seconds/60)

extern uint32_t g_tracker_id;
typedef enum { FLAG_TCP, FLAG_UDP, FLAG_MCA, FLAG_SELFPIPE } PROTO_FLAG;

#define OT_PEER_COMPARE_SIZE6 ((OT_IP_SIZE6)+(OT_PORT_SIZE))
#define OT_PEER_COMPARE_SIZE4 ((OT_IP_SIZE4)+(OT_PORT_SIZE))
#define OT_PEER_COMPARE_SIZE_FROM_PEER_SIZE(PEER_SIZE) ((PEER_SIZE)-(OT_TIME_SIZE)-(OT_FLAG_SIZE))

#define OT_PEER_SIZE6 ((OT_TIME_SIZE)+(OT_FLAG_SIZE)+(OT_PEER_COMPARE_SIZE6))
#define OT_PEER_SIZE4 ((OT_TIME_SIZE)+(OT_FLAG_SIZE)+(OT_PEER_COMPARE_SIZE4))

typedef uint8_t ot_peer[1]; /* Generic pointer to a v6 or v4 peer */
typedef uint8_t ot_peer6[OT_PEER_SIZE6];
typedef uint8_t ot_peer4[OT_PEER_SIZE4];
static const uint8_t PEER_FLAG_SEEDING   = 0x80;
static const uint8_t PEER_FLAG_COMPLETED = 0x40;
static const uint8_t PEER_FLAG_STOPPED   = 0x20;
static const uint8_t PEER_FLAG_FROM_SYNC = 0x10;
static const uint8_t PEER_FLAG_LEECHING  = 0x00;

/* Takes an ot_peer6 and returns the proper pointer to the peer and sets peer_size */
ot_peer *peer_from_peer6(ot_peer6 *peer, size_t *peer_size);
size_t   peer_size_from_peer6(ot_peer6 *peer);

/* New style */
#define OT_SETIP(peer,ip)              memcpy((uint8_t*)(peer),(ip),OT_IP_SIZE6)
#define OT_SETPORT(peer,port)          memcpy(((uint8_t*)(peer))+(OT_IP_SIZE6),(port),2)
#define OT_PEERFLAG(peer)             (((uint8_t*)(peer))[(OT_IP_SIZE6)+2])
#define OT_PEERFLAG_D(peer,peersize)  (((uint8_t*)(peer))[(peersize)-2])
#define OT_PEERTIME(peer,peersize)    (((uint8_t*)(peer))[(peersize)-1])

#define PEERS_BENCODED6 "6:peers6"
#define PEERS_BENCODED4 "5:peers"

#define OT_HASH_COMPARE_SIZE (sizeof(ot_hash))

struct ot_peerlist;
typedef struct ot_peerlist ot_peerlist;
typedef struct {
  ot_hash      hash;
  ot_peerlist *peer_list6;
  ot_peerlist *peer_list4;
} ot_torrent;

#include "ot_vector.h"

struct ot_peerlist {
  ot_time        base;
  size_t         seed_count;
  size_t         peer_count;
  size_t         down_count;
/* normal peers vector or
   pointer to ot_vector[32] buckets if data != NULL and space == 0
*/
  ot_vector      peers;
};
#define OT_PEERLIST_HASBUCKETS(peer_list) ((peer_list)->peers.size > (peer_list)->peers.space)

struct ot_workstruct {
  /* Thread specific, static */
  char    *inbuf;
#define   G_INBUF_SIZE    8192
  char    *outbuf;
#define   G_OUTBUF_SIZE   8192
#ifdef    _DEBUG_HTTPERROR
  char    *debugbuf;
#define   G_DEBUGBUF_SIZE 8192
#endif

  /* The peer currently in the working */
  ot_peer6 peer; /* Can fit v6 and v4 peers */

  /* Pointers into the request buffer */
  ot_hash *hash;
  char    *peer_id;

  /* HTTP specific, non static */
  char    *request;
  ssize_t  request_size;
  ssize_t  header_size;
  char    *reply;
  ssize_t  reply_size;

  /* Entropy state for rand48 function so that threads don't need to acquire mutexes for
     global random() or arc4random() state, which causes heavy load on linuxes */
  uint16_t rand48_state[3];

  int      keep_alive;
};

/*
   Exported functions
*/

#ifdef WANT_SYNC_LIVE
#define WANT_SYNC
#endif

#ifdef WANT_SYNC
#define WANT_SYNC_PARAM( param ) , param
#else
#define WANT_SYNC_PARAM( param )
#endif

#ifdef WANT_LOG_NETWORKS
#error Live logging networks disabled at the moment.
#endif

void trackerlogic_init( void );
void trackerlogic_deinit( void );
void exerr( char * message );

/* add_peer_to_torrent does only release the torrent bucket if from_sync is set,
   otherwise it is released in return_peers_for_torrent */
size_t  add_peer_to_torrent_and_return_peers( PROTO_FLAG proto, struct ot_workstruct *ws, size_t amount );
size_t  remove_peer_from_torrent( PROTO_FLAG proto, struct ot_workstruct *ws );
size_t  return_tcp_scrape_for_torrent( ot_hash const *hash_list, int amount, char *reply );
size_t  return_udp_scrape_for_torrent( ot_hash const hash, char *reply );
void    add_torrent_from_saved_state( ot_hash const hash, ot_time base, size_t down_count );

/* torrent iterator */
void iterate_all_torrents( int (*for_each)( ot_torrent* torrent, uintptr_t data ), uintptr_t data );

/* Helper, before it moves to its own object */
void free_peerlist( ot_peerlist *peer_list );

#endif
