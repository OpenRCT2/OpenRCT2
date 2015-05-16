#ifdef __AVR__
#include "avrsupport.h"
#endif
#ifndef ONFLASH_HEADER_INCLUDED
#define ONFLASH_HEADER_INCLUDED

#ifndef ON_FLASH
#define ON_FLASH
#endif

#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/>.
 */

#define NS_FOSSA_VERSION "2.0.0"
/*
 * Copyright (c) 2015 Cesanta Software Limited
 * All rights reserved
 */

#ifndef OSDEP_HEADER_INCLUDED
#define OSDEP_HEADER_INCLUDED

#if !defined(NS_DISABLE_FILESYSTEM) && defined(AVR_NOFS)
#define NS_DISABLE_FILESYSTEM
#endif

#undef UNICODE                  /* Use ANSI WinAPI functions */
#undef _UNICODE                 /* Use multibyte encoding on Windows */
#define _MBCS                   /* Use multibyte encoding on Windows */
#define _INTEGRAL_MAX_BITS 64   /* Enable _stati64() on Windows */
#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2005+ */
#undef WIN32_LEAN_AND_MEAN      /* Let windows.h always include winsock2.h */
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600    /* For flockfile() on Linux */
#define __STDC_FORMAT_MACROS /* <inttypes.h> wants this for C++ */
#define __STDC_LIMIT_MACROS  /* C++ wants that for INT64_MAX */
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE /* Enable fseeko() and ftello() functions */
#endif
#define _FILE_OFFSET_BITS 64 /* Enable 64-bit file offsets */

#ifndef BYTE_ORDER
#define LITTLE_ENDIAN 0x41424344
#define BIG_ENDIAN 0x44434241
#define PDP_ENDIAN 0x42414443
/* TODO(lsm): fix for big-endian machines. 'ABCD' is not portable */
/*#define BYTE_ORDER 'ABCD'*/
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/*
 * MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
 * MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
 * MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
 * MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
 * MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
 * MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
 * MSVC++ 7.0  _MSC_VER == 1300
 * MSVC++ 6.0  _MSC_VER == 1200
 * MSVC++ 5.0  _MSC_VER == 1100
 */
#ifdef _MSC_VER
#pragma warning(disable : 4127) /* FD_SET() emits warning, disable it */
#pragma warning(disable : 4204) /* missing c99 support */
#endif

#ifndef AVR_LIBC
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#endif

#include <assert.h>
#ifndef NO_LIBC
#include <ctype.h>
#endif
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef va_copy
#ifdef __va_copy
#define va_copy __va_copy
#else
#define va_copy(x, y) (x) = (y)
#endif
#endif

#ifdef _WIN32
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib") /* Linking with winsock library */
#endif
#include <windows.h>
#include <process.h>
#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif
#ifndef __func__
#define STRX(x) #x
#define STR(x) STRX(x)
#define __func__ __FILE__ ":" STR(__LINE__)
#endif
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define sleep(x) Sleep((x) *1000)
#define to64(x) _atoi64(x)
#define popen(x, y) _popen((x), (y))
#define pclose(x) _pclose(x)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define fseeko(x, y, z) _fseeki64((x), (y), (z))
#else
#define fseeko(x, y, z) fseek((x), (y), (z))
#endif
typedef int socklen_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
typedef SOCKET sock_t;
typedef uint32_t in_addr_t;
#ifndef pid_t
#define pid_t HANDLE
#endif
#define INT64_FMT "I64d"
#ifdef __MINGW32__
typedef struct stat ns_stat_t;
#else
typedef struct _stati64 ns_stat_t;
#endif
#ifndef S_ISDIR
#define S_ISDIR(x) ((x) &_S_IFDIR)
#endif
#define DIRSEP '\\'
#else /* not _WIN32 */
#ifndef NO_LIBC
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h> /* For inet_pton() when NS_ENABLE_IPV6 is defined */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#ifndef AVR_LIBC
#define closesocket(x) close(x)
#define __cdecl
#define INVALID_SOCKET (-1)
#define INT64_FMT PRId64
#define to64(x) strtoll(x, NULL, 10)
typedef int sock_t;
typedef struct stat ns_stat_t;
#define DIRSEP '/'
#endif
#ifdef __APPLE__
int64_t strtoll(const char* str, char** endptr, int base);
#endif
#endif /* _WIN32 */

#ifdef NS_ENABLE_DEBUG
#define DBG(x)                  \
  do {                          \
    printf("%-20s ", __func__); \
    printf x;                   \
    putchar('\n');              \
    fflush(stdout);             \
  } while (0)
#else
#define DBG(x)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

#endif /* OSDEP_HEADER_INCLUDED */
/*
 * Copyright (c) 2015 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === Memory Buffers
 *
 * Mbufs are mutable/growing memory buffers, like C++ strings.
 * Mbuf can append data to the end of a buffer, or insert data into arbitrary
 * position in the middle of a buffer. The buffer grows automatically when
 * needed.
 */

#ifndef MBUF_H_INCLUDED
#define MBUF_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

#ifndef MBUF_SIZE_MULTIPLIER
#define MBUF_SIZE_MULTIPLIER 1.5
#endif

/* Memory buffer descriptor */
struct mbuf {
  char *buf;   /* Buffer pointer */
  size_t len;  /* Data length. Data is located between offset 0 and len. */
  size_t size; /* Buffer size allocated by realloc(1). Must be >= len */
};

/*
 * Initialize an Mbuf.
 * `initial_capacity` specifies the initial capacity of the mbuf.
 */
void mbuf_init(struct mbuf *, size_t initial_capacity);

/* Free the space allocated for the mbuffer and resets the mbuf structure. */
void mbuf_free(struct mbuf *);

/*
 * Appends data to the Mbuf.
 *
 * Return the number of bytes appended, or 0 if out of memory.
 */
size_t mbuf_append(struct mbuf *, const void *data, size_t data_size);

/*
 * Insert data at a specified offset in the Mbuf.
 *
 * Existing data will be shifted forwards and the buffer will
 * be grown if necessary.
 * Return the number of bytes inserted.
 */
size_t mbuf_insert(struct mbuf *, size_t, const void *, size_t);

/* Remove `data_size` bytes from the beginning of the buffer. */
void mbuf_remove(struct mbuf *, size_t data_size);

/*
 * Resize an Mbuf.
 *
 * If `new_size` is smaller than buffer's `len`, the
 * resize is not performed.
 */
void mbuf_resize(struct mbuf *, size_t new_size);

/* Shrink an Mbuf by resizing its `size` to `len`. */
void mbuf_trim(struct mbuf *);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MBUF_H_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#if !defined(NS_SHA1_HEADER_INCLUDED) && !defined(DISABLE_SHA1)
#define NS_SHA1_HEADER_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t state[5];
  uint32_t count[2];
  unsigned char buffer[64];
} SHA1_CTX;

void SHA1Init(SHA1_CTX *);
void SHA1Update(SHA1_CTX *, const unsigned char *data, uint32_t len);
void SHA1Final(unsigned char digest[20], SHA1_CTX *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_SHA1_HEADER_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef MD5_HEADER_DEFINED
#define MD5_HEADER_DEFINED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct MD5Context {
  uint32_t buf[4];
  uint32_t bits[2];
  unsigned char in[64];
} MD5_CTX;

void MD5_Init(MD5_CTX *c);
void MD5_Update(MD5_CTX *c, const unsigned char *data, size_t len);
void MD5_Final(unsigned char *md, MD5_CTX *c);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#if !defined(BASE64_H_INCLUDED) && !defined(DISABLE_BASE64)
#define BASE64_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

void base64_encode(const unsigned char *src, int src_len, char *dst);
int base64_decode(const unsigned char *s, int len, char *dst);

#ifdef __cplusplus
}
#endif
#endif
/*
 * Copyright (c) 2004-2013 Sergey Lyubka <valenok@gmail.com>
 * Copyright (c) 2013 Cesanta Software Limited
 * All rights reserved
 *
 * This library is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http: *www.gnu.org/licenses/>.
 *
 * You are free to use this library under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, as set out in <http://cesanta.com/products.html>.
 */

#ifndef FROZEN_HEADER_INCLUDED
#define FROZEN_HEADER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>

enum json_type {
  JSON_TYPE_EOF     = 0,      /* End of parsed tokens marker */
  JSON_TYPE_STRING  = 1,
  JSON_TYPE_NUMBER  = 2,
  JSON_TYPE_OBJECT  = 3,
  JSON_TYPE_TRUE    = 4,
  JSON_TYPE_FALSE   = 5,
  JSON_TYPE_NULL    = 6,
  JSON_TYPE_ARRAY   = 7
};

struct json_token {
  const char *ptr;      /* Points to the beginning of the token */
  int len;              /* Token length */
  int num_desc;         /* For arrays and object, total number of descendants */
  enum json_type type;  /* Type of the token, possible values above */
};

/* Error codes */
#define JSON_STRING_INVALID           -1
#define JSON_STRING_INCOMPLETE        -2
#define JSON_TOKEN_ARRAY_TOO_SMALL    -3

int parse_json(const char *json_string, int json_string_length,
               struct json_token *tokens_array, int size_of_tokens_array);
