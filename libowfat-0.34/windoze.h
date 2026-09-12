#ifdef __MINGW32__

#ifdef __cplusplus
extern "C" {
#endif

/* set errno to WSAGetLastError() */
int winsock2errno(long l);
void __winsock_init(void);

#ifdef __cplusplus
}
#endif

#else

#define winsock2errno(fnord) (fnord)
#define __winsock_init()

#endif
