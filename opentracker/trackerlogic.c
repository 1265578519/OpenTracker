/* This software was written by Dirk Engling <erdgeist@erdgeist.org>
   It is considered beerware. Prost. Skol. Cheers or whatever.

   $id$ */

/* System */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

/* Libowfat */
#include "byte.h"
#include "io.h"
#include "iob.h"
#include "ip6.h"
#include "array.h"

/* Opentracker */
#include "trackerlogic.h"
#include "ot_vector.h"
#include "ot_mutex.h"
#include "ot_stats.h"
#include "ot_clean.h"
#include "ot_http.h"
#include "ot_accesslist.h"
#include "ot_fullscrape.h"
#include "ot_livesync.h"

/* Forward declaration */
size_t return_peers_for_torrent( struct ot_workstruct * ws, ot_torrent *torrent, size_t amount, char *reply, PROTO_FLAG proto );

void free_peerlist( ot_peerlist *peer_list ) {
  if( peer_list->peers.data ) {
    if( OT_PEERLIST_HASBUCKETS( peer_list ) )
      vector_clean_list( (ot_vector*)peer_list->peers.data, peer_list->peers.size );
    else
      free( peer_list->peers.data );
  }
  free( peer_list );
}

void add_torrent_from_saved_state( ot_hash const hash, ot_time base, size_t down_count ) {
  int         exactmatch;
  ot_torrent *torrent;
  ot_vector  *torrents_list = mutex_bucket_lock_by_hash( hash );

  if( !accesslist_hashisvalid( hash ) )
    return mutex_bucket_unlock_by_hash( hash, 0 );

  torrent = vector_find_or_insert( torrents_list, (void*)hash, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch );
  if( !torrent || exactmatch )
    return mutex_bucket_unlock_by_hash( hash, 0 );

  /* Create a new torrent entry, then */
  byte_zero( torrent, sizeof( ot_torrent ) );
  memcpy( torrent->hash, hash, sizeof(ot_hash) );

  if( !( torrent->peer_list6 = malloc( sizeof (ot_peerlist) ) ) ||
      !( torrent->peer_list4 = malloc( sizeof (ot_peerlist) ) ) ) {
    vector_remove_torrent( torrents_list, torrent );
    return mutex_bucket_unlock_by_hash( hash, 0 );
  }

  byte_zero( torrent->peer_list6, sizeof( ot_peerlist ) );
  byte_zero( torrent->peer_list4, sizeof( ot_peerlist ) );
  torrent->peer_list6->base = base;
  torrent->peer_list4->base = base;
  torrent->peer_list6->down_count = down_count;
  torrent->peer_list4->down_count = down_count;

  return mutex_bucket_unlock_by_hash( hash, 1 );
}