struct json_token *parse_json2(const char *json_string, int string_length);
struct json_token *find_json_token(struct json_token *toks, const char *path);

int json_emit_long(char *buf, int buf_len, long value);
int json_emit_double(char *buf, int buf_len, double value);
int json_emit_quoted_str(char *buf, int buf_len, const char *str, int len);
int json_emit_unquoted_str(char *buf, int buf_len, const char *str, int len);
int json_emit(char *buf, int buf_len, const char *fmt, ...);
int json_emit_va(char *buf, int buf_len, const char *fmt, va_list);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FROZEN_HEADER_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/>.
 */

/*
 * === Core: TCP/UDP/SSL
 *
 * NOTE: Fossa manager is single threaded. It does not protect
 * its data structures by mutexes, therefore all functions that are dealing
 * with particular event manager should be called from the same thread,
 * with exception of `mg_broadcast()` function. It is fine to have different
 * event managers handled by different threads.
 */

#ifndef NS_NET_HEADER_INCLUDED
#define NS_NET_HEADER_INCLUDED


#ifdef NS_ENABLE_SSL
#ifdef __APPLE__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <openssl/ssl.h>
#else
typedef void *SSL;
typedef void *SSL_CTX;
#endif

#ifdef NS_USE_READ_WRITE
#define NS_RECV_FUNC(s, b, l, f) read(s, b, l)
#define NS_SEND_FUNC(s, b, l, f) write(s, b, l)
#else
#define NS_RECV_FUNC(s, b, l, f) recv(s, b, l, f)
#define NS_SEND_FUNC(s, b, l, f) send(s, b, l, f)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

union socket_address {
  struct sockaddr sa;
  struct sockaddr_in sin;
#ifdef NS_ENABLE_IPV6
  struct sockaddr_in6 sin6;
#else
  struct sockaddr sin6;
#endif
};

/* Describes chunk of memory */
struct ns_str {
  const char *p; /* Memory chunk pointer */
  size_t len;    /* Memory chunk length */
};

#define NS_STR(str_literal) \
  { str_literal, sizeof(str_literal) - 1 }

/*
 * Callback function (event handler) prototype, must be defined by user.
 * Fossa calls event handler, passing events defined below.
 */
struct ns_connection;
typedef void (*ns_event_handler_t)(struct ns_connection *, int ev, void *);

/* Events. Meaning of event parameter (evp) is given in the comment. */
#define NS_POLL 0    /* Sent to each connection on each ns_mgr_poll() call */
#define NS_ACCEPT 1  /* New connection accepted. union socket_address *addr */
#define NS_CONNECT 2 /* connect() succeeded or failed. int *success_status */
#define NS_RECV 3    /* Data has benn received. int *num_bytes */
#define NS_SEND 4    /* Data has been written to a socket. int *num_bytes */
#define NS_CLOSE 5   /* Connection is closed. NULL */

/*
 * Fossa event manager.
 */
struct ns_mgr {
  struct ns_connection *active_connections;
  const char *hexdump_file; /* Debug hexdump file path */
  sock_t ctl[2];            /* Socketpair for mg_wakeup() */
  void *user_data;          /* User data */
};

/*
 * Fossa connection.
 */
struct ns_connection {
  struct ns_connection *next, *prev; /* ns_mgr::active_connections linkage */
  struct ns_connection *listener;    /* Set only for accept()-ed connections */
  struct ns_mgr *mgr;                /* Pointer to containing manager */

  sock_t sock;             /* Socket to the remote peer */
  union socket_address sa; /* Remote peer address */
  size_t recv_mbuf_limit;  /* Max size of recv buffer */
  struct mbuf recv_mbuf;   /* Received data */
  struct mbuf send_mbuf;   /* Data scheduled for sending */
  SSL *ssl;
  SSL_CTX *ssl_ctx;
  time_t last_io_time;              /* Timestamp of the last socket IO */
  ns_event_handler_t proto_handler; /* Protocol-specific event handler */
  void *proto_data;                 /* Protocol-specific data */
  ns_event_handler_t handler;       /* Event handler function */
  void *user_data;                  /* User-specific data */

  unsigned long flags;
/* Flags set by Fossa */
#define NSF_LISTENING (1 << 0)          /* This connection is listening */
#define NSF_UDP (1 << 1)                /* This connection is UDP */
#define NSF_RESOLVING (1 << 2)          /* Waiting for async resolver */
#define NSF_CONNECTING (1 << 3)         /* connect() call in progress */
#define NSF_SSL_HANDSHAKE_DONE (1 << 4) /* SSL specific */
#define NSF_WANT_READ (1 << 5)          /* SSL specific */
#define NSF_WANT_WRITE (1 << 6)         /* SSL specific */
#define NSF_IS_WEBSOCKET (1 << 7)       /* Websocket specific */

/* Flags that are settable by user */
#define NSF_SEND_AND_CLOSE (1 << 10)      /* Push remaining data and close  */
#define NSF_DONT_SEND (1 << 11)           /* Do not send data to peer */
#define NSF_CLOSE_IMMEDIATELY (1 << 12)   /* Disconnect */
#define NSF_WEBSOCKET_NO_DEFRAG (1 << 13) /* Websocket specific */

#define NSF_USER_1 (1 << 20) /* Flags left for application */
#define NSF_USER_2 (1 << 21)
#define NSF_USER_3 (1 << 22)
#define NSF_USER_4 (1 << 23)
#define NSF_USER_5 (1 << 24)
#define NSF_USER_6 (1 << 25)
};

/*
 * Initialize Fossa manager. Side effect: ignores SIGPIPE signal.
 * `mgr->user_data` field will be initialized with `user_data` parameter.
 * That is an arbitrary pointer, where user code can associate some data
 * with the particular Fossa manager. For example, a C++ wrapper class
 * could be written, in which case `user_data` can hold a pointer to the
 * class instance.
 */
void ns_mgr_init(struct ns_mgr *mgr, void *user_data);

/*
 * De-initializes Fossa manager.
 *
 * Close and deallocate all active connections.
 */
void ns_mgr_free(struct ns_mgr *);

/*
 * This function performs the actual IO, and must be called in a loop
 * (an event loop). Returns the current timestamp.
 * `milli` is the maximum number of milliseconds to sleep.
 * `ns_mgr_poll()` checks all connection for IO readiness. If at least one
 * of the connections is IO-ready, `ns_mgr_poll()` triggers respective
 * event handlers and returns.
 */
time_t ns_mgr_poll(struct ns_mgr *, int milli);

/*
 * Pass a message of a given length to all connections.
 *
 * Must be called from a thread that does NOT call `ns_mgr_poll()`.
 * Note that `ns_broadcast()` is the only function
 * that can be, and must be, called from a different (non-IO) thread.
 *
 * `func` callback function will be called by the IO thread for each
 * connection. When called, event would be `NS_POLL`, and message will
 * be passed as `ev_data` pointer. Maximum message size is capped
 * by `NS_CTL_MSG_MESSAGE_SIZE` which is set to 8192 bytes.
 */
void ns_broadcast(struct ns_mgr *, ns_event_handler_t func, void *, size_t);

/*
 * Iterate over all active connections.
 *
 * Returns next connection from the list
 * of active connections, or `NULL` if there is no more connections. Below
 * is the iteration idiom:
 *
 * [source,c]
 * ----
 * for (c = ns_next(srv, NULL); c != NULL; c = ns_next(srv, c)) {
 *   // Do something with connection `c`
 * }
 * ----
 */
struct ns_connection *ns_next(struct ns_mgr *, struct ns_connection *);

/*
 * Optional parameters to ns_add_sock_opt()
 * `flags` is an initial `struct ns_connection::flags` bitmask to set,
 * see `NSF_*` flags definitions.
 */
struct ns_add_sock_opts {
  void *user_data;           /* Initial value for connection's user_data */
  unsigned int flags;        /* Initial connection flags */
  const char **error_string; /* Placeholder for the error string */
};

/*
 * Create a connection, associate it with the given socket and event handler,
 * and add it to the manager.
 *
 * For more options see the `ns_add_sock_opt` variant.
 */
struct ns_connection *ns_add_sock(struct ns_mgr *, sock_t, ns_event_handler_t);

/*
 * Create a connection, associate it with the given socket and event handler,
 * and add to the manager.
 *
 * See the `ns_add_sock_opts` structure for a description of the options.
 */
struct ns_connection *ns_add_sock_opt(struct ns_mgr *, sock_t,
                                      ns_event_handler_t,
                                      struct ns_add_sock_opts);

/*
 * Optional parameters to ns_bind_opt()
 * `flags` is an initial `struct ns_connection::flags` bitmask to set,
 * see `NSF_*` flags definitions.
 */
struct ns_bind_opts {
  void *user_data;           /* Initial value for connection's user_data */
  unsigned int flags;        /* Extra connection flags */
  const char **error_string; /* Placeholder for the error string */
};

/*
 * Create listening connection.
 *
 * See `ns_bind_opt` for full documentation.
 */
struct ns_connection *ns_bind(struct ns_mgr *, const char *,
                              ns_event_handler_t);
