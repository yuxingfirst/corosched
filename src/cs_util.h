#ifndef _CS_UTIL_H_
#define _CS_UTIL_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "cs_string.h"

#define LF                  (uint8_t) 10
#define CR                  (uint8_t) 13
#define CRLF                "\x0d\x0a"
#define CRLF_LEN            (sizeof("\x0d\x0a") - 1)

#define NELEMS(a)           ((sizeof(a)) / sizeof((a)[0]))

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define SQUARE(d)           ((d) * (d))
#define VAR(s, s2, n)       (((n) < 2) ? 0.0 : ((s2) - SQUARE(s)/(n)) / ((n) - 1))
#define STDDEV(s, s2, n)    (((n) < 2) ? 0.0 : sqrt(VAR((s), (s2), (n))))

#define M_INET4_ADDRSTRLEN (sizeof("255.255.255.255") - 1)
#define M_INET6_ADDRSTRLEN \
    (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)
#define M_INET_ADDRSTRLEN  MAX(M_INET4_ADDRSTRLEN, M_INET6_ADDRSTRLEN)
#define M_UNIX_ADDRSTRLEN  \
    (sizeof(struct sockaddr_un) - offsetof(struct sockaddr_un, sun_path))

#define M_MAXHOSTNAMELEN   256

/*
 * Length of 1 byte, 2 bytes, 4 bytes, 8 bytes and largest integral
 * type (uintmax_t) in ascii, including the null terminator '\0'
 *
 * From stdint.h, we have:
 * # define UINT8_MAX	(255)
 * # define UINT16_MAX	(65535)
 * # define UINT32_MAX	(4294967295U)
 * # define UINT64_MAX	(__UINT64_C(18446744073709551615))
 */
#define M_UINT8_MAXLEN     (3 + 1)
#define M_UINT16_MAXLEN    (5 + 1)
#define M_UINT32_MAXLEN    (10 + 1)
#define M_UINT64_MAXLEN    (20 + 1)
#define M_UINTMAX_MAXLEN   M_UINT64_MAXLEN

/*
 * Make data 'd' or pointer 'p', n-byte aligned, where n is a power of 2
 * of 2.
 */
#define M_ALIGNMENT        sizeof(unsigned long) /* platform word */
#define M_ALIGN(d, n)      (((d) + (n - 1)) & ~(n - 1))
#define M_ALIGN_PTR(p, n)  \
    (void *) (((uintptr_t) (p) + ((uintptr_t) n - 1)) & ~((uintptr_t) n - 1))

/*
 * Wrapper to workaround well known, safe, implicit type conversion when
 * invoking system calls.
 */
#define cs_gethostname(_name, _len) \
    gethostname((char *)_name, (size_t)_len)

#define cs_atoi(_line, _n)          \
    _cs_atoi((uint8_t *)_line, (size_t)_n)

int cs_set_blocking(int sd);
int cs_set_nonblocking(int sd);
int cs_set_reuseaddr(int sd);
int cs_set_tcpnodelay(int sd);
int cs_set_linger(int sd, int timeout);
int cs_set_sndbuf(int sd, int size);
int cs_set_rcvbuf(int sd, int size);
int cs_get_soerror(int sd);
int cs_get_sndbuf(int sd);
int cs_get_rcvbuf(int sd);

int _cs_atoi(uint8_t *line, size_t n);
bool cs_valid_port(int n);

/*
 * Memory allocation and free wrappers.
 *
 * These wrappers enables us to loosely detect double free, dangling
 * pointer access and zero-byte alloc.
 */
#define cs_alloc(_s)                    \
    _cs_alloc((size_t)(_s), __FILE__, __LINE__)

#define cs_zalloc(_s)                   \
    _cs_zalloc((size_t)(_s), __FILE__, __LINE__)

#define cs_calloc(_n, _s)               \
    _cs_calloc((size_t)(_n), (size_t)(_s), __FILE__, __LINE__)

#define cs_realloc(_p, _s)              \
    _cs_realloc(_p, (size_t)(_s), __FILE__, __LINE__)

#define cs_free(_p) do {                \
    _cs_free(_p, __FILE__, __LINE__);   \
    (_p) = NULL;                        \
} while (0)

void *_cs_alloc(size_t size, const char *name, int line);
void *_cs_zalloc(size_t size, const char *name, int line);
void *_cs_calloc(size_t nmemb, size_t size, const char *name, int line);
void *_cs_realloc(void *ptr, size_t size, const char *name, int line);
void _cs_free(void *ptr, const char *name, int line);

/*
 * Wrappers to send or receive n byte message on a blocking
 * socket descriptor.
 */
#define cs_sendn(_s, _b, _n)    \
    _cs_sendn(_s, _b, (size_t)(_n))

#define cs_recvn(_s, _b, _n)    \
    _cs_recvn(_s, _b, (size_t)(_n))

/*
 * Wrappers to read or write data to/from (multiple) buffers
 * to a file or socket descriptor.
 */
#define cs_read(_d, _b, _n)     \
    read(_d, _b, (size_t)(_n))

#define cs_readv(_d, _b, _n)    \
    readv(_d, _b, (int)(_n))

#define cs_write(_d, _b, _n)    \
    write(_d, _b, (size_t)(_n))

#define cs_writev(_d, _b, _n)   \
    writev(_d, _b, (int)(_n))

ssize_t _cs_sendn(int sd, const void *vptr, size_t n);
ssize_t _cs_recvn(int sd, void *vptr, size_t n);

/*
 * Wrappers for defining custom assert based on whether macro
 * NC_ASSERT_PANIC or NC_ASSERT_LOG was defined at the moment
 * ASSERT was called.
 */
#ifdef CS_ASSERT_PANIC

#define ASSERT(_x) do {                         \
    if (!(_x)) {                                \
        nc_assert(#_x, __FILE__, __LINE__, 1);  \
    }                                           \
} while (0)

#define NOT_REACHED() ASSERT(0)

#elif CS_ASSERT_LOG

#define ASSERT(_x) do {                         \
    if (!(_x)) {                                \
        nc_assert(#_x, __FILE__, __LINE__, 0);  \
    }                                           \
} while (0)

#define NOT_REACHED() ASSERT(0)

#else

#define ASSERT(_x)

#define NOT_REACHED()

#endif

void cs_assert(const char *cond, const char *file, int line, int panic);
void cs_stacktrace(int skip_count);

int _scnprintf(char *buf, size_t size, const char *fmt, ...);
int _vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int64_t cs_usec_now(void);
int64_t cs_msec_now(void);

/*
 * Address resolution for internet (ipv4 and ipv6) and unix domain
 * socket address.
 */

struct sockinfo {
    int       family;              /* socket address family */
    socklen_t addrlen;             /* socket address length */
    union {
        struct sockaddr_in  in;    /* ipv4 socket address */
        struct sockaddr_in6 in6;   /* ipv6 socket address */
        struct sockaddr_un  un;    /* unix domain address */
    } addr;
};

int cs_resolve(struct string *name, int port, struct sockinfo *si);
char *cs__unresolve_addr(struct sockaddr *addr, socklen_t addrlen);
char *cs__unresolve_peer_desc(int sd);
char *cs__unresolve_desc(int sd);

#endif