size_t add_peer_to_torrent_and_return_peers( PROTO_FLAG proto, struct ot_workstruct *ws, size_t amount ) {
  int           exactmatch, delta_torrentcount = 0;
  ot_torrent   *torrent;
  ot_peer      *peer_dest;
  ot_vector    *torrents_list = mutex_bucket_lock_by_hash( *ws->hash );
  ot_peerlist  *peer_list;
  size_t        peer_size; /* initialized in next line */
  ot_peer const *peer_src = peer_from_peer6(&ws->peer, &peer_size);

  if( !accesslist_hashisvalid( *ws->hash ) ) {
    mutex_bucket_unlock_by_hash( *ws->hash, 0 );
    if( proto == FLAG_TCP ) {
      const char invalid_hash[] = "d14:failure reason63:Requested download is not authorized for use with this tracker.e";
      memcpy( ws->reply, invalid_hash, strlen( invalid_hash ) );
      return strlen( invalid_hash );
    }
    return 0;
  }

  torrent = vector_find_or_insert( torrents_list, (void*)ws->hash, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch );
  if( !torrent ) {
    mutex_bucket_unlock_by_hash( *ws->hash, 0 );
    return 0;
  }

  if( !exactmatch ) {
    /* Create a new torrent entry, then */
    byte_zero( torrent, sizeof(ot_torrent));
    memcpy( torrent->hash, *ws->hash, sizeof(ot_hash) );

    if( !( torrent->peer_list6 = malloc( sizeof (ot_peerlist) ) ) ||
        !( torrent->peer_list4 = malloc( sizeof (ot_peerlist) ) ) ) {
      vector_remove_torrent( torrents_list, torrent );
      mutex_bucket_unlock_by_hash( *ws->hash, 0 );
      return 0;
    }

    byte_zero( torrent->peer_list6, sizeof( ot_peerlist ) );
    byte_zero( torrent->peer_list4, sizeof( ot_peerlist ) );
    delta_torrentcount = 1;
  } else
    clean_single_torrent( torrent );

  torrent->peer_list6->base = g_now_minutes;
  torrent->peer_list4->base = g_now_minutes;

  peer_list = peer_size == OT_PEER_SIZE6 ? torrent->peer_list6 : torrent->peer_list4;

  /* Check for peer in torrent */
  peer_dest = vector_find_or_insert_peer( &(peer_list->peers), peer_src, peer_size, &exactmatch );
  if( !peer_dest ) {
    mutex_bucket_unlock_by_hash( *ws->hash, delta_torrentcount );
    return 0;
  }

  /* Tell peer that it's fresh */
  OT_PEERTIME( ws->peer, OT_PEER_SIZE6 ) = 0;

  /* Sanitize flags: Whoever claims to have completed download, must be a seeder */
  if( ( OT_PEERFLAG( ws->peer ) & ( PEER_FLAG_COMPLETED | PEER_FLAG_SEEDING ) ) == PEER_FLAG_COMPLETED )
    OT_PEERFLAG( ws->peer ) ^= PEER_FLAG_COMPLETED;

  /* If we hadn't had a match create peer there */
  if( !exactmatch ) {

#ifdef WANT_SYNC_LIVE
    if( proto == FLAG_MCA )
      OT_PEERFLAG( ws->peer ) |= PEER_FLAG_FROM_SYNC;
    else
      livesync_tell( ws );
#endif

    peer_list->peer_count++;
    if( OT_PEERFLAG( ws->peer ) & PEER_FLAG_COMPLETED ) {
      peer_list->down_count++;
      stats_issue_event( EVENT_COMPLETED, 0, (uintptr_t)ws );
    }
    if( OT_PEERFLAG(ws->peer) & PEER_FLAG_SEEDING )
      peer_list->seed_count++;

  } else {
    stats_issue_event( EVENT_RENEW, 0, OT_PEERTIME( peer_dest, peer_size ) );
#ifdef WANT_SPOT_WOODPECKER
    if( ( OT_PEERTIME(peer_dest, peer_size) > 0 ) && ( OT_PEERTIME(peer_dest, peer_size) < 20 ) )
      stats_issue_event( EVENT_WOODPECKER, 0, (uintptr_t)&ws->peer );
#endif
#ifdef WANT_SYNC_LIVE
    /* Won't live sync peers that come back too fast. Only exception:
       fresh "completed" reports */
    if( proto != FLAG_MCA ) {
      if( OT_PEERTIME( peer_dest, peer_size ) > OT_CLIENT_SYNC_RENEW_BOUNDARY ||
         ( !(OT_PEERFLAG_D(peer_dest, peer_size) & PEER_FLAG_COMPLETED ) && (OT_PEERFLAG(ws->peer) & PEER_FLAG_COMPLETED ) ) )
        livesync_tell( ws );
    }
#endif

    if(  (OT_PEERFLAG_D(peer_dest, peer_size) & PEER_FLAG_SEEDING )   && !(OT_PEERFLAG(ws->peer) & PEER_FLAG_SEEDING ) )
      peer_list->seed_count--;
    if( !(OT_PEERFLAG_D(peer_dest, peer_size) & PEER_FLAG_SEEDING )   &&  (OT_PEERFLAG(ws->peer) & PEER_FLAG_SEEDING ) )
      peer_list->seed_count++;
    if( !(OT_PEERFLAG_D(peer_dest, peer_size) & PEER_FLAG_COMPLETED ) &&  (OT_PEERFLAG(ws->peer) & PEER_FLAG_COMPLETED ) ) {
      peer_list->down_count++;
      stats_issue_event( EVENT_COMPLETED, 0, (uintptr_t)ws );
    }
    if(   OT_PEERFLAG_D(peer_dest, peer_size) & PEER_FLAG_COMPLETED )
      OT_PEERFLAG( ws->peer ) |= PEER_FLAG_COMPLETED;
  }

  memcpy( peer_dest, peer_src, peer_size );
#ifdef WANT_SYNC
  if( proto == FLAG_MCA ) {
    mutex_bucket_unlock_by_hash( *ws->hash, delta_torrentcount );
    return 0;
  }
#endif

  ws->reply_size = return_peers_for_torrent( ws, torrent, amount, ws->reply, proto );
  mutex_bucket_unlock_by_hash( *ws->hash, delta_torrentcount );
  return ws->reply_size;
}