/*
 * Create listening connection.
 *
 * `address` parameter tells which address to bind to. It's format is the same
 * as for the `ns_connect()` call, where `HOST` part is optional. `address`
 * can be just a port number, e.g. `:8000`. To bind to a specific interface,
 * an IP address can be specified, e.g. `1.2.3.4:8000`. By default, a TCP
 * connection is created. To create UDP connection, prepend `udp://` prefix,
 * e.g. `udp://:8000`. To summarize, `address` paramer has following format:
 * `[PROTO://][IP_ADDRESS]:PORT`, where `PROTO` could be `tcp` or `udp`.
 *
 * See the `ns_bind_opts` structure for a description of the optional
 * parameters.
 *
 * Return a new listening connection, or `NULL` on error.
 * NOTE: Connection remains owned by the manager, do not free().
 */
struct ns_connection *ns_bind_opt(struct ns_mgr *, const char *,
                                  ns_event_handler_t, struct ns_bind_opts);

/* Optional parameters to ns_connect_opt() */
struct ns_connect_opts {
  void *user_data;           /* Initial value for connection's user_data */
  unsigned int flags;        /* Extra connection flags */
  const char **error_string; /* Placeholder for the error string */
};

/*
 * Connect to a remote host.
 *
 * See `ns_connect_opt()` for full documentation.
 */
struct ns_connection *ns_connect(struct ns_mgr *, const char *,
                                 ns_event_handler_t);

/*
 * Connect to a remote host.
 *
 * `address` format is `[PROTO://]HOST:PORT`. `PROTO` could be `tcp` or `udp`.
 * `HOST` could be an IP address,
 * IPv6 address (if Fossa is compiled with `-DNS_ENABLE_IPV6`), or a host name.
 * If `HOST` is a name, Fossa will resolve it asynchronously. Examples of
 * valid addresses: `google.com:80`, `udp://1.2.3.4:53`, `10.0.0.1:443`,
 * `[::1]:80`
 *
 * See the `ns_connect_opts` structure for a description of the optional
 * parameters.
 *
 * Returns a new outbound connection, or `NULL` on error.
 *
 * NOTE: Connection remains owned by the manager, do not free().
 *
 * NOTE: To enable IPv6 addresses, `-DNS_ENABLE_IPV6` should be specified
 * in the compilation flags.
 *
 * NOTE: New connection will receive `NS_CONNECT` as it's first event
 * which will report connect success status.
 * If asynchronous resolution fail, or `connect()` syscall fail for whatever
 * reason (e.g. with `ECONNREFUSED` or `ENETUNREACH`), then `NS_CONNECT`
 * event report failure. Code example below:
 *
 * [source,c]
 * ----
 * static void ev_handler(struct ns_connection *nc, int ev, void *ev_data) {
 *   int connect_status;
 *
 *   switch (ev) {
 *     case NS_CONNECT:
 *       connect_status = * (int *) ev_data;
 *       if (connect_status == 0) {
 *         // Success
 *       } else  {
 *         // Error
 *         printf("connect() error: %s\n", strerror(connect_status));
 *       }
 *       break;
 *     ...
 *   }
 * }
 *
 *   ...
 *   ns_connect(mgr, "my_site.com:80", ev_handler);
 * ----
 */
struct ns_connection *ns_connect_opt(struct ns_mgr *, const char *,
                                     ns_event_handler_t,
                                     struct ns_connect_opts);
const char *ns_set_ssl(struct ns_connection *nc, const char *, const char *);

/*
 * Send data to the connection.
 *
 * Return number of written bytes. Note that sending
 * functions do not actually push data to the socket. They just append data
 * to the output buffer. The exception is UDP connections. For UDP, data is
 * sent immediately, and returned value indicates an actual number of bytes
 * sent to the socket.
 */
int ns_send(struct ns_connection *, const void *buf, int len);

/*
 * Send `printf`-style formatted data to the connection.
 *
 * See `ns_send` for more details on send semantics.
 */
int ns_printf(struct ns_connection *, const char *fmt, ...);

/* Same as `ns_printf()`, but takes `va_list ap` as an argument. */
int ns_vprintf(struct ns_connection *, const char *fmt, va_list ap);

/*
 * Create a socket pair.
 * `sock_type` can be either `SOCK_STREAM` or `SOCK_DGRAM`.
 * Return 0 on failure, 1 on success.
 */
int ns_socketpair(sock_t[2], int sock_type);

/*
 * Convert domain name into IP address.
 *
 * This is a utility function. If compilation flags have
 * `-DNS_ENABLE_GETADDRINFO`, then `getaddrinfo()` call is used for name
 * resolution. Otherwise, `gethostbyname()` is used.
 *
 * CAUTION: this function can block.
 * Return 1 on success, 0 on failure.
 */
int ns_resolve(const char *domain_name, char *ip_addr_buf, size_t buf_len);

/*
 * Verify given IP address against the ACL.
 *
 * `remote_ip` - an IPv4 address to check, in host byte order
 * `acl` - a comma separated list of IP subnets: `x.x.x.x/x` or `x.x.x.x`.
 * Each subnet is
 * prepended by either a - or a + sign. A plus sign means allow, where a
 * minus sign means deny. If a subnet mask is omitted, such as `-1.2.3.4`,
 * this means to deny only that single IP address.
 * Subnet masks may vary from 0 to 32, inclusive. The default setting
 * is to allow all accesses. On each request the full list is traversed,
 * and the last match wins. Example:
 *
 * `-0.0.0.0/0,+192.168/16` - deny all acccesses, only allow 192.168/16 subnet
 *
 * To learn more about subnet masks, see the
 * link:https://en.wikipedia.org/wiki/Subnetwork[Wikipedia page on Subnetwork]
 *
 * Return -1 if ACL is malformed, 0 if address is disallowed, 1 if allowed.
 */
int ns_check_ip_acl(const char *acl, uint32_t remote_ip);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NS_NET_HEADER_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === Utilities
 */

#ifndef NS_UTIL_HEADER_DEFINED
#define NS_UTIL_HEADER_DEFINED

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE 500
#endif

/*
 * Fetch substring from input string `s`, `end` into `v`.
 * Skips initial delimiter characters. Records first non-delimiter character
 * as the beginning of substring `v`. Then scans the rest of the string
 * until a delimiter character or end-of-string is found.
 * `delimiters` is a 0-terminated string containing delimiter characters.
 * Either one of `delimiters` or `end_string` terminates the search.
 * Return an `s` pointer, advanced forward where parsing stopped.
 */
const char *ns_skip(const char *s, const char *end_string,
                    const char *delimiters, struct ns_str *v);

/*
 * Cross-platform version of `strncasecmp()`.
 */
int ns_ncasecmp(const char *s1, const char *s2, size_t len);

/*
 * Cross-platform version of `strcasecmp()`.
 */
int ns_casecmp(const char *s1, const char *s2);

/*
 * Cross-platform version of `strcmp()` where where first string is
 * specified by `struct ns_str`.
 */
int ns_vcmp(const struct ns_str *str2, const char *str1);

/*
 * Cross-platform version of `strncasecmp()` where first string is
 * specified by `struct ns_str`.
 */
int ns_vcasecmp(const struct ns_str *str2, const char *str1);

/*
 * Decode base64-encoded string `s`, `len` into the destination `dst`.
 * Destination has to have enough space to hold decoded buffer.
 * Decoding stops either when all string has been decoded, or invalid
 * character appeared.
 * Destination is '\0'-terminated.
 * Return number of decoded characters. On success, that should be equal to
 * `len`. On error (invalid character) the return value is smaller then `len`.
 */
int ns_base64_decode(const unsigned char *s, int len, char *dst);

/*
 * Base64-encode chunk of memory `src`, `src_len` into the destination `dst`.
 * Destination has to have enough space to hold encoded buffer.
 * Destination is '\0'-terminated.
 */
void ns_base64_encode(const unsigned char *src, int src_len, char *dst);

#ifndef NS_DISABLE_FILESYSTEM
/*
 * Perform a 64-bit `stat()` call against given file.
 *
 * `path` should be UTF8 encoded.
 *
 * Return value is the same as for `stat()` syscall.
 */
int ns_stat(const char *path, ns_stat_t *st);

/*
 * Open the given file and return a file stream.
 *
 * `path` and `mode` should be UTF8 encoded.
 *
 * Return value is the same as for the `fopen()` call.
 */
FILE *ns_fopen(const char *path, const char *mode);

/*
 * Open the given file and return a file stream.
 *
 * `path` should be UTF8 encoded.
 *
 * Return value is the same as for the `open()` syscall.
 */
int ns_open(const char *path, int flag, int mode);
#endif
#ifndef AVR_LIBC
/*
 * Start a new detached thread.
 * Arguments and semantic is the same as pthead's `pthread_create()`.
 * `thread_func` is a thread function, `thread_func_param` is a parameter
 * that is passed to the thread function.
 */
void *ns_start_thread(void *(*thread_func)(void *), void *thread_func_param);
#endif

void ns_set_close_on_exec(sock_t);

