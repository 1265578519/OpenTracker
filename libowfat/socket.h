/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>

#include <compiler.h>
#include <uint16.h>
#include <uint32.h>

#ifdef __cplusplus
extern "C" {
#endif

int socket_tcp4(void);
int socket_tcp4b(void);
int socket_udp4(void);
int socket_udp4b(void);
int socket_tcp6(void);
int socket_tcp6b(void);
int socket_udp6(void);
int socket_udp6b(void);

int socket_sctp4(void);
int socket_sctp4b(void);
int socket_sctp6(void);
int socket_sctp6b(void);

#define socket_tcp() socket_tcp4()
#define socket_udp() socket_udp4()

int socket_connect4(int s,const char ip[4],uint16 port);
int socket_connect6(int s,const char ip[16],uint16 port,uint32 scope_id);
int socket_connected(int s);
int socket_bind4(int s,const char ip[4],uint16 port);
int socket_bind4_reuse(int s,const char ip[4],uint16 port);
int socket_bind6(int s,const char ip[16],uint16 port,uint32 scope_id);
int socket_bind6_reuse(int s,const char ip[16],uint16 port,uint32 scope_id);
int socket_listen(int s,unsigned int backlog);
int socket_accept4(int s,char ip[4],uint16* port);
int socket_accept6(int s,char ip[16],uint16* port,uint32* scope_id);
int socket_accept4_makenonblocking(int s,char ip[4],uint16* port);
int socket_accept6_makenonblocking(int s,char ip[16],uint16* port,uint32* scope_id);
int socket_accept4_setcloseonexec(int s,char ip[4],uint16* port);
int socket_accept6_setcloseonexec(int s,char ip[16],uint16* port,uint32* scope_id);
int socket_accept4_makenonblocking_setcloseonexec(int s,char ip[4],uint16* port);
int socket_accept6_makenonblocking_setcloseonexec(int s,char ip[16],uint16* port,uint32* scope_id);

/* These are internal wrappers around accept4, not meant for external use.
 * flags can be SOCKET_NONBLOCK or SOCKET_CLOEXEC or both */
enum {
  SOCKET_NONBLOCK=1,
  SOCKET_CLOEXEC=2,
};
int socket_accept4_flags(int s,char ip[4],uint16* port, int flags);
int socket_accept6_flags(int s,char ip[16],uint16* port,uint32* scope_id, int flags);

att_writen(2,3) att_nonnull(2)
ssize_t socket_recv4(int s,char* buf,size_t len,char ip[4],uint16* port);
att_writen(2,3) att_nonnull(2)
ssize_t socket_recv6(int s,char* buf,size_t len,char ip[16],uint16* port,uint32* scope_id);

att_readn(2,3) att_nonnull(2)
ssize_t socket_send4(int s,const char* buf,size_t len,const char ip[4],uint16 port);
att_readn(2,3) att_nonnull(2)
ssize_t socket_send4_flag(int s,const char* buf,size_t len,const char ip[4],uint16 port,int flags);
att_readn(2,3) att_nonnull(2)
ssize_t socket_send6(int s,const char* buf,size_t len,const char ip[16],uint16 port,uint32 scope_id);
att_readn(2,3) att_nonnull(2)
ssize_t socket_send6_flag(int s,const char* buf,size_t len,const char ip[16],uint16 port,uint32 scope_id,int flags);

int socket_local4(int s,char ip[4],uint16* port);
int socket_local6(int s,char ip[16],uint16* port,uint32* scope_id);
int socket_remote4(int s,char ip[4],uint16* port);
int socket_remote6(int s,char ip[16],uint16* port,uint32* scope_id);

/* enable sending udp packets to the broadcast address */
int socket_broadcast(int s);
/* join a multicast group on the given interface */
int socket_mcjoin4(int s,const char groupip[4],const char _interface[4]);
int socket_mcjoin6(int s,const char groupip[16],int _interface);
/* leave a multicast group on the given interface */
int socket_mcleave4(int s,const char groupip[4]);
int socket_mcleave6(int s,const char groupip[16]);
/* set multicast TTL/hop count for outgoing packets */
int socket_mcttl4(int s,char hops);
int socket_mchopcount6(int s,char hops);
/* enable multicast loopback, pass 1 for enable, 0 for disable */
int socket_mcloop4(int s,char loop);
int socket_mcloop6(int s,char loop);

/* Please note that these are platform specific.  Do not expect them to
 * work.  You might still get an accept() signalled even though there is
 * no data available.  So far, DATAIN is supported on FreeBSD and Linux,
 * and HTTPIN is supported on FreeBSD. */
enum defermode {
  DATAIN,	/* only signal accept() if there is data coming in */
  HTTPIN,	/* only signal accept() if a HTTP header has come in */
};

void socket_deferaccept(int s,enum defermode mode);

void socket_tryreservein(int s,int size);

const char* socket_getifname(uint32 _interface);
uint32 socket_getifidx(const char* ifname);

extern int noipv6;


/* if HAVE_SOCKET_FASTOPEN is #defined, your version of libowfat
 * has socket_fastopen, socket_fastopen_connect4,
 * socket_fastopen_connect6 and socket_quickack */
#define HAVE_SOCKET_FASTOPEN

/* Turn on server-side TCP fast open support (0 success, -1 error) */
int socket_fastopen(int s);

/* Turn quick ack mode on or off for the socket s. */
int socket_quickack(int s,int value);

/* For client-side TCP fast open, connect and sending the first data is
 * just one step, so we need an API to do it in one step */
att_readn(4,5) att_nonnull(4)
ssize_t socket_fastopen_connect4(int s,const char ip[4],uint16 port,const char* buf,size_t len);
att_readn(5,6) att_nonnull(5)
ssize_t socket_fastopen_connect6(int s,const char ip[16],uint16 port,uint32 scope_id,const char* buf,size_t len);


#ifdef __MINGW32__
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEINPROGRESS
#define EALREADY WSAEALREADY
#define ENOTSOCK WSAENOTSOCK
#define EDESTADDRREQ WSAEDESTADDRREQ
#define EMSGSIZE WSAEMSGSIZE
#define EPROTOTYPE WSAEPROTOTYPE
#define ENOPROTOOPT WSAENOPROTOOPT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define EOPNOTSUPP WSAEOPNOTSUPP
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#define EADDRINUSE WSAEADDRINUSE
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define ENETDOWN WSAENETDOWN
#define ENETUNREACH WSAENETUNREACH
#define ENETRESET WSAENETRESET
#define ECONNABORTED WSAECONNABORTED
#define ECONNRESET WSAECONNRESET
#define ENOBUFS WSAENOBUFS
#define EISCONN WSAEISCONN
#define ENOTCONN WSAENOTCONN
#define ESHUTDOWN WSAESHUTDOWN
#define ETOOMANYREFS WSAETOOMANYREFS
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ELOOP WSAELOOP
#define EHOSTDOWN WSAEHOSTDOWN
#define EHOSTUNREACH WSAEHOSTUNREACH
#define EPROCLIM WSAEPROCLIM
#define EUSERS WSAEUSERS
#define EDQUOT WSAEDQUOT
#define ESTALE WSAESTALE
#define EREMOTE WSAEREMOTE
#define EDISCON WSAEDISCON
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