static size_t return_peers_all( ot_peerlist *peer_list, size_t peer_size, char *reply ) {
  unsigned int bucket, num_buckets = 1;
  ot_vector  * bucket_list = &peer_list->peers;
  size_t       compare_size = OT_PEER_COMPARE_SIZE_FROM_PEER_SIZE(peer_size);
  size_t       result = compare_size * peer_list->peer_count;
  char       * r_end = reply + result;

  if( OT_PEERLIST_HASBUCKETS(peer_list) ) {
    num_buckets = bucket_list->size;
    bucket_list = (ot_vector *)bucket_list->data;
  }

  for( bucket = 0; bucket<num_buckets; ++bucket ) {
    ot_peer *peers = bucket_list[bucket].data;
    size_t   peer_count = bucket_list[bucket].size;
    while( peer_count-- ) {
      if( OT_PEERFLAG_D(peers, peer_size) & PEER_FLAG_SEEDING ) {
        r_end -= compare_size;
        memcpy( r_end, peers, compare_size);
      } else {
        memcpy( reply, peers, compare_size );
        reply += compare_size;
      }
      peers += peer_size;
    }
  }
  return result;
}

static size_t return_peers_selection( struct ot_workstruct *ws, ot_peerlist *peer_list, size_t peer_size, size_t amount, char *reply ) {
  unsigned int bucket_offset, bucket_index = 0, num_buckets = 1;
  ot_vector  * bucket_list = &peer_list->peers;
  unsigned int shifted_pc = peer_list->peer_count;
  unsigned int shifted_step = 0;
  unsigned int shift = 0;
  size_t       compare_size = OT_PEER_COMPARE_SIZE_FROM_PEER_SIZE(peer_size);
  size_t       result = compare_size * amount;
  char       * r_end = reply + result;

  if( OT_PEERLIST_HASBUCKETS(peer_list) ) {
    num_buckets = bucket_list->size;
    bucket_list = (ot_vector *)bucket_list->data;
  }

  /* Make fixpoint arithmetic as exact as possible */
#define MAXPRECBIT (1<<(8*sizeof(int)-3))
  while( !(shifted_pc & MAXPRECBIT ) ) { shifted_pc <<= 1; shift++; }
  shifted_step = shifted_pc/amount;
#undef MAXPRECBIT

  /* Initialize somewhere in the middle of peers so that
   fixpoint's aliasing doesn't alway miss the same peers */
  bucket_offset = nrand48(ws->rand48_state) % peer_list->peer_count;

  while( amount-- ) {
    ot_peer *peer;

    /* This is the aliased, non shifted range, next value may fall into */
    unsigned int diff = ( ( ( amount + 1 ) * shifted_step ) >> shift ) -
                        ( (   amount       * shifted_step ) >> shift );
    bucket_offset += 1 + nrand48(ws->rand48_state) % diff;

    while( bucket_offset >= bucket_list[bucket_index].size ) {
      bucket_offset -= bucket_list[bucket_index].size;
      bucket_index = ( bucket_index + 1 ) % num_buckets;
    }
    peer = bucket_list[bucket_index].data + peer_size * bucket_offset;
    if( OT_PEERFLAG_D(peer, peer_size) & PEER_FLAG_SEEDING ) {
      r_end -= compare_size;
      memcpy(r_end, peer, compare_size);
    } else {
      memcpy(reply, peer, compare_size);
      reply += compare_size;
    }
  }
  return result;
}