#define NS_SOCK_STRINGIFY_IP 1
#define NS_SOCK_STRINGIFY_PORT 2
#define NS_SOCK_STRINGIFY_REMOTE 4
/*
 * Convert socket's local or remote address into string.
 *
 * The `flags` parameter is a bit mask that controls the behavior,
 * see `NS_SOCK_STRINGIFY_*` definitions.
 *
 * - NS_SOCK_STRINGIFY_IP - print IP address
 * - NS_SOCK_STRINGIFY_PORT - print port number
 * - NS_SOCK_STRINGIFY_REMOTE - print remote peer's IP/port, not local address
 *
 * If both port number and IP address are printed, they are separated by `:`.
 * If compiled with `-DNS_ENABLE_IPV6`, IPv6 addresses are supported.
 */
void ns_sock_to_str(sock_t sock, char *buf, size_t len, int flags);

/*
 * Convert socket's address into string.
 *
 * `flags` is NS_SOCK_STRINGIFY_IP and/or NS_SOCK_STRINGIFY_PORT.
 */
void ns_sock_addr_to_str(const union socket_address *sa, char *buf, size_t len,
                         int flags);

/*
 * Generates human-readable hexdump of memory chunk.
 *
 * Takes a memory buffer `buf` of length `len` and creates a hex dump of that
 * buffer in `dst`. Generated output is a-la hexdump(1).
 * Return length of generated string, excluding terminating `\0`. If returned
 * length is bigger than `dst_len`, overflow bytes are discarded.
 */
int ns_hexdump(const void *buf, int len, char *dst, int dst_len);

/*
 * Generates human-readable hexdump of the data sent or received by connection.
 * `path` is a file name where hexdump should be written. `num_bytes` is
 * a number of bytes sent/received. `ev` is one of the `NS_*` events sent to
 * an event handler. This function is supposed to be called from the
 * event handler.
 */
void ns_hexdump_connection(struct ns_connection *nc, const char *path,
                           int num_bytes, int ev);
/*
 * Print message to buffer. If buffer is large enough to hold the message,
 * return buffer. If buffer is to small, allocate large enough buffer on heap,
 * and return allocated buffer.
 * This is a supposed use case:
 *
 *    char buf[5], *p = buf;
 *    p = ns_avprintf(&p, sizeof(buf), "%s", "hi there");
 *    use_p_somehow(p);
 *    if (p != buf) {
 *      free(p);
 *    }
 *
 * The purpose of this is to avoid malloc-ing if generated strings are small.
 */
int ns_avprintf(char **buf, size_t size, const char *fmt, va_list ap);

/*
 * Return true if target platform is big endian.
 */
int ns_is_big_endian(void);

/*
 * A helper function for traversing a comma separated list of values.
 * It returns a list pointer shifted to the next value, or NULL if the end
 * of the list found.
 * Value is stored in val vector. If value has form "x=y", then eq_val
 * vector is initialized to point to the "y" part, and val vector length
 * is adjusted to point only to "x".
 * If list is just a comma separated list of entries, like "aa,bb,cc" then
 * `eq_val` will contain zero-length string.
 *
 * The purpose of this function is to parse comma separated string without
 * any copying/memory allocation.
 */
const char *ns_next_comma_list_entry(const char *list, struct ns_str *val,
                                     struct ns_str *eq_val);

/*
 * Match 0-terminated string against a glob pattern.
 * Match is case-insensitive. Return number of bytes matched, or -1 if no match.
 */
int ns_match_prefix(const char *pattern, int pattern_len, const char *str);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_UTIL_HEADER_DEFINED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === HTTP + Websocket
 */

#ifndef NS_HTTP_HEADER_DEFINED
#define NS_HTTP_HEADER_DEFINED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NS_MAX_HTTP_HEADERS
#define NS_MAX_HTTP_HEADERS 40
#endif

#ifndef NS_MAX_HTTP_REQUEST_SIZE
#define NS_MAX_HTTP_REQUEST_SIZE 8192
#endif

#ifndef NS_MAX_PATH
#define NS_MAX_PATH 1024
#endif

#ifndef NS_MAX_HTTP_SEND_IOBUF
#define NS_MAX_HTTP_SEND_IOBUF 4096
#endif

#ifndef NS_WEBSOCKET_PING_INTERVAL_SECONDS
#define NS_WEBSOCKET_PING_INTERVAL_SECONDS 5
#endif

#ifndef NS_CGI_ENVIRONMENT_SIZE
#define NS_CGI_ENVIRONMENT_SIZE 8192
#endif

#ifndef NS_MAX_CGI_ENVIR_VARS
#define NS_MAX_CGI_ENVIR_VARS 64
#endif

#ifndef NS_ENV_EXPORT_TO_CGI
#define NS_ENV_EXPORT_TO_CGI "FOSSA_CGI"
#endif

/* HTTP message */
struct http_message {
  struct ns_str message; /* Whole message: request line + headers + body */

  struct ns_str proto; /* "HTTP/1.1" -- for both request and response */
  /* HTTP Request line (or HTTP response line) */
  struct ns_str method; /* "GET" */
  struct ns_str uri;    /* "/my_file.html" */
  /* For responses, code and response status message are set */
  int resp_code;
  struct ns_str resp_status_msg;

  /*
   * Query-string part of the URI. For example, for HTTP request
   *    GET /foo/bar?param1=val1&param2=val2
   *    |    uri    |     query_string     |
   *
   * Note that question mark character doesn't belong neither to the uri,
   * nor to the query_string
   */
  struct ns_str query_string;

  /* Headers */
  struct ns_str header_names[NS_MAX_HTTP_HEADERS];
  struct ns_str header_values[NS_MAX_HTTP_HEADERS];

  /* Message body */
  struct ns_str body; /* Zero-length for requests with no body */
};

struct websocket_message {
  unsigned char *data;
  size_t size;
  unsigned char flags;
};

/* HTTP and websocket events. void *ev_data is described in a comment. */
#define NS_HTTP_REQUEST 100 /* struct http_message * */
#define NS_HTTP_REPLY 101   /* struct http_message * */

#define NS_WEBSOCKET_HANDSHAKE_REQUEST 111 /* NULL */
#define NS_WEBSOCKET_HANDSHAKE_DONE 112    /* NULL */
#define NS_WEBSOCKET_FRAME 113             /* struct websocket_message * */
#define NS_WEBSOCKET_CONTROL_FRAME 114     /* struct websocket_message * */

/*
 * Attach built-in HTTP event handler to the given connection.
 * User-defined event handler will receive following extra events:
 *
 * - NS_HTTP_REQUEST: HTTP request has arrived. Parsed HTTP request is passed as
 *   `struct http_message` through the handler's `void *ev_data` pointer.
 * - NS_HTTP_REPLY: HTTP reply has arrived. Parsed HTTP reply is passed as
 *   `struct http_message` through the handler's `void *ev_data` pointer.
 * - NS_WEBSOCKET_HANDSHAKE_REQUEST: server has received websocket handshake
 *   request. `ev_data` contains parsed HTTP request.
 * - NS_WEBSOCKET_HANDSHAKE_DONE: server has completed Websocket handshake.
 *   `ev_data` is `NULL`.
 * - NS_WEBSOCKET_FRAME: new websocket frame has arrived. `ev_data` is
 *   `struct websocket_message *`
 */
void ns_set_protocol_http_websocket(struct ns_connection *nc);

/*
 * Send websocket handshake to the server.
 *
 * `nc` must be a valid connection, connected to a server. `uri` is an URI
 * to fetch, extra_headers` is extra HTTP headers to send or `NULL`.
 *
 * This function is intended to be used by websocket client.
 */
void ns_send_websocket_handshake(struct ns_connection *nc, const char *uri,
                                 const char *extra_headers);

/*
 * Send websocket frame to the remote end.
 *
 * `op` specifies frame's type , one of:
 *
 * - WEBSOCKET_OP_CONTINUE
 * - WEBSOCKET_OP_TEXT
 * - WEBSOCKET_OP_BINARY
 * - WEBSOCKET_OP_CLOSE
 * - WEBSOCKET_OP_PING
 * - WEBSOCKET_OP_PONG
 * `data` and `data_len` contain frame data.
 */
void ns_send_websocket_frame(struct ns_connection *nc, int op, const void *data,
                             size_t data_len);

/*
 * Send multiple websocket frames.
 *
 * Like `ns_send_websocket_frame()`, but composes a frame from multiple buffers.
 */
void ns_send_websocket_framev(struct ns_connection *nc, int op,
                              const struct ns_str *strings, int num_strings);

/*
 * Send websocket frame to the remote end.
 *
 * Like `ns_send_websocket_frame()`, but allows to create formatted message
 * with `printf()`-like semantics.
 */
void ns_printf_websocket_frame(struct ns_connection *nc, int op,
                               const char *fmt, ...);

/*
 * Send buffer `buf` of size `len` to the client using chunked HTTP encoding.
 * This function first sends buffer size as hex number + newline, then
 * buffer itself, then newline. For example,
 *   `ns_send_http_chunk(nc, "foo", 3)` whill append `3\r\nfoo\r\n` string to
 * the `nc->send_mbuf` output IO buffer.
 *
 * NOTE: HTTP header "Transfer-Encoding: chunked" should be sent prior to
 * using this function.
 *
 * NOTE: do not forget to send empty chunk at the end of the response,
 * to tell the client that everything was sent. Example:
 *
 * ```
 *   ns_printf_http_chunk(nc, "%s", "my response!");
 *   ns_send_http_chunk(nc, "", 0); // Tell the client we're finished
 * ```
 */
void ns_send_http_chunk(struct ns_connection *nc, const char *buf, size_t len);

/*
 * Send printf-formatted HTTP chunk.
 * Functionality is similar to `ns_send_http_chunk()`.
 */
void ns_printf_http_chunk(struct ns_connection *, const char *, ...);

/* Websocket opcodes, from http://tools.ietf.org/html/rfc6455 */
#define WEBSOCKET_OP_CONTINUE 0
#define WEBSOCKET_OP_TEXT 1
#define WEBSOCKET_OP_BINARY 2
#define WEBSOCKET_OP_CLOSE 8
#define WEBSOCKET_OP_PING 9
#define WEBSOCKET_OP_PONG 10

/*
 * Parse a HTTP message.
 *
 * `is_req` should be set to 1 if parsing request, 0 if reply.
 *
 * Return number of bytes parsed. If HTTP message is
 * incomplete, `0` is returned. On parse error, negative number is returned.
 */
int ns_parse_http(const char *s, int n, struct http_message *hm, int is_req);

/*
 * Search and return header `name` in parsed HTTP message `hm`.
 * If header is not found, NULL is returned. Example:
 *
 *     struct ns_str *host_hdr = ns_get_http_header(hm, "Host");
 */
struct ns_str *ns_get_http_header(struct http_message *hm, const char *name);

/*
 * Parse HTTP header `hdr`. Find variable `var_name` and store it's value
 * in the buffer `buf`, `buf_size`. Return 0 if variable not found, non-zero
 * otherwise.
 *
 * This function is supposed to parse
 * cookies, authentication headers, etcetera. Example (error handling omitted):
 *
 *     char user[20];
 *     struct ns_str *hdr = ns_get_http_header(hm, "Authorization");
 *     ns_http_parse_header(hdr, "username", user, sizeof(user));
 *
 * Return length of the variable's value. If buffer is not large enough,
 * or variable not found, 0 is returned.
 */
int ns_http_parse_header(struct ns_str *hdr, const char *var_name, char *buf,
                         size_t buf_size);

/*
 * Fetch an HTTP form variable.
 *
 * Fetch a variable `name` from a `buf` into a buffer specified by
 * `dst`, `dst_len`. Destination is always zero-terminated. Return length
 * of a fetched variable. If not found, 0 is returned. `buf` must be
 * valid url-encoded buffer. If destination is too small, `-1` is returned.
 */
int ns_get_http_var(const struct ns_str *, const char *, char *dst, size_t);

/* Create Digest authentication header for client request. */
int ns_http_create_digest_auth_header(char *buf, size_t buf_len,
                                      const char *method, const char *uri,
                                      const char *auth_domain, const char *user,
                                      const char *passwd);
/*
 * Helper function that creates outbound HTTP connection.
 *
 * `url` is a URL to fetch. It must be properly URL-encoded, e.g. have
 * no spaces, etc. By default, `ns_connect_http()` sends Connection and
 * Host headers. `extra_headers` is an extra HTTP headers to send, e.g.
 * `"User-Agent: my-app\r\n"`.
 * If `post_data` is NULL, then GET request is created. Otherwise, POST request
 * is created with the specified POST data. Examples:
 *
 * [source,c]
 * ----
 *   nc1 = ns_connect_http(mgr, ev_handler_1, "http://www.google.com", NULL,
 *                         NULL);
 *   nc2 = ns_connect_http(mgr, ev_handler_1, "https://github.com", NULL, NULL);
 *   nc3 = ns_connect_http(mgr, ev_handler_1, "my_server:8000/form_submit/",
 *                         NULL, "var_1=value_1&var_2=value_2");
 * ----
 */
struct ns_connection *ns_connect_http(struct ns_mgr *,
                                      ns_event_handler_t event_handler,
                                      const char *url,
                                      const char *extra_headers,
                                      const char *post_data);

/*
 * This structure defines how `ns_serve_http()` works.
 * Best practice is to set only required settings, and leave the rest as NULL.
 */
struct ns_serve_http_opts {
  /* Path to web root directory */
  const char *document_root;

  /* List of index files. Default is "" */
  const char *index_files;

  /*
   * Leave as NULL to disable authentication.
   * To enable directory protection with authentication, set this to ".htpasswd"
   * Then, creating ".htpasswd" file in any directory automatically protects
   * it with digest authentication.
   * Use `mongoose` web server binary, or `htdigest` Apache utility to
   * create/manipulate passwords file.
   * Make sure `auth_domain` is set to a valid domain name.
   */
  const char *per_directory_auth_file;

  /* Authorization domain (domain name of this web server) */
  const char *auth_domain;

  /*
   * Leave as NULL to disable authentication.
   * Normally, only selected directories in the document root are protected.
   * If absolutely every access to the web server needs to be authenticated,
   * regardless of the URI, set this option to the path to the passwords file.
   * Format of that file is the same as ".htpasswd" file. Make sure that file
   * is located outside document root to prevent people fetching it.
   */
  const char *global_auth_file;

  /* Set to "no" to disable directory listing. Enabled by default. */
  const char *enable_directory_listing;

  /* SSI files suffix. By default is NULL, SSI is disabled */
  const char *ssi_suffix;

  /* IP ACL. By default, NULL, meaning all IPs are allowed to connect */
  const char *ip_acl;

  /* URL rewrites.
   *
   * Comma-separated list of `uri_pattern=file_or_directory_path` rewrites.
   * When HTTP request is received, Fossa constructs a file name from the
   * requested URI by combining `document_root` and the URI. However, if the
   * rewrite option is used and `uri_pattern` matches requested URI, then
   * `document_root` is ignored. Instead, `file_or_directory_path` is used,
   * which should be a full path name or a path relative to the web server's
   * current working directory. Note that `uri_pattern`, as all Fossa patterns,
   * is a prefix pattern.
   *
   * If uri_pattern starts with `@` symbol, then Fossa compares it with the
   * HOST header of the request. If they are equal, Fossa sets document root
   * to `file_or_directory_path`, implementing virtual hosts support.
   */
  const char *url_rewrites;

  /* DAV document root. If NULL, DAV requests are going to fail. */
  const char *dav_document_root;

  /* Glob pattern for the files to hide. */
  const char *hidden_file_pattern;

  /* Set to non-NULL to enable CGI, e.g. **.cgi$|**.php$" */
  const char *cgi_file_pattern;

  /* If not NULL, ignore CGI script hashbang and use this interpreter */
  const char *cgi_interpreter;

  /*
   * Comma-separated list of Content-Type overrides for path suffixes, e.g.
   * ".txt=text/plain; charset=utf-8,.c=text/plain"
   */
  const char *custom_mime_types;
};

/*
 * Serve given HTTP request according to the `options`.
 *
 * Example code snippet:
 *
 * [source,c]
 * .web_server.c
 * ----
 * static void ev_handler(struct ns_connection *nc, int ev, void *ev_data) {
 *   struct http_message *hm = (struct http_message *) ev_data;
 *   struct ns_serve_http_opts opts = { .document_root = "/var/www" };  // C99
 *
 *   switch (ev) {
 *     case NS_HTTP_REQUEST:
 *       ns_serve_http(nc, hm, opts);
 *       break;
 *     default:
 *       break;
 *   }
 * }
 * ----
 */
void ns_serve_http(struct ns_connection *, struct http_message *,
                   struct ns_serve_http_opts);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_HTTP_HEADER_DEFINED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === JSON-RPC
 */

#ifndef NS_JSON_RPC_HEADER_DEFINED
#define NS_JSON_RPC_HEADER_DEFINED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* JSON-RPC request */
struct ns_rpc_request {
  struct json_token *message; /* Whole RPC message */
  struct json_token *id;      /* Message ID */
  struct json_token *method;  /* Method name */
  struct json_token *params;  /* Method params */
};

/* JSON-RPC response */
struct ns_rpc_reply {
  struct json_token *message; /* Whole RPC message */
  struct json_token *id;      /* Message ID */
  struct json_token *result;  /* Remote call result */
};

/* JSON-RPC error */
struct ns_rpc_error {
  struct json_token *message;       /* Whole RPC message */
  struct json_token *id;            /* Message ID */
  struct json_token *error_code;    /* error.code */
  struct json_token *error_message; /* error.message */
  struct json_token *error_data;    /* error.data, can be NULL */
};

/*
 * Parse JSON-RPC reply contained in `buf`, `len` into JSON tokens array
 * `toks`, `max_toks`. If buffer contains valid reply, `reply` structure is
 * populated. The result of RPC call is located in `reply.result`. On error,
 * `error` structure is populated. Returns: the result of calling
 * `parse_json(buf, len, toks, max_toks)`:
 *
 * On success, an offset inside `json_string` is returned
 * where parsing has finished. On failure, a negative number is
 * returned, one of:
 *
 *  - #define JSON_STRING_INVALID           -1
 *  - #define JSON_STRING_INCOMPLETE        -2
 *  - #define JSON_TOKEN_ARRAY_TOO_SMALL    -3
 */
int ns_rpc_parse_reply(const char *buf, int len, struct json_token *toks,
                       int max_toks, struct ns_rpc_reply *,
                       struct ns_rpc_error *);