static size_t return_peers_for_torrent_udp( struct ot_workstruct * ws, ot_torrent *torrent, size_t amount, char *reply ) {
  char                *r = reply;
  size_t       peer_size = peer_size_from_peer6(&ws->peer);
  ot_peerlist *peer_list = peer_size == OT_PEER_SIZE6 ? torrent->peer_list6 : torrent->peer_list4;
  size_t       peer_count = torrent->peer_list6->peer_count + torrent->peer_list4->peer_count;
  size_t       seed_count = torrent->peer_list6->seed_count + torrent->peer_list4->seed_count;

  if( amount > peer_list->peer_count )
    amount = peer_list->peer_count;

  *(uint32_t*)(r+0) = htonl( OT_CLIENT_REQUEST_INTERVAL_RANDOM );
  *(uint32_t*)(r+4) = htonl( peer_count - seed_count );
  *(uint32_t*)(r+8) = htonl( seed_count );
  r += 12;

  if( amount ) {
    if( amount == peer_list->peer_count )
      r += return_peers_all( peer_list, peer_size, r );
    else
      r += return_peers_selection( ws, peer_list, peer_size, amount, r );
  }
  return r - reply;
}

static size_t return_peers_for_torrent_tcp( struct ot_workstruct * ws, ot_torrent *torrent, size_t amount, char *reply ) {
  char   *r = reply;
  int     erval = OT_CLIENT_REQUEST_INTERVAL_RANDOM;
  size_t  seed_count = torrent->peer_list6->seed_count + torrent->peer_list4->seed_count;
  size_t  down_count = torrent->peer_list6->down_count + torrent->peer_list4->down_count;
  size_t  peer_count = torrent->peer_list6->peer_count + torrent->peer_list4->peer_count - seed_count;

  /* Simple case: amount of peers in both lists is less than requested, here we return all results */
  size_t  amount_v4 = torrent->peer_list4->peer_count;
  size_t  amount_v6 = torrent->peer_list6->peer_count;

  /* Complex case: both lists have more than enough entries and we need to split between v4 and v6 clients */
  if( amount_v4 + amount_v6 > amount ) {
    size_t amount_left, percent_v6 = 0, percent_v4 = 0, left_v6, left_v4;
    const size_t SCALE = 1024;

    /* If possible, fill at least a quarter of peer from each family */
    if( amount / 4 <= amount_v4 )
        amount_v4 = amount / 4;
    if( amount / 4 <= amount_v6 )
        amount_v6 = amount / 4;

    /* Fill the rest according to which family's pool provides more peers */
    amount_left = amount - (amount_v4 + amount_v6);

    left_v4     = torrent->peer_list4->peer_count - amount_v4;
    left_v6     = torrent->peer_list6->peer_count - amount_v6;

    if( left_v4 + left_v6 ) {
      percent_v4  = (SCALE * left_v4) / (left_v4 + left_v6);
      percent_v6  = (SCALE * left_v6) / (left_v4 + left_v6);
    }

    amount_v4  += (amount_left * percent_v4) / SCALE;
    amount_v6  += (amount_left * percent_v6) / SCALE;

    /* Integer division rounding can leave out a peer */
    if( amount_v4 + amount_v6 < amount && amount_v6 < torrent->peer_list6->peer_count )
      ++amount_v6;
    if( amount_v4 + amount_v6 < amount && amount_v4 < torrent->peer_list4->peer_count )
      ++amount_v4;
  }

  r += sprintf( r, "d8:completei%zde10:downloadedi%zde10:incompletei%zde8:intervali%ie12:min intervali%ie", seed_count, down_count, peer_count, erval, erval/2 );

  if( amount_v4 ) {
    r += sprintf( r, PEERS_BENCODED4 "%zd:", OT_PEER_COMPARE_SIZE4 * amount_v4);
    if( amount_v4 == torrent->peer_list4->peer_count )
      r += return_peers_all( torrent->peer_list4, OT_PEER_SIZE4, r );
    else
      r += return_peers_selection( ws, torrent->peer_list4, OT_PEER_SIZE4, amount_v4, r );
  }

  if( amount_v6 ) {
    r += sprintf( r, PEERS_BENCODED6 "%zd:", OT_PEER_COMPARE_SIZE6 * amount_v6);
    if( amount_v6 == torrent->peer_list6->peer_count )
      r += return_peers_all( torrent->peer_list6, OT_PEER_SIZE6, r );
    else
      r += return_peers_selection( ws, torrent->peer_list6, OT_PEER_SIZE6, amount_v6, r );
  }

  *r++ = 'e';

  return r - reply;
}