/*
 * Create JSON-RPC request in a given buffer.
 *
 * Return length of the request, which
 * can be larger then `len` that indicates an overflow.
 * `params_fmt` format string should conform to `json_emit()` API,
 * see https://github.com/cesanta/frozen
 */
int ns_rpc_create_request(char *buf, int len, const char *method,
                          const char *id, const char *params_fmt, ...);

/*
 * Create JSON-RPC reply in a given buffer.
 *
 * Return length of the reply, which
 * can be larger then `len` that indicates an overflow.
 * `result_fmt` format string should conform to `json_emit()` API,
 * see https://github.com/cesanta/frozen
 */
int ns_rpc_create_reply(char *buf, int len, const struct ns_rpc_request *req,
                        const char *result_fmt, ...);

/*
 * Create JSON-RPC error reply in a given buffer.
 *
 * Return length of the error, which
 * can be larger then `len` that indicates an overflow.
 * `fmt` format string should conform to `json_emit()` API,
 * see https://github.com/cesanta/frozen
 */
int ns_rpc_create_error(char *buf, int len, struct ns_rpc_request *req,
                        int code, const char *message, const char *fmt, ...);

/* JSON-RPC standard error codes */
#define JSON_RPC_PARSE_ERROR (-32700)
#define JSON_RPC_INVALID_REQUEST_ERROR (-32600)
#define JSON_RPC_METHOD_NOT_FOUND_ERROR (-32601)
#define JSON_RPC_INVALID_PARAMS_ERROR (-32602)
#define JSON_RPC_INTERNAL_ERROR (-32603)
#define JSON_RPC_SERVER_ERROR (-32000)

/*
 * Create JSON-RPC error in a given buffer.
 *
 * Return length of the error, which
 * can be larger then `len` that indicates an overflow. See
 * JSON_RPC_*_ERROR definitions for standard error values:
 *
 * - #define JSON_RPC_PARSE_ERROR (-32700)
 * - #define JSON_RPC_INVALID_REQUEST_ERROR (-32600)
 * - #define JSON_RPC_METHOD_NOT_FOUND_ERROR (-32601)
 * - #define JSON_RPC_INVALID_PARAMS_ERROR (-32602)
 * - #define JSON_RPC_INTERNAL_ERROR (-32603)
 * - #define JSON_RPC_SERVER_ERROR (-32000)
 */
int ns_rpc_create_std_error(char *, int, struct ns_rpc_request *, int code);

typedef int (*ns_rpc_handler_t)(char *buf, int len, struct ns_rpc_request *);

/*
 * Dispatches a JSON-RPC request.
 *
 * Parses JSON-RPC request contained in `buf`, `len`.
 * Then, dispatches the request to the correct handler method.
 * Valid method names should be specified in NULL
 * terminated array `methods`, and corresponding handlers in `handlers`.
 * Result is put in `dst`, `dst_len`. Return: length of the result, which
 * can be larger then `dst_len` that indicates an overflow.
 * Overflown bytes are not written to the buffer.
 * If method is not found, an error is automatically generated.
 */
int ns_rpc_dispatch(const char *buf, int, char *dst, int dst_len,
                    const char **methods, ns_rpc_handler_t *handlers);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_JSON_RPC_HEADER_DEFINED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/>.
 */

/*
 * === MQTT
 */

#ifndef NS_MQTT_HEADER_INCLUDED
#define NS_MQTT_HEADER_INCLUDED


struct ns_mqtt_message {
  int cmd;
  struct ns_str payload;
  int qos;
  uint8_t connack_ret_code; /* connack */
  uint16_t message_id;      /* puback */
  char *topic;
};

struct ns_mqtt_topic_expression {
  const char *topic;
  uint8_t qos;
};

struct ns_send_mqtt_handshake_opts {
  unsigned char flags; /* connection flags */
  uint16_t keep_alive;
  const char *will_topic;
  const char *will_message;
  const char *user_name;
  const char *password;
};

/* Message types */
#define NS_MQTT_CMD_CONNECT 1
#define NS_MQTT_CMD_CONNACK 2
#define NS_MQTT_CMD_PUBLISH 3
#define NS_MQTT_CMD_PUBACK 4
#define NS_MQTT_CMD_PUBREC 5
#define NS_MQTT_CMD_PUBREL 6
#define NS_MQTT_CMD_PUBCOMP 7
#define NS_MQTT_CMD_SUBSCRIBE 8
#define NS_MQTT_CMD_SUBACK 9
#define NS_MQTT_CMD_UNSUBSCRIBE 10
#define NS_MQTT_CMD_UNSUBACK 11
#define NS_MQTT_CMD_PINGREQ 12
#define NS_MQTT_CMD_PINGRESP 13
#define NS_MQTT_CMD_DISCONNECT 14

/* MQTT event types */
#define NS_MQTT_EVENT_BASE 200
#define NS_MQTT_CONNECT (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_CONNECT)
#define NS_MQTT_CONNACK (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_CONNACK)
#define NS_MQTT_PUBLISH (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PUBLISH)
#define NS_MQTT_PUBACK (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PUBACK)
#define NS_MQTT_PUBREC (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PUBREC)
#define NS_MQTT_PUBREL (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PUBREL)
#define NS_MQTT_PUBCOMP (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PUBCOMP)
#define NS_MQTT_SUBSCRIBE (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_SUBSCRIBE)
#define NS_MQTT_SUBACK (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_SUBACK)
#define NS_MQTT_UNSUBSCRIBE (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_UNSUBSCRIBE)
#define NS_MQTT_UNSUBACK (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_UNSUBACK)
#define NS_MQTT_PINGREQ (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PINGREQ)
#define NS_MQTT_PINGRESP (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_PINGRESP)
#define NS_MQTT_DISCONNECT (NS_MQTT_EVENT_BASE + NS_MQTT_CMD_DISCONNECT)

/* Message flags */
#define NS_MQTT_RETAIN 0x1
#define NS_MQTT_DUP 0x4
#define NS_MQTT_QOS(qos) ((qos) << 1)
#define NS_MQTT_GET_QOS(flags) (((flags) &0x6) >> 1)
#define NS_MQTT_SET_QOS(flags, qos) (flags) = ((flags) & ~0x6) | ((qos) << 1)

/* Connection flags */
#define NS_MQTT_CLEAN_SESSION 0x02
#define NS_MQTT_HAS_WILL 0x04
#define NS_MQTT_WILL_RETAIN 0x20
#define NS_MQTT_HAS_PASSWORD 0x40
#define NS_MQTT_HAS_USER_NAME 0x80
#define NS_MQTT_GET_WILL_QOS(flags) (((flags) &0x18) >> 3)
#define NS_MQTT_SET_WILL_QOS(flags, qos) \
  (flags) = ((flags) & ~0x18) | ((qos) << 3)

/* CONNACK return codes */
#define NS_MQTT_CONNACK_ACCEPTED 0
#define NS_MQTT_CONNACK_UNACCEPTABLE_VERSION 1
#define NS_MQTT_CONNACK_IDENTIFIER_REJECTED 2
#define NS_MQTT_CONNACK_SERVER_UNAVAILABLE 3
#define NS_MQTT_CONNACK_BAD_AUTH 4
#define NS_MQTT_CONNACK_NOT_AUTHORIZED 5

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Attach built-in MQTT event handler to the given connection.
 *
 * The user-defined event handler will receive following extra events:
 *
 * - NS_MQTT_CONNACK
 * - NS_MQTT_PUBLISH
 * - NS_MQTT_PUBACK
 * - NS_MQTT_PUBREC
 * - NS_MQTT_PUBREL
 * - NS_MQTT_PUBCOMP
 * - NS_MQTT_SUBACK
 */
void ns_set_protocol_mqtt(struct ns_connection *);

/* Send MQTT handshake. */
void ns_send_mqtt_handshake(struct ns_connection *nc, const char *client_id);

/* Send MQTT handshake with optional parameters. */
void ns_send_mqtt_handshake_opt(struct ns_connection *, const char *client_id,
                                struct ns_send_mqtt_handshake_opts);

/* Publish a message to a given topic. */
void ns_mqtt_publish(struct ns_connection *nc, const char *topic,
                     uint16_t message_id, int flags, const void *data,
                     size_t len);

/* Subscribe to a bunch of topics. */
void ns_mqtt_subscribe(struct ns_connection *nc,
                       const struct ns_mqtt_topic_expression *topics,
                       size_t topics_len, uint16_t message_id);

/* Unsubscribe from a bunch of topics. */
void ns_mqtt_unsubscribe(struct ns_connection *nc, char **topics,
                         size_t topics_len, uint16_t message_id);

/* Send a DISCONNECT command. */
void ns_mqtt_disconnect(struct ns_connection *nc);

/* Send a CONNACK command with a given `return_code`. */
void ns_mqtt_connack(struct ns_connection *, uint8_t);

/* Send a PUBACK command with a given `message_id`. */
void ns_mqtt_puback(struct ns_connection *, uint16_t);

/* Send a PUBREC command with a given `message_id`. */
void ns_mqtt_pubrec(struct ns_connection *, uint16_t);

/* Send a PUBREL command with a given `message_id`. */
void ns_mqtt_pubrel(struct ns_connection *, uint16_t);

/* Send a PUBCOMP command with a given `message_id`. */
void ns_mqtt_pubcomp(struct ns_connection *, uint16_t);

/*
 * Send a SUBACK command with a given `message_id`
 * and a sequence of granted QoSs.
 */
void ns_mqtt_suback(struct ns_connection *, uint8_t *, size_t, uint16_t);

/* Send a UNSUBACK command with a given `message_id`. */
void ns_mqtt_unsuback(struct ns_connection *, uint16_t);

/* Send a PINGREQ command. */
void ns_mqtt_ping(struct ns_connection *);

/* Send a PINGRESP command. */
void ns_mqtt_pong(struct ns_connection *);

/*
 * Extract the next topic expression from a SUBSCRIBE command payload.
 *
 * Topic expression name will point to a string in the payload buffer.
 * Return the pos of the next topic expression or -1 when the list
 * of topics is exhausted.
 */
int ns_mqtt_next_subscribe_topic(struct ns_mqtt_message *, struct ns_str *,
                                 uint8_t *, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NS_MQTT_HEADER_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/>.
 */

/*
 * === MQTT Broker
 */

#ifndef NS_MQTT_BROKER_HEADER_INCLUDED
#define NS_MQTT_BROKER_HEADER_INCLUDED

#ifdef NS_ENABLE_MQTT_BROKER


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NS_MQTT_MAX_SESSION_SUBSCRIPTIONS 512;

struct ns_mqtt_broker;

/* MQTT session (Broker side). */
struct ns_mqtt_session {
  struct ns_mqtt_broker *brk;          /* Broker */
  struct ns_mqtt_session *next, *prev; /* ns_mqtt_broker::sessions linkage */
  struct ns_connection *nc;            /* Connection with the client */
  size_t num_subscriptions;            /* Size of `subscriptions` array */
  struct ns_mqtt_topic_expression *subscriptions;
  void *user_data; /* User data */
};

/* MQTT broker. */
struct ns_mqtt_broker {
  struct ns_mqtt_session *sessions; /* Session list */
  void *user_data;                  /* User data */
};

/* Initialize a MQTT broker. */
void ns_mqtt_broker_init(struct ns_mqtt_broker *, void *);

/*
 * Process a MQTT broker message.
 *
 * Listening connection expects a pointer to an initialized `ns_mqtt_broker`
 * structure in the `user_data` field.
 *
 * Basic usage:
 *
 * [source,c]
 * -----
 * ns_mqtt_broker_init(&brk, NULL);
 *
 * if ((nc = ns_bind(&mgr, address, ns_mqtt_broker)) == NULL) {
 *   // fail;
 * }
 * nc->user_data = &brk;
 * -----
 *
 * New incoming connections will receive a `ns_mqtt_session` structure
 * in the connection `user_data`. The original `user_data` will be stored
 * in the `user_data` field of the session structure. This allows the user
 * handler to store user data before `ns_mqtt_broker` creates the session.
 *
 * Since only the NS_ACCEPT message is processed by the listening socket,
 * for most events the `user_data` will thus point to a `ns_mqtt_session`.
 */
void ns_mqtt_broker(struct ns_connection *, int, void *);

/*
 * Iterate over all mqtt sessions connections. Example:
 *
 *    struct ns_mqtt_session *s;
 *    for (s = ns_mqtt_next(brk, NULL); s != NULL; s = ns_mqtt_next(brk, s)) {
 *       // Do something
 *    }
 */
struct ns_mqtt_session *ns_mqtt_next(struct ns_mqtt_broker *,
                                     struct ns_mqtt_session *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NS_ENABLE_MQTT_BROKER */
#endif /* NS_MQTT_HEADER_INCLUDED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === DNS
 */

#ifndef NS_DNS_HEADER_DEFINED
#define NS_DNS_HEADER_DEFINED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NS_DNS_A_RECORD 0x01     /* Lookup IP address */
#define NS_DNS_CNAME_RECORD 0x05 /* Lookup CNAME */
#define NS_DNS_AAAA_RECORD 0x1c  /* Lookup IPv6 address */
#define NS_DNS_MX_RECORD 0x0f    /* Lookup mail server for domain */

#define NS_MAX_DNS_QUESTIONS 32
#define NS_MAX_DNS_ANSWERS 32

#define NS_DNS_MESSAGE 100 /* High-level DNS message event */

enum ns_dns_resource_record_kind {
  NS_DNS_INVALID_RECORD = 0,
  NS_DNS_QUESTION,
  NS_DNS_ANSWER
};

/* DNS resource record. */
struct ns_dns_resource_record {
  struct ns_str name; /* buffer with compressed name */
  int rtype;
  int rclass;
  int ttl;
  enum ns_dns_resource_record_kind kind;
  struct ns_str rdata; /* protocol data (can be a compressed name) */
};

/* DNS message (request and response). */
struct ns_dns_message {
  struct ns_str pkt; /* packet body */
  uint16_t flags;
  uint16_t transaction_id;
  int num_questions;
  int num_answers;
  struct ns_dns_resource_record questions[NS_MAX_DNS_QUESTIONS];
  struct ns_dns_resource_record answers[NS_MAX_DNS_ANSWERS];
};

struct ns_dns_resource_record *ns_dns_next_record(
    struct ns_dns_message *, int, struct ns_dns_resource_record *);

/*
 * Parse the record data from a DNS resource record.
 *
 *  - A:     struct in_addr *ina
 *  - AAAA:  struct in6_addr *ina
 *  - CNAME: char buffer
 *
 * Returns -1 on error.
 *
 * TODO(mkm): MX
 */
int ns_dns_parse_record_data(struct ns_dns_message *,
                             struct ns_dns_resource_record *, void *, size_t);

/*
 * Send a DNS query to the remote end.
 */
void ns_send_dns_query(struct ns_connection *, const char *, int);

/*
 * Insert a DNS header to an IO buffer.
 *
 * Return number of bytes inserted.
 */
int ns_dns_insert_header(struct mbuf *, size_t, struct ns_dns_message *);

/*
 * Append already encoded body from an existing message.
 *
 * This is useful when generating a DNS reply message which includes
 * all question records.
 *
 * Return number of appened bytes.
 */
int ns_dns_copy_body(struct mbuf *, struct ns_dns_message *);

/*
 * Encode and append a DNS resource record to an IO buffer.
 *
 * The record metadata is taken from the `rr` parameter, while the name and data
 * are taken from the parameters, encoded in the appropriate format depending on
 * record type, and stored in the IO buffer. The encoded values might contain
 * offsets within the IO buffer. It's thus important that the IO buffer doesn't
 * get trimmed while a sequence of records are encoded while preparing a DNS
 *reply.
 *
 * This function doesn't update the `name` and `rdata` pointers in the `rr`
 *struct
 * because they might be invalidated as soon as the IO buffer grows again.
 *
 * Return the number of bytes appened or -1 in case of error.
 */
int ns_dns_encode_record(struct mbuf *, struct ns_dns_resource_record *,
                         const char *, size_t, const void *, size_t);

/* Low-level: parses a DNS response. */
int ns_parse_dns(const char *, int, struct ns_dns_message *);

/*
 * Uncompress a DNS compressed name.
 *
 * The containing dns message is required because the compressed encoding
 * and reference suffixes present elsewhere in the packet.
 *
 * If name is less than `dst_len` characters long, the remainder
 * of `dst` is terminated with `\0' characters. Otherwise, `dst` is not
 *terminated.
 *
 * If `dst_len` is 0 `dst` can be NULL.
 * Return the uncompressed name length.
 */
size_t ns_dns_uncompress_name(struct ns_dns_message *, struct ns_str *, char *,
                              int);

/*
 * Attach built-in DNS event handler to the given listening connection.
 *
 * DNS event handler parses incoming UDP packets, treating them as DNS
 * requests. If incoming packet gets successfully parsed by the DNS event
 * handler, a user event handler will receive `NS_DNS_REQUEST` event, with
 * `ev_data` pointing to the parsed `struct ns_dns_message`.
 *
 * See
 * https://github.com/cesanta/fossa/tree/master/examples/captive_dns_server[captive_dns_server]
 * example on how to handle DNS request and send DNS reply.
 */
void ns_set_protocol_dns(struct ns_connection *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_HTTP_HEADER_DEFINED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === DNS server
 *
 * Disabled by default; enable with `-DNS_ENABLE_DNS_SERVER`.
 */

#ifndef NS_DNS_SERVER_HEADER_DEFINED
#define NS_DNS_SERVER_HEADER_DEFINED

#ifdef NS_ENABLE_DNS_SERVER


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NS_DNS_SERVER_DEFAULT_TTL 3600

struct ns_dns_reply {
  struct ns_dns_message *msg;
  struct mbuf *io;
  size_t start;
};

/*
 * Create a DNS reply.
 *
 * The reply will be based on an existing query message `msg`.
 * The query body will be appended to the output buffer.
 * "reply + recursion allowed" will be added to the message flags and
 * message's num_answers will be set to 0.
 *
 * Answer records can be appended with `ns_dns_send_reply` or by lower
 * level function defined in the DNS API.
 *
 * In order to send the reply use `ns_dns_send_reply`.
 * It's possible to use a connection's send buffer as reply buffers,
 * and it will work for both UDP and TCP connections.
 *
 * Example:
 *
 * [source,c]
 * -----
 * reply = ns_dns_create_reply(&nc->send_mbuf, msg);
 * for (i = 0; i < msg->num_questions; i++) {
 *   rr = &msg->questions[i];
 *   if (rr->rtype == NS_DNS_A_RECORD) {
 *     ns_dns_reply_record(&reply, rr, 3600, &dummy_ip_addr, 4);
 *   }
 * }
 * ns_dns_send_reply(nc, &reply);
 * -----
 */
struct ns_dns_reply ns_dns_create_reply(struct mbuf *, struct ns_dns_message *);

/*
 * Append a DNS reply record to the IO buffer and to the DNS message.
 *
 * The message num_answers field will be incremented. It's caller's duty
 * to ensure num_answers is propertly initialized.
 *
 * Returns -1 on error.
 */
int ns_dns_reply_record(struct ns_dns_reply *, struct ns_dns_resource_record *,
                        const char *, int, int, const void *, size_t);

/*
 * Send a DNS reply through a connection.
 *
 * The DNS data is stored in an IO buffer pointed by reply structure in `r`.
 * This function mutates the content of that buffer in order to ensure that
 * the DNS header reflects size and flags of the mssage, that might have been
 * updated either with `ns_dns_reply_record` or by direct manipulation of
 * `r->message`.
 *
 * Once sent, the IO buffer will be trimmed unless the reply IO buffer
 * is the connection's send buffer and the connection is not in UDP mode.
 */
int ns_dns_send_reply(struct ns_connection *, struct ns_dns_reply *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NS_ENABLE_DNS_SERVER */
#endif /* NS_HTTP_HEADER_DEFINED */
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

/*
 * === Asynchronouns DNS resolver
 */

#ifndef NS_RESOLV_HEADER_DEFINED
#define NS_RESOLV_HEADER_DEFINED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*ns_resolve_callback_t)(struct ns_dns_message *, void *);

/* Options for `ns_resolve_async_opt`. */
struct ns_resolve_async_opts {
  const char *nameserver_url;
  int max_retries;    /* defaults to 2 if zero */
  int timeout;        /* in seconds; defaults to 5 if zero */
  int accept_literal; /* pseudo-resolve literal ipv4 and ipv6 addrs */
  int only_literal;   /* only resolves literal addrs; sync cb invocation */
};

/* See `ns_resolve_async_opt()` */
int ns_resolve_async(struct ns_mgr *, const char *, int, ns_resolve_callback_t,
                     void *data);

/*
 * Resolved a DNS name asynchronously.
 *
 * Upon successful resolution, the user callback will be invoked
 * with the full DNS response message and a pointer to the user's
 * context `data`.
 *
 * In case of timeout while performing the resolution the callback
 * will receive a NULL `msg`.
 *
 * The DNS answers can be extracted with `ns_next_record` and
 * `ns_dns_parse_record_data`:
 *
 * [source,c]
 * ----
 * struct in_addr ina;
 * struct ns_dns_resource_record *rr = ns_next_record(msg, NS_DNS_A_RECORD,
 *   NULL);
 * ns_dns_parse_record_data(msg, rr, &ina, sizeof(ina));
 * ----
 */
int ns_resolve_async_opt(struct ns_mgr *, const char *, int,
                         ns_resolve_callback_t, void *data,
                         struct ns_resolve_async_opts opts);

/*
 * Resolve a name from `/etc/hosts`.
 *
 * Returns 0 on success, -1 on failure.
 */
int ns_resolve_from_hosts_file(const char *host, union socket_address *usa);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* NS_RESOLV_HEADER_DEFINED */
/*
 * Copyright (c) 2015 Cesanta Software Limited
 * All rights reserved
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/>.
 */

/*
 * === CoAP
 *
 * CoAP message format:
 *
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *    |Ver| T | TKL | Code | Message ID | Token (if any, TKL bytes) ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *    | Options (if any) ...            |1 1 1 1 1 1 1 1| Payload (if any) ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

#ifndef NS_COAP_HEADER_INCLUDED
#define NS_COAP_HEADER_INCLUDED

#ifdef NS_ENABLE_COAP

#define NS_COAP_MSG_TYPE_FIELD 0x2
#define NS_COAP_CODE_CLASS_FIELD 0x4
#define NS_COAP_CODE_DETAIL_FIELD 0x8
#define NS_COAP_MSG_ID_FIELD 0x10
#define NS_COAP_TOKEN_FIELD 0x20
#define NS_COAP_OPTIONS_FIELD 0x40
#define NS_COAP_PAYLOAD_FIELD 0x80

#define NS_COAP_ERROR 0x10000
#define NS_COAP_FORMAT_ERROR (NS_COAP_ERROR | 0x20000)
#define NS_COAP_IGNORE (NS_COAP_ERROR | 0x40000)
#define NS_COAP_NOT_ENOUGH_DATA (NS_COAP_ERROR | 0x80000)
#define NS_COAP_NETWORK_ERROR (NS_COAP_ERROR | 0x100000)

#define NS_COAP_MSG_CON 0
#define NS_COAP_MSG_NOC 1
#define NS_COAP_MSG_ACK 2
#define NS_COAP_MSG_RST 3
#define NS_COAP_MSG_MAX 3

#define NS_COAP_CODECLASS_REQUEST 0
#define NS_COAP_CODECLASS_RESP_OK 2
#define NS_COAP_CODECLASS_CLIENT_ERR 4
#define NS_COAP_CODECLASS_SRV_ERR 5

#define NS_COAP_EVENT_BASE 300
#define NS_COAP_CON (NS_COAP_EVENT_BASE + NS_COAP_MSG_CON)
#define NS_COAP_NOC (NS_COAP_EVENT_BASE + NS_COAP_MSG_NOC)
#define NS_COAP_ACK (NS_COAP_EVENT_BASE + NS_COAP_MSG_ACK)
#define NS_COAP_RST (NS_COAP_EVENT_BASE + NS_COAP_MSG_RST)

/*
 * CoAP options.
 * Use ns_coap_add_option and ns_coap_free_options
 * for creation and destruction.
 */
struct ns_coap_option {
  struct ns_coap_option *next;
  uint32_t number;
  struct ns_str value;
};

/* CoAP message. See RFC 7252 for details. */
struct ns_coap_message {
  uint32_t flags;
  uint8_t msg_type;
  uint8_t code_class;
  uint8_t code_detail;
  uint16_t msg_id;
  struct ns_str token;
  struct ns_coap_option *options;
  struct ns_str payload;
  struct ns_coap_option *options_tail;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Set CoAP protocol handler - trigger CoAP specific events */
int ns_set_protocol_coap(struct ns_connection *nc);

/*
 * Add new option to ns_coap_message structure.
 * Returns pointer to the newly created option.
 */
struct ns_coap_option *ns_coap_add_option(struct ns_coap_message *cm,
                                          uint32_t number, char *value,
                                          size_t len);

/*
 * Free the memory allocated for options,
 * if cm paramater doesn't contain any option does nothing.
 */
void ns_coap_free_options(struct ns_coap_message *cm);

/*
 * Compose CoAP message from `ns_coap_message`
 * and send it into `nc` connection.
 * Return 0 on success. On error, it is a bitmask:
 *
 * - #define NS_COAP_ERROR 0x10000
 * - #define NS_COAP_FORMAT_ERROR (NS_COAP_ERROR | 0x20000)
 * - #define NS_COAP_IGNORE (NS_COAP_ERROR | 0x40000)
 * - #define NS_COAP_NOT_ENOUGH_DATA (NS_COAP_ERROR | 0x80000)
 * - #define NS_COAP_NETWORK_ERROR (NS_COAP_ERROR | 0x100000)
 */
uint32_t ns_coap_send_message(struct ns_connection *nc,
                              struct ns_coap_message *cm);

/*
 * Compose CoAP acknowledgement from `ns_coap_message`
 * and send it into `nc` connection.
 * Return value: see `ns_coap_send_message()`
 */
uint32_t ns_coap_send_ack(struct ns_connection *nc, uint16_t msg_id);

/*
 * Parse COAP message and fills ns_coap_message and returns cm->flags.
 * This is a helper function.
 *
 * NOTE: usually CoAP work over UDP, so lack of data means format error,
 * but in theory it is possible to use CoAP over TCP (according to RFC)
 *
 * The caller have to check results and treat COAP_NOT_ENOUGH_DATA according to
 * underlying protocol:
 *
 * - in case of UDP COAP_NOT_ENOUGH_DATA means COAP_FORMAT_ERROR,
 * - in case of TCP client can try to recieve more data
 *
 * Return value: see `ns_coap_send_message()`
 */
uint32_t ns_coap_parse(struct mbuf *io, struct ns_coap_message *cm);

/*
 * Composes CoAP message from ns_coap_message structure.
 * This is a helper function.
 * Return value: see `ns_coap_send_message()`
 */
uint32_t ns_coap_compose(struct ns_coap_message *cm, struct mbuf *io);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NS_ENABLE_COAP */

#endif /* NS_COAP_HEADER_INCLUDED */