/* Compiles a list of random peers for a torrent
   * Reply must have enough space to hold:
   * 92 + 6 * amount bytes for TCP/IPv4
   * 92 + 18 * amount bytes for TCP/IPv6
   * 12 + 6 * amount bytes for UDP/IPv4
   * 12 + 18 * amount bytes for UDP/IPv6
   * Does not yet check not to return self
*/
size_t return_peers_for_torrent( struct ot_workstruct * ws, ot_torrent *torrent, size_t amount, char *reply, PROTO_FLAG proto ) {
  return proto == FLAG_TCP ? return_peers_for_torrent_tcp(ws, torrent, amount, reply) : return_peers_for_torrent_udp(ws, torrent, amount, reply);
}

/* Fetches scrape info for a specific torrent */
size_t return_udp_scrape_for_torrent( ot_hash const hash, char *reply ) {
  int          exactmatch, delta_torrentcount = 0;
  ot_vector   *torrents_list = mutex_bucket_lock_by_hash( hash );
  ot_torrent  *torrent = binary_search( hash, torrents_list->data, torrents_list->size, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch );

  if( !exactmatch ) {
    memset( reply, 0, 12);
  } else {
    uint32_t *r = (uint32_t*) reply;

    if( clean_single_torrent( torrent ) ) {
      vector_remove_torrent( torrents_list, torrent );
      memset( reply, 0, 12);
      delta_torrentcount = -1;
    } else {
      r[0] = htonl( torrent->peer_list6->seed_count + torrent->peer_list4->seed_count );
      r[1] = htonl( torrent->peer_list6->down_count + torrent->peer_list4->down_count );
      r[2] = htonl( torrent->peer_list6->peer_count + torrent->peer_list4->peer_count -
                    torrent->peer_list6->seed_count - torrent->peer_list4->seed_count);
    }
  }
  mutex_bucket_unlock_by_hash( hash, delta_torrentcount );
  return 12;
}

/* Fetches scrape info for a specific torrent */
size_t return_tcp_scrape_for_torrent( ot_hash const *hash_list, int amount, char *reply ) {
  char *r = reply;
  int   exactmatch, i;

  r += sprintf( r, "d5:filesd" );

  for( i=0; i<amount; ++i ) {
    int            delta_torrentcount = 0;
    ot_hash const *hash = hash_list + i;
    ot_vector     *torrents_list = mutex_bucket_lock_by_hash( *hash );
    ot_torrent    *torrent = binary_search( hash, torrents_list->data, torrents_list->size, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch );

    if( exactmatch ) {
      if( clean_single_torrent( torrent ) ) {
        vector_remove_torrent( torrents_list, torrent );
        delta_torrentcount = -1;
      } else {
        *r++='2';*r++='0';*r++=':';
        memcpy( r, hash, sizeof(ot_hash) ); r+=sizeof(ot_hash);
        r += sprintf( r, "d8:completei%zde10:downloadedi%zde10:incompletei%zdee",
          torrent->peer_list6->seed_count + torrent->peer_list4->seed_count,
          torrent->peer_list6->down_count + torrent->peer_list4->down_count,
          torrent->peer_list6->peer_count + torrent->peer_list4->peer_count -
          torrent->peer_list6->seed_count - torrent->peer_list4->seed_count);
      }
    }
    mutex_bucket_unlock_by_hash( *hash, delta_torrentcount );
  }

  *r++ = 'e'; *r++ = 'e';
  return r - reply;
}

static ot_peerlist dummy_list;
size_t remove_peer_from_torrent( PROTO_FLAG proto, struct ot_workstruct *ws ) {
  int            exactmatch;
  ot_vector     *torrents_list = mutex_bucket_lock_by_hash( *ws->hash );
  ot_torrent    *torrent = binary_search( ws->hash, torrents_list->data, torrents_list->size, sizeof( ot_torrent ), OT_HASH_COMPARE_SIZE, &exactmatch );
  ot_peerlist   *peer_list = &dummy_list;
  size_t         peer_size; /* initialized in next line */
  ot_peer const *peer_src = peer_from_peer6(&ws->peer, &peer_size);
  size_t         peer_count = 0, seed_count = 0;

#ifdef WANT_SYNC_LIVE
  if( proto != FLAG_MCA ) {
    OT_PEERFLAG( ws->peer ) |= PEER_FLAG_STOPPED;
    livesync_tell( ws );
  }
#endif

  if( exactmatch ) {
    peer_list = peer_size == OT_PEER_SIZE6 ? torrent->peer_list6 : torrent->peer_list4;
    switch( vector_remove_peer( &peer_list->peers, peer_src, peer_size ) ) {
      case 2:  peer_list->seed_count--; /* Intentional fallthrough */
      case 1:  peer_list->peer_count--; /* Intentional fallthrough */
      default: break;
    }

    peer_count = torrent->peer_list6->peer_count + torrent->peer_list4->peer_count;
    seed_count = torrent->peer_list6->seed_count + torrent->peer_list4->seed_count;
  }


  if( proto == FLAG_TCP ) {
    int erval = OT_CLIENT_REQUEST_INTERVAL_RANDOM;
    ws->reply_size = sprintf( ws->reply, "d8:completei%zde10:incompletei%zde8:intervali%ie12:min intervali%ie%s0:e", seed_count, peer_count - seed_count, erval, erval / 2, peer_size == OT_PEER_SIZE6 ? PEERS_BENCODED6 : PEERS_BENCODED4 );
  }

  /* Handle UDP reply */
  if( proto == FLAG_UDP ) {
    ((uint32_t*)ws->reply)[2] = htonl( OT_CLIENT_REQUEST_INTERVAL_RANDOM );
    ((uint32_t*)ws->reply)[3] = htonl( peer_count - seed_count );
    ((uint32_t*)ws->reply)[4] = htonl( seed_count);
    ws->reply_size = 20;
  }

  mutex_bucket_unlock_by_hash( *ws->hash, 0 );
  return ws->reply_size;
}

void iterate_all_torrents( int (*for_each)( ot_torrent* torrent, uintptr_t data ), uintptr_t data ) {
  int bucket;
  size_t j;

  for( bucket=0; bucket<OT_BUCKET_COUNT; ++bucket ) {
    ot_vector  *torrents_list = mutex_bucket_lock( bucket );
    ot_torrent *torrents = (ot_torrent*)(torrents_list->data);

    for( j=0; j<torrents_list->size; ++j )
      if( for_each( torrents + j, data ) )
        break;

    mutex_bucket_unlock( bucket, 0 );
    if( !g_opentracker_running ) return;
  }
}

ot_peer *peer_from_peer6( ot_peer6 *peer, size_t *peer_size ) {
  ot_ip6 *ip = (ot_ip6*)peer;
  if( !ip6_isv4mapped(ip) ) {
    *peer_size = OT_PEER_SIZE6;
    return (ot_peer*)peer;
  }
  *peer_size = OT_PEER_SIZE4;
  return (ot_peer*)(((uint8_t*)peer) + 12);
}

size_t   peer_size_from_peer6(ot_peer6 *peer) {
  ot_ip6 *ip = (ot_ip6*)peer;
  if( !ip6_isv4mapped(ip))
    return OT_PEER_SIZE6;
  return OT_PEER_SIZE4;
}

void exerr( char * message ) {
  fprintf( stderr, "%s\n", message );
  exit( 111 );
}

void trackerlogic_init( ) {
  g_tracker_id = random();

  if( !g_stats_path )
    g_stats_path = "stats";
  g_stats_path_len = strlen( g_stats_path );

  /* Initialise background worker threads */
  mutex_init( );
  clean_init( );
  fullscrape_init( );
  accesslist_init( );
  livesync_init( );
  stats_init( );
}

void trackerlogic_deinit( void ) {
  int bucket, delta_torrentcount = 0;
  size_t j;

  /* Free all torrents... */
  for(bucket=0; bucket<OT_BUCKET_COUNT; ++bucket ) {
    ot_vector *torrents_list = mutex_bucket_lock( bucket );
    if( torrents_list->size ) {
      for( j=0; j<torrents_list->size; ++j ) {
        ot_torrent *torrent = ((ot_torrent*)(torrents_list->data)) + j;
        free_peerlist( torrent->peer_list6 );
        free_peerlist( torrent->peer_list4 );
        delta_torrentcount -= 1;
      }
      free( torrents_list->data );
    }
    mutex_bucket_unlock( bucket, delta_torrentcount );
  }

  /* Deinitialise background worker threads */
  stats_deinit( );
  livesync_deinit( );
  accesslist_deinit( );
  fullscrape_deinit( );
  clean_deinit( );
  /* Release mutexes */
  mutex_deinit( );
}

const char *g_version_trackerlogic_c = "$Source$: $Revision$\n";
