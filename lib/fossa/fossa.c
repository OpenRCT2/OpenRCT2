#include "fossa.h"
#ifdef NS_MODULE_LINES
#line 1 "src/internal.h"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef NS_INTERNAL_HEADER_INCLUDED
#define NS_INTERNAL_HEADER_INCLUDED

#ifndef NS_MALLOC
#define NS_MALLOC malloc
#endif

#ifndef NS_CALLOC
#define NS_CALLOC calloc
#endif

#ifndef NS_REALLOC
#define NS_REALLOC realloc
#endif

#ifndef NS_FREE
#define NS_FREE free
#endif

#ifndef MBUF_REALLOC
#define MBUF_REALLOC NS_REALLOC
#endif

#ifndef MBUF_FREE
#define MBUF_FREE NS_FREE
#endif

#define NS_SET_PTRPTR(_ptr, _v) \
  do {                          \
    if (_ptr) *(_ptr) = _v;     \
  } while (0)

#ifndef NS_INTERNAL
#define NS_INTERNAL static
#endif


/* internals that need to be accessible in unit tests */
NS_INTERNAL struct ns_connection *ns_finish_connect(struct ns_connection *nc,
                                                    int proto,
                                                    union socket_address *sa,
                                                    struct ns_add_sock_opts);

NS_INTERNAL int ns_parse_address(const char *str, union socket_address *sa,
                                 int *proto, char *host, size_t host_len);

#ifdef _WIN32
NS_INTERNAL void to_wchar(const char *path, wchar_t *wbuf, size_t wbuf_len);
#endif

#endif /* NS_INTERNAL_HEADER_INCLUDED */
#ifdef NS_MODULE_LINES
#line 1 "src/../../common/mbuf.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#include <assert.h>
#include <string.h>

#ifndef MBUF_REALLOC
#define MBUF_REALLOC realloc
#endif

#ifndef MBUF_FREE
#define MBUF_FREE free
#endif

ON_FLASH void mbuf_init(struct mbuf *mbuf, size_t initial_size) {
  mbuf->len = mbuf->size = 0;
  mbuf->buf = NULL;
  mbuf_resize(mbuf, initial_size);
}

ON_FLASH void mbuf_free(struct mbuf *mbuf) {
  if (mbuf->buf != NULL) {
    MBUF_FREE(mbuf->buf);
    mbuf_init(mbuf, 0);
  }
}

ON_FLASH void mbuf_resize(struct mbuf *a, size_t new_size) {
  char *p;
  if ((new_size > a->size || (new_size < a->size && new_size >= a->len)) &&
      (p = (char *) MBUF_REALLOC(a->buf, new_size)) != NULL) {
    a->size = new_size;
    a->buf = p;
  }
}

ON_FLASH void mbuf_trim(struct mbuf *mbuf) {
  mbuf_resize(mbuf, mbuf->len);
}

ON_FLASH size_t
mbuf_insert(struct mbuf *a, size_t off, const void *buf, size_t len) {
  char *p = NULL;

#ifndef NO_LIBC
  assert(a != NULL);
  assert(a->len <= a->size);
  assert(off <= a->len);
#endif

  /* check overflow */
  if (~(size_t) 0 - (size_t) a->buf < len) return 0;

  if (a->len + len <= a->size) {
    memmove(a->buf + off + len, a->buf + off, a->len - off);
    if (buf != NULL) {
      memcpy(a->buf + off, buf, len);
    }
    a->len += len;
  } else if ((p = (char *) MBUF_REALLOC(
                  a->buf, (a->len + len) * MBUF_SIZE_MULTIPLIER)) != NULL) {
    a->buf = p;
    memmove(a->buf + off + len, a->buf + off, a->len - off);
    if (buf != NULL) {
      memcpy(a->buf + off, buf, len);
    }
    a->len += len;
    a->size = a->len * MBUF_SIZE_MULTIPLIER;
  } else {
    len = 0;
  }

  return len;
}

ON_FLASH size_t mbuf_append(struct mbuf *a, const void *buf, size_t len) {
  return mbuf_insert(a, a->len, buf, len);
}

ON_FLASH void mbuf_remove(struct mbuf *mb, size_t n) {
  if (n > 0 && n <= mb->len) {
    memmove(mb->buf, mb->buf + n, mb->len - n);
    mb->len -= n;
  }
}
#ifdef NS_MODULE_LINES
#line 1 "src/../../common/sha1.c"
/**/
#endif
/* Copyright(c) By Steve Reid <steve@edmweb.com> */
/* 100% Public Domain */

#ifndef ISABLE_SHA1


#define SHA1HANDSOFF
#if defined(__sun)
#endif

union char64long16 { unsigned char c[64]; uint32_t l[16]; };

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

ON_FLASH static uint32_t blk0(union char64long16 *block, int i) {
  /* Forrest: SHA expect BIG_ENDIAN, swap if LITTLE_ENDIAN */
#if BYTE_ORDER == LITTLE_ENDIAN
    block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) |
      (rol(block->l[i], 8) & 0x00FF00FF);
#endif
  return block->l[i];
}

/* Avoid redefine warning (ARM /usr/include/sys/ucontext.h define R0~R4) */
#undef blk
#undef R0
#undef R1
#undef R2
#undef R3
#undef R4

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(block, i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

ON_FLASH void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]) {
  uint32_t a, b, c, d, e;
  union char64long16 block[1];

  memcpy(block, buffer, 64);
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
  R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
  R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
  R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
  R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
  R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
  R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
  R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
  R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
  R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
  R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
  R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
  R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
  R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
  R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
  R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
  R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
  R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
  R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
  R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  /* Erase working structures. The order of operations is important,
   * used to ensure that compiler doesn't optimize those out. */
  memset(block, 0, sizeof(block));
  a = b = c = d = e = 0;
  (void) a; (void) b; (void) c; (void) d; (void) e;
}

ON_FLASH void SHA1Init(SHA1_CTX *context) {
  context->state[0] = 0x67452301;
  context->state[1] = 0xEFCDAB89;
  context->state[2] = 0x98BADCFE;
  context->state[3] = 0x10325476;
  context->state[4] = 0xC3D2E1F0;
  context->count[0] = context->count[1] = 0;
}

ON_FLASH void SHA1Update(SHA1_CTX *context, const unsigned char *data, uint32_t len) {
  uint32_t i, j;

  j = context->count[0];
  if ((context->count[0] += len << 3) < j)
    context->count[1]++;
  context->count[1] += (len>>29);
  j = (j >> 3) & 63;
  if ((j + len) > 63) {
    memcpy(&context->buffer[j], data, (i = 64-j));
    SHA1Transform(context->state, context->buffer);
    for ( ; i + 63 < len; i += 64) {
      SHA1Transform(context->state, &data[i]);
    }
    j = 0;
  }
  else i = 0;
  memcpy(&context->buffer[j], &data[i], len - i);
}

ON_FLASH void SHA1Final(unsigned char digest[20], SHA1_CTX *context) {
  unsigned i;
  unsigned char finalcount[8], c;

  for (i = 0; i < 8; i++) {
    finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
                                     >> ((3-(i & 3)) * 8) ) & 255);
  }
  c = 0200;
  SHA1Update(context, &c, 1);
  while ((context->count[0] & 504) != 448) {
    c = 0000;
    SHA1Update(context, &c, 1);
  }
  SHA1Update(context, finalcount, 8);
  for (i = 0; i < 20; i++) {
    digest[i] = (unsigned char)
      ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
  }
  memset(context, '\0', sizeof(*context));
  memset(&finalcount, '\0', sizeof(finalcount));
}
#endif
#ifdef NS_MODULE_LINES
#line 1 "src/../../common/md5.c"
/**/
#endif
/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

#ifndef DISABLE_MD5


ON_FLASH static void byteReverse(unsigned char *buf, unsigned longs) {

  /* Forrest: MD5 expect LITTLE_ENDIAN, swap if BIG_ENDIAN */
#if BYTE_ORDER == BIG_ENDIAN
  do {
  uint32_t t = (uint32_t) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
      ((unsigned) buf[1] << 8 | buf[0]);
    * (uint32_t *) buf = t;
    buf += 4;
  } while (--longs);
#else
  (void) buf;
  (void) longs;
#endif
}

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s) \
  ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
ON_FLASH void MD5_Init(MD5_CTX *ctx) {
  ctx->buf[0] = 0x67452301;
  ctx->buf[1] = 0xefcdab89;
  ctx->buf[2] = 0x98badcfe;
  ctx->buf[3] = 0x10325476;

  ctx->bits[0] = 0;
  ctx->bits[1] = 0;
}

ON_FLASH static void MD5Transform(uint32_t buf[4], uint32_t const in[16]) {
  register uint32_t a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
  MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
  MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
  MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
  MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
  MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
  MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
  MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
  MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
  MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
  MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
  MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
  MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
  MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
  MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
  MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
  MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
  MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
  MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
  MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
  MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
  MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
  MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

  MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
  MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
  MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
  MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
  MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

ON_FLASH void MD5_Update(MD5_CTX *ctx, const unsigned char *buf, size_t len) {
  uint32_t t;

  t = ctx->bits[0];
  if ((ctx->bits[0] = t + ((uint32_t) len << 3)) < t)
    ctx->bits[1]++;
  ctx->bits[1] += (uint32_t) len >> 29;

  t = (t >> 3) & 0x3f;

  if (t) {
    unsigned char *p = (unsigned char *) ctx->in + t;

    t = 64 - t;
    if (len < t) {
      memcpy(p, buf, len);
      return;
    }
    memcpy(p, buf, t);
    byteReverse(ctx->in, 16);
    MD5Transform(ctx->buf, (uint32_t *) ctx->in);
    buf += t;
    len -= t;
  }

  while (len >= 64) {
    memcpy(ctx->in, buf, 64);
    byteReverse(ctx->in, 16);
    MD5Transform(ctx->buf, (uint32_t *) ctx->in);
    buf += 64;
    len -= 64;
  }

  memcpy(ctx->in, buf, len);
}

ON_FLASH void MD5_Final(unsigned char digest[16], MD5_CTX *ctx) {
  unsigned count;
  unsigned char *p;
  uint32_t *a;

  count = (ctx->bits[0] >> 3) & 0x3F;

  p = ctx->in + count;
  *p++ = 0x80;
  count = 64 - 1 - count;
  if (count < 8) {
    memset(p, 0, count);
    byteReverse(ctx->in, 16);
    MD5Transform(ctx->buf, (uint32_t *) ctx->in);
    memset(ctx->in, 0, 56);
  } else {
    memset(p, 0, count - 8);
  }
  byteReverse(ctx->in, 14);

  a = (uint32_t *)ctx->in;
  a[14] = ctx->bits[0];
  a[15] = ctx->bits[1];

  MD5Transform(ctx->buf, (uint32_t *) ctx->in);
  byteReverse((unsigned char *) ctx->buf, 4);
  memcpy(digest, ctx->buf, 16);
  memset((char *) ctx, 0, sizeof(*ctx));
}
#endif
#ifdef NS_MODULE_LINES
#line 1 "src/../../common/base64.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */


ON_FLASH void base64_encode(const unsigned char *src, int src_len, char *dst) {
  static const char *b64 =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int i, j, a, b, c;

  for (i = j = 0; i < src_len; i += 3) {
    a = src[i];
    b = i + 1 >= src_len ? 0 : src[i + 1];
    c = i + 2 >= src_len ? 0 : src[i + 2];

    dst[j++] = b64[a >> 2];
    dst[j++] = b64[((a & 3) << 4) | (b >> 4)];
    if (i + 1 < src_len) {
      dst[j++] = b64[(b & 15) << 2 | (c >> 6)];
    }
    if (i + 2 < src_len) {
      dst[j++] = b64[c & 63];
    }
  }
  while (j % 4 != 0) {
    dst[j++] = '=';
  }
  dst[j++] = '\0';
}

/* Convert one byte of encoded base64 input stream to 6-bit chunk */
ON_FLASH static unsigned char from_b64(unsigned char ch) {
  /* Inverse lookup map */
  static const unsigned char tab[128] = {
      255, 255, 255, 255,
      255, 255, 255, 255, /*  0 */
      255, 255, 255, 255,
      255, 255, 255, 255, /*  8 */
      255, 255, 255, 255,
      255, 255, 255, 255, /*  16 */
      255, 255, 255, 255,
      255, 255, 255, 255, /*  24 */
      255, 255, 255, 255,
      255, 255, 255, 255, /*  32 */
      255, 255, 255, 62,
      255, 255, 255, 63, /*  40 */
      52,  53,  54,  55,
      56,  57,  58,  59, /*  48 */
      60,  61,  255, 255,
      255, 200, 255, 255, /*  56   '=' is 200, on index 61 */
      255, 0,   1,   2,
      3,   4,   5,   6, /*  64 */
      7,   8,   9,   10,
      11,  12,  13,  14, /*  72 */
      15,  16,  17,  18,
      19,  20,  21,  22, /*  80 */
      23,  24,  25,  255,
      255, 255, 255, 255, /*  88 */
      255, 26,  27,  28,
      29,  30,  31,  32, /*  96 */
      33,  34,  35,  36,
      37,  38,  39,  40, /*  104 */
      41,  42,  43,  44,
      45,  46,  47,  48, /*  112 */
      49,  50,  51,  255,
      255, 255, 255, 255, /*  120 */
  };
  return tab[ch & 127];
}

ON_FLASH int base64_decode(const unsigned char *s, int len, char *dst) {
  unsigned char a, b, c, d;
  int orig_len = len;
  while (len >= 4 && (a = from_b64(s[0])) != 255 &&
         (b = from_b64(s[1])) != 255 && (c = from_b64(s[2])) != 255 &&
         (d = from_b64(s[3])) != 255) {
    s += 4;
    len -= 4;
    if (a == 200 || b == 200) break; /* '=' can't be there */
    *dst++ = a << 2 | b >> 4;
    if (c == 200) break;
    *dst++ = b << 4 | c >> 2;
    if (d == 200) break;
    *dst++ = c << 6 | d;
  }
  *dst = 0;
  return orig_len - len;
}
#ifdef NS_MODULE_LINES
#line 1 "src/../deps/frozen/frozen.c"
/**/
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

#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2005+ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#define snprintf _snprintf
#endif

#ifndef FROZEN_REALLOC
#define FROZEN_REALLOC realloc
#endif

#ifndef FROZEN_FREE
#define FROZEN_FREE free
#endif

struct frozen {
  const char *end;
  const char *cur;
  struct json_token *tokens;
  int max_tokens;
  int num_tokens;
  int do_realloc;
};

static int parse_object(struct frozen *f);
static int parse_value(struct frozen *f);

#define EXPECT(cond, err_code) do { if (!(cond)) return (err_code); } while (0)
#define TRY(expr) do { int _n = expr; if (_n < 0) return _n; } while (0)
#define END_OF_STRING (-1)

static int left(const struct frozen *f) {
  return f->end - f->cur;
}

static int is_space(int ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

static void skip_whitespaces(struct frozen *f) {
  while (f->cur < f->end && is_space(*f->cur)) f->cur++;
}

static int cur(struct frozen *f) {
  skip_whitespaces(f);
  return f->cur >= f->end ? END_OF_STRING : * (unsigned char *) f->cur;
}

static int test_and_skip(struct frozen *f, int expected) {
  int ch = cur(f);
  if (ch == expected) { f->cur++; return 0; }
  return ch == END_OF_STRING ? JSON_STRING_INCOMPLETE : JSON_STRING_INVALID;
}

static int is_alpha(int ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static int is_digit(int ch) {
  return ch >= '0' && ch <= '9';
}

static int is_hex_digit(int ch) {
  return is_digit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

static int get_escape_len(const char *s, int len) {
  switch (*s) {
    case 'u':
      return len < 6 ? JSON_STRING_INCOMPLETE :
        is_hex_digit(s[1]) && is_hex_digit(s[2]) &&
        is_hex_digit(s[3]) && is_hex_digit(s[4]) ? 5 : JSON_STRING_INVALID;
    case '"': case '\\': case '/': case 'b':
    case 'f': case 'n': case 'r': case 't':
      return len < 2 ? JSON_STRING_INCOMPLETE : 1;
    default:
      return JSON_STRING_INVALID;
  }
}

static int capture_ptr(struct frozen *f, const char *ptr, enum json_type type) {
  if (f->do_realloc && f->num_tokens >= f->max_tokens) {
    int new_size = f->max_tokens == 0 ? 100 : f->max_tokens * 2;
    void *p = FROZEN_REALLOC(f->tokens, new_size * sizeof(f->tokens[0]));
    if (p == NULL) return JSON_TOKEN_ARRAY_TOO_SMALL;
    f->max_tokens = new_size;
    f->tokens = (struct json_token *) p;
  }
  if (f->tokens == NULL || f->max_tokens == 0) return 0;
  if (f->num_tokens >= f->max_tokens) return JSON_TOKEN_ARRAY_TOO_SMALL;
  f->tokens[f->num_tokens].ptr = ptr;
  f->tokens[f->num_tokens].type = type;
  f->num_tokens++;
  return 0;
}

static int capture_len(struct frozen *f, int token_index, const char *ptr) {
  if (f->tokens == 0 || f->max_tokens == 0) return 0;
  EXPECT(token_index >= 0 && token_index < f->max_tokens, JSON_STRING_INVALID);
  f->tokens[token_index].len = ptr - f->tokens[token_index].ptr;
  f->tokens[token_index].num_desc = (f->num_tokens - 1) - token_index;
  return 0;
}

/* identifier = letter { letter | digit | '_' } */
static int parse_identifier(struct frozen *f) {
  EXPECT(is_alpha(cur(f)), JSON_STRING_INVALID);
  TRY(capture_ptr(f, f->cur, JSON_TYPE_STRING));
  while (f->cur < f->end &&
         (*f->cur == '_' || is_alpha(*f->cur) || is_digit(*f->cur))) {
    f->cur++;
  }
  capture_len(f, f->num_tokens - 1, f->cur);
  return 0;
}

static int get_utf8_char_len(unsigned char ch) {
  if ((ch & 0x80) == 0) return 1;
  switch (ch & 0xf0) {
    case 0xf0: return 4;
    case 0xe0: return 3;
    default: return 2;
  }
}

/* string = '"' { quoted_printable_chars } '"' */
static int parse_string(struct frozen *f) {
  int n, ch = 0, len = 0;
  TRY(test_and_skip(f, '"'));
  TRY(capture_ptr(f, f->cur, JSON_TYPE_STRING));
  for (; f->cur < f->end; f->cur += len) {
    ch = * (unsigned char *) f->cur;
    len = get_utf8_char_len((unsigned char) ch);
    EXPECT(ch >= 32 && len > 0, JSON_STRING_INVALID);  /* No control chars */
    EXPECT(len < left(f), JSON_STRING_INCOMPLETE);
    if (ch == '\\') {
      EXPECT((n = get_escape_len(f->cur + 1, left(f))) > 0, n);
      len += n;
    } else if (ch == '"') {
      capture_len(f, f->num_tokens - 1, f->cur);
      f->cur++;
      break;
    };
  }
  return ch == '"' ? 0 : JSON_STRING_INCOMPLETE;
}

/* number = [ '-' ] digit+ [ '.' digit+ ] [ ['e'|'E'] ['+'|'-'] digit+ ] */
static int parse_number(struct frozen *f) {
  int ch = cur(f);
  TRY(capture_ptr(f, f->cur, JSON_TYPE_NUMBER));
  if (ch == '-') f->cur++;
  EXPECT(f->cur < f->end, JSON_STRING_INCOMPLETE);
  EXPECT(is_digit(f->cur[0]), JSON_STRING_INVALID);
  while (f->cur < f->end && is_digit(f->cur[0])) f->cur++;
  if (f->cur < f->end && f->cur[0] == '.') {
    f->cur++;
    EXPECT(f->cur < f->end, JSON_STRING_INCOMPLETE);
    EXPECT(is_digit(f->cur[0]), JSON_STRING_INVALID);
    while (f->cur < f->end && is_digit(f->cur[0])) f->cur++;
  }
  if (f->cur < f->end && (f->cur[0] == 'e' || f->cur[0] == 'E')) {
    f->cur++;
    EXPECT(f->cur < f->end, JSON_STRING_INCOMPLETE);
    if ((f->cur[0] == '+' || f->cur[0] == '-')) f->cur++;
    EXPECT(f->cur < f->end, JSON_STRING_INCOMPLETE);
    EXPECT(is_digit(f->cur[0]), JSON_STRING_INVALID);
    while (f->cur < f->end && is_digit(f->cur[0])) f->cur++;
  }
  capture_len(f, f->num_tokens - 1, f->cur);
  return 0;
}

/* array = '[' [ value { ',' value } ] ']' */
static int parse_array(struct frozen *f) {
  int ind;
  TRY(test_and_skip(f, '['));
  TRY(capture_ptr(f, f->cur - 1, JSON_TYPE_ARRAY));
  ind = f->num_tokens - 1;
  while (cur(f) != ']') {
    TRY(parse_value(f));
    if (cur(f) == ',') f->cur++;
  }
  TRY(test_and_skip(f, ']'));
  capture_len(f, ind, f->cur);
  return 0;
}

static int compare(const char *s, const char *str, int len) {
  int i = 0;
  while (i < len && s[i] == str[i]) i++;
  return i == len ? 1 : 0;
}

static int expect(struct frozen *f, const char *s, int len, enum json_type t) {
  int i, n = left(f);

  TRY(capture_ptr(f, f->cur, t));
  for (i = 0; i < len; i++) {
    if (i >= n) return JSON_STRING_INCOMPLETE;
    if (f->cur[i] != s[i]) return JSON_STRING_INVALID;
  }
  f->cur += len;
  TRY(capture_len(f, f->num_tokens - 1, f->cur));

  return 0;
}

/* value = 'null' | 'true' | 'false' | number | string | array | object */
static int parse_value(struct frozen *f) {
  int ch = cur(f);

  switch (ch) {
    case '"': TRY(parse_string(f)); break;
    case '{': TRY(parse_object(f)); break;
    case '[': TRY(parse_array(f)); break;
    case 'n': TRY(expect(f, "null", 4, JSON_TYPE_NULL)); break;
    case 't': TRY(expect(f, "true", 4, JSON_TYPE_TRUE)); break;
    case 'f': TRY(expect(f, "false", 5, JSON_TYPE_FALSE)); break;
    case '-': case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      TRY(parse_number(f));
      break;
    default:
      return ch == END_OF_STRING ? JSON_STRING_INCOMPLETE : JSON_STRING_INVALID;
  }

  return 0;
}

/* key = identifier | string */
static int parse_key(struct frozen *f) {
  int ch = cur(f);
#if 0
  printf("%s 1 [%.*s]\n", __func__, (int) (f->end - f->cur), f->cur);
#endif
  if (is_alpha(ch)) {
    TRY(parse_identifier(f));
  } else if (ch == '"') {
    TRY(parse_string(f));
  } else {
    return ch == END_OF_STRING ? JSON_STRING_INCOMPLETE : JSON_STRING_INVALID;
  }
  return 0;
}

/* pair = key ':' value */
static int parse_pair(struct frozen *f) {
  TRY(parse_key(f));
  TRY(test_and_skip(f, ':'));
  TRY(parse_value(f));
  return 0;
}

/* object = '{' pair { ',' pair } '}' */
static int parse_object(struct frozen *f) {
  int ind;
  TRY(test_and_skip(f, '{'));
  TRY(capture_ptr(f, f->cur - 1, JSON_TYPE_OBJECT));
  ind = f->num_tokens - 1;
  while (cur(f) != '}') {
    TRY(parse_pair(f));
    if (cur(f) == ',') f->cur++;
  }
  TRY(test_and_skip(f, '}'));
  capture_len(f, ind, f->cur);
  return 0;
}

static int doit(struct frozen *f) {
  if (f->cur == 0 || f->end < f->cur) return JSON_STRING_INVALID;
  if (f->end == f->cur) return JSON_STRING_INCOMPLETE;
  TRY(parse_object(f));
  TRY(capture_ptr(f, f->cur, JSON_TYPE_EOF));
  capture_len(f, f->num_tokens, f->cur);
  return 0;
}

/* json = object */
int parse_json(const char *s, int s_len, struct json_token *arr, int arr_len) {
  struct frozen frozen;

  memset(&frozen, 0, sizeof(frozen));
  frozen.end = s + s_len;
  frozen.cur = s;
  frozen.tokens = arr;
  frozen.max_tokens = arr_len;

  TRY(doit(&frozen));

  return frozen.cur - s;
}

struct json_token *parse_json2(const char *s, int s_len) {
  struct frozen frozen;

  memset(&frozen, 0, sizeof(frozen));
  frozen.end = s + s_len;
  frozen.cur = s;
  frozen.do_realloc = 1;

  if (doit(&frozen) < 0) {
    FROZEN_FREE((void *) frozen.tokens);
    frozen.tokens = NULL;
  }
  return frozen.tokens;
}

static int path_part_len(const char *p) {
  int i = 0;
  while (p[i] != '\0' && p[i] != '[' && p[i] != '.') i++;
  return i;
}

struct json_token *find_json_token(struct json_token *toks, const char *path) {
  while (path != 0 && path[0] != '\0') {
    int i, ind2 = 0, ind = -1, skip = 2, n = path_part_len(path);
    if (path[0] == '[') {
      if (toks->type != JSON_TYPE_ARRAY || !is_digit(path[1])) return 0;
      for (ind = 0, n = 1; path[n] != ']' && path[n] != '\0'; n++) {
        if (!is_digit(path[n])) return 0;
        ind *= 10;
        ind += path[n] - '0';
      }
      if (path[n++] != ']') return 0;
      skip = 1;  /* In objects, we skip 2 elems while iterating, in arrays 1. */
    } else if (toks->type != JSON_TYPE_OBJECT) return 0;
    toks++;
    for (i = 0; i < toks[-1].num_desc; i += skip, ind2++) {
      /* ind == -1 indicated that we're iterating an array, not object */
      if (ind == -1 && toks[i].type != JSON_TYPE_STRING) return 0;
      if (ind2 == ind ||
          (ind == -1 && toks[i].len == n && compare(path, toks[i].ptr, n))) {
        i += skip - 1;
        break;
      };
      if (toks[i - 1 + skip].type == JSON_TYPE_ARRAY ||
          toks[i - 1 + skip].type == JSON_TYPE_OBJECT) {
        i += toks[i - 1 + skip].num_desc;
      }
    }
    if (i == toks[-1].num_desc) return 0;
    path += n;
    if (path[0] == '.') path++;
    if (path[0] == '\0') return &toks[i];
    toks += i;
  }
  return 0;
}

int json_emit_long(char *buf, int buf_len, long int value) {
  char tmp[20];
  int n = snprintf(tmp, sizeof(tmp), "%ld", value);
  strncpy(buf, tmp, buf_len > 0 ? buf_len : 0);
  return n;
}

int json_emit_double(char *buf, int buf_len, double value) {
  char tmp[20];
  int n = snprintf(tmp, sizeof(tmp), "%g", value);
  strncpy(buf, tmp, buf_len > 0 ? buf_len : 0);
  return n;
}

int json_emit_quoted_str(char *s, int s_len, const char *str, int len) {
  const char *begin = s, *end = s + s_len, *str_end = str + len;
  char ch;

#define EMIT(x) do { if (s < end) *s = x; s++; } while (0)

  EMIT('"');
  while (str < str_end) {
    ch = *str++;
    switch (ch) {
      case '"':  EMIT('\\'); EMIT('"'); break;
      case '\\': EMIT('\\'); EMIT('\\'); break;
      case '\b': EMIT('\\'); EMIT('b'); break;
      case '\f': EMIT('\\'); EMIT('f'); break;
      case '\n': EMIT('\\'); EMIT('n'); break;
      case '\r': EMIT('\\'); EMIT('r'); break;
      case '\t': EMIT('\\'); EMIT('t'); break;
      default: EMIT(ch);
    }
  }
  EMIT('"');
  if (s < end) {
    *s = '\0';
  }

  return s - begin;
}

int json_emit_unquoted_str(char *buf, int buf_len, const char *str, int len) {
  if (buf_len > 0 && len > 0) {
    int n = len < buf_len ? len : buf_len;
    memcpy(buf, str, n);
    if (n < buf_len) {
      buf[n] = '\0';
    }
  }
  return len;
}

int json_emit_va(char *s, int s_len, const char *fmt, va_list ap) {
  const char *end = s + s_len, *str, *orig = s;
  size_t len;

  while (*fmt != '\0') {
    switch (*fmt) {
      case '[': case ']': case '{': case '}': case ',': case ':':
      case ' ': case '\r': case '\n': case '\t':
        if (s < end) {
          *s = *fmt;
        }
        s++;
        break;
      case 'i':
        s += json_emit_long(s, end - s, va_arg(ap, long));
        break;
      case 'f':
        s += json_emit_double(s, end - s, va_arg(ap, double));
        break;
      case 'v':
        str = va_arg(ap, char *);
        len = va_arg(ap, size_t);
        s += json_emit_quoted_str(s, end - s, str, len);
        break;
      case 'V':
        str = va_arg(ap, char *);
        len = va_arg(ap, size_t);
        s += json_emit_unquoted_str(s, end - s, str, len);
        break;
      case 's':
        str = va_arg(ap, char *);
        s += json_emit_quoted_str(s, end - s, str, strlen(str));
        break;
      case 'S':
        str = va_arg(ap, char *);
        s += json_emit_unquoted_str(s, end - s, str, strlen(str));
        break;
      case 'T':
        s += json_emit_unquoted_str(s, end - s, "true", 4);
        break;
      case 'F':
        s += json_emit_unquoted_str(s, end - s, "false", 5);
        break;
      case 'N':
        s += json_emit_unquoted_str(s, end - s, "null", 4);
        break;
      default:
        return 0;
    }
    fmt++;
  }

  /* Best-effort to 0-terminate generated string */
  if (s < end) {
    *s = '\0';
  }

  return s - orig;
}

int json_emit(char *buf, int buf_len, const char *fmt, ...) {
  int len;
  va_list ap;

  va_start(ap, fmt);
  len = json_emit_va(buf, buf_len, fmt, ap);
  va_end(ap);

  return len;
}
#ifdef NS_MODULE_LINES
#line 1 "src/net.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 *
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


#define NS_CTL_MSG_MESSAGE_SIZE 8192
#define NS_READ_BUFFER_SIZE 2048
#define NS_UDP_RECEIVE_BUFFER_SIZE 2000
#define NS_VPRINTF_BUFFER_SIZE 500
#define NS_MAX_HOST_LEN 200

#define NS_COPY_COMMON_CONNECTION_OPTIONS(dst, src) \
  memcpy(dst, src, sizeof(*dst));

struct ctl_msg {
  ns_event_handler_t callback;
  char message[NS_CTL_MSG_MESSAGE_SIZE];
};

static void ns_add_conn(struct ns_mgr *mgr, struct ns_connection *c) {
  c->next = mgr->active_connections;
  mgr->active_connections = c;
  c->prev = NULL;
  if (c->next != NULL) c->next->prev = c;
}

static void ns_remove_conn(struct ns_connection *conn) {
  if (conn->prev == NULL) conn->mgr->active_connections = conn->next;
  if (conn->prev) conn->prev->next = conn->next;
  if (conn->next) conn->next->prev = conn->prev;
}

static void ns_call(struct ns_connection *nc, int ev, void *ev_data) {
  ns_event_handler_t ev_handler;

#ifndef NS_DISABLE_FILESYSTEM
  /* LCOV_EXCL_START */
  if (nc->mgr->hexdump_file != NULL && ev != NS_POLL) {
    int len = (ev == NS_RECV || ev == NS_SEND) ? *(int *) ev_data : 0;
    ns_hexdump_connection(nc, nc->mgr->hexdump_file, len, ev);
  }
/* LCOV_EXCL_STOP */
#endif

  /*
   * If protocol handler is specified, call it. Otherwise, call user-specified
   * event handler.
   */
  ev_handler = nc->proto_handler ? nc->proto_handler : nc->handler;
  if (ev_handler != NULL) {
    ev_handler(nc, ev, ev_data);
  }
}

static size_t ns_out(struct ns_connection *nc, const void *buf, size_t len) {
  if (nc->flags & NSF_UDP) {
    int n = sendto(nc->sock, buf, len, 0, &nc->sa.sa, sizeof(nc->sa.sin));
    DBG(("%p %d %d %d %s:%hu", nc, nc->sock, n, errno,
         inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port)));
    return n < 0 ? 0 : n;
  } else {
    return mbuf_append(&nc->send_mbuf, buf, len);
  }
}

static void ns_destroy_conn(struct ns_connection *conn) {
  if (conn->sock != INVALID_SOCKET) {
    closesocket(conn->sock);
    /*
     * avoid users accidentally double close a socket
     * because it can lead to difficult to debug situations.
     * It would happen only if reusing a destroyed ns_connection
     * but it's not always possible to run the code through an
     * address sanitizer.
     */
    conn->sock = INVALID_SOCKET;
  }
  mbuf_free(&conn->recv_mbuf);
  mbuf_free(&conn->send_mbuf);
#ifdef NS_ENABLE_SSL
  if (conn->ssl != NULL) {
    SSL_free(conn->ssl);
  }
  if (conn->ssl_ctx != NULL) {
    SSL_CTX_free(conn->ssl_ctx);
  }
#endif
  NS_FREE(conn);
}

static void ns_close_conn(struct ns_connection *conn) {
  DBG(("%p %lu", conn, conn->flags));
  ns_call(conn, NS_CLOSE, NULL);
  ns_remove_conn(conn);
  ns_destroy_conn(conn);
}

void ns_mgr_init(struct ns_mgr *s, void *user_data) {
  memset(s, 0, sizeof(*s));
  s->ctl[0] = s->ctl[1] = INVALID_SOCKET;
  s->user_data = user_data;

#ifdef _WIN32
  {
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
  }
#elif !defined(AVR_LIBC)
  /* Ignore SIGPIPE signal, so if client cancels the request, it
   * won't kill the whole process. */
  signal(SIGPIPE, SIG_IGN);
#endif

#ifndef NS_DISABLE_SOCKETPAIR
  do {
    ns_socketpair(s->ctl, SOCK_DGRAM);
  } while (s->ctl[0] == INVALID_SOCKET);
#endif

#ifdef NS_ENABLE_SSL
  {
    static int init_done;
    if (!init_done) {
      SSL_library_init();
      init_done++;
    }
  }
#endif
}

void ns_mgr_free(struct ns_mgr *s) {
  struct ns_connection *conn, *tmp_conn;

  DBG(("%p", s));
  if (s == NULL) return;
  /* Do one last poll, see https://github.com/cesanta/mongoose/issues/286 */
  ns_mgr_poll(s, 0);

  if (s->ctl[0] != INVALID_SOCKET) closesocket(s->ctl[0]);
  if (s->ctl[1] != INVALID_SOCKET) closesocket(s->ctl[1]);
  s->ctl[0] = s->ctl[1] = INVALID_SOCKET;

  for (conn = s->active_connections; conn != NULL; conn = tmp_conn) {
    tmp_conn = conn->next;
    ns_close_conn(conn);
  }
}

int ns_vprintf(struct ns_connection *nc, const char *fmt, va_list ap) {
  char mem[NS_VPRINTF_BUFFER_SIZE], *buf = mem;
  int len;

  if ((len = ns_avprintf(&buf, sizeof(mem), fmt, ap)) > 0) {
    ns_out(nc, buf, len);
  }
  if (buf != mem && buf != NULL) {
    NS_FREE(buf); /* LCOV_EXCL_LINE */
  }               /* LCOV_EXCL_LINE */

  return len;
}

int ns_printf(struct ns_connection *conn, const char *fmt, ...) {
  int len;
  va_list ap;
  va_start(ap, fmt);
  len = ns_vprintf(conn, fmt, ap);
  va_end(ap);
  return len;
}

static void ns_set_non_blocking_mode(sock_t sock) {
#ifdef _WIN32
  unsigned long on = 1;
  ioctlsocket(sock, FIONBIO, &on);
#else
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

#ifndef NS_DISABLE_SOCKETPAIR
int ns_socketpair(sock_t sp[2], int sock_type) {
  union socket_address sa;
  sock_t sock;
  socklen_t len = sizeof(sa.sin);
  int ret = 0;

  sock = sp[0] = sp[1] = INVALID_SOCKET;

  (void) memset(&sa, 0, sizeof(sa));
  sa.sin.sin_family = AF_INET;
  sa.sin.sin_port = htons(0);
  sa.sin.sin_addr.s_addr = htonl(0x7f000001);

  if ((sock = socket(AF_INET, sock_type, 0)) == INVALID_SOCKET) {
  } else if (bind(sock, &sa.sa, len) != 0) {
  } else if (sock_type == SOCK_STREAM && listen(sock, 1) != 0) {
  } else if (getsockname(sock, &sa.sa, &len) != 0) {
  } else if ((sp[0] = socket(AF_INET, sock_type, 0)) == INVALID_SOCKET) {
  } else if (connect(sp[0], &sa.sa, len) != 0) {
  } else if (sock_type == SOCK_DGRAM &&
             (getsockname(sp[0], &sa.sa, &len) != 0 ||
              connect(sock, &sa.sa, len) != 0)) {
  } else if ((sp[1] = (sock_type == SOCK_DGRAM ? sock
                                               : accept(sock, &sa.sa, &len))) ==
             INVALID_SOCKET) {
  } else {
    ns_set_close_on_exec(sp[0]);
    ns_set_close_on_exec(sp[1]);
    if (sock_type == SOCK_STREAM) closesocket(sock);
    ret = 1;
  }

  if (!ret) {
    if (sp[0] != INVALID_SOCKET) closesocket(sp[0]);
    if (sp[1] != INVALID_SOCKET) closesocket(sp[1]);
    if (sock != INVALID_SOCKET) closesocket(sock);
    sock = sp[0] = sp[1] = INVALID_SOCKET;
  }

  return ret;
}
#endif /* NS_DISABLE_SOCKETPAIR */

/* TODO(lsm): use non-blocking resolver */
static int ns_resolve2(const char *host, struct in_addr *ina) {
#ifdef NS_ENABLE_GETADDRINFO
  int rv = 0;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *h = NULL;
  char *ip = NS_MALLOC(17);
  memset(ip, '\0', 17);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((rv = getaddrinfo(host, NULL, NULL, &servinfo)) != 0) {
    DBG(("getaddrinfo(%s) failed: %s", host, strerror(errno)));
    return 0;
  }
  for (p = servinfo; p != NULL; p = p->ai_next) {
    memcpy(&h, &p->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(ina, &h->sin_addr, sizeof(ina));
  }
  freeaddrinfo(servinfo);
  return 1;
#else
  struct hostent *he;
  if ((he = gethostbyname(host)) == NULL) {
    DBG(("gethostbyname(%s) failed: %s", host, strerror(errno)));
  } else {
    memcpy(ina, he->h_addr_list[0], sizeof(*ina));
    return 1;
  }
  return 0;
#endif
}

int ns_resolve(const char *host, char *buf, size_t n) {
  struct in_addr ad;
  return ns_resolve2(host, &ad) ? snprintf(buf, n, "%s", inet_ntoa(ad)) : 0;
}

NS_INTERNAL struct ns_connection *ns_create_connection(
    struct ns_mgr *mgr, ns_event_handler_t callback,
    struct ns_add_sock_opts opts) {
  struct ns_connection *conn;

  if ((conn = (struct ns_connection *) NS_MALLOC(sizeof(*conn))) != NULL) {
    memset(conn, 0, sizeof(*conn));
    conn->sock = INVALID_SOCKET;
    conn->handler = callback;
    conn->mgr = mgr;
    conn->last_io_time = time(NULL);
    conn->flags = opts.flags;
    conn->user_data = opts.user_data;
    /*
     * SIZE_MAX is defined as a long long constant in
     * system headers on some platforms and so it
     * doesn't compile with pedantic ansi flags.
     */
    conn->recv_mbuf_limit = ~0;
  }

  return conn;
}

/* Associate a socket to a connection and and add to the manager. */
NS_INTERNAL void ns_set_sock(struct ns_connection *nc, sock_t sock) {
  ns_set_non_blocking_mode(sock);
  ns_set_close_on_exec(sock);
  nc->sock = sock;
  ns_add_conn(nc->mgr, nc);
  DBG(("%p %d", nc, sock));
}

/*
 * Address format: [PROTO://][HOST]:PORT
 *
 * HOST could be IPv4/IPv6 address or a host name.
 * `host` is a destination buffer to hold parsed HOST part. Shoud be at least
 * NS_MAX_HOST_LEN bytes long.
 * `proto` is a returned socket type, either SOCK_STREAM or SOCK_DGRAM
 *
 * Return:
 *   -1   on parse error
 *    0   if HOST needs DNS lookup
 *   >0   length of the address string
 */
NS_INTERNAL int ns_parse_address(const char *str, union socket_address *sa,
                                 int *proto, char *host, size_t host_len) {
  unsigned int a, b, c, d, port = 0;
  int len = 0;
#ifdef NS_ENABLE_IPV6
  char buf[100];
#endif

  /*
   * MacOS needs that. If we do not zero it, subsequent bind() will fail.
   * Also, all-zeroes in the socket address means binding to all addresses
   * for both IPv4 and IPv6 (INADDR_ANY and IN6ADDR_ANY_INIT).
   */
  memset(sa, 0, sizeof(*sa));
  sa->sin.sin_family = AF_INET;

  *proto = SOCK_STREAM;

  if (strncmp(str, "udp://", 6) == 0) {
    str += 6;
    *proto = SOCK_DGRAM;
  } else if (strncmp(str, "tcp://", 6) == 0) {
    str += 6;
  }

  if (sscanf(str, "%u.%u.%u.%u:%u%n", &a, &b, &c, &d, &port, &len) == 5) {
    /* Bind to a specific IPv4 address, e.g. 192.168.1.5:8080 */
    sa->sin.sin_addr.s_addr =
        htonl(((uint32_t) a << 24) | ((uint32_t) b << 16) | c << 8 | d);
    sa->sin.sin_port = htons((uint16_t) port);
#ifdef NS_ENABLE_IPV6
  } else if (sscanf(str, "[%99[^]]]:%u%n", buf, &port, &len) == 2 &&
             inet_pton(AF_INET6, buf, &sa->sin6.sin6_addr)) {
    /* IPv6 address, e.g. [3ffe:2a00:100:7031::1]:8080 */
    sa->sin6.sin6_family = AF_INET6;
    sa->sin.sin_port = htons((uint16_t) port);
#endif
#ifndef NS_DISABLE_RESOLVER
  } else if (strlen(str) < host_len &&
             sscanf(str, "%[^ :]:%u%n", host, &port, &len) == 2) {
    sa->sin.sin_port = htons((uint16_t) port);
    if (ns_resolve_from_hosts_file(host, sa) != 0) {
      return 0;
    }
#endif
  } else if (sscanf(str, ":%u%n", &port, &len) == 1 ||
             sscanf(str, "%u%n", &port, &len) == 1) {
    /* If only port is specified, bind to IPv4, INADDR_ANY */
    sa->sin.sin_port = htons((uint16_t) port);
  } else {
    return -1;
  }

  return port < 0xffffUL && str[len] == '\0' ? len : -1;
}

/* 'sa' must be an initialized address to bind to */
static sock_t ns_open_listening_socket(union socket_address *sa, int proto) {
  socklen_t sa_len =
      (sa->sa.sa_family == AF_INET) ? sizeof(sa->sin) : sizeof(sa->sin6);
  sock_t sock = INVALID_SOCKET;
  int on = 1;

  if ((sock = socket(sa->sa.sa_family, proto, 0)) != INVALID_SOCKET &&
#if defined(_WIN32) && defined(SO_EXCLUSIVEADDRUSE)
      /* "Using SO_REUSEADDR and SO_EXCLUSIVEADDRUSE" http://goo.gl/RmrFTm */
      !setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (void *) &on,
                  sizeof(on)) &&
#endif

#if 1 || !defined(_WIN32) || defined(SO_EXCLUSIVEADDRUSE)
      /*
       * SO_RESUSEADDR is not enabled on Windows because the semantics of
       * SO_REUSEADDR on UNIX and Windows is different. On Windows,
       * SO_REUSEADDR allows to bind a socket to a port without error even if
       * the port is already open by another program. This is not the behavior
       * SO_REUSEADDR was designed for, and leads to hard-to-track failure
       * scenarios. Therefore, SO_REUSEADDR was disabled on Windows unless
       * SO_EXCLUSIVEADDRUSE is supported and set on a socket.
       */
      !setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on)) &&
#endif

      !bind(sock, &sa->sa, sa_len) &&
      (proto == SOCK_DGRAM || listen(sock, SOMAXCONN) == 0)) {
    ns_set_non_blocking_mode(sock);
    /* In case port was set to 0, get the real port number */
    (void) getsockname(sock, &sa->sa, &sa_len);
  } else if (sock != INVALID_SOCKET) {
    closesocket(sock);
    sock = INVALID_SOCKET;
  }

  return sock;
}

#ifdef NS_ENABLE_SSL
/* Certificate generation script is at */
/* https://github.com/cesanta/fossa/blob/master/scripts/gen_certs.sh */

static int ns_use_ca_cert(SSL_CTX *ctx, const char *cert) {
  if (ctx == NULL) {
    return -1;
  } else if (cert == NULL || cert[0] == '\0') {
    return 0;
  }
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
  return SSL_CTX_load_verify_locations(ctx, cert, NULL) == 1 ? 0 : -2;
}

static int ns_use_cert(SSL_CTX *ctx, const char *pem_file) {
  if (ctx == NULL) {
    return -1;
  } else if (pem_file == NULL || pem_file[0] == '\0') {
    return 0;
  } else if (SSL_CTX_use_certificate_file(ctx, pem_file, 1) == 0 ||
             SSL_CTX_use_PrivateKey_file(ctx, pem_file, 1) == 0) {
    return -2;
  } else {
    SSL_CTX_set_mode(ctx, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
    SSL_CTX_use_certificate_chain_file(ctx, pem_file);
    return 0;
  }
}

/*
 * Turn the connection into SSL mode.
 * `cert` is the certificate file in PEM format. For listening connections,
 * certificate file must contain private key and server certificate,
 * concatenated. `ca_cert` is a certificate authority (CA) PEM file, and
 * it is optional (can be set to NULL). If `ca_cert` is non-NULL, then
 * the connection is so-called two-way-SSL: other peer's certificate is
 * checked against the `ca_cert`.
 *
 * Handy OpenSSL command to generate test self-signed certificate:
 *
 *    openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 999
 *
 * Return NULL on success, or error message on failure.
 */
const char *ns_set_ssl(struct ns_connection *nc, const char *cert,
                       const char *ca_cert) {
  const char *result = NULL;

  if ((nc->flags & NSF_LISTENING) &&
      (nc->ssl_ctx = SSL_CTX_new(SSLv23_server_method())) == NULL) {
    result = "SSL_CTX_new() failed";
  } else if (!(nc->flags & NSF_LISTENING) &&
             (nc->ssl_ctx = SSL_CTX_new(SSLv23_client_method())) == NULL) {
    result = "SSL_CTX_new() failed";
  } else if (ns_use_cert(nc->ssl_ctx, cert) != 0) {
    result = "Invalid ssl cert";
  } else if (ns_use_ca_cert(nc->ssl_ctx, ca_cert) != 0) {
    result = "Invalid CA cert";
  } else if (!(nc->flags & NSF_LISTENING) &&
             (nc->ssl = SSL_new(nc->ssl_ctx)) == NULL) {
    result = "SSL_new() failed";
  } else if (!(nc->flags & NSF_LISTENING)) {
    SSL_set_fd(nc->ssl, nc->sock);
  }
  return result;
}

static int ns_ssl_err(struct ns_connection *conn, int res) {
  int ssl_err = SSL_get_error(conn->ssl, res);
  if (ssl_err == SSL_ERROR_WANT_READ) conn->flags |= NSF_WANT_READ;
  if (ssl_err == SSL_ERROR_WANT_WRITE) conn->flags |= NSF_WANT_WRITE;
  return ssl_err;
}
#endif /* NS_ENABLE_SSL */

static struct ns_connection *accept_conn(struct ns_connection *ls) {
  struct ns_connection *c = NULL;
  union socket_address sa;
  socklen_t len = sizeof(sa);
  sock_t sock = INVALID_SOCKET;

  /* NOTE(lsm): on Windows, sock is always > FD_SETSIZE */
  if ((sock = accept(ls->sock, &sa.sa, &len)) == INVALID_SOCKET) {
  } else if ((c = ns_add_sock(ls->mgr, sock, ls->handler)) == NULL) {
    closesocket(sock);
#ifdef NS_ENABLE_SSL
  } else if (ls->ssl_ctx != NULL && ((c->ssl = SSL_new(ls->ssl_ctx)) == NULL ||
                                     SSL_set_fd(c->ssl, sock) != 1)) {
    DBG(("SSL error"));
    ns_close_conn(c);
    c = NULL;
#endif
  } else {
    c->listener = ls;
    c->proto_data = ls->proto_data;
    c->proto_handler = ls->proto_handler;
    c->user_data = ls->user_data;
    c->recv_mbuf_limit = ls->recv_mbuf_limit;
    ns_call(c, NS_ACCEPT, &sa);
    DBG(("%p %d %p %p", c, c->sock, c->ssl_ctx, c->ssl));
  }

  return c;
}

static int ns_is_error(int n) {
  return n == 0 || (n < 0 && errno != EINTR && errno != EINPROGRESS &&
                    errno != EAGAIN && errno != EWOULDBLOCK
#ifdef _WIN32
                    && WSAGetLastError() != WSAEINTR &&
                    WSAGetLastError() != WSAEWOULDBLOCK
#endif
                    );
}

static size_t recv_avail_size(struct ns_connection *conn, size_t max) {
  size_t avail;
  if (conn->recv_mbuf_limit < conn->recv_mbuf.len) return 0;
  avail = conn->recv_mbuf_limit - conn->recv_mbuf.len;
  return avail > max ? max : avail;
}

static void ns_read_from_socket(struct ns_connection *conn) {
  char buf[NS_READ_BUFFER_SIZE];
  int n = 0;

  if (conn->flags & NSF_CONNECTING) {
    int ok = 1, ret;
    socklen_t len = sizeof(ok);

    ret = getsockopt(conn->sock, SOL_SOCKET, SO_ERROR, (char *) &ok, &len);
#ifdef NS_ENABLE_SSL
    if (ret == 0 && ok == 0 && conn->ssl != NULL) {
      int res = SSL_connect(conn->ssl);
      int ssl_err = ns_ssl_err(conn, res);
      if (res == 1) {
        conn->flags |= NSF_SSL_HANDSHAKE_DONE;
        conn->flags &= ~(NSF_WANT_READ | NSF_WANT_WRITE);
      } else if (ssl_err == SSL_ERROR_WANT_READ ||
                 ssl_err == SSL_ERROR_WANT_WRITE) {
        return; /* Call us again */
      } else {
        ok = 1;
      }
    }
#endif
    (void) ret;
    conn->flags &= ~NSF_CONNECTING;
    DBG(("%p ok=%d", conn, ok));
    if (ok != 0) {
      conn->flags |= NSF_CLOSE_IMMEDIATELY;
    }
    ns_call(conn, NS_CONNECT, &ok);
    return;
  }

#ifdef NS_ENABLE_SSL
  if (conn->ssl != NULL) {
    if (conn->flags & NSF_SSL_HANDSHAKE_DONE) {
      /* SSL library may have more bytes ready to read then we ask to read.
       * Therefore, read in a loop until we read everything. Without the loop,
       * we skip to the next select() cycle which can just timeout. */
      while ((n = SSL_read(conn->ssl, buf, sizeof(buf))) > 0) {
        DBG(("%p %lu <- %d bytes (SSL)", conn, conn->flags, n));
        mbuf_append(&conn->recv_mbuf, buf, n);
        ns_call(conn, NS_RECV, &n);
      }
      ns_ssl_err(conn, n);
    } else {
      int res = SSL_accept(conn->ssl);
      int ssl_err = ns_ssl_err(conn, res);
      if (res == 1) {
        conn->flags |= NSF_SSL_HANDSHAKE_DONE;
        conn->flags &= ~(NSF_WANT_READ | NSF_WANT_WRITE);
      } else if (ssl_err == SSL_ERROR_WANT_READ ||
                 ssl_err == SSL_ERROR_WANT_WRITE) {
        return; /* Call us again */
      } else {
        conn->flags |= NSF_CLOSE_IMMEDIATELY;
      }
      return;
    }
  } else
#endif
  {
    while ((n = (int) NS_RECV_FUNC(
                conn->sock, buf, recv_avail_size(conn, sizeof(buf)), 0)) > 0) {
      DBG(("%p %lu <- %d bytes (PLAIN)", conn, conn->flags, n));
      mbuf_append(&conn->recv_mbuf, buf, n);
      ns_call(conn, NS_RECV, &n);
    }
  }

  if (ns_is_error(n)) {
    conn->flags |= NSF_CLOSE_IMMEDIATELY;
  }
}

static void ns_write_to_socket(struct ns_connection *conn) {
  struct mbuf *io = &conn->send_mbuf;
  int n = 0;

#ifdef NS_ENABLE_SSL
  if (conn->ssl != NULL) {
    n = SSL_write(conn->ssl, io->buf, io->len);
    if (n <= 0) {
      int ssl_err = ns_ssl_err(conn, n);
      if (ssl_err == SSL_ERROR_WANT_READ || ssl_err == SSL_ERROR_WANT_WRITE) {
        return; /* Call us again */
      } else {
        conn->flags |= NSF_CLOSE_IMMEDIATELY;
      }
    } else {
      /* Successful SSL operation, clear off SSL wait flags */
      conn->flags &= ~(NSF_WANT_READ | NSF_WANT_WRITE);
    }
  } else
#endif
  {
    n = (int) NS_SEND_FUNC(conn->sock, io->buf, io->len, 0);
  }

  DBG(("%p %lu -> %d bytes", conn, conn->flags, n));

  ns_call(conn, NS_SEND, &n);
  if (ns_is_error(n)) {
    conn->flags |= NSF_CLOSE_IMMEDIATELY;
  } else if (n > 0) {
    mbuf_remove(io, n);
  }
}

int ns_send(struct ns_connection *conn, const void *buf, int len) {
  return (int) ns_out(conn, buf, len);
}

static void ns_handle_udp(struct ns_connection *ls) {
  struct ns_connection nc;
  char buf[NS_UDP_RECEIVE_BUFFER_SIZE];
  int n;
  socklen_t s_len = sizeof(nc.sa);

  memset(&nc, 0, sizeof(nc));
  n = recvfrom(ls->sock, buf, sizeof(buf), 0, &nc.sa.sa, &s_len);
  if (n <= 0) {
    DBG(("%p recvfrom: %s", ls, strerror(errno)));
  } else {
    union socket_address sa = nc.sa;
    /* Copy all attributes, preserving sender address */
    nc = *ls;

    /* Then override some */
    nc.sa = sa;
    nc.recv_mbuf.buf = buf;
    nc.recv_mbuf.len = nc.recv_mbuf.size = n;
    nc.listener = ls;
    nc.flags = NSF_UDP;

    /* Call NS_RECV handler */
    DBG(("%p %d bytes received", ls, n));
    ns_call(&nc, NS_RECV, &n);

    /*
     * See https://github.com/cesanta/fossa/issues/207
     * ns_call migth set flags. They need to be synced back to ls.
     */
    ls->flags = nc.flags;
  }
}

static void ns_add_to_set(sock_t sock, fd_set *set, sock_t *max_fd) {
  if (sock != INVALID_SOCKET) {
    FD_SET(sock, set);
    if (*max_fd == INVALID_SOCKET || sock > *max_fd) {
      *max_fd = sock;
    }
  }
}

time_t ns_mgr_poll(struct ns_mgr *mgr, int milli) {
  struct ns_connection *nc, *tmp;
  struct timeval tv;
  fd_set read_set, write_set, err_set;
  sock_t max_fd = INVALID_SOCKET;
  time_t current_time = time(NULL);

  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  FD_ZERO(&err_set);
  ns_add_to_set(mgr->ctl[1], &read_set, &max_fd);

  for (nc = mgr->active_connections; nc != NULL; nc = tmp) {
    tmp = nc->next;
    if (!(nc->flags & (NSF_LISTENING | NSF_CONNECTING))) {
      ns_call(nc, NS_POLL, &current_time);
    }

    /*
     * NS_POLL handler could have signaled us to close the connection
     * by setting NSF_CLOSE_IMMEDIATELY flag. In this case, we don't want to
     * trigger any other events on that connection, but close it right away.
     */
    if (nc->flags & NSF_CLOSE_IMMEDIATELY) {
      /* NOTE(lsm): this call removes nc from the mgr->active_connections */
      ns_close_conn(nc);
      continue;
    }

    if (!(nc->flags & NSF_WANT_WRITE) &&
        nc->recv_mbuf.len < nc->recv_mbuf_limit) {
      ns_add_to_set(nc->sock, &read_set, &max_fd);
    }

    if (((nc->flags & NSF_CONNECTING) && !(nc->flags & NSF_WANT_READ)) ||
        (nc->send_mbuf.len > 0 && !(nc->flags & NSF_CONNECTING) &&
         !(nc->flags & NSF_DONT_SEND))) {
      ns_add_to_set(nc->sock, &write_set, &max_fd);
      ns_add_to_set(nc->sock, &err_set, &max_fd);
    }
  }

  tv.tv_sec = milli / 1000;
  tv.tv_usec = (milli % 1000) * 1000;

  if (select((int) max_fd + 1, &read_set, &write_set, &err_set, &tv) > 0) {
    /* select() might have been waiting for a long time, reset current_time
     *  now to prevent last_io_time being set to the past. */
    current_time = time(NULL);

#ifndef __AVR__
    /*
     * Note: it seems, avr-gcc breaks on long functions
     * As workaround unused (on AVR) part just excluded
     * TODO(alashkin): investigate this
     */

    /* Read wakeup messages */
    if (mgr->ctl[1] != INVALID_SOCKET && FD_ISSET(mgr->ctl[1], &read_set)) {
      struct ctl_msg ctl_msg;
      int len = (int) NS_RECV_FUNC(mgr->ctl[1], (char *) &ctl_msg,
                                   sizeof(ctl_msg), 0);
      NS_SEND_FUNC(mgr->ctl[1], ctl_msg.message, 1, 0);
      if (len >= (int) sizeof(ctl_msg.callback) && ctl_msg.callback != NULL) {
        struct ns_connection *c;
        for (c = ns_next(mgr, NULL); c != NULL; c = ns_next(mgr, c)) {
          ctl_msg.callback(c, NS_POLL, ctl_msg.message);
        }
      }
    }
#endif

    for (nc = mgr->active_connections; nc != NULL; nc = tmp) {
      tmp = nc->next;

      /* Windows reports failed connect() requests in err_set */
      if (FD_ISSET(nc->sock, &err_set) && (nc->flags & NSF_CONNECTING)) {
        nc->last_io_time = current_time;
        ns_read_from_socket(nc);
      }

      if (FD_ISSET(nc->sock, &read_set)) {
        nc->last_io_time = current_time;
        if (nc->flags & NSF_UDP) {
          ns_handle_udp(nc);
        } else if (nc->flags & NSF_LISTENING) {
          /*
           * We're not looping here, and accepting just one connection at
           * a time. The reason is that eCos does not respect non-blocking
           * flag on a listening socket and hangs in a loop.
           */
          accept_conn(nc);
        } else {
          ns_read_from_socket(nc);
        }
      }

      if (FD_ISSET(nc->sock, &write_set)) {
        nc->last_io_time = current_time;
        if (nc->flags & NSF_CONNECTING) {
          ns_read_from_socket(nc);
        } else if (!(nc->flags & NSF_DONT_SEND) &&
                   !(nc->flags & NSF_CLOSE_IMMEDIATELY)) {
          ns_write_to_socket(nc);
        }
      }
    }
  }

  for (nc = mgr->active_connections; nc != NULL; nc = tmp) {
    tmp = nc->next;
    if ((nc->flags & NSF_CLOSE_IMMEDIATELY) ||
        (nc->send_mbuf.len == 0 && (nc->flags & NSF_SEND_AND_CLOSE))) {
      ns_close_conn(nc);
    }
  }

  return current_time;
}

/*
 * Schedules an async connect for a resolved address and proto.
 * Called from two places: `ns_connect_opt()` and from async resolver.
 * When called from the async resolver, it must trigger `NS_CONNECT` event
 * with a failure flag to indicate connection failure.
 */
NS_INTERNAL struct ns_connection *ns_finish_connect(struct ns_connection *nc,
                                                    int proto,
                                                    union socket_address *sa,
                                                    struct ns_add_sock_opts o) {
  sock_t sock = INVALID_SOCKET;
  int rc;

  DBG(("%p %s://%s:%hu", nc, proto == SOCK_DGRAM ? "udp" : "tcp",
       inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port)));

  if ((sock = socket(AF_INET, proto, 0)) == INVALID_SOCKET) {
    int failure = errno;
    NS_SET_PTRPTR(o.error_string, "cannot create socket");
    ns_call(nc, NS_CONNECT, &failure);
    ns_call(nc, NS_CLOSE, NULL);
    ns_destroy_conn(nc);
    return NULL;
  }

  ns_set_non_blocking_mode(sock);
  rc = (proto == SOCK_DGRAM) ? 0 : connect(sock, &sa->sa, sizeof(sa->sin));

  if (rc != 0 && ns_is_error(rc)) {
    NS_SET_PTRPTR(o.error_string, "cannot connect to socket");
    ns_call(nc, NS_CONNECT, &rc);
    ns_call(nc, NS_CLOSE, NULL);
    ns_destroy_conn(nc);
    close(sock);
    return NULL;
  }

  /* No ns_destroy_conn() call after this! */
  ns_set_sock(nc, sock);

  if (rc == 0) {
    /* connect() succeeded. Trigger successful NS_CONNECT event */
    ns_call(nc, NS_CONNECT, &rc);
  } else {
    nc->flags |= NSF_CONNECTING;
  }

  return nc;
}

#ifndef NS_DISABLE_RESOLVER
/*
 * Callback for the async resolver on ns_connect_opt() call.
 * Main task of this function is to trigger NS_CONNECT event with
 *    either failure (and dealloc the connection)
 *    or success (and proceed with connect()
 */
static void resolve_cb(struct ns_dns_message *msg, void *data) {
  struct ns_connection *nc = (struct ns_connection *) data;
  int i;
  int failure = -1;

  if (msg != NULL) {
    /*
     * Take the first DNS A answer and run...
     */
    for (i = 0; i < msg->num_answers; i++) {
      if (msg->answers[i].rtype == NS_DNS_A_RECORD) {
        static struct ns_add_sock_opts opts;
        /*
         * Async resolver guarantees that there is at least one answer.
         * TODO(lsm): handle IPv6 answers too
         */
        ns_dns_parse_record_data(msg, &msg->answers[i], &nc->sa.sin.sin_addr,
                                 4);
        /* ns_finish_connect() triggers NS_CONNECT on failure */
        ns_finish_connect(nc, nc->flags & NSF_UDP ? SOCK_DGRAM : SOCK_STREAM,
                          &nc->sa, opts);
        return;
      }
    }
  }

  /*
   * If we get there was no NS_DNS_A_RECORD in the answer
   */
  ns_call(nc, NS_CONNECT, &failure);
  ns_call(nc, NS_CLOSE, NULL);
  ns_destroy_conn(nc);
}
#endif

struct ns_connection *ns_connect(struct ns_mgr *mgr, const char *address,
                                 ns_event_handler_t callback) {
  static struct ns_connect_opts opts;
  return ns_connect_opt(mgr, address, callback, opts);
}

struct ns_connection *ns_connect_opt(struct ns_mgr *mgr, const char *address,
                                     ns_event_handler_t callback,
                                     struct ns_connect_opts opts) {
  struct ns_connection *nc = NULL;
  int proto, rc;
  struct ns_add_sock_opts add_sock_opts;
  char host[NS_MAX_HOST_LEN];

  NS_COPY_COMMON_CONNECTION_OPTIONS(&add_sock_opts, &opts);

  if ((nc = ns_create_connection(mgr, callback, add_sock_opts)) == NULL) {
    return NULL;
  } else if ((rc = ns_parse_address(address, &nc->sa, &proto, host,
                                    sizeof(host))) < 0) {
    /* Address is malformed */
    NS_SET_PTRPTR(opts.error_string, "cannot parse address");
    ns_destroy_conn(nc);
    return NULL;
  }

  nc->flags |= opts.flags;
  nc->flags |= (proto == SOCK_DGRAM) ? NSF_UDP : 0;
  nc->user_data = opts.user_data;

  if (rc == 0) {
#ifndef NS_DISABLE_RESOLVER
    /*
     * DNS resolution is required for host.
     * ns_parse_address() fills port in nc->sa, which we pass to resolve_cb()
     */

    if (ns_resolve_async(nc->mgr, host, NS_DNS_A_RECORD, resolve_cb, nc) != 0) {
      NS_SET_PTRPTR(opts.error_string, "cannot schedule DNS lookup");
      ns_destroy_conn(nc);
      return NULL;
    }

    return nc;
#else
    NS_SET_PTRPTR(opts.error_string, "Resolver is disabled");
    ns_destroy_conn(nc);
    return NULL;
#endif
  } else {
    /* Address is parsed and resolved to IP. proceed with connect() */
    return ns_finish_connect(nc, proto, &nc->sa, add_sock_opts);
  }
}

struct ns_connection *ns_bind(struct ns_mgr *srv, const char *address,
                              ns_event_handler_t event_handler) {
  static struct ns_bind_opts opts;
  return ns_bind_opt(srv, address, event_handler, opts);
}

struct ns_connection *ns_bind_opt(struct ns_mgr *mgr, const char *address,
                                  ns_event_handler_t callback,
                                  struct ns_bind_opts opts) {
  union socket_address sa;
  struct ns_connection *nc = NULL;
  int proto;
  sock_t sock;
  struct ns_add_sock_opts add_sock_opts;
  char host[NS_MAX_HOST_LEN];

  NS_COPY_COMMON_CONNECTION_OPTIONS(&add_sock_opts, &opts);

  if (ns_parse_address(address, &sa, &proto, host, sizeof(host)) <= 0) {
    NS_SET_PTRPTR(opts.error_string, "cannot parse address");
  } else if ((sock = ns_open_listening_socket(&sa, proto)) == INVALID_SOCKET) {
    DBG(("Failed to open listener: %d", errno));
    NS_SET_PTRPTR(opts.error_string, "failed to open listener");
  } else if ((nc = ns_add_sock_opt(mgr, sock, callback, add_sock_opts)) ==
             NULL) {
    /* opts.error_string set by ns_add_sock_opt */
    DBG(("Failed to ns_add_sock"));
    closesocket(sock);
  } else {
    nc->sa = sa;
    nc->flags |= NSF_LISTENING;
    nc->handler = callback;

    if (proto == SOCK_DGRAM) {
      nc->flags |= NSF_UDP;
    }

    DBG(("%p sock %d/%d", nc, sock, proto));
  }

  return nc;
}

struct ns_connection *ns_add_sock(struct ns_mgr *s, sock_t sock,
                                  ns_event_handler_t callback) {
  static struct ns_add_sock_opts opts;
  return ns_add_sock_opt(s, sock, callback, opts);
}

struct ns_connection *ns_add_sock_opt(struct ns_mgr *s, sock_t sock,
                                      ns_event_handler_t callback,
                                      struct ns_add_sock_opts opts) {
  struct ns_connection *nc = ns_create_connection(s, callback, opts);
  if (nc != NULL) {
    ns_set_sock(nc, sock);
  }
  return nc;
}

struct ns_connection *ns_next(struct ns_mgr *s, struct ns_connection *conn) {
  return conn == NULL ? s->active_connections : conn->next;
}

void ns_broadcast(struct ns_mgr *mgr, ns_event_handler_t cb, void *data,
                  size_t len) {
  struct ctl_msg ctl_msg;

  /*
   * Fossa manager has a socketpair, `struct ns_mgr::ctl`,
   * where `ns_broadcast()` pushes the message.
   * `ns_mgr_poll()` wakes up, reads a message from the socket pair, and calls
   * specified callback for each connection. Thus the callback function executes
   * in event manager thread.
   */
  if (mgr->ctl[0] != INVALID_SOCKET && data != NULL &&
      len < sizeof(ctl_msg.message)) {
    ctl_msg.callback = cb;
    memcpy(ctl_msg.message, data, len);
    NS_SEND_FUNC(mgr->ctl[0], (char *) &ctl_msg,
                 offsetof(struct ctl_msg, message) + len, 0);
    NS_RECV_FUNC(mgr->ctl[0], (char *) &len, 1, 0);
  }
}

static int isbyte(int n) {
  return n >= 0 && n <= 255;
}

static int parse_net(const char *spec, uint32_t *net, uint32_t *mask) {
  int n, a, b, c, d, slash = 32, len = 0;

  if ((sscanf(spec, "%d.%d.%d.%d/%d%n", &a, &b, &c, &d, &slash, &n) == 5 ||
       sscanf(spec, "%d.%d.%d.%d%n", &a, &b, &c, &d, &n) == 4) &&
      isbyte(a) && isbyte(b) && isbyte(c) && isbyte(d) && slash >= 0 &&
      slash < 33) {
    len = n;
    *net =
        ((uint32_t) a << 24) | ((uint32_t) b << 16) | ((uint32_t) c << 8) | d;
    *mask = slash ? 0xffffffffU << (32 - slash) : 0;
  }

  return len;
}

int ns_check_ip_acl(const char *acl, uint32_t remote_ip) {
  int allowed, flag;
  uint32_t net, mask;
  struct ns_str vec;

  /* If any ACL is set, deny by default */
  allowed = (acl == NULL || *acl == '\0') ? '+' : '-';

  while ((acl = ns_next_comma_list_entry(acl, &vec, NULL)) != NULL) {
    flag = vec.p[0];
    if ((flag != '+' && flag != '-') ||
        parse_net(&vec.p[1], &net, &mask) == 0) {
      return -1;
    }

    if (net == (remote_ip & mask)) {
      allowed = flag;
    }
  }

  return allowed == '+';
}
#ifdef NS_MODULE_LINES
#line 1 "src/http.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef NS_DISABLE_HTTP_WEBSOCKET


enum http_proto_data_type { DATA_FILE, DATA_PUT, DATA_CGI };

struct proto_data_http {
  FILE *fp;     /* Opened file. */
  int64_t cl;   /* Content-Length. How many bytes to send. */
  int64_t sent; /* How many bytes have been already sent. */
  struct ns_connection *cgi_nc;
  enum http_proto_data_type type;
};

/*
 * This structure helps to create an environment for the spawned CGI program.
 * Environment is an array of "VARIABLE=VALUE\0" ASCIIZ strings,
 * last element must be NULL.
 * However, on Windows there is a requirement that all these VARIABLE=VALUE\0
 * strings must reside in a contiguous buffer. The end of the buffer is
 * marked by two '\0' characters.
 * We satisfy both worlds: we create an envp array (which is vars), all
 * entries are actually pointers inside buf.
 */
struct cgi_env_block {
  struct ns_connection *nc;
  char buf[NS_CGI_ENVIRONMENT_SIZE];       /* Environment buffer */
  const char *vars[NS_MAX_CGI_ENVIR_VARS]; /* char *envp[] */
  int len;                                 /* Space taken */
  int nvars;                               /* Number of variables in envp[] */
};

#define MIME_ENTRY(_ext, _type) \
  { _ext, sizeof(_ext) - 1, _type }
static const struct {
  const char *extension;
  size_t ext_len;
  const char *mime_type;
} static_builtin_mime_types[] = {
    MIME_ENTRY("html", "text/html"),
    MIME_ENTRY("html", "text/html"),
    MIME_ENTRY("htm", "text/html"),
    MIME_ENTRY("shtm", "text/html"),
    MIME_ENTRY("shtml", "text/html"),
    MIME_ENTRY("css", "text/css"),
    MIME_ENTRY("js", "application/x-javascript"),
    MIME_ENTRY("ico", "image/x-icon"),
    MIME_ENTRY("gif", "image/gif"),
    MIME_ENTRY("jpg", "image/jpeg"),
    MIME_ENTRY("jpeg", "image/jpeg"),
    MIME_ENTRY("png", "image/png"),
    MIME_ENTRY("svg", "image/svg+xml"),
    MIME_ENTRY("txt", "text/plain"),
    MIME_ENTRY("torrent", "application/x-bittorrent"),
    MIME_ENTRY("wav", "audio/x-wav"),
    MIME_ENTRY("mp3", "audio/x-mp3"),
    MIME_ENTRY("mid", "audio/mid"),
    MIME_ENTRY("m3u", "audio/x-mpegurl"),
    MIME_ENTRY("ogg", "application/ogg"),
    MIME_ENTRY("ram", "audio/x-pn-realaudio"),
    MIME_ENTRY("xml", "text/xml"),
    MIME_ENTRY("ttf", "application/x-font-ttf"),
    MIME_ENTRY("json", "application/json"),
    MIME_ENTRY("xslt", "application/xml"),
    MIME_ENTRY("xsl", "application/xml"),
    MIME_ENTRY("ra", "audio/x-pn-realaudio"),
    MIME_ENTRY("doc", "application/msword"),
    MIME_ENTRY("exe", "application/octet-stream"),
    MIME_ENTRY("zip", "application/x-zip-compressed"),
    MIME_ENTRY("xls", "application/excel"),
    MIME_ENTRY("tgz", "application/x-tar-gz"),
    MIME_ENTRY("tar", "application/x-tar"),
    MIME_ENTRY("gz", "application/x-gunzip"),
    MIME_ENTRY("arj", "application/x-arj-compressed"),
    MIME_ENTRY("rar", "application/x-rar-compressed"),
    MIME_ENTRY("rtf", "application/rtf"),
    MIME_ENTRY("pdf", "application/pdf"),
    MIME_ENTRY("swf", "application/x-shockwave-flash"),
    MIME_ENTRY("mpg", "video/mpeg"),
    MIME_ENTRY("webm", "video/webm"),
    MIME_ENTRY("mpeg", "video/mpeg"),
    MIME_ENTRY("mov", "video/quicktime"),
    MIME_ENTRY("mp4", "video/mp4"),
    MIME_ENTRY("m4v", "video/x-m4v"),
    MIME_ENTRY("asf", "video/x-ms-asf"),
    MIME_ENTRY("avi", "video/x-msvideo"),
    MIME_ENTRY("bmp", "image/bmp"),
    {NULL, 0, NULL}};

#ifndef NS_DISABLE_FILESYSTEM

static struct ns_str get_mime_type(const char *path, const char *dflt,
                                   const struct ns_serve_http_opts *opts) {
  const char *ext, *overrides;
  size_t i, path_len;
  struct ns_str r, k, v;

  path_len = strlen(path);

  overrides = opts->custom_mime_types;
  while ((overrides = ns_next_comma_list_entry(overrides, &k, &v)) != NULL) {
    ext = path + (path_len - k.len);
    if (path_len > k.len && ns_vcasecmp(&k, ext) == 0) {
      return v;
    }
  }

  for (i = 0; static_builtin_mime_types[i].extension != NULL; i++) {
    ext = path + (path_len - static_builtin_mime_types[i].ext_len);
    if (path_len > static_builtin_mime_types[i].ext_len && ext[-1] == '.' &&
        ns_casecmp(ext, static_builtin_mime_types[i].extension) == 0) {
      r.p = static_builtin_mime_types[i].mime_type;
      r.len = strlen(r.p);
      return r;
    }
  }

  r.p = dflt;
  r.len = strlen(r.p);
  return r;
}
#endif

/*
 * Check whether full request is buffered. Return:
 *   -1  if request is malformed
 *    0  if request is not yet fully buffered
 *   >0  actual request length, including last \r\n\r\n
 */
static int get_request_len(const char *s, int buf_len) {
  const unsigned char *buf = (unsigned char *) s;
  int i;

  for (i = 0; i < buf_len; i++) {
    if (!isprint(buf[i]) && buf[i] != '\r' && buf[i] != '\n' && buf[i] < 128) {
      return -1;
    } else if (buf[i] == '\n' && i + 1 < buf_len && buf[i + 1] == '\n') {
      return i + 2;
    } else if (buf[i] == '\n' && i + 2 < buf_len && buf[i + 1] == '\r' &&
               buf[i + 2] == '\n') {
      return i + 3;
    }
  }

  return 0;
}

static const char *parse_http_headers(const char *s, const char *end, int len,
                                      struct http_message *req) {
  int i;
  for (i = 0; i < (int) ARRAY_SIZE(req->header_names); i++) {
    struct ns_str *k = &req->header_names[i], *v = &req->header_values[i];

    s = ns_skip(s, end, ": ", k);
    s = ns_skip(s, end, "\r\n", v);

    while (v->len > 0 && v->p[v->len - 1] == ' ') {
      v->len--; /* Trim trailing spaces in header value */
    }

    if (k->len == 0 || v->len == 0) {
      k->p = v->p = NULL;
      k->len = v->len = 0;
      break;
    }

    if (!ns_ncasecmp(k->p, "Content-Length", 14)) {
      req->body.len = to64(v->p);
      req->message.len = len + req->body.len;
    }
  }

  return s;
}

int ns_parse_http(const char *s, int n, struct http_message *hm, int is_req) {
  const char *end, *qs;
  int len = get_request_len(s, n);

  if (len <= 0) return len;

  memset(hm, 0, sizeof(*hm));
  hm->message.p = s;
  hm->body.p = s + len;
  hm->message.len = hm->body.len = (size_t) ~0;
  end = s + len;

  /* Request is fully buffered. Skip leading whitespaces. */
  while (s < end && isspace(*(unsigned char *) s)) s++;

  if (is_req) {
    /* Parse request line: method, URI, proto */
    s = ns_skip(s, end, " ", &hm->method);
    s = ns_skip(s, end, " ", &hm->uri);
    s = ns_skip(s, end, "\r\n", &hm->proto);
    if (hm->uri.p <= hm->method.p || hm->proto.p <= hm->uri.p) return -1;

    /* If URI contains '?' character, initialize query_string */
    if ((qs = (char *) memchr(hm->uri.p, '?', hm->uri.len)) != NULL) {
      hm->query_string.p = qs + 1;
      hm->query_string.len = &hm->uri.p[hm->uri.len] - (qs + 1);
      hm->uri.len = qs - hm->uri.p;
    }
  } else {
    s = ns_skip(s, end, " ", &hm->proto);
    if (end - s < 4 || s[3] != ' ') return -1;
    hm->resp_code = atoi(s);
    if (hm->resp_code < 100 || hm->resp_code >= 600) return -1;
    s += 4;
    s = ns_skip(s, end, "\r\n", &hm->resp_status_msg);
  }

  s = parse_http_headers(s, end, len, hm);

  /*
   * ns_parse_http() is used to parse both HTTP requests and HTTP
   * responses. If HTTP response does not have Content-Length set, then
   * body is read until socket is closed, i.e. body.len is infinite (~0).
   *
   * For HTTP requests though, according to
   * http://tools.ietf.org/html/rfc7231#section-8.1.3,
   * only POST and PUT methods have defined body semantics.
   * Therefore, if Content-Length is not specified and methods are
   * not one of PUT or POST, set body length to 0.
   *
   * So,
   * if it is HTTP request, and Content-Length is not set,
   * and method is not (PUT or POST) then reset body length to zero.
   */
  if (hm->body.len == (size_t) ~0 && is_req &&
      ns_vcasecmp(&hm->method, "PUT") != 0 &&
      ns_vcasecmp(&hm->method, "POST") != 0) {
    hm->body.len = 0;
    hm->message.len = len;
  }

  return len;
}

struct ns_str *ns_get_http_header(struct http_message *hm, const char *name) {
  size_t i, len = strlen(name);

  for (i = 0; i < ARRAY_SIZE(hm->header_names); i++) {
    struct ns_str *h = &hm->header_names[i], *v = &hm->header_values[i];
    if (h->p != NULL && h->len == len && !ns_ncasecmp(h->p, name, len))
      return v;
  }

  return NULL;
}

static int is_ws_fragment(unsigned char flags) {
  return (flags & 0x80) == 0 || (flags & 0x0f) == 0;
}

static int is_ws_first_fragment(unsigned char flags) {
  return (flags & 0x80) == 0 && (flags & 0x0f) != 0;
}

static void handle_incoming_websocket_frame(struct ns_connection *nc,
                                            struct websocket_message *wsm) {
  if (wsm->flags & 0x8) {
    nc->handler(nc, NS_WEBSOCKET_CONTROL_FRAME, wsm);
  } else {
    nc->handler(nc, NS_WEBSOCKET_FRAME, wsm);
  }
}

static int deliver_websocket_data(struct ns_connection *nc) {
  /* Using unsigned char *, cause of integer arithmetic below */
  uint64_t i, data_len = 0, frame_len = 0, buf_len = nc->recv_mbuf.len, len,
              mask_len = 0, header_len = 0;
  unsigned char *p = (unsigned char *) nc->recv_mbuf.buf, *buf = p,
                *e = p + buf_len;
  unsigned *sizep = (unsigned *) &p[1]; /* Size ptr for defragmented frames */
  int ok, reass = buf_len > 0 && is_ws_fragment(p[0]) &&
                  !(nc->flags & NSF_WEBSOCKET_NO_DEFRAG);

  /* If that's a continuation frame that must be reassembled, handle it */
  if (reass && !is_ws_first_fragment(p[0]) && buf_len >= 1 + sizeof(*sizep) &&
      buf_len >= 1 + sizeof(*sizep) + *sizep) {
    buf += 1 + sizeof(*sizep) + *sizep;
    buf_len -= 1 + sizeof(*sizep) + *sizep;
  }

  if (buf_len >= 2) {
    len = buf[1] & 127;
    mask_len = buf[1] & 128 ? 4 : 0;
    if (len < 126 && buf_len >= mask_len) {
      data_len = len;
      header_len = 2 + mask_len;
    } else if (len == 126 && buf_len >= 4 + mask_len) {
      header_len = 4 + mask_len;
      data_len = ntohs(*(uint16_t *) &buf[2]);
    } else if (buf_len >= 10 + mask_len) {
      header_len = 10 + mask_len;
      data_len = (((uint64_t) ntohl(*(uint32_t *) &buf[2])) << 32) +
                 ntohl(*(uint32_t *) &buf[6]);
    }
  }

  frame_len = header_len + data_len;
  ok = frame_len > 0 && frame_len <= buf_len;

  if (ok) {
    struct websocket_message wsm;

    wsm.size = (size_t) data_len;
    wsm.data = buf + header_len;
    wsm.flags = buf[0];

    /* Apply mask if necessary */
    if (mask_len > 0) {
      for (i = 0; i < data_len; i++) {
        buf[i + header_len] ^= (buf + header_len - mask_len)[i % 4];
      }
    }

    if (reass) {
      /* On first fragmented frame, nullify size */
      if (is_ws_first_fragment(wsm.flags)) {
        mbuf_resize(&nc->recv_mbuf, nc->recv_mbuf.size + sizeof(*sizep));
        p[0] &= ~0x0f; /* Next frames will be treated as continuation */
        buf = p + 1 + sizeof(*sizep);
        *sizep = 0; /* TODO(lsm): fix. this can stomp over frame data */
      }

      /* Append this frame to the reassembled buffer */
      memmove(buf, wsm.data, e - wsm.data);
      (*sizep) += wsm.size;
      nc->recv_mbuf.len -= wsm.data - buf;

      /* On last fragmented frame - call user handler and remove data */
      if (wsm.flags & 0x80) {
        wsm.data = p + 1 + sizeof(*sizep);
        wsm.size = *sizep;
        handle_incoming_websocket_frame(nc, &wsm);
        mbuf_remove(&nc->recv_mbuf, 1 + sizeof(*sizep) + *sizep);
      }
    } else {
      /* TODO(lsm): properly handle OOB control frames during defragmentation */
      handle_incoming_websocket_frame(nc, &wsm);
      mbuf_remove(&nc->recv_mbuf, (size_t) frame_len); /* Cleanup frame */
    }

    /* If client closes, close too */
    if ((buf[0] & 0x0f) == WEBSOCKET_OP_CLOSE) {
      nc->flags |= NSF_SEND_AND_CLOSE;
    }
  }

  return ok;
}

static void ns_send_ws_header(struct ns_connection *nc, int op, size_t len) {
  int header_len;
  unsigned char header[10];

  header[0] = 0x80 + (op & 0x0f);
  if (len < 126) {
    header[1] = len;
    header_len = 2;
  } else if (len < 65535) {
    uint16_t tmp = htons((uint16_t) len);
    header[1] = 126;
    memcpy(&header[2], &tmp, sizeof(tmp));
    header_len = 4;
  } else {
    uint32_t tmp;
    header[1] = 127;
    tmp = htonl((uint32_t)((uint64_t) len >> 32));
    memcpy(&header[2], &tmp, sizeof(tmp));
    tmp = htonl((uint32_t)(len & 0xffffffff));
    memcpy(&header[6], &tmp, sizeof(tmp));
    header_len = 10;
  }
  ns_send(nc, header, header_len);
}

void ns_send_websocket_frame(struct ns_connection *nc, int op, const void *data,
                             size_t len) {
  ns_send_ws_header(nc, op, len);
  ns_send(nc, data, len);

  if (op == WEBSOCKET_OP_CLOSE) {
    nc->flags |= NSF_SEND_AND_CLOSE;
  }
}

void ns_send_websocket_framev(struct ns_connection *nc, int op,
                              const struct ns_str *strv, int strvcnt) {
  int i;
  int len = 0;
  for (i = 0; i < strvcnt; i++) {
    len += strv[i].len;
  }

  ns_send_ws_header(nc, op, len);

  for (i = 0; i < strvcnt; i++) {
    ns_send(nc, strv[i].p, strv[i].len);
  }

  if (op == WEBSOCKET_OP_CLOSE) {
    nc->flags |= NSF_SEND_AND_CLOSE;
  }
}

void ns_printf_websocket_frame(struct ns_connection *nc, int op,
                               const char *fmt, ...) {
  char mem[4192], *buf = mem;
  va_list ap;
  int len;

  va_start(ap, fmt);
  if ((len = ns_avprintf(&buf, sizeof(mem), fmt, ap)) > 0) {
    ns_send_websocket_frame(nc, op, buf, len);
  }
  va_end(ap);

  if (buf != mem && buf != NULL) {
    NS_FREE(buf);
  }
}

static void websocket_handler(struct ns_connection *nc, int ev, void *ev_data) {
  nc->handler(nc, ev, ev_data);

  switch (ev) {
    case NS_RECV:
      do {
      } while (deliver_websocket_data(nc));
      break;
    case NS_POLL:
      /* Ping idle websocket connections */
      {
        time_t now = *(time_t *) ev_data;
        if (nc->flags & NSF_IS_WEBSOCKET &&
            now > nc->last_io_time + NS_WEBSOCKET_PING_INTERVAL_SECONDS) {
          ns_send_websocket_frame(nc, WEBSOCKET_OP_PING, "", 0);
        }
      }
      break;
    default:
      break;
  }
}

static void ws_handshake(struct ns_connection *nc, const struct ns_str *key) {
  static const char *magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  char buf[500], sha[20], b64_sha[sizeof(sha) * 2];
  SHA1_CTX sha_ctx;

  snprintf(buf, sizeof(buf), "%.*s%s", (int) key->len, key->p, magic);

  SHA1Init(&sha_ctx);
  SHA1Update(&sha_ctx, (unsigned char *) buf, strlen(buf));
  SHA1Final((unsigned char *) sha, &sha_ctx);

  ns_base64_encode((unsigned char *) sha, sizeof(sha), b64_sha);
  ns_printf(nc, "%s%s%s",
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: ",
            b64_sha, "\r\n\r\n");
}

static void free_http_proto_data(struct ns_connection *nc) {
  struct proto_data_http *dp = (struct proto_data_http *) nc->proto_data;
  if (dp != NULL) {
    if (dp->fp != NULL) {
      fclose(dp->fp);
    }
    if (dp->cgi_nc != NULL) {
      dp->cgi_nc->flags |= NSF_CLOSE_IMMEDIATELY;
    }
    NS_FREE(dp);
    nc->proto_data = NULL;
  }
}

/* Move data from one connection to another */
static void ns_forward(struct ns_connection *from, struct ns_connection *to) {
  ns_send(to, from->recv_mbuf.buf, from->recv_mbuf.len);
  mbuf_remove(&from->recv_mbuf, from->recv_mbuf.len);
}

static void transfer_file_data(struct ns_connection *nc) {
  struct proto_data_http *dp = (struct proto_data_http *) nc->proto_data;
  char buf[NS_MAX_HTTP_SEND_IOBUF];
  int64_t left = dp->cl - dp->sent;
  size_t n = 0, to_read = 0;

  if (dp->type == DATA_FILE) {
    struct mbuf *io = &nc->send_mbuf;
    if (io->len < sizeof(buf)) {
      to_read = sizeof(buf) - io->len;
    }

    if (left > 0 && to_read > (size_t) left) {
      to_read = left;
    }

    if (to_read == 0) {
      /* Rate limiting. send_mbuf is too full, wait until it's drained. */
    } else if (dp->sent < dp->cl && (n = fread(buf, 1, to_read, dp->fp)) > 0) {
      ns_send(nc, buf, n);
      dp->sent += n;
    } else {
      free_http_proto_data(nc);
    }
  } else if (dp->type == DATA_PUT) {
    struct mbuf *io = &nc->recv_mbuf;
    size_t to_write =
        left <= 0 ? 0 : left < (int64_t) io->len ? (size_t) left : io->len;
    size_t n = fwrite(io->buf, 1, to_write, dp->fp);
    if (n > 0) {
      mbuf_remove(io, n);
      dp->sent += n;
    }
    if (n == 0 || dp->sent >= dp->cl) {
      free_http_proto_data(nc);
    }
  } else if (dp->type == DATA_CGI) {
    /* This is POST data that needs to be forwarded to the CGI process */
    if (dp->cgi_nc != NULL) {
      ns_forward(nc, dp->cgi_nc);
    } else {
      nc->flags |= NSF_SEND_AND_CLOSE;
    }
  }
}

static void http_handler(struct ns_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;
  struct http_message hm;
  struct ns_str *vec;
  int req_len;
  const int is_req = (nc->listener != NULL);

  /*
   * For HTTP messages without Content-Length, always send HTTP message
   * before NS_CLOSE message.
   */
  if (ev == NS_CLOSE && io->len > 0 &&
      ns_parse_http(io->buf, io->len, &hm, is_req) > 0) {
    hm.message.len = io->len;
    hm.body.len = io->buf + io->len - hm.body.p;
    nc->handler(nc, is_req ? NS_HTTP_REQUEST : NS_HTTP_REPLY, &hm);
    free_http_proto_data(nc);
  }

  if (nc->proto_data != NULL) {
    transfer_file_data(nc);
  }

  nc->handler(nc, ev, ev_data);

  if (ev == NS_RECV) {
    req_len = ns_parse_http(io->buf, io->len, &hm, is_req);
    if (req_len < 0 || (req_len == 0 && io->len >= NS_MAX_HTTP_REQUEST_SIZE)) {
      nc->flags |= NSF_CLOSE_IMMEDIATELY;
    } else if (req_len == 0) {
      /* Do nothing, request is not yet fully buffered */
    } else if (nc->listener == NULL &&
               ns_get_http_header(&hm, "Sec-WebSocket-Accept")) {
      /* We're websocket client, got handshake response from server. */
      /* TODO(lsm): check the validity of accept Sec-WebSocket-Accept */
      mbuf_remove(io, req_len);
      nc->proto_handler = websocket_handler;
      nc->flags |= NSF_IS_WEBSOCKET;
      nc->handler(nc, NS_WEBSOCKET_HANDSHAKE_DONE, NULL);
      websocket_handler(nc, NS_RECV, ev_data);
    } else if (nc->listener != NULL &&
               (vec = ns_get_http_header(&hm, "Sec-WebSocket-Key")) != NULL) {
      /* This is a websocket request. Switch protocol handlers. */
      mbuf_remove(io, req_len);
      nc->proto_handler = websocket_handler;
      nc->flags |= NSF_IS_WEBSOCKET;

      /* Send handshake */
      nc->handler(nc, NS_WEBSOCKET_HANDSHAKE_REQUEST, &hm);
      if (!(nc->flags & NSF_CLOSE_IMMEDIATELY)) {
        if (nc->send_mbuf.len == 0) {
          ws_handshake(nc, vec);
        }
        nc->handler(nc, NS_WEBSOCKET_HANDSHAKE_DONE, NULL);
        websocket_handler(nc, NS_RECV, ev_data);
      }
    } else if (hm.message.len <= io->len) {
      /* Whole HTTP message is fully buffered, call event handler */
      nc->handler(nc, nc->listener ? NS_HTTP_REQUEST : NS_HTTP_REPLY, &hm);
      mbuf_remove(io, hm.message.len);
    }
  }
}

void ns_set_protocol_http_websocket(struct ns_connection *nc) {
  nc->proto_handler = http_handler;
}

void ns_send_websocket_handshake(struct ns_connection *nc, const char *uri,
                                 const char *extra_headers) {
  unsigned long random = (unsigned long) uri;
  char key[sizeof(random) * 3];

  ns_base64_encode((unsigned char *) &random, sizeof(random), key);
  ns_printf(nc,
            "GET %s HTTP/1.1\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Version: 13\r\n"
            "Sec-WebSocket-Key: %s\r\n"
            "%s\r\n",
            uri, key, extra_headers == NULL ? "" : extra_headers);
}

#ifndef NS_DISABLE_FILESYSTEM
static void send_http_error(struct ns_connection *nc, int code,
                            const char *reason) {
  if (reason == NULL) {
    reason = "";
  }
  ns_printf(nc, "HTTP/1.1 %d %s\r\nContent-Length: 0\r\n\r\n", code, reason);
}

/* Suffix must be smaller then string */
static int has_suffix(const char *str, const char *suffix) {
  return str != NULL && suffix != NULL && strlen(suffix) < strlen(str) &&
         strcmp(str + strlen(str) - strlen(suffix), suffix) == 0;
}

#ifndef NS_DISABLE_SSI
static void send_ssi_file(struct ns_connection *, const char *, FILE *, int,
                          const struct ns_serve_http_opts *);

static void send_file_data(struct ns_connection *nc, FILE *fp) {
  char buf[BUFSIZ];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
    ns_send(nc, buf, n);
  }
}

static void do_ssi_include(struct ns_connection *nc, const char *ssi, char *tag,
                           int include_level,
                           const struct ns_serve_http_opts *opts) {
  char file_name[BUFSIZ], path[MAX_PATH_SIZE], *p;
  FILE *fp;

  /*
   * sscanf() is safe here, since send_ssi_file() also uses buffer
   * of size MG_BUF_LEN to get the tag. So strlen(tag) is always < MG_BUF_LEN.
   */
  if (sscanf(tag, " virtual=\"%[^\"]\"", file_name) == 1) {
    /* File name is relative to the webserver root */
    snprintf(path, sizeof(path), "%s/%s", opts->document_root, file_name);
  } else if (sscanf(tag, " abspath=\"%[^\"]\"", file_name) == 1) {
    /*
     * File name is relative to the webserver working directory
     * or it is absolute system path
     */
    snprintf(path, sizeof(path), "%s", file_name);
  } else if (sscanf(tag, " file=\"%[^\"]\"", file_name) == 1 ||
             sscanf(tag, " \"%[^\"]\"", file_name) == 1) {
    /* File name is relative to the currect document */
    snprintf(path, sizeof(path), "%s", ssi);
    if ((p = strrchr(path, '/')) != NULL) {
      p[1] = '\0';
    }
    snprintf(path + strlen(path), sizeof(path) - strlen(path), "%s", file_name);
  } else {
    ns_printf(nc, "Bad SSI #include: [%s]", tag);
    return;
  }

  if ((fp = fopen(path, "rb")) == NULL) {
    ns_printf(nc, "SSI include error: fopen(%s): %s", path, strerror(errno));
  } else {
    ns_set_close_on_exec(fileno(fp));
    if (has_suffix(path, opts->ssi_suffix)) {
      send_ssi_file(nc, path, fp, include_level + 1, opts);
    } else {
      send_file_data(nc, fp);
    }
    fclose(fp);
  }
}

#ifndef NS_DISABLE_POPEN
static void do_ssi_exec(struct ns_connection *nc, char *tag) {
  char cmd[BUFSIZ];
  FILE *fp;

  if (sscanf(tag, " \"%[^\"]\"", cmd) != 1) {
    ns_printf(nc, "Bad SSI #exec: [%s]", tag);
  } else if ((fp = popen(cmd, "r")) == NULL) {
    ns_printf(nc, "Cannot SSI #exec: [%s]: %s", cmd, strerror(errno));
  } else {
    send_file_data(nc, fp);
    pclose(fp);
  }
}
#endif /* !NS_DISABLE_POPEN */

static void send_ssi_file(struct ns_connection *nc, const char *path, FILE *fp,
                          int include_level,
                          const struct ns_serve_http_opts *opts) {
  char buf[BUFSIZ];
  int ch, offset, len, in_ssi_tag;

  if (include_level > 10) {
    ns_printf(nc, "SSI #include level is too deep (%s)", path);
    return;
  }

  in_ssi_tag = len = offset = 0;
  while ((ch = fgetc(fp)) != EOF) {
    if (in_ssi_tag && ch == '>') {
      in_ssi_tag = 0;
      buf[len++] = (char) ch;
      buf[len] = '\0';
      assert(len <= (int) sizeof(buf));
      if (len < 6 || memcmp(buf, "<!--#", 5) != 0) {
        /* Not an SSI tag, pass it */
        (void) ns_send(nc, buf, (size_t) len);
      } else {
        if (!memcmp(buf + 5, "include", 7)) {
          do_ssi_include(nc, path, buf + 12, include_level, opts);
#ifndef NS_DISABLE_POPEN
        } else if (!memcmp(buf + 5, "exec", 4)) {
          do_ssi_exec(nc, buf + 9);
#endif /* !NO_POPEN */
        } else {
          /* Silently ignoring unknown SSI commands. */
        }
      }
      len = 0;
    } else if (in_ssi_tag) {
      if (len == 5 && memcmp(buf, "<!--#", 5) != 0) {
        /* Not an SSI tag */
        in_ssi_tag = 0;
      } else if (len == (int) sizeof(buf) - 2) {
        ns_printf(nc, "%s: SSI tag is too large", path);
        len = 0;
      }
      buf[len++] = ch & 0xff;
    } else if (ch == '<') {
      in_ssi_tag = 1;
      if (len > 0) {
        ns_send(nc, buf, (size_t) len);
      }
      len = 0;
      buf[len++] = ch & 0xff;
    } else {
      buf[len++] = ch & 0xff;
      if (len == (int) sizeof(buf)) {
        ns_send(nc, buf, (size_t) len);
        len = 0;
      }
    }
  }

  /* Send the rest of buffered data */
  if (len > 0) {
    ns_send(nc, buf, (size_t) len);
  }
}

static void handle_ssi_request(struct ns_connection *nc, const char *path,
                               const struct ns_serve_http_opts *opts) {
  FILE *fp;
  struct ns_str mime_type;

  if ((fp = fopen(path, "rb")) == NULL) {
    send_http_error(nc, 404, "Not Found");
  } else {
    ns_set_close_on_exec(fileno(fp));

    mime_type = get_mime_type(path, "text/plain", opts);
    ns_printf(nc,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: %.*s\r\n"
              "Connection: close\r\n\r\n",
              (int) mime_type.len, mime_type.p);
    send_ssi_file(nc, path, fp, 0, opts);
    fclose(fp);
    nc->flags |= NSF_SEND_AND_CLOSE;
  }
}
#else
static void handle_ssi_request(struct ns_connection *nc, const char *path,
                               const struct ns_serve_http_opts *opts) {
  (void) path;
  (void) opts;
  send_http_error(nc, 500, "SSI disabled");
}
#endif /* NS_DISABLE_SSI */

static void construct_etag(char *buf, size_t buf_len, const ns_stat_t *st) {
  snprintf(buf, buf_len, "\"%lx.%" INT64_FMT "\"", (unsigned long) st->st_mtime,
           (int64_t) st->st_size);
}
static void gmt_time_string(char *buf, size_t buf_len, time_t *t) {
  strftime(buf, buf_len, "%a, %d %b %Y %H:%M:%S GMT", gmtime(t));
}

static int parse_range_header(const struct ns_str *header, int64_t *a,
                              int64_t *b) {
  /*
   * There is no snscanf. Headers are not guaranteed to be NUL-terminated,
   * so we have this. Ugh.
   */
  int result;
  char *p = (char *) NS_MALLOC(header->len + 1);
  if (p == NULL) return 0;
  memcpy(p, header->p, header->len);
  p[header->len] = '\0';
  result = sscanf(p, "bytes=%" INT64_FMT "-%" INT64_FMT, a, b);
  NS_FREE(p);
  return result;
}

static void ns_send_http_file2(struct ns_connection *nc, const char *path,
                               ns_stat_t *st, struct http_message *hm,
                               struct ns_serve_http_opts *opts) {
  struct proto_data_http *dp;
  struct ns_str mime_type;

  free_http_proto_data(nc);
  if ((dp = (struct proto_data_http *) NS_CALLOC(1, sizeof(*dp))) == NULL) {
    send_http_error(nc, 500, "Server Error"); /* LCOV_EXCL_LINE */
  } else if ((dp->fp = fopen(path, "rb")) == NULL) {
    NS_FREE(dp);
    nc->proto_data = NULL;
    send_http_error(nc, 500, "Server Error");
  } else if (has_suffix(path, opts->ssi_suffix)) {
    handle_ssi_request(nc, path, opts);
  } else {
    char etag[50], current_time[50], last_modified[50], range[50];
    time_t t = time(NULL);
    int64_t r1 = 0, r2 = 0, cl = st->st_size;
    struct ns_str *range_hdr = ns_get_http_header(hm, "Range");
    int n, status_code = 200;
    const char *status_message = "OK";

    /* Handle Range header */
    range[0] = '\0';
    if (range_hdr != NULL &&
        (n = parse_range_header(range_hdr, &r1, &r2)) > 0 && r1 >= 0 &&
        r2 >= 0) {
      /* If range is specified like "400-", set second limit to content len */
      if (n == 1) {
        r2 = cl - 1;
      }
      if (r1 > r2 || r2 >= cl) {
        status_code = 416;
        status_message = "Requested range not satisfiable";
        cl = 0;
        snprintf(range, sizeof(range),
                 "Content-Range: bytes */%" INT64_FMT "\r\n",
                 (int64_t) st->st_size);
      } else {
        status_code = 206;
        status_message = "Partial Content";
        cl = r2 - r1 + 1;
        snprintf(range, sizeof(range), "Content-Range: bytes %" INT64_FMT
                                       "-%" INT64_FMT "/%" INT64_FMT "\r\n",
                 r1, r1 + cl - 1, (int64_t) st->st_size);
        fseeko(dp->fp, r1, SEEK_SET);
      }
    }

    construct_etag(etag, sizeof(etag), st);
    gmt_time_string(current_time, sizeof(current_time), &t);
    gmt_time_string(last_modified, sizeof(last_modified), &st->st_mtime);
    mime_type = get_mime_type(path, "text/plain", opts);
    ns_printf(nc,
              "HTTP/1.1 %d %s\r\n"
              "Date: %s\r\n"
              "Last-Modified: %s\r\n"
              "Accept-Ranges: bytes\r\n"
              "Content-Type: %.*s\r\n"
              "Content-Length: %" INT64_FMT
              "\r\n"
              "%s"
              "Etag: %s\r\n"
              "\r\n",
              status_code, status_message, current_time, last_modified,
              (int) mime_type.len, mime_type.p, cl, range, etag);
    nc->proto_data = (void *) dp;
    dp->cl = cl;
    transfer_file_data(nc);
  }
}

static void remove_double_dots(char *s) {
  char *p = s;

  while (*s != '\0') {
    *p++ = *s++;
    if (s[-1] == '/' || s[-1] == '\\') {
      while (s[0] != '\0') {
        if (s[0] == '/' || s[0] == '\\') {
          s++;
        } else if (s[0] == '.' && s[1] == '.') {
          s += 2;
        } else {
          break;
        }
      }
    }
  }
  *p = '\0';
}

#endif

static int ns_url_decode(const char *src, int src_len, char *dst, int dst_len,
                         int is_form_url_encoded) {
  int i, j, a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

  for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
    if (src[i] == '%') {
      if (i < src_len - 2 && isxdigit(*(const unsigned char *) (src + i + 1)) &&
          isxdigit(*(const unsigned char *) (src + i + 2))) {
        a = tolower(*(const unsigned char *) (src + i + 1));
        b = tolower(*(const unsigned char *) (src + i + 2));
        dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
        i += 2;
      } else {
        return -1;
      }
    } else if (is_form_url_encoded && src[i] == '+') {
      dst[j] = ' ';
    } else {
      dst[j] = src[i];
    }
  }

  dst[j] = '\0'; /* Null-terminate the destination */

  return i >= src_len ? j : -1;
}

int ns_get_http_var(const struct ns_str *buf, const char *name, char *dst,
                    size_t dst_len) {
  const char *p, *e, *s;
  size_t name_len;
  int len;

  if (dst == NULL || dst_len == 0) {
    len = -2;
  } else if (buf->p == NULL || name == NULL || buf->len == 0) {
    len = -1;
    dst[0] = '\0';
  } else {
    name_len = strlen(name);
    e = buf->p + buf->len;
    len = -1;
    dst[0] = '\0';

    for (p = buf->p; p + name_len < e; p++) {
      if ((p == buf->p || p[-1] == '&') && p[name_len] == '=' &&
          !ns_ncasecmp(name, p, name_len)) {
        p += name_len + 1;
        s = (const char *) memchr(p, '&', (size_t)(e - p));
        if (s == NULL) {
          s = e;
        }
        len = ns_url_decode(p, (size_t)(s - p), dst, dst_len, 1);
        if (len == -1) {
          len = -2;
        }
        break;
      }
    }
  }

  return len;
}

void ns_send_http_chunk(struct ns_connection *nc, const char *buf, size_t len) {
  char chunk_size[50];
  int n;

  n = snprintf(chunk_size, sizeof(chunk_size), "%lX\r\n", (unsigned long) len);
  ns_send(nc, chunk_size, n);
  ns_send(nc, buf, len);
  ns_send(nc, "\r\n", 2);
}

void ns_printf_http_chunk(struct ns_connection *nc, const char *fmt, ...) {
  char mem[500], *buf = mem;
  int len;
  va_list ap;

  va_start(ap, fmt);
  len = ns_avprintf(&buf, sizeof(mem), fmt, ap);
  va_end(ap);

  if (len >= 0) {
    ns_send_http_chunk(nc, buf, len);
  }

  /* LCOV_EXCL_START */
  if (buf != mem && buf != NULL) {
    NS_FREE(buf);
  }
  /* LCOV_EXCL_STOP */
}

int ns_http_parse_header(struct ns_str *hdr, const char *var_name, char *buf,
                         size_t buf_size) {
  int ch = ' ', ch1 = ',', len = 0, n = strlen(var_name);
  const char *p, *end = hdr->p + hdr->len, *s = NULL;

  if (buf != NULL && buf_size > 0) buf[0] = '\0';

  /* Find where variable starts */
  for (s = hdr->p; s != NULL && s + n < end; s++) {
    if ((s == hdr->p || s[-1] == ch || s[-1] == ch1) && s[n] == '=' &&
        !memcmp(s, var_name, n))
      break;
  }

  if (s != NULL && &s[n + 1] < end) {
    s += n + 1;
    if (*s == '"' || *s == '\'') {
      ch = ch1 = *s++;
    }
    p = s;
    while (p < end && p[0] != ch && p[0] != ch1 && len < (int) buf_size) {
      if (ch != ' ' && p[0] == '\\' && p[1] == ch) p++;
      buf[len++] = *p++;
    }
    if (len >= (int) buf_size || (ch != ' ' && *p != ch)) {
      len = 0;
    } else {
      if (len > 0 && s[len - 1] == ',') len--;
      if (len > 0 && s[len - 1] == ';') len--;
      buf[len] = '\0';
    }
  }

  return len;
}

#ifndef NS_DISABLE_FILESYSTEM
static int is_file_hidden(const char *path,
                          const struct ns_serve_http_opts *opts) {
  const char *p1 = opts->per_directory_auth_file;
  const char *p2 = opts->hidden_file_pattern;
  return !strcmp(path, ".") || !strcmp(path, "..") ||
         (p1 != NULL && !strcmp(path, p1)) ||
         (p2 != NULL && ns_match_prefix(p2, strlen(p2), path) > 0);
}

#ifndef NS_DISABLE_HTTP_DIGEST_AUTH
static FILE *open_auth_file(const char *path, int is_directory,
                            const struct ns_serve_http_opts *opts) {
  char buf[MAX_PATH_SIZE];
  const char *p;
  FILE *fp = NULL;

  if (opts->global_auth_file != NULL) {
    fp = fopen(opts->global_auth_file, "r");
  } else if (is_directory && opts->per_directory_auth_file) {
    snprintf(buf, sizeof(buf), "%s%c%s", path, DIRSEP,
             opts->per_directory_auth_file);
    fp = fopen(buf, "r");
  } else if (opts->per_directory_auth_file) {
    if ((p = strrchr(path, '/')) == NULL && (p = strrchr(path, '\\')) == NULL) {
      p = path;
    }
    snprintf(buf, sizeof(buf), "%.*s/%s", (int) (p - path), path,
             opts->per_directory_auth_file);
    fp = fopen(buf, "r");
  }

  return fp;
}

/*
 * Stringify binary data. Output buffer size must be 2 * size_of_input + 1
 * because each byte of input takes 2 bytes in string representation
 * plus 1 byte for the terminating \0 character.
 */
static void bin2str(char *to, const unsigned char *p, size_t len) {
  static const char *hex = "0123456789abcdef";

  for (; len--; p++) {
    *to++ = hex[p[0] >> 4];
    *to++ = hex[p[0] & 0x0f];
  }
  *to = '\0';
}

static char *ns_md5(char *buf, ...) {
  unsigned char hash[16];
  const unsigned char *p;
  va_list ap;
  MD5_CTX ctx;

  MD5_Init(&ctx);

  va_start(ap, buf);
  while ((p = va_arg(ap, const unsigned char *) ) != NULL) {
    size_t len = va_arg(ap, size_t);
    MD5_Update(&ctx, p, len);
  }
  va_end(ap);

  MD5_Final(hash, &ctx);
  bin2str(buf, hash, sizeof(hash));

  return buf;
}

static void mkmd5resp(const char *method, size_t method_len, const char *uri,
                      size_t uri_len, const char *ha1, size_t ha1_len,
                      const char *nonce, size_t nonce_len, const char *nc,
                      size_t nc_len, const char *cnonce, size_t cnonce_len,
                      const char *qop, size_t qop_len, char *resp) {
  static const char colon[] = ":";
  static const size_t one = 1;
  char ha2[33];

  ns_md5(ha2, method, method_len, colon, one, uri, uri_len, NULL);
  ns_md5(resp, ha1, ha1_len, colon, one, nonce, nonce_len, colon, one, nc,
         nc_len, colon, one, cnonce, cnonce_len, colon, one, qop, qop_len,
         colon, one, ha2, sizeof(ha2) - 1, NULL);
}

int ns_http_create_digest_auth_header(char *buf, size_t buf_len,
                                      const char *method, const char *uri,
                                      const char *auth_domain, const char *user,
                                      const char *passwd) {
  static const char colon[] = ":", qop[] = "auth";
  static const size_t one = 1;
  char ha1[33], resp[33], cnonce[40];

  snprintf(cnonce, sizeof(cnonce), "%x", (unsigned int) time(NULL));
  ns_md5(ha1, user, (size_t) strlen(user), colon, one, auth_domain,
         (size_t) strlen(auth_domain), colon, one, passwd,
         (size_t) strlen(passwd), NULL);
  mkmd5resp(method, strlen(method), uri, strlen(uri), ha1, sizeof(ha1) - 1,
            cnonce, strlen(cnonce), "1", one, cnonce, strlen(cnonce), qop,
            sizeof(qop) - 1, resp);
  return snprintf(buf, buf_len,
                  "Authorization: Digest username=\"%s\","
                  "realm=\"%s\",uri=\"%s\",qop=%s,nc=1,cnonce=%s,"
                  "nonce=%s,response=%s\r\n",
                  user, auth_domain, uri, qop, cnonce, cnonce, resp);
}

/*
 * Check for authentication timeout.
 * Clients send time stamp encoded in nonce. Make sure it is not too old,
 * to prevent replay attacks.
 * Assumption: nonce is a hexadecimal number of seconds since 1970.
 */
static int check_nonce(const char *nonce) {
  unsigned long now = (unsigned long) time(NULL);
  unsigned long val = (unsigned long) strtoul(nonce, NULL, 16);
  return 1 || now < val || now - val < 3600;
}

/*
 * Authenticate HTTP request against opened passwords file.
 * Returns 1 if authenticated, 0 otherwise.
 */
static int ns_http_check_digest_auth(struct http_message *hm,
                                     const char *auth_domain, FILE *fp) {
  struct ns_str *hdr;
  char buf[128], f_user[sizeof(buf)], f_ha1[sizeof(buf)], f_domain[sizeof(buf)];
  char user[50], cnonce[20], response[40], uri[200], qop[20], nc[20], nonce[30];
  char expected_response[33];

  /* Parse "Authorization:" header, fail fast on parse error */
  if (hm == NULL || fp == NULL ||
      (hdr = ns_get_http_header(hm, "Authorization")) == NULL ||
      ns_http_parse_header(hdr, "username", user, sizeof(user)) == 0 ||
      ns_http_parse_header(hdr, "cnonce", cnonce, sizeof(cnonce)) == 0 ||
      ns_http_parse_header(hdr, "response", response, sizeof(response)) == 0 ||
      ns_http_parse_header(hdr, "uri", uri, sizeof(uri)) == 0 ||
      ns_http_parse_header(hdr, "qop", qop, sizeof(qop)) == 0 ||
      ns_http_parse_header(hdr, "nc", nc, sizeof(nc)) == 0 ||
      ns_http_parse_header(hdr, "nonce", nonce, sizeof(nonce)) == 0 ||
      check_nonce(nonce) == 0) {
    return 0;
  }

  /*
   * Read passwords file line by line. If should have htdigest format,
   * i.e. each line should be a colon-separated sequence:
   * USER_NAME:DOMAIN_NAME:HA1_HASH_OF_USER_DOMAIN_AND_PASSWORD
   */
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    if (sscanf(buf, "%[^:]:%[^:]:%s", f_user, f_domain, f_ha1) == 3 &&
        strcmp(user, f_user) == 0 &&
        /* NOTE(lsm): due to a bug in MSIE, we do not compare URIs */
        strcmp(auth_domain, f_domain) == 0) {
      /* User and domain matched, check the password */
      mkmd5resp(hm->method.p, hm->method.len, hm->uri.p, hm->uri.len, f_ha1,
                strlen(f_ha1), nonce, strlen(nonce), nc, strlen(nc), cnonce,
                strlen(cnonce), qop, strlen(qop), expected_response);
      return ns_casecmp(response, expected_response) == 0;
    }
  }

  /* None of the entries in the passwords file matched - return failure */
  return 0;
}

static int is_authorized(struct http_message *hm, const char *path,
                         int is_directory, struct ns_serve_http_opts *opts) {
  FILE *fp;
  int authorized = 1;

  if (opts->auth_domain != NULL && (opts->per_directory_auth_file != NULL ||
                                    opts->global_auth_file != NULL) &&
      (fp = open_auth_file(path, is_directory, opts)) != NULL) {
    authorized = ns_http_check_digest_auth(hm, opts->auth_domain, fp);
    fclose(fp);
  }

  return authorized;
}
#else
static int is_authorized(struct http_message *hm, const char *path,
                         int is_directory, struct ns_serve_http_opts *opts) {
  (void) hm;
  (void) path;
  (void) is_directory;
  (void) opts;
  return 1;
}
#endif

#ifndef NS_DISABLE_DIRECTORY_LISTING
/* Implementation of POSIX opendir/closedir/readdir for Windows. */
#ifdef _WIN32
struct dirent {
  char d_name[MAX_PATH_SIZE];
};

typedef struct DIR {
  HANDLE handle;
  WIN32_FIND_DATAW info;
  struct dirent result;
} DIR;

static DIR *opendir(const char *name) {
  DIR *dir = NULL;
  wchar_t wpath[MAX_PATH_SIZE];
  DWORD attrs;

  if (name == NULL) {
    SetLastError(ERROR_BAD_ARGUMENTS);
  } else if ((dir = (DIR *) NS_MALLOC(sizeof(*dir))) == NULL) {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
  } else {
    to_wchar(name, wpath, ARRAY_SIZE(wpath));
    attrs = GetFileAttributesW(wpath);
    if (attrs != 0xFFFFFFFF && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
      (void) wcscat(wpath, L"\\*");
      dir->handle = FindFirstFileW(wpath, &dir->info);
      dir->result.d_name[0] = '\0';
    } else {
      NS_FREE(dir);
      dir = NULL;
    }
  }

  return dir;
}

static int closedir(DIR *dir) {
  int result = 0;

  if (dir != NULL) {
    if (dir->handle != INVALID_HANDLE_VALUE)
      result = FindClose(dir->handle) ? 0 : -1;

    NS_FREE(dir);
  } else {
    result = -1;
    SetLastError(ERROR_BAD_ARGUMENTS);
  }

  return result;
}

static struct dirent *readdir(DIR *dir) {
  struct dirent *result = 0;

  if (dir) {
    if (dir->handle != INVALID_HANDLE_VALUE) {
      result = &dir->result;
      (void) WideCharToMultiByte(CP_UTF8, 0, dir->info.cFileName, -1,
                                 result->d_name, sizeof(result->d_name), NULL,
                                 NULL);

      if (!FindNextFileW(dir->handle, &dir->info)) {
        (void) FindClose(dir->handle);
        dir->handle = INVALID_HANDLE_VALUE;
      }

    } else {
      SetLastError(ERROR_FILE_NOT_FOUND);
    }
  } else {
    SetLastError(ERROR_BAD_ARGUMENTS);
  }

  return result;
}
#endif /* _WIN32  POSIX opendir/closedir/readdir implementation */

static size_t ns_url_encode(const char *src, size_t s_len, char *dst,
                            size_t dst_len) {
  static const char *dont_escape = "._-$,;~()";
  static const char *hex = "0123456789abcdef";
  size_t i = 0, j = 0;

  for (i = j = 0; dst_len > 0 && i < s_len && j + 2 < dst_len - 1; i++, j++) {
    if (isalnum(*(const unsigned char *) (src + i)) ||
        strchr(dont_escape, *(const unsigned char *) (src + i)) != NULL) {
      dst[j] = src[i];
    } else if (j + 3 < dst_len) {
      dst[j] = '%';
      dst[j + 1] = hex[(*(const unsigned char *) (src + i)) >> 4];
      dst[j + 2] = hex[(*(const unsigned char *) (src + i)) & 0xf];
      j += 2;
    }
  }

  dst[j] = '\0';
  return j;
}

static void escape(const char *src, char *dst, size_t dst_len) {
  size_t n = 0;
  while (*src != '\0' && n + 5 < dst_len) {
    unsigned char ch = *(unsigned char *) src++;
    if (ch == '<') {
      n += snprintf(dst + n, dst_len - n, "%s", "&lt;");
    } else {
      dst[n++] = ch;
    }
  }
  dst[n] = '\0';
}

static void print_dir_entry(struct ns_connection *nc, const char *file_name,
                            ns_stat_t *stp) {
  char size[64], mod[64], href[MAX_PATH_SIZE * 3], path[MAX_PATH_SIZE];
  int64_t fsize = stp->st_size;
  int is_dir = S_ISDIR(stp->st_mode);
  const char *slash = is_dir ? "/" : "";

  if (is_dir) {
    snprintf(size, sizeof(size), "%s", "[DIRECTORY]");
  } else {
    /*
     * We use (double) cast below because MSVC 6 compiler cannot
     * convert unsigned __int64 to double.
     */
    if (fsize < 1024) {
      snprintf(size, sizeof(size), "%d", (int) fsize);
    } else if (fsize < 0x100000) {
      snprintf(size, sizeof(size), "%.1fk", (double) fsize / 1024.0);
    } else if (fsize < 0x40000000) {
      snprintf(size, sizeof(size), "%.1fM", (double) fsize / 1048576);
    } else {
      snprintf(size, sizeof(size), "%.1fG", (double) fsize / 1073741824);
    }
  }
  strftime(mod, sizeof(mod), "%d-%b-%Y %H:%M", localtime(&stp->st_mtime));
  escape(file_name, path, sizeof(path));
  ns_url_encode(file_name, strlen(file_name), href, sizeof(href));
  ns_printf_http_chunk(nc,
                       "<tr><td><a href=\"%s%s\">%s%s</a></td>"
                       "<td>%s</td><td name=%" INT64_FMT ">%s</td></tr>\n",
                       href, slash, path, slash, mod, is_dir ? -1 : fsize,
                       size);
}

static void scan_directory(struct ns_connection *nc, const char *dir,
                           const struct ns_serve_http_opts *opts,
                           void (*func)(struct ns_connection *, const char *,
                                        ns_stat_t *)) {
  char path[MAX_PATH_SIZE];
  ns_stat_t st;
  struct dirent *dp;
  DIR *dirp;

  if ((dirp = (opendir(dir))) != NULL) {
    while ((dp = readdir(dirp)) != NULL) {
      /* Do not show current dir and hidden files */
      if (is_file_hidden(dp->d_name, opts)) {
        continue;
      }
      snprintf(path, sizeof(path), "%s/%s", dir, dp->d_name);
      if (ns_stat(path, &st) == 0) {
        func(nc, dp->d_name, &st);
      }
    }
    closedir(dirp);
  }
}

static void send_directory_listing(struct ns_connection *nc, const char *dir,
                                   struct http_message *hm,
                                   struct ns_serve_http_opts *opts) {
  static const char *sort_js_code =
      "<script>function srt(tb, col) {"
      "var tr = Array.prototype.slice.call(tb.rows, 0),"
      "tr = tr.sort(function (a, b) { var c1 = a.cells[col], c2 = b.cells[col],"
      "n1 = c1.getAttribute('name'), n2 = c2.getAttribute('name'), "
      "t1 = a.cells[2].getAttribute('name'), "
      "t2 = b.cells[2].getAttribute('name'); "
      "return t1 < 0 && t2 >= 0 ? -1 : t2 < 0 && t1 >= 0 ? 1 : "
      "n1 ? parseInt(n2) - parseInt(n1) : "
      "c1.textContent.trim().localeCompare(c2.textContent.trim()); });";
  static const char *sort_js_code2 =
      "for (var i = 0; i < tr.length; i++) tb.appendChild(tr[i]);}"
      "window.onload = function() { "
      "var tb = document.getElementById('tb');"
      "document.onclick = function(ev){ "
      "var c = ev.target.rel; if (c) srt(tb, c)}; srt(tb, 2); };</script>";

  ns_printf(nc, "%s\r\n%s: %s\r\n%s: %s\r\n\r\n", "HTTP/1.1 200 OK",
            "Transfer-Encoding", "chunked", "Content-Type",
            "text/html; charset=utf-8");

  ns_printf_http_chunk(
      nc,
      "<html><head><title>Index of %.*s</title>%s%s"
      "<style>th,td {text-align: left; padding-right: 1em; }</style></head>"
      "<body><h1>Index of %.*s</h1><pre><table cellpadding=\"0\"><thead>"
      "<tr><th><a href=# rel=0>Name</a></th><th>"
      "<a href=# rel=1>Modified</a</th>"
      "<th><a href=# rel=2>Size</a></th></tr>"
      "<tr><td colspan=\"3\"><hr></td></tr></thead><tbody id=tb>",
      (int) hm->uri.len, hm->uri.p, sort_js_code, sort_js_code2,
      (int) hm->uri.len, hm->uri.p);
  scan_directory(nc, dir, opts, print_dir_entry);
  ns_printf_http_chunk(nc, "%s", "</tbody></body></html>");
  ns_send_http_chunk(nc, "", 0);
}
#endif /* NS_DISABLE_DIRECTORY_LISTING */

#ifndef NS_DISABLE_DAV
static void print_props(struct ns_connection *nc, const char *name,
                        ns_stat_t *stp) {
  char mtime[64], buf[MAX_PATH_SIZE * 3];
  time_t t = stp->st_mtime; /* store in local variable for NDK compile */
  gmt_time_string(mtime, sizeof(mtime), &t);
  ns_url_encode(name, strlen(name), buf, sizeof(buf));
  ns_printf(nc,
            "<d:response>"
            "<d:href>%s</d:href>"
            "<d:propstat>"
            "<d:prop>"
            "<d:resourcetype>%s</d:resourcetype>"
            "<d:getcontentlength>%" INT64_FMT
            "</d:getcontentlength>"
            "<d:getlastmodified>%s</d:getlastmodified>"
            "</d:prop>"
            "<d:status>HTTP/1.1 200 OK</d:status>"
            "</d:propstat>"
            "</d:response>\n",
            buf, S_ISDIR(stp->st_mode) ? "<d:collection/>" : "",
            (int64_t) stp->st_size, mtime);
}

static void handle_propfind(struct ns_connection *nc, const char *path,
                            ns_stat_t *stp, struct http_message *hm,
                            struct ns_serve_http_opts *opts) {
  static const char header[] =
      "HTTP/1.1 207 Multi-Status\r\n"
      "Connection: close\r\n"
      "Content-Type: text/xml; charset=utf-8\r\n\r\n"
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
      "<d:multistatus xmlns:d='DAV:'>\n";
  static const char footer[] = "</d:multistatus>\n";
  const struct ns_str *depth = ns_get_http_header(hm, "Depth");

  /* Print properties for the requested resource itself */
  if (S_ISDIR(stp->st_mode) &&
      strcmp(opts->enable_directory_listing, "yes") != 0) {
    ns_printf(nc, "%s", "HTTP/1.1 403 Directory Listing Denied\r\n\r\n");
  } else {
    char uri[MAX_PATH_SIZE];
    ns_send(nc, header, sizeof(header) - 1);
    snprintf(uri, sizeof(uri), "%.*s", (int) hm->uri.len, hm->uri.p);
    print_props(nc, uri, stp);
    if (S_ISDIR(stp->st_mode) && (depth == NULL || ns_vcmp(depth, "0") != 0)) {
      scan_directory(nc, path, opts, print_props);
    }
    ns_send(nc, footer, sizeof(footer) - 1);
    nc->flags |= NSF_SEND_AND_CLOSE;
  }
}

static void handle_mkcol(struct ns_connection *nc, const char *path,
                         struct http_message *hm) {
  int status_code = 500;
  if (ns_get_http_header(hm, "Content-Length") != NULL) {
    status_code = 415;
  } else if (!mkdir(path, 0755)) {
    status_code = 201;
  } else if (errno == EEXIST) {
    status_code = 405;
  } else if (errno == EACCES) {
    status_code = 403;
  } else if (errno == ENOENT) {
    status_code = 409;
  }
  send_http_error(nc, status_code, NULL);
}

static int remove_directory(const char *dir) {
  char path[MAX_PATH_SIZE];
  struct dirent *dp;
  ns_stat_t st;
  DIR *dirp;

  if ((dirp = opendir(dir)) == NULL) return 0;

  while ((dp = readdir(dirp)) != NULL) {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) continue;
    snprintf(path, sizeof(path), "%s%c%s", dir, '/', dp->d_name);
    ns_stat(path, &st);
    if (S_ISDIR(st.st_mode)) {
      remove_directory(path);
    } else {
      remove(path);
    }
  }
  closedir(dirp);
  rmdir(dir);

  return 1;
}

static void handle_delete(struct ns_connection *nc, const char *path) {
  ns_stat_t st;
  if (ns_stat(path, &st) != 0) {
    send_http_error(nc, 404, NULL);
  } else if (S_ISDIR(st.st_mode)) {
    remove_directory(path);
    send_http_error(nc, 204, NULL);
  } else if (remove(path) == 0) {
    send_http_error(nc, 204, NULL);
  } else {
    send_http_error(nc, 423, NULL);
  }
}

/* Return -1 on error, 1 on success. */
static int create_itermediate_directories(const char *path) {
  const char *s = path;

  /* Create intermediate directories if they do not exist */
  while (*s) {
    if (*s == '/') {
      char buf[MAX_PATH_SIZE];
      ns_stat_t st;
      snprintf(buf, sizeof(buf), "%.*s", (int) (s - path), path);
      buf[sizeof(buf) - 1] = '\0';
      if (ns_stat(buf, &st) != 0 && mkdir(buf, 0755) != 0) {
        return -1;
      }
    }
    s++;
  }

  return 1;
}

static void handle_put(struct ns_connection *nc, const char *path,
                       struct http_message *hm) {
  ns_stat_t st;
  const struct ns_str *cl_hdr = ns_get_http_header(hm, "Content-Length");
  int rc, status_code = ns_stat(path, &st) == 0 ? 200 : 201;
  struct proto_data_http *dp = (struct proto_data_http *) nc->proto_data;

  free_http_proto_data(nc);
  if ((rc = create_itermediate_directories(path)) == 0) {
    ns_printf(nc, "HTTP/1.1 %d OK\r\nContent-Length: 0\r\n\r\n", status_code);
  } else if (rc == -1) {
    send_http_error(nc, 500, NULL);
  } else if (cl_hdr == NULL) {
    send_http_error(nc, 411, NULL);
  } else if ((dp = (struct proto_data_http *) NS_CALLOC(1, sizeof(*dp))) ==
             NULL) {
    send_http_error(nc, 500, NULL); /* LCOV_EXCL_LINE */
  } else if ((dp->fp = fopen(path, "w+b")) == NULL) {
    send_http_error(nc, 500, NULL);
    free_http_proto_data(nc);
  } else {
    const struct ns_str *range_hdr = ns_get_http_header(hm, "Content-Range");
    int64_t r1 = 0, r2 = 0;
    dp->type = DATA_PUT;
    ns_set_close_on_exec(fileno(dp->fp));
    dp->cl = to64(cl_hdr->p);
    if (range_hdr != NULL && parse_range_header(range_hdr, &r1, &r2) > 0) {
      status_code = 206;
      fseeko(dp->fp, r1, SEEK_SET);
      dp->cl = r2 > r1 ? r2 - r1 + 1 : dp->cl - r1;
    }
    ns_printf(nc, "HTTP/1.1 %d OK\r\nContent-Length: 0\r\n\r\n", status_code);
    nc->proto_data = dp;
    /* Remove HTTP request from the mbuf, leave only payload */
    mbuf_remove(&nc->recv_mbuf, hm->message.len - hm->body.len);
    transfer_file_data(nc);
  }
}
#endif /* NS_DISABLE_DAV */

static int is_dav_request(const struct ns_str *s) {
  return !ns_vcmp(s, "PUT") || !ns_vcmp(s, "DELETE") || !ns_vcmp(s, "MKCOL") ||
         !ns_vcmp(s, "PROPFIND");
}

static int find_index_file(char *path, size_t path_len, const char *list,
                           ns_stat_t *stp) {
  ns_stat_t st;
  size_t n = strlen(path);
  struct ns_str vec;
  int found = 0;

  /* The 'path' given to us points to the directory. Remove all trailing */
  /* directory separator characters from the end of the path, and */
  /* then append single directory separator character. */
  while (n > 0 && (path[n - 1] == '/' || path[n - 1] == '\\')) {
    n--;
  }

  /* Traverse index files list. For each entry, append it to the given */
  /* path and see if the file exists. If it exists, break the loop */
  while ((list = ns_next_comma_list_entry(list, &vec, NULL)) != NULL) {
    /* Prepare full path to the index file */
    snprintf(path + n, path_len - n, "/%.*s", (int) vec.len, vec.p);
    path[path_len - 1] = '\0';

    /* Does it exist? */
    if (!ns_stat(path, &st)) {
      /* Yes it does, break the loop */
      *stp = st;
      found = 1;
      break;
    }
  }

  /* If no index file exists, restore directory path */
  if (!found) {
    path[n] = '\0';
  }

  return found;
}

static void uri_to_path(struct http_message *hm, char *buf, size_t buf_len,
                        const struct ns_serve_http_opts *opts) {
  char uri[NS_MAX_PATH];
  struct ns_str a, b, *host_hdr = ns_get_http_header(hm, "Host");
  const char *rewrites = opts->url_rewrites;

  ns_url_decode(hm->uri.p, hm->uri.len, uri, sizeof(uri), 0);
  remove_double_dots(uri);
  snprintf(buf, buf_len, "%s%s", opts->document_root, uri);

#ifndef NS_DISABLE_DAV
  if (is_dav_request(&hm->method) && opts->dav_document_root != NULL) {
    snprintf(buf, buf_len, "%s%s", opts->dav_document_root, uri);
  }
#endif

  /* Handle URL rewrites */
  while ((rewrites = ns_next_comma_list_entry(rewrites, &a, &b)) != NULL) {
    if (a.len > 1 && a.p[0] == '@' && host_hdr != NULL &&
        host_hdr->len == a.len - 1 &&
        ns_ncasecmp(a.p + 1, host_hdr->p, a.len - 1) == 0) {
      /* This is a virtual host rewrite: @domain.name=document_root_dir */
      snprintf(buf, buf_len, "%.*s%s", (int) b.len, b.p, uri);
      break;
    } else {
      /* This is a usual rewrite, URI=directory */
      int match_len = ns_match_prefix(a.p, a.len, uri);
      if (match_len > 0) {
        snprintf(buf, buf_len, "%.*s%s", (int) b.len, b.p, uri + match_len);
        break;
      }
    }
  }
}

#ifndef NS_DISABLE_CGI
#ifdef _WIN32
struct threadparam {
  sock_t s;
  HANDLE hPipe;
};

static int wait_until_ready(sock_t sock, int for_read) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(sock, &set);
  return select(sock + 1, for_read ? &set : 0, for_read ? 0 : &set, 0, 0) == 1;
}

static void *push_to_stdin(void *arg) {
  struct threadparam *tp = (struct threadparam *) arg;
  int n, sent, stop = 0;
  DWORD k;
  char buf[BUFSIZ];

  while (!stop && wait_until_ready(tp->s, 1) &&
         (n = recv(tp->s, buf, sizeof(buf), 0)) > 0) {
    if (n == -1 && GetLastError() == WSAEWOULDBLOCK) continue;
    for (sent = 0; !stop && sent < n; sent += k) {
      if (!WriteFile(tp->hPipe, buf + sent, n - sent, &k, 0)) stop = 1;
    }
  }
  DBG(("%s", "FORWARED EVERYTHING TO CGI"));
  CloseHandle(tp->hPipe);
  NS_FREE(tp);
  _endthread();
  return NULL;
}

static void *pull_from_stdout(void *arg) {
  struct threadparam *tp = (struct threadparam *) arg;
  int k = 0, stop = 0;
  DWORD n, sent;
  char buf[BUFSIZ];

  while (!stop && ReadFile(tp->hPipe, buf, sizeof(buf), &n, NULL)) {
    for (sent = 0; !stop && sent < n; sent += k) {
      if (wait_until_ready(tp->s, 0) &&
          (k = send(tp->s, buf + sent, n - sent, 0)) <= 0)
        stop = 1;
    }
  }
  DBG(("%s", "EOF FROM CGI"));
  CloseHandle(tp->hPipe);
  shutdown(tp->s, 2);  // Without this, IO thread may get truncated data
  closesocket(tp->s);
  NS_FREE(tp);
  _endthread();
  return NULL;
}

static void spawn_stdio_thread(sock_t sock, HANDLE hPipe,
                               void *(*func)(void *)) {
  struct threadparam *tp = (struct threadparam *) NS_MALLOC(sizeof(*tp));
  if (tp != NULL) {
    tp->s = sock;
    tp->hPipe = hPipe;
    ns_start_thread(func, tp);
  }
}

static void abs_path(const char *utf8_path, char *abs_path, size_t len) {
  wchar_t buf[MAX_PATH_SIZE], buf2[MAX_PATH_SIZE];
  to_wchar(utf8_path, buf, ARRAY_SIZE(buf));
  GetFullPathNameW(buf, ARRAY_SIZE(buf2), buf2, NULL);
  WideCharToMultiByte(CP_UTF8, 0, buf2, wcslen(buf2) + 1, abs_path, len, 0, 0);
}

static pid_t start_process(const char *interp, const char *cmd, const char *env,
                           const char *envp[], const char *dir, sock_t sock) {
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  HANDLE a[2], b[2], me = GetCurrentProcess();
  wchar_t wcmd[MAX_PATH_SIZE], full_dir[MAX_PATH_SIZE];
  char buf[MAX_PATH_SIZE], buf2[MAX_PATH_SIZE], buf5[MAX_PATH_SIZE],
      buf4[MAX_PATH_SIZE], cmdline[MAX_PATH_SIZE];
  DWORD flags = DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS;
  FILE *fp;

  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));

  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

  CreatePipe(&a[0], &a[1], NULL, 0);
  CreatePipe(&b[0], &b[1], NULL, 0);
  DuplicateHandle(me, a[0], me, &si.hStdInput, 0, TRUE, flags);
  DuplicateHandle(me, b[1], me, &si.hStdOutput, 0, TRUE, flags);

  if (interp == NULL && (fp = fopen(cmd, "r")) != NULL) {
    buf[0] = buf[1] = '\0';
    fgets(buf, sizeof(buf), fp);
    buf[sizeof(buf) - 1] = '\0';
    if (buf[0] == '#' && buf[1] == '!') {
      interp = buf + 2;
      /* Trim leading spaces: https://github.com/cesanta/mongoose/issues/489 */
      while (*interp != '\0' && isspace(*(unsigned char *) interp)) {
        interp++;
      }
    }
    fclose(fp);
  }

  snprintf(buf, sizeof(buf), "%s/%s", dir, cmd);
  abs_path(buf, buf2, ARRAY_SIZE(buf2));

  abs_path(dir, buf5, ARRAY_SIZE(buf5));
  to_wchar(dir, full_dir, ARRAY_SIZE(full_dir));

  if (interp != NULL) {
    abs_path(interp, buf4, ARRAY_SIZE(buf4));
    snprintf(cmdline, sizeof(cmdline), "%s \"%s\"", buf4, buf2);
  } else {
    snprintf(cmdline, sizeof(cmdline), "\"%s\"", buf2);
  }
  to_wchar(cmdline, wcmd, ARRAY_SIZE(wcmd));

#if 0
  printf("[%ls] [%ls]\n", full_dir, wcmd);
#endif

  if (CreateProcessW(NULL, wcmd, NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP,
                     (void *) env, full_dir, &si, &pi) != 0) {
    spawn_stdio_thread(sock, a[1], push_to_stdin);
    spawn_stdio_thread(sock, b[0], pull_from_stdout);
  } else {
    CloseHandle(a[1]);
    CloseHandle(b[0]);
    closesocket(sock);
  }
  DBG(("CGI command: [%ls] -> %p", wcmd, pi.hProcess));

  /* Not closing a[0] and b[1] because we've used DUPLICATE_CLOSE_SOURCE */
  CloseHandle(si.hStdOutput);
  CloseHandle(si.hStdInput);
  /* TODO(lsm): check if we need close process and thread handles too */
  /* CloseHandle(pi.hThread); */
  /* CloseHandle(pi.hProcess); */

  return pi.hProcess;
}
#else
static pid_t start_process(const char *interp, const char *cmd, const char *env,
                           const char *envp[], const char *dir, sock_t sock) {
  char buf[500];
  pid_t pid = fork();
  (void) env;

  if (pid == 0) {
    (void) chdir(dir);
    (void) dup2(sock, 0);
    (void) dup2(sock, 1);
    closesocket(sock);

    /*
     * After exec, all signal handlers are restored to their default values,
     * with one exception of SIGCHLD. According to POSIX.1-2001 and Linux's
     * implementation, SIGCHLD's handler will leave unchanged after exec
     * if it was set to be ignored. Restore it to default action.
     */
    signal(SIGCHLD, SIG_DFL);

    if (interp == NULL) {
      execle(cmd, cmd, (char *) 0, envp); /* (char *) 0 to squash warning */
    } else {
      execle(interp, interp, cmd, (char *) 0, envp);
    }
    snprintf(buf, sizeof(buf),
             "Status: 500\r\n\r\n"
             "500 Server Error: %s%s%s: %s",
             interp == NULL ? "" : interp, interp == NULL ? "" : " ", cmd,
             strerror(errno));
    send(1, buf, strlen(buf), 0);
    exit(EXIT_FAILURE); /* exec call failed */
  }

  return pid;
}
#endif /* _WIN32 */

/*
 * Append VARIABLE=VALUE\0 string to the buffer, and add a respective
 * pointer into the vars array.
 */
static char *addenv(struct cgi_env_block *block, const char *fmt, ...) {
  int n, space;
  char *added = block->buf + block->len;
  va_list ap;

  /* Calculate how much space is left in the buffer */
  space = sizeof(block->buf) - (block->len + 2);
  if (space > 0) {
    /* Copy VARIABLE=VALUE\0 string into the free space */
    va_start(ap, fmt);
    n = vsnprintf(added, (size_t) space, fmt, ap);
    va_end(ap);

    /* Make sure we do not overflow buffer and the envp array */
    if (n > 0 && n + 1 < space &&
        block->nvars < (int) ARRAY_SIZE(block->vars) - 2) {
      /* Append a pointer to the added string into the envp array */
      block->vars[block->nvars++] = added;
      /* Bump up used length counter. Include \0 terminator */
      block->len += n + 1;
    }
  }

  return added;
}

static void addenv2(struct cgi_env_block *blk, const char *name) {
  const char *s;
  if ((s = getenv(name)) != NULL) addenv(blk, "%s=%s", name, s);
}

static void prepare_cgi_environment(struct ns_connection *nc, const char *prog,
                                    const struct http_message *hm,
                                    const struct ns_serve_http_opts *opts,
                                    struct cgi_env_block *blk) {
  const char *s, *slash;
  struct ns_str *h;
  char *p;
  size_t i;

  blk->len = blk->nvars = 0;
  blk->nc = nc;

  if ((s = getenv("SERVER_NAME")) != NULL) {
    addenv(blk, "SERVER_NAME=%s", s);
  } else {
    char buf[100];
    ns_sock_to_str(nc->sock, buf, sizeof(buf), 3);
    addenv(blk, "SERVER_NAME=%s", buf);
  }
  addenv(blk, "SERVER_ROOT=%s", opts->document_root);
  addenv(blk, "DOCUMENT_ROOT=%s", opts->document_root);
  addenv(blk, "SERVER_SOFTWARE=%s/%s", "Fossa", NS_FOSSA_VERSION);

  /* Prepare the environment block */
  addenv(blk, "%s", "GATEWAY_INTERFACE=CGI/1.1");
  addenv(blk, "%s", "SERVER_PROTOCOL=HTTP/1.1");
  addenv(blk, "%s", "REDIRECT_STATUS=200"); /* For PHP */

  /* TODO(lsm): fix this for IPv6 case */
  /*addenv(blk, "SERVER_PORT=%d", ri->remote_port); */

  addenv(blk, "REQUEST_METHOD=%.*s", (int) hm->method.len, hm->method.p);
#if 0
  addenv(blk, "REMOTE_ADDR=%s", ri->remote_ip);
  addenv(blk, "REMOTE_PORT=%d", ri->remote_port);
#endif
  addenv(blk, "REQUEST_URI=%.*s%s%.*s", (int) hm->uri.len, hm->uri.p,
         hm->query_string.len == 0 ? "" : "?", (int) hm->query_string.len,
         hm->query_string.p);

/* SCRIPT_NAME */
#if 0
  if (nc->path_info != NULL) {
    addenv(blk, "SCRIPT_NAME=%.*s",
           (int) (strlen(ri->uri) - strlen(nc->path_info)), ri->uri);
    addenv(blk, "PATH_INFO=%s", nc->path_info);
  } else {
#endif
  s = strrchr(prog, '/');
  slash = hm->uri.p + hm->uri.len;
  while (slash > hm->uri.p && *slash != '/') {
    slash--;
  }
  addenv(blk, "SCRIPT_NAME=%.*s%s", (int) (slash - hm->uri.p), hm->uri.p,
         s == NULL ? prog : s);
#if 0
  }
#endif

  addenv(blk, "SCRIPT_FILENAME=%s", prog);
  addenv(blk, "PATH_TRANSLATED=%s", prog);
  addenv(blk, "HTTPS=%s", nc->ssl != NULL ? "on" : "off");

  if ((h = ns_get_http_header((struct http_message *) hm, "Content-Type")) !=
      NULL) {
    addenv(blk, "CONTENT_TYPE=%.*s", (int) h->len, h->p);
  }

  if (hm->query_string.len > 0) {
    addenv(blk, "QUERY_STRING=%.*s", (int) hm->query_string.len,
           hm->query_string.p);
  }

  if ((h = ns_get_http_header((struct http_message *) hm, "Content-Length")) !=
      NULL) {
    addenv(blk, "CONTENT_LENGTH=%.*s", (int) h->len, h->p);
  }

  addenv2(blk, "PATH");
  addenv2(blk, "TMP");
  addenv2(blk, "TEMP");
  addenv2(blk, "TMPDIR");
  addenv2(blk, "PERLLIB");
  addenv2(blk, NS_ENV_EXPORT_TO_CGI);

#if defined(_WIN32)
  addenv2(blk, "COMSPEC");
  addenv2(blk, "SYSTEMROOT");
  addenv2(blk, "SystemDrive");
  addenv2(blk, "ProgramFiles");
  addenv2(blk, "ProgramFiles(x86)");
  addenv2(blk, "CommonProgramFiles(x86)");
#else
  addenv2(blk, "LD_LIBRARY_PATH");
#endif /* _WIN32 */

  /* Add all headers as HTTP_* variables */
  for (i = 0; hm->header_names[i].len > 0; i++) {
    p = addenv(blk, "HTTP_%.*s=%.*s", (int) hm->header_names[i].len,
               hm->header_names[i].p, (int) hm->header_values[i].len,
               hm->header_values[i].p);

    /* Convert variable name into uppercase, and change - to _ */
    for (; *p != '=' && *p != '\0'; p++) {
      if (*p == '-') *p = '_';
      *p = (char) toupper(*(unsigned char *) p);
    }
  }

  blk->vars[blk->nvars++] = NULL;
  blk->buf[blk->len++] = '\0';
}

static void cgi_ev_handler(struct ns_connection *cgi_nc, int ev,
                           void *ev_data) {
  struct ns_connection *nc = (struct ns_connection *) cgi_nc->user_data;
  (void) ev_data;

  if (nc == NULL) return;

  switch (ev) {
    case NS_RECV:
      /*
       * CGI script does not output reply line, like "HTTP/1.1 CODE XXXXX\n"
       * It outputs headers, then body. Headers might include "Status"
       * header, which changes CODE, and it might include "Location" header
       * which changes CODE to 302.
       *
       * Therefore we do not send the output from the CGI script to the user
       * until all CGI headers are parsed (by setting NSF_DONT_SEND flag).
       *
       * Here we parse the output from the CGI script, and if all headers has
       * been received, amend the reply line, and clear NSF_DONT_SEND flag,
       * which makes data to be sent to the user.
       */
      if (nc->flags & NSF_USER_1) {
        struct mbuf *io = &cgi_nc->recv_mbuf;
        int len = get_request_len(io->buf, io->len);

        if (len == 0) break;
        if (len < 0 || io->len > NS_MAX_HTTP_REQUEST_SIZE) {
          cgi_nc->flags |= NSF_CLOSE_IMMEDIATELY;
          send_http_error(nc, 500, "Bad headers");
        } else {
          struct http_message hm;
          struct ns_str *h;
          parse_http_headers(io->buf, io->buf + io->len, io->len, &hm);
          /*printf("=== %d [%.*s]\n", k, k, io->buf);*/
          if (ns_get_http_header(&hm, "Location") != NULL) {
            ns_printf(nc, "%s", "HTTP/1.1 302 Moved\r\n");
          } else if ((h = ns_get_http_header(&hm, "Status")) != NULL) {
            ns_printf(nc, "HTTP/1.1 %.*s\r\n", (int) h->len, h->p);
          } else {
            ns_printf(nc, "%s", "HTTP/1.1 200 OK\r\n");
          }
        }
        nc->flags &= ~NSF_USER_1;
      }
      if (!(nc->flags & NSF_USER_1)) {
        ns_forward(cgi_nc, nc);
      }
      break;
    case NS_CLOSE:
      free_http_proto_data(nc);
      nc->flags |= NSF_SEND_AND_CLOSE;
      nc->user_data = NULL;
      break;
  }
}

static void handle_cgi(struct ns_connection *nc, const char *prog,
                       const struct http_message *hm,
                       const struct ns_serve_http_opts *opts) {
  struct proto_data_http *dp;
  struct cgi_env_block blk;
  char dir[MAX_PATH_SIZE];
  const char *p;
  sock_t fds[2];

  prepare_cgi_environment(nc, prog, hm, opts, &blk);
  /*
   * CGI must be executed in its own directory. 'dir' must point to the
   * directory containing executable program, 'p' must point to the
   * executable program name relative to 'dir'.
   */
  if ((p = strrchr(prog, '/')) == NULL) {
    snprintf(dir, sizeof(dir), "%s", ".");
  } else {
    snprintf(dir, sizeof(dir), "%.*s", (int) (p - prog), prog);
    prog = p + 1;
  }

  /*
   * Try to create socketpair in a loop until success. ns_socketpair()
   * can be interrupted by a signal and fail.
   * TODO(lsm): use sigaction to restart interrupted syscall
   */
  do {
    ns_socketpair(fds, SOCK_STREAM);
  } while (fds[0] == INVALID_SOCKET);

  free_http_proto_data(nc);
  if ((dp = (struct proto_data_http *) NS_CALLOC(1, sizeof(*dp))) == NULL) {
    send_http_error(nc, 500, "OOM"); /* LCOV_EXCL_LINE */
  } else if (start_process(opts->cgi_interpreter, prog, blk.buf, blk.vars, dir,
                           fds[1]) != 0) {
    size_t n = nc->recv_mbuf.len - (hm->message.len - hm->body.len);
    dp->type = DATA_CGI;
    dp->cgi_nc = ns_add_sock(nc->mgr, fds[0], cgi_ev_handler);
    dp->cgi_nc->user_data = nc;
    nc->flags |= NSF_USER_1;
    /* Push POST data to the CGI */
    if (n > 0 && n < nc->recv_mbuf.len) {
      ns_send(dp->cgi_nc, hm->body.p, n);
    }
    mbuf_remove(&nc->recv_mbuf, nc->recv_mbuf.len);
  } else {
    closesocket(fds[0]);
    send_http_error(nc, 500, "CGI failure");
  }

#ifndef _WIN32
  closesocket(fds[1]); /* On Windows, CGI stdio thread closes that socket */
#endif
}
#endif

void ns_send_http_file(struct ns_connection *nc, char *path,
                       size_t path_buf_len, struct http_message *hm,
                       struct ns_serve_http_opts *opts) {
  int stat_result, is_directory, is_dav = is_dav_request(&hm->method);
  uint32_t remote_ip = ntohl(*(uint32_t *) &nc->sa.sin.sin_addr);
  ns_stat_t st;

  stat_result = ns_stat(path, &st);
  is_directory = !stat_result && S_ISDIR(st.st_mode);

  if (ns_check_ip_acl(opts->ip_acl, remote_ip) != 1) {
    /* Not allowed to connect */
    nc->flags |= NSF_CLOSE_IMMEDIATELY;
  } else if (is_dav && opts->dav_document_root == NULL) {
    send_http_error(nc, 501, NULL);
  } else if (!is_authorized(hm, path, is_directory, opts)) {
    ns_printf(nc,
              "HTTP/1.1 401 Unauthorized\r\n"
              "WWW-Authenticate: Digest qop=\"auth\", "
              "realm=\"%s\", nonce=\"%lu\"\r\n"
              "Content-Length: 0\r\n\r\n",
              opts->auth_domain, (unsigned long) time(NULL));
  } else if ((stat_result != 0 || is_file_hidden(path, opts)) && !is_dav) {
    ns_printf(nc, "%s", "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
  } else if (is_directory && path[strlen(path) - 1] != '/' && !is_dav) {
    ns_printf(nc,
              "HTTP/1.1 301 Moved\r\nLocation: %.*s/\r\n"
              "Content-Length: 0\r\n\r\n",
              (int) hm->uri.len, hm->uri.p);
#ifndef NS_DISABLE_DAV
  } else if (!ns_vcmp(&hm->method, "PROPFIND")) {
    handle_propfind(nc, path, &st, hm, opts);
  } else if (!ns_vcmp(&hm->method, "MKCOL")) {
    handle_mkcol(nc, path, hm);
  } else if (!ns_vcmp(&hm->method, "DELETE")) {
    handle_delete(nc, path);
  } else if (!ns_vcmp(&hm->method, "PUT")) {
    handle_put(nc, path, hm);
#endif
  } else if (S_ISDIR(st.st_mode) &&
             !find_index_file(path, path_buf_len, opts->index_files, &st)) {
    if (strcmp(opts->enable_directory_listing, "yes") == 0) {
#ifndef NS_DISABLE_DIRECTORY_LISTING
      send_directory_listing(nc, path, hm, opts);
#else
      send_http_error(nc, 501, NULL);
#endif
    } else {
      send_http_error(nc, 403, NULL);
    }
  } else if (ns_match_prefix(opts->cgi_file_pattern,
                             strlen(opts->cgi_file_pattern), path) > 0) {
#if !defined(NS_DISABLE_CGI)
    handle_cgi(nc, path, hm, opts);
#else
    send_http_error(nc, 501, NULL);
#endif /* NS_DISABLE_CGI */
  } else {
    ns_send_http_file2(nc, path, &st, hm, opts);
  }
}

void ns_serve_http(struct ns_connection *nc, struct http_message *hm,
                   struct ns_serve_http_opts opts) {
  char path[NS_MAX_PATH];
  uri_to_path(hm, path, sizeof(path), &opts);
  if (opts.per_directory_auth_file == NULL) {
    opts.per_directory_auth_file = ".htpasswd";
  }
  if (opts.enable_directory_listing == NULL) {
    opts.enable_directory_listing = "yes";
  }
  if (opts.cgi_file_pattern == NULL) {
    opts.cgi_file_pattern = "**.cgi$|**.php$";
  }
  if (opts.index_files == NULL) {
    opts.index_files = "index.html,index.htm,index.shtml,index.cgi,index.php";
  }
  ns_send_http_file(nc, path, sizeof(path), hm, &opts);
}

#endif /* NS_DISABLE_FILESYSTEM */

struct ns_connection *ns_connect_http(struct ns_mgr *mgr,
                                      ns_event_handler_t ev_handler,
                                      const char *url,
                                      const char *extra_headers,
                                      const char *post_data) {
  struct ns_connection *nc;
  char addr[1100], path[4096]; /* NOTE: keep sizes in sync with sscanf below */
  int use_ssl = 0;

  if (memcmp(url, "http://", 7) == 0) {
    url += 7;
  } else if (memcmp(url, "https://", 8) == 0) {
    url += 8;
    use_ssl = 1;
#ifndef NS_ENABLE_SSL
    return NULL; /* SSL is not enabled, cannot do HTTPS URLs */
#endif
  }

  addr[0] = path[0] = '\0';

  /* addr buffer size made smaller to allow for port to be prepended */
  sscanf(url, "%1095[^/]/%4095s", addr, path);
  if (strchr(addr, ':') == NULL) {
    strncat(addr, use_ssl ? ":443" : ":80", sizeof(addr) - (strlen(addr) + 1));
  }

  if ((nc = ns_connect(mgr, addr, ev_handler)) != NULL) {
    ns_set_protocol_http_websocket(nc);

    if (use_ssl) {
#ifdef NS_ENABLE_SSL
      ns_set_ssl(nc, NULL, NULL);
#endif
    }

    ns_printf(nc,
              "%s /%s HTTP/1.1\r\nHost: %s\r\nContent-Length: %lu\r\n%s\r\n%s",
              post_data == NULL ? "GET" : "POST", path, addr,
              post_data == NULL ? 0 : strlen(post_data),
              extra_headers == NULL ? "" : extra_headers,
              post_data == NULL ? "" : post_data);
  }

  return nc;
}

#endif /* NS_DISABLE_HTTP_WEBSOCKET */
#ifdef NS_MODULE_LINES
#line 1 "src/util.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */


const char *ns_skip(const char *s, const char *end, const char *delims,
                    struct ns_str *v) {
  v->p = s;
  while (s < end && strchr(delims, *(unsigned char *) s) == NULL) s++;
  v->len = s - v->p;
  while (s < end && strchr(delims, *(unsigned char *) s) != NULL) s++;
  return s;
}

static int lowercase(const char *s) {
  return tolower(*(const unsigned char *) s);
}

int ns_ncasecmp(const char *s1, const char *s2, size_t len) {
  int diff = 0;

  if (len > 0) do {
      diff = lowercase(s1++) - lowercase(s2++);
    } while (diff == 0 && s1[-1] != '\0' && --len > 0);

  return diff;
}

int ns_casecmp(const char *s1, const char *s2) {
  return ns_ncasecmp(s1, s2, (size_t) ~0);
}

int ns_vcasecmp(const struct ns_str *str1, const char *str2) {
  size_t n2 = strlen(str2), n1 = str1->len;
  int r = ns_ncasecmp(str1->p, str2, (n1 < n2) ? n1 : n2);
  if (r == 0) {
    return n1 - n2;
  }
  return r;
}

int ns_vcmp(const struct ns_str *str1, const char *str2) {
  size_t n2 = strlen(str2), n1 = str1->len;
  int r = memcmp(str1->p, str2, (n1 < n2) ? n1 : n2);
  if (r == 0) {
    return n1 - n2;
  }
  return r;
}

#ifdef _WIN32
NS_INTERNAL void to_wchar(const char *path, wchar_t *wbuf, size_t wbuf_len) {
  char buf[MAX_PATH_SIZE * 2], buf2[MAX_PATH_SIZE * 2], *p;

  strncpy(buf, path, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';

  /* Trim trailing slashes. Leave backslash for paths like "X:\" */
  p = buf + strlen(buf) - 1;
  while (p > buf && p[-1] != ':' && (p[0] == '\\' || p[0] == '/')) *p-- = '\0';

  /*
   * Convert to Unicode and back. If doubly-converted string does not
   * match the original, something is fishy, reject.
   */
  memset(wbuf, 0, wbuf_len * sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, (int) wbuf_len);
  WideCharToMultiByte(CP_UTF8, 0, wbuf, (int) wbuf_len, buf2, sizeof(buf2),
                      NULL, NULL);
  if (strcmp(buf, buf2) != 0) {
    wbuf[0] = L'\0';
  }
}
#endif /* _WIN32 */

#ifndef NS_DISABLE_FILESYSTEM
int ns_stat(const char *path, ns_stat_t *st) {
#ifdef _WIN32
  wchar_t wpath[MAX_PATH_SIZE];
  to_wchar(path, wpath, ARRAY_SIZE(wpath));
  DBG(("[%ls] -> %d", wpath, _wstati64(wpath, st)));
  return _wstati64(wpath, st);
#else
  return stat(path, st);
#endif
}

FILE *ns_fopen(const char *path, const char *mode) {
#ifdef _WIN32
  wchar_t wpath[MAX_PATH_SIZE], wmode[10];
  to_wchar(path, wpath, ARRAY_SIZE(wpath));
  to_wchar(mode, wmode, ARRAY_SIZE(wmode));
  return _wfopen(wpath, wmode);
#else
  return fopen(path, mode);
#endif
}

int ns_open(const char *path, int flag, int mode) { /* LCOV_EXCL_LINE */
#ifdef _WIN32
  wchar_t wpath[MAX_PATH_SIZE];
  to_wchar(path, wpath, ARRAY_SIZE(wpath));
  return _wopen(wpath, flag, mode);
#else
  return open(path, flag, mode); /* LCOV_EXCL_LINE */
#endif
}
#endif

void ns_base64_encode(const unsigned char *src, int src_len, char *dst) {
  base64_encode(src, src_len, dst);
}

int ns_base64_decode(const unsigned char *s, int len, char *dst) {
  return base64_decode(s, len, dst);
}

#ifdef NS_ENABLE_THREADS
void *ns_start_thread(void *(*f)(void *), void *p) {
#ifdef _WIN32
  return (void *) _beginthread((void(__cdecl *) (void *) ) f, 0, p);
#else
  pthread_t thread_id = (pthread_t) 0;
  pthread_attr_t attr;

  (void) pthread_attr_init(&attr);
  (void) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(NS_STACK_SIZE) && NS_STACK_SIZE > 1
  (void) pthread_attr_setstacksize(&attr, NS_STACK_SIZE);
#endif

  pthread_create(&thread_id, &attr, f, p);
  pthread_attr_destroy(&attr);

  return (void *) thread_id;
#endif
}
#endif /* NS_ENABLE_THREADS */

/* Set close-on-exec bit for a given socket. */
void ns_set_close_on_exec(sock_t sock) {
#ifdef _WIN32
  (void) SetHandleInformation((HANDLE) sock, HANDLE_FLAG_INHERIT, 0);
#else
  fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif
}

void ns_sock_to_str(sock_t sock, char *buf, size_t len, int flags) {
  union socket_address sa;
  socklen_t slen = sizeof(sa);

  memset(&sa, 0, sizeof(sa));
  if (flags & NS_SOCK_STRINGIFY_REMOTE) {
    getpeername(sock, &sa.sa, &slen);
  } else {
    getsockname(sock, &sa.sa, &slen);
  }

  ns_sock_addr_to_str(&sa, buf, len, flags);
}

void ns_sock_addr_to_str(const union socket_address *sa, char *buf, size_t len,
                         int flags) {
  int is_v6;
  if (buf == NULL || len <= 0) return;
  buf[0] = '\0';
#if defined(NS_ENABLE_IPV6)
  is_v6 = sa->sa.sa_family == AF_INET6;
#else
  is_v6 = 0;
#endif
  if (flags & NS_SOCK_STRINGIFY_IP) {
#if defined(NS_ENABLE_IPV6)
    const void *addr = NULL;
    char *start = buf;
    socklen_t capacity = len;
    if (!is_v6) {
      addr = &sa->sin.sin_addr;
    } else {
      addr = (void *) &sa->sin6.sin6_addr;
      if (flags & NS_SOCK_STRINGIFY_PORT) {
        *buf = '[';
        start++;
        capacity--;
      }
    }
    if (inet_ntop(sa->sa.sa_family, addr, start, capacity) == NULL) {
      *buf = '\0';
    }
#elif defined(_WIN32)
    /* Only Windoze Vista (and newer) have inet_ntop() */
    strncpy(buf, inet_ntoa(sa->sin.sin_addr), len);
#else
    inet_ntop(AF_INET, (void *) &sa->sin.sin_addr, buf, len);
#endif
  }
  if (flags & NS_SOCK_STRINGIFY_PORT) {
    int port = ntohs(sa->sin.sin_port);
    if (flags & NS_SOCK_STRINGIFY_IP) {
      snprintf(buf + strlen(buf), len - (strlen(buf) + 1), "%s:%d",
               (is_v6 ? "]" : ""), port);
    } else {
      snprintf(buf, len, "%d", port);
    }
  }
}

int ns_hexdump(const void *buf, int len, char *dst, int dst_len) {
  const unsigned char *p = (const unsigned char *) buf;
  char ascii[17] = "";
  int i, idx, n = 0;

  for (i = 0; i < len; i++) {
    idx = i % 16;
    if (idx == 0) {
      if (i > 0) n += snprintf(dst + n, dst_len - n, "  %s\n", ascii);
      n += snprintf(dst + n, dst_len - n, "%04x ", i);
    }
    n += snprintf(dst + n, dst_len - n, " %02x", p[i]);
    ascii[idx] = p[i] < 0x20 || p[i] > 0x7e ? '.' : p[i];
    ascii[idx + 1] = '\0';
  }

  while (i++ % 16) n += snprintf(dst + n, dst_len - n, "%s", "   ");
  n += snprintf(dst + n, dst_len - n, "  %s\n\n", ascii);

  return n;
}

int ns_avprintf(char **buf, size_t size, const char *fmt, va_list ap) {
  va_list ap_copy;
  int len;

  va_copy(ap_copy, ap);
  len = vsnprintf(*buf, size, fmt, ap_copy);
  va_end(ap_copy);

  if (len < 0) {
    /* eCos and Windows are not standard-compliant and return -1 when
     * the buffer is too small. Keep allocating larger buffers until we
     * succeed or out of memory. */
    *buf = NULL; /* LCOV_EXCL_START */
    while (len < 0) {
      NS_FREE(*buf);
      size *= 2;
      if ((*buf = (char *) NS_MALLOC(size)) == NULL) break;
      va_copy(ap_copy, ap);
      len = vsnprintf(*buf, size, fmt, ap_copy);
      va_end(ap_copy);
    }
    /* LCOV_EXCL_STOP */
  } else if (len > (int) size) {
    /* Standard-compliant code path. Allocate a buffer that is large enough. */
    if ((*buf = (char *) NS_MALLOC(len + 1)) == NULL) {
      len = -1; /* LCOV_EXCL_LINE */
    } else {    /* LCOV_EXCL_LINE */
      va_copy(ap_copy, ap);
      len = vsnprintf(*buf, len + 1, fmt, ap_copy);
      va_end(ap_copy);
    }
  }

  return len;
}

#ifndef NS_DISABLE_FILESYSTEM
void ns_hexdump_connection(struct ns_connection *nc, const char *path,
                           int num_bytes, int ev) {
  const struct mbuf *io = ev == NS_SEND ? &nc->send_mbuf : &nc->recv_mbuf;
  FILE *fp;
  char *buf, src[60], dst[60];
  int buf_size = num_bytes * 5 + 100;

  if ((fp = fopen(path, "a")) != NULL) {
    ns_sock_to_str(nc->sock, src, sizeof(src), 3);
    ns_sock_to_str(nc->sock, dst, sizeof(dst), 7);
    fprintf(fp, "%lu %p %s %s %s %d\n", (unsigned long) time(NULL),
            nc->user_data, src,
            ev == NS_RECV ? "<-" : ev == NS_SEND ? "->" : ev == NS_ACCEPT
                                                              ? "<A"
                                                              : ev == NS_CONNECT
                                                                    ? "C>"
                                                                    : "XX",
            dst, num_bytes);
    if (num_bytes > 0 && (buf = (char *) NS_MALLOC(buf_size)) != NULL) {
      ns_hexdump(io->buf + (ev == NS_SEND ? 0 : io->len) -
                     (ev == NS_SEND ? 0 : num_bytes),
                 num_bytes, buf, buf_size);
      fprintf(fp, "%s", buf);
      NS_FREE(buf);
    }
    fclose(fp);
  }
}
#endif

int ns_is_big_endian(void) {
  static const int n = 1;
  /* TODO(mkm) use compiletime check with 4-byte char literal */
  return ((char *) &n)[0] == 0;
}

const char *ns_next_comma_list_entry(const char *list, struct ns_str *val,
                                     struct ns_str *eq_val) {
  if (list == NULL || *list == '\0') {
    /* End of the list */
    list = NULL;
  } else {
    val->p = list;
    if ((list = strchr(val->p, ',')) != NULL) {
      /* Comma found. Store length and shift the list ptr */
      val->len = list - val->p;
      list++;
    } else {
      /* This value is the last one */
      list = val->p + strlen(val->p);
      val->len = list - val->p;
    }

    if (eq_val != NULL) {
      /* Value has form "x=y", adjust pointers and lengths */
      /* so that val points to "x", and eq_val points to "y". */
      eq_val->len = 0;
      eq_val->p = (const char *) memchr(val->p, '=', val->len);
      if (eq_val->p != NULL) {
        eq_val->p++; /* Skip over '=' character */
        eq_val->len = val->p + val->len - eq_val->p;
        val->len = (eq_val->p - val->p) - 1;
      }
    }
  }

  return list;
}

int ns_match_prefix(const char *pattern, int pattern_len, const char *str) {
  const char *or_str;
  int len, res, i = 0, j = 0;

  if ((or_str = (const char *) memchr(pattern, '|', pattern_len)) != NULL) {
    res = ns_match_prefix(pattern, or_str - pattern, str);
    return res > 0 ? res : ns_match_prefix(
                               or_str + 1,
                               (pattern + pattern_len) - (or_str + 1), str);
  }

  for (; i < pattern_len; i++, j++) {
    if (pattern[i] == '?' && str[j] != '\0') {
      continue;
    } else if (pattern[i] == '$') {
      return str[j] == '\0' ? j : -1;
    } else if (pattern[i] == '*') {
      i++;
      if (pattern[i] == '*') {
        i++;
        len = (int) strlen(str + j);
      } else {
        len = (int) strcspn(str + j, "/");
      }
      if (i == pattern_len) {
        return j + len;
      }
      do {
        res = ns_match_prefix(pattern + i, pattern_len - i, str + j + len);
      } while (res == -1 && len-- > 0);
      return res == -1 ? -1 : j + res + len;
    } else if (lowercase(&pattern[i]) != lowercase(&str[j])) {
      return -1;
    }
  }
  return j;
}
#ifdef NS_MODULE_LINES
#line 1 "src/json-rpc.c"
/**/
#endif
/* Copyright (c) 2014 Cesanta Software Limited */
/* All rights reserved */

#ifndef NS_DISABLE_JSON_RPC


int ns_rpc_create_reply(char *buf, int len, const struct ns_rpc_request *req,
                        const char *result_fmt, ...) {
  static const struct json_token null_tok = {"null", 4, 0, JSON_TYPE_NULL};
  const struct json_token *id = req->id == NULL ? &null_tok : req->id;
  va_list ap;
  int n = 0;

  n += json_emit(buf + n, len - n, "{s:s,s:", "jsonrpc", "2.0", "id");
  if (id->type == JSON_TYPE_STRING) {
    n += json_emit_quoted_str(buf + n, len - n, id->ptr, id->len);
  } else {
    n += json_emit_unquoted_str(buf + n, len - n, id->ptr, id->len);
  }
  n += json_emit(buf + n, len - n, ",s:", "result");

  va_start(ap, result_fmt);
  n += json_emit_va(buf + n, len - n, result_fmt, ap);
  va_end(ap);

  n += json_emit(buf + n, len - n, "}");

  return n;
}

int ns_rpc_create_request(char *buf, int len, const char *method,
                          const char *id, const char *params_fmt, ...) {
  va_list ap;
  int n = 0;

  n += json_emit(buf + n, len - n, "{s:s,s:s,s:s,s:", "jsonrpc", "2.0", "id",
                 id, "method", method, "params");
  va_start(ap, params_fmt);
  n += json_emit_va(buf + n, len - n, params_fmt, ap);
  va_end(ap);

  n += json_emit(buf + n, len - n, "}");

  return n;
}

int ns_rpc_create_error(char *buf, int len, struct ns_rpc_request *req,
                        int code, const char *message, const char *fmt, ...) {
  va_list ap;
  int n = 0;

  n += json_emit(buf + n, len - n, "{s:s,s:V,s:{s:i,s:s,s:", "jsonrpc", "2.0",
                 "id", req->id == NULL ? "null" : req->id->ptr,
                 req->id == NULL ? 4 : req->id->len, "error", "code", code,
                 "message", message, "data");
  va_start(ap, fmt);
  n += json_emit_va(buf + n, len - n, fmt, ap);
  va_end(ap);

  n += json_emit(buf + n, len - n, "}}");

  return n;
}

int ns_rpc_create_std_error(char *buf, int len, struct ns_rpc_request *req,
                            int code) {
  const char *message = NULL;

  switch (code) {
    case JSON_RPC_PARSE_ERROR:
      message = "parse error";
      break;
    case JSON_RPC_INVALID_REQUEST_ERROR:
      message = "invalid request";
      break;
    case JSON_RPC_METHOD_NOT_FOUND_ERROR:
      message = "method not found";
      break;
    case JSON_RPC_INVALID_PARAMS_ERROR:
      message = "invalid parameters";
      break;
    case JSON_RPC_SERVER_ERROR:
      message = "server error";
      break;
    default:
      message = "unspecified error";
      break;
  }

  return ns_rpc_create_error(buf, len, req, code, message, "N");
}

int ns_rpc_dispatch(const char *buf, int len, char *dst, int dst_len,
                    const char **methods, ns_rpc_handler_t *handlers) {
  struct json_token tokens[200];
  struct ns_rpc_request req;
  int i, n;

  memset(&req, 0, sizeof(req));
  n = parse_json(buf, len, tokens, sizeof(tokens) / sizeof(tokens[0]));
  if (n <= 0) {
    int err_code = (n == JSON_STRING_INVALID) ? JSON_RPC_PARSE_ERROR
                                              : JSON_RPC_SERVER_ERROR;
    return ns_rpc_create_std_error(dst, dst_len, &req, err_code);
  }

  req.message = tokens;
  req.id = find_json_token(tokens, "id");
  req.method = find_json_token(tokens, "method");
  req.params = find_json_token(tokens, "params");

  if (req.id == NULL || req.method == NULL) {
    return ns_rpc_create_std_error(dst, dst_len, &req,
                                   JSON_RPC_INVALID_REQUEST_ERROR);
  }

  for (i = 0; methods[i] != NULL; i++) {
    int mlen = strlen(methods[i]);
    if (mlen == req.method->len &&
        memcmp(methods[i], req.method->ptr, mlen) == 0)
      break;
  }

  if (methods[i] == NULL) {
    return ns_rpc_create_std_error(dst, dst_len, &req,
                                   JSON_RPC_METHOD_NOT_FOUND_ERROR);
  }

  return handlers[i](dst, dst_len, &req);
}

int ns_rpc_parse_reply(const char *buf, int len, struct json_token *toks,
                       int max_toks, struct ns_rpc_reply *rep,
                       struct ns_rpc_error *er) {
  int n = parse_json(buf, len, toks, max_toks);

  memset(rep, 0, sizeof(*rep));
  memset(er, 0, sizeof(*er));

  if (n > 0) {
    if ((rep->result = find_json_token(toks, "result")) != NULL) {
      rep->message = toks;
      rep->id = find_json_token(toks, "id");
    } else {
      er->message = toks;
      er->id = find_json_token(toks, "id");
      er->error_code = find_json_token(toks, "error.code");
      er->error_message = find_json_token(toks, "error.message");
      er->error_data = find_json_token(toks, "error.data");
    }
  }
  return n;
}

#endif /* NS_DISABLE_JSON_RPC */
#ifdef NS_MODULE_LINES
#line 1 "src/mqtt.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef NS_DISABLE_MQTT


static int parse_mqtt(struct mbuf *io, struct ns_mqtt_message *mm) {
  uint8_t header;
  int cmd;
  size_t len = 0;
  int var_len = 0;
  char *vlen = &io->buf[1];

  if (io->len < 2) return -1;

  header = io->buf[0];
  cmd = header >> 4;

  /* decode mqtt variable length */
  do {
    len += (*vlen & 127) << 7 * (vlen - &io->buf[1]);
  } while ((*vlen++ & 128) != 0 && ((size_t)(vlen - io->buf) <= io->len));

  if (io->len < (size_t)(len - 1)) return -1;

  mbuf_remove(io, 1 + (vlen - &io->buf[1]));
  mm->cmd = cmd;
  mm->qos = NS_MQTT_GET_QOS(header);

  switch (cmd) {
    case NS_MQTT_CMD_CONNECT:
      /* TODO(mkm): parse keepalive and will */
      break;
    case NS_MQTT_CMD_CONNACK:
      mm->connack_ret_code = io->buf[1];
      var_len = 2;
      break;
    case NS_MQTT_CMD_PUBACK:
    case NS_MQTT_CMD_PUBREC:
    case NS_MQTT_CMD_PUBREL:
    case NS_MQTT_CMD_PUBCOMP:
    case NS_MQTT_CMD_SUBACK:
      mm->message_id = ntohs(*(uint16_t *) io->buf);
      var_len = 2;
      break;
    case NS_MQTT_CMD_PUBLISH: {
      uint16_t topic_len = ntohs(*(uint16_t *) io->buf);
      mm->topic = (char *) NS_MALLOC(topic_len + 1);
      mm->topic[topic_len] = 0;
      strncpy(mm->topic, io->buf + 2, topic_len);
      var_len = topic_len + 2;

      if (NS_MQTT_GET_QOS(header) > 0) {
        mm->message_id = ntohs(*(uint16_t *) io->buf);
        var_len += 2;
      }
    } break;
    case NS_MQTT_CMD_SUBSCRIBE:
      /*
       * topic expressions are left in the payload and can be parsed with
       * `ns_mqtt_next_subscribe_topic`
       */
      mm->message_id = ntohs(*(uint16_t *) io->buf);
      var_len = 2;
      break;
    default:
      printf("TODO: UNHANDLED COMMAND %d\n", cmd);
      break;
  }

  mbuf_remove(io, var_len);
  return len - var_len;
}

static void mqtt_handler(struct ns_connection *nc, int ev, void *ev_data) {
  int len;
  struct mbuf *io = &nc->recv_mbuf;
  struct ns_mqtt_message mm;
  memset(&mm, 0, sizeof(mm));

  nc->handler(nc, ev, ev_data);

  switch (ev) {
    case NS_RECV:
      len = parse_mqtt(io, &mm);
      if (len == -1) break; /* not fully buffered */
      mm.payload.p = io->buf;
      mm.payload.len = len;

      nc->handler(nc, NS_MQTT_EVENT_BASE + mm.cmd, &mm);

      if (mm.topic) {
        NS_FREE(mm.topic);
      }
      mbuf_remove(io, mm.payload.len);
      break;
  }
}

void ns_set_protocol_mqtt(struct ns_connection *nc) {
  nc->proto_handler = mqtt_handler;
}

void ns_send_mqtt_handshake(struct ns_connection *nc, const char *client_id) {
  static struct ns_send_mqtt_handshake_opts opts;
  ns_send_mqtt_handshake_opt(nc, client_id, opts);
}

void ns_send_mqtt_handshake_opt(struct ns_connection *nc, const char *client_id,
                                struct ns_send_mqtt_handshake_opts opts) {
  uint8_t header = NS_MQTT_CMD_CONNECT << 4;
  uint8_t rem_len;
  uint16_t keep_alive;
  uint16_t client_id_len;

  /*
   * 9: version_header(len, magic_string, version_number), 1: flags, 2:
   * keep-alive timer,
   * 2: client_identifier_len, n: client_id
   */
  rem_len = 9 + 1 + 2 + 2 + strlen(client_id);

  ns_send(nc, &header, 1);
  ns_send(nc, &rem_len, 1);
  ns_send(nc, "\00\06MQIsdp\03", 9);
  ns_send(nc, &opts.flags, 1);

  if (opts.keep_alive == 0) {
    opts.keep_alive = 60;
  }
  keep_alive = htons(opts.keep_alive);
  ns_send(nc, &keep_alive, 2);

  client_id_len = htons(strlen(client_id));
  ns_send(nc, &client_id_len, 2);
  ns_send(nc, client_id, strlen(client_id));
}

static void ns_mqtt_prepend_header(struct ns_connection *nc, uint8_t cmd,
                                   uint8_t flags, size_t len) {
  size_t off = nc->send_mbuf.len - len;
  uint8_t header = cmd << 4 | (uint8_t) flags;

  uint8_t buf[1 + sizeof(size_t)];
  uint8_t *vlen = &buf[1];

  assert(nc->send_mbuf.len >= len);

  buf[0] = header;

  /* mqtt variable length encoding */
  do {
    *vlen = len % 0x80;
    len /= 0x80;
    if (len > 0) *vlen |= 0x80;
    vlen++;
  } while (len > 0);

  mbuf_insert(&nc->send_mbuf, off, buf, vlen - buf);
}

void ns_mqtt_publish(struct ns_connection *nc, const char *topic,
                     uint16_t message_id, int flags, const void *data,
                     size_t len) {
  size_t old_len = nc->send_mbuf.len;

  uint16_t topic_len = htons(strlen(topic));
  uint16_t message_id_net = htons(message_id);

  ns_send(nc, &topic_len, 2);
  ns_send(nc, topic, strlen(topic));
  if (NS_MQTT_GET_QOS(flags) > 0) {
    ns_send(nc, &message_id_net, 2);
  }
  ns_send(nc, data, len);

  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_PUBLISH, flags,
                         nc->send_mbuf.len - old_len);
}

void ns_mqtt_subscribe(struct ns_connection *nc,
                       const struct ns_mqtt_topic_expression *topics,
                       size_t topics_len, uint16_t message_id) {
  size_t old_len = nc->send_mbuf.len;

  uint16_t message_id_n = htons(message_id);
  size_t i;

  ns_send(nc, (char *) &message_id_n, 2);
  for (i = 0; i < topics_len; i++) {
    uint16_t topic_len_n = htons(strlen(topics[i].topic));
    ns_send(nc, &topic_len_n, 2);
    ns_send(nc, topics[i].topic, strlen(topics[i].topic));
    ns_send(nc, &topics[i].qos, 1);
  }

  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_SUBSCRIBE, NS_MQTT_QOS(1),
                         nc->send_mbuf.len - old_len);
}

int ns_mqtt_next_subscribe_topic(struct ns_mqtt_message *msg,
                                 struct ns_str *topic, uint8_t *qos, int pos) {
  unsigned char *buf = (unsigned char *) msg->payload.p + pos;
  if ((size_t) pos >= msg->payload.len) {
    return -1;
  }

  topic->len = buf[0] << 8 | buf[1];
  topic->p = (char *) buf + 2;
  *qos = buf[2 + topic->len];
  return pos + 2 + topic->len + 1;
}

void ns_mqtt_unsubscribe(struct ns_connection *nc, char **topics,
                         size_t topics_len, uint16_t message_id) {
  size_t old_len = nc->send_mbuf.len;

  uint16_t message_id_n = htons(message_id);
  size_t i;

  ns_send(nc, (char *) &message_id_n, 2);
  for (i = 0; i < topics_len; i++) {
    uint16_t topic_len_n = htons(strlen(topics[i]));
    ns_send(nc, &topic_len_n, 2);
    ns_send(nc, topics[i], strlen(topics[i]));
  }

  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_UNSUBSCRIBE, NS_MQTT_QOS(1),
                         nc->send_mbuf.len - old_len);
}

void ns_mqtt_connack(struct ns_connection *nc, uint8_t return_code) {
  uint8_t unused = 0;
  ns_send(nc, &unused, 1);
  ns_send(nc, &return_code, 1);
  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_CONNACK, 0, 2);
}

/*
 * Sends a command which contains only a `message_id` and a QoS level of 1.
 *
 * Helper function.
 */
static void ns_send_mqtt_short_command(struct ns_connection *nc, uint8_t cmd,
                                       uint16_t message_id) {
  uint16_t message_id_net = htons(message_id);
  ns_send(nc, &message_id_net, 2);
  ns_mqtt_prepend_header(nc, cmd, NS_MQTT_QOS(1), 2);
}

void ns_mqtt_puback(struct ns_connection *nc, uint16_t message_id) {
  ns_send_mqtt_short_command(nc, NS_MQTT_CMD_PUBACK, message_id);
}

void ns_mqtt_pubrec(struct ns_connection *nc, uint16_t message_id) {
  ns_send_mqtt_short_command(nc, NS_MQTT_CMD_PUBREC, message_id);
}

void ns_mqtt_pubrel(struct ns_connection *nc, uint16_t message_id) {
  ns_send_mqtt_short_command(nc, NS_MQTT_CMD_PUBREL, message_id);
}

void ns_mqtt_pubcomp(struct ns_connection *nc, uint16_t message_id) {
  ns_send_mqtt_short_command(nc, NS_MQTT_CMD_PUBCOMP, message_id);
}

void ns_mqtt_suback(struct ns_connection *nc, uint8_t *qoss, size_t qoss_len,
                    uint16_t message_id) {
  size_t i;
  uint16_t message_id_net = htons(message_id);
  ns_send(nc, &message_id_net, 2);
  for (i = 0; i < qoss_len; i++) {
    ns_send(nc, &qoss[i], 1);
  }
  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_SUBACK, NS_MQTT_QOS(1), 2 + qoss_len);
}

void ns_mqtt_unsuback(struct ns_connection *nc, uint16_t message_id) {
  ns_send_mqtt_short_command(nc, NS_MQTT_CMD_UNSUBACK, message_id);
}

void ns_mqtt_ping(struct ns_connection *nc) {
  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_PINGREQ, 0, 0);
}

void ns_mqtt_pong(struct ns_connection *nc) {
  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_PINGRESP, 0, 0);
}

void ns_mqtt_disconnect(struct ns_connection *nc) {
  ns_mqtt_prepend_header(nc, NS_MQTT_CMD_DISCONNECT, 0, 0);
}

#endif /* NS_DISABLE_MQTT */
#ifdef NS_MODULE_LINES
#line 1 "src/mqtt-broker.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */


#ifdef NS_ENABLE_MQTT_BROKER

static void ns_mqtt_session_init(struct ns_mqtt_broker *brk,
                                 struct ns_mqtt_session *s,
                                 struct ns_connection *nc) {
  s->brk = brk;
  s->subscriptions = NULL;
  s->num_subscriptions = 0;
  s->nc = nc;
}

static void ns_mqtt_add_session(struct ns_mqtt_session *s) {
  s->next = s->brk->sessions;
  s->brk->sessions = s;
  s->prev = NULL;
  if (s->next != NULL) s->next->prev = s;
}

static void ns_mqtt_remove_session(struct ns_mqtt_session *s) {
  if (s->prev == NULL) s->brk->sessions = s->next;
  if (s->prev) s->prev->next = s->next;
  if (s->next) s->next->prev = s->prev;
}

static void ns_mqtt_destroy_session(struct ns_mqtt_session *s) {
  size_t i;
  for (i = 0; i < s->num_subscriptions; i++) {
    NS_FREE((void *) s->subscriptions[i].topic);
  }
  NS_FREE(s);
}

static void ns_mqtt_close_session(struct ns_mqtt_session *s) {
  ns_mqtt_remove_session(s);
  ns_mqtt_destroy_session(s);
}

void ns_mqtt_broker_init(struct ns_mqtt_broker *brk, void *user_data) {
  brk->sessions = NULL;
  brk->user_data = user_data;
}

static void ns_mqtt_broker_handle_connect(struct ns_mqtt_broker *brk,
                                          struct ns_connection *nc) {
  struct ns_mqtt_session *s = (struct ns_mqtt_session *) malloc(sizeof *s);
  if (s == NULL) {
    /* LCOV_EXCL_START */
    ns_mqtt_connack(nc, NS_MQTT_CONNACK_SERVER_UNAVAILABLE);
    return;
    /* LCOV_EXCL_STOP */
  }

  /* TODO(mkm): check header (magic and version) */

  ns_mqtt_session_init(brk, s, nc);
  s->user_data = nc->user_data;
  nc->user_data = s;
  ns_mqtt_add_session(s);

  ns_mqtt_connack(nc, NS_MQTT_CONNACK_ACCEPTED);
}

static void ns_mqtt_broker_handle_subscribe(struct ns_connection *nc,
                                            struct ns_mqtt_message *msg) {
  struct ns_mqtt_session *ss = (struct ns_mqtt_session *) nc->user_data;
  uint8_t qoss[512];
  size_t qoss_len = 0;
  struct ns_str topic;
  uint8_t qos;
  int pos;
  struct ns_mqtt_topic_expression *te;

  for (pos = 0;
       (pos = ns_mqtt_next_subscribe_topic(msg, &topic, &qos, pos)) != -1;) {
    qoss[qoss_len++] = qos;
  }

  ss->subscriptions = (struct ns_mqtt_topic_expression *) realloc(
      ss->subscriptions, sizeof(*ss->subscriptions) * qoss_len);
  for (pos = 0;
       (pos = ns_mqtt_next_subscribe_topic(msg, &topic, &qos, pos)) != -1;
       ss->num_subscriptions++) {
    te = &ss->subscriptions[ss->num_subscriptions];
    te->topic = (char *) malloc(topic.len + 1);
    te->qos = qos;
    strncpy((char *) te->topic, topic.p, topic.len + 1);
  }

  ns_mqtt_suback(nc, qoss, qoss_len, msg->message_id);
}

/*
 * Matches a topic against a topic expression
 *
 * See http://goo.gl/iWk21X
 *
 * Returns 1 if it matches; 0 otherwise.
 */
static int ns_mqtt_match_topic_expression(const char *exp, const char *topic) {
  /* TODO(mkm): implement real matching */
  int len = strlen(exp);
  if (strchr(exp, '#')) {
    len -= 2;
  }
  return strncmp(exp, topic, len) == 0;
}

static void ns_mqtt_broker_handle_publish(struct ns_mqtt_broker *brk,
                                          struct ns_mqtt_message *msg) {
  struct ns_mqtt_session *s;
  size_t i;

  for (s = ns_mqtt_next(brk, NULL); s != NULL; s = ns_mqtt_next(brk, s)) {
    for (i = 0; i < s->num_subscriptions; i++) {
      if (ns_mqtt_match_topic_expression(s->subscriptions[i].topic,
                                         msg->topic)) {
        ns_mqtt_publish(s->nc, msg->topic, 0, 0, msg->payload.p,
                        msg->payload.len);
        break;
      }
    }
  }
}

void ns_mqtt_broker(struct ns_connection *nc, int ev, void *data) {
  struct ns_mqtt_message *msg = (struct ns_mqtt_message *) data;
  struct ns_mqtt_broker *brk;

  if (nc->listener) {
    brk = (struct ns_mqtt_broker *) nc->listener->user_data;
  } else {
    brk = (struct ns_mqtt_broker *) nc->user_data;
  }

  switch (ev) {
    case NS_ACCEPT:
      ns_set_protocol_mqtt(nc);
      break;
    case NS_MQTT_CONNECT:
      ns_mqtt_broker_handle_connect(brk, nc);
      break;
    case NS_MQTT_SUBSCRIBE:
      ns_mqtt_broker_handle_subscribe(nc, msg);
      break;
    case NS_MQTT_PUBLISH:
      ns_mqtt_broker_handle_publish(brk, msg);
      break;
    case NS_CLOSE:
      if (nc->listener) {
        ns_mqtt_close_session((struct ns_mqtt_session *) nc->user_data);
      }
      break;
  }
}

struct ns_mqtt_session *ns_mqtt_next(struct ns_mqtt_broker *brk,
                                     struct ns_mqtt_session *s) {
  return s == NULL ? brk->sessions : s->next;
}

#endif /* NS_ENABLE_MQTT_BROKER */
#ifdef NS_MODULE_LINES
#line 1 "src/dns.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef NS_DISABLE_DNS


#define MAX_DNS_PACKET_LEN 2048

static int ns_dns_tid = 0xa0;

struct ns_dns_header {
  uint16_t transaction_id;
  uint16_t flags;
  uint16_t num_questions;
  uint16_t num_answers;
  uint16_t num_authority_prs;
  uint16_t num_other_prs;
};

struct ns_dns_resource_record *ns_dns_next_record(
    struct ns_dns_message *msg, int query,
    struct ns_dns_resource_record *prev) {
  struct ns_dns_resource_record *rr;

  for (rr = (prev == NULL ? msg->answers : prev + 1);
       rr - msg->answers < msg->num_answers; rr++) {
    if (rr->rtype == query) {
      return rr;
    }
  }
  return NULL;
}

int ns_dns_parse_record_data(struct ns_dns_message *msg,
                             struct ns_dns_resource_record *rr, void *data,
                             size_t data_len) {
  switch (rr->rtype) {
    case NS_DNS_A_RECORD:
      if (data_len < sizeof(struct in_addr)) {
        return -1;
      }
      if (rr->rdata.p + data_len > msg->pkt.p + msg->pkt.len) {
        return -1;
      }
      memcpy(data, rr->rdata.p, data_len);
      return 0;
#ifdef NS_ENABLE_IPV6
    case NS_DNS_AAAA_RECORD:
      if (data_len < sizeof(struct in6_addr)) {
        return -1; /* LCOV_EXCL_LINE */
      }
      memcpy(data, rr->rdata.p, data_len);
      return 0;
#endif
    case NS_DNS_CNAME_RECORD:
      ns_dns_uncompress_name(msg, &rr->rdata, (char *) data, data_len);
      return 0;
  }

  return -1;
}

int ns_dns_insert_header(struct mbuf *io, size_t pos,
                         struct ns_dns_message *msg) {
  struct ns_dns_header header;

  memset(&header, 0, sizeof(header));
  header.transaction_id = msg->transaction_id;
  header.flags = htons(msg->flags);
  header.num_questions = htons(msg->num_questions);
  header.num_answers = htons(msg->num_answers);

  return mbuf_insert(io, pos, &header, sizeof(header));
}

int ns_dns_copy_body(struct mbuf *io, struct ns_dns_message *msg) {
  return mbuf_append(io, msg->pkt.p + sizeof(struct ns_dns_header),
                     msg->pkt.len - sizeof(struct ns_dns_header));
}

static int ns_dns_encode_name(struct mbuf *io, const char *name, size_t len) {
  const char *s;
  unsigned char n;
  size_t pos = io->len;

  do {
    if ((s = strchr(name, '.')) == NULL) {
      s = name + len;
    }

    if (s - name > 127) {
      return -1; /* TODO(mkm) cover */
    }
    n = s - name;           /* chunk length */
    mbuf_append(io, &n, 1); /* send length */
    mbuf_append(io, name, n);

    if (*s == '.') {
      n++;
    }

    name += n;
    len -= n;
  } while (*s != '\0');
  mbuf_append(io, "\0", 1); /* Mark end of host name */

  return io->len - pos;
}

int ns_dns_encode_record(struct mbuf *io, struct ns_dns_resource_record *rr,
                         const char *name, size_t nlen, const void *rdata,
                         size_t rlen) {
  size_t pos = io->len;
  uint16_t u16;
  uint32_t u32;

  if (rr->kind == NS_DNS_INVALID_RECORD) {
    return -1; /* LCOV_EXCL_LINE */
  }

  if (ns_dns_encode_name(io, name, nlen) == -1) {
    return -1;
  }

  u16 = htons(rr->rtype);
  mbuf_append(io, &u16, 2);
  u16 = htons(rr->rclass);
  mbuf_append(io, &u16, 2);

  if (rr->kind == NS_DNS_ANSWER) {
    u32 = htonl(rr->ttl);
    mbuf_append(io, &u32, 4);

    if (rr->rtype == NS_DNS_CNAME_RECORD) {
      int clen;
      /* fill size after encoding */
      size_t off = io->len;
      mbuf_append(io, &u16, 2);
      if ((clen = ns_dns_encode_name(io, (const char *) rdata, rlen)) == -1) {
        return -1;
      }
      u16 = clen;
      io->buf[off] = u16 >> 8;
      io->buf[off + 1] = u16 & 0xff;
    } else {
      u16 = htons(rlen);
      mbuf_append(io, &u16, 2);
      mbuf_append(io, rdata, rlen);
    }
  }

  return io->len - pos;
}

void ns_send_dns_query(struct ns_connection *nc, const char *name,
                       int query_type) {
  struct ns_dns_message msg;
  struct mbuf pkt;
  struct ns_dns_resource_record *rr = &msg.questions[0];

  mbuf_init(&pkt, MAX_DNS_PACKET_LEN);
  memset(&msg, 0, sizeof(msg));

  msg.transaction_id = ++ns_dns_tid;
  msg.flags = 0x100;
  msg.num_questions = 1;

  ns_dns_insert_header(&pkt, 0, &msg);

  rr->rtype = query_type;
  rr->rclass = 1; /* Class: inet */
  rr->kind = NS_DNS_QUESTION;

  if (ns_dns_encode_record(&pkt, rr, name, strlen(name), NULL, 0) == -1) {
    /* TODO(mkm): return an error code */
    return; /* LCOV_EXCL_LINE */
  }

  /* TCP DNS requires messages to be prefixed with len */
  if (!(nc->flags & NSF_UDP)) {
    uint16_t len = htons(pkt.len);
    mbuf_insert(&pkt, 0, &len, 2);
  }

  ns_send(nc, pkt.buf, pkt.len);
  mbuf_free(&pkt);
}

static unsigned char *ns_parse_dns_resource_record(
    unsigned char *data, unsigned char *end, struct ns_dns_resource_record *rr,
    int reply) {
  unsigned char *name = data;
  int chunk_len, data_len;

  while (data < end && (chunk_len = *data)) {
    if (((unsigned char *) data)[0] & 0xc0) {
      data += 1;
      break;
    }
    data += chunk_len + 1;
  }

  rr->name.p = (char *) name;
  rr->name.len = data - name + 1;

  data++;
  if (data > end - 4) {
    return data;
  }

  rr->rtype = data[0] << 8 | data[1];
  data += 2;

  rr->rclass = data[0] << 8 | data[1];
  data += 2;

  rr->kind = reply ? NS_DNS_ANSWER : NS_DNS_QUESTION;
  if (reply) {
    if (data >= end - 6) {
      return data;
    }

    rr->ttl = (uint32_t) data[0] << 24 | (uint32_t) data[1] << 16 |
              data[2] << 8 | data[3];
    data += 4;

    data_len = *data << 8 | *(data + 1);
    data += 2;

    rr->rdata.p = (char *) data;
    rr->rdata.len = data_len;
    data += data_len;
  }
  return data;
}

int ns_parse_dns(const char *buf, int len, struct ns_dns_message *msg) {
  struct ns_dns_header *header = (struct ns_dns_header *) buf;
  unsigned char *data = (unsigned char *) buf + sizeof(*header);
  unsigned char *end = (unsigned char *) buf + len;
  int i;
  msg->pkt.p = buf;
  msg->pkt.len = len;

  if (len < (int) sizeof(*header)) {
    return -1; /* LCOV_EXCL_LINE */
  }

  msg->transaction_id = header->transaction_id;
  msg->flags = ntohs(header->flags);
  msg->num_questions = ntohs(header->num_questions);
  msg->num_answers = ntohs(header->num_answers);

  for (i = 0; i < msg->num_questions && i < (int) ARRAY_SIZE(msg->questions);
       i++) {
    data = ns_parse_dns_resource_record(data, end, &msg->questions[i], 0);
  }

  for (i = 0; i < msg->num_answers && i < (int) ARRAY_SIZE(msg->answers); i++) {
    data = ns_parse_dns_resource_record(data, end, &msg->answers[i], 1);
  }

  return 0;
}

size_t ns_dns_uncompress_name(struct ns_dns_message *msg, struct ns_str *name,
                              char *dst, int dst_len) {
  int chunk_len;
  char *old_dst = dst;
  const unsigned char *data = (unsigned char *) name->p;
  const unsigned char *end = (unsigned char *) msg->pkt.p + msg->pkt.len;

  if (data >= end) {
    return 0;
  }

  while ((chunk_len = *data++)) {
    int leeway = dst_len - (dst - old_dst);
    if (data >= end) {
      return 0;
    }

    if (chunk_len & 0xc0) {
      uint16_t off = (data[-1] & (~0xc0)) << 8 | data[0];
      if (off >= msg->pkt.len) {
        return 0;
      }
      data = (unsigned char *) msg->pkt.p + off;
      continue;
    }
    if (chunk_len > leeway) {
      chunk_len = leeway;
    }

    if (data + chunk_len >= end) {
      return 0;
    }

    memcpy(dst, data, chunk_len);
    data += chunk_len;
    dst += chunk_len;
    leeway -= chunk_len;
    if (leeway == 0) {
      return dst - old_dst;
    }
    *dst++ = '.';
  }

  if (dst != old_dst) {
    *--dst = 0;
  }
  return dst - old_dst;
}

static void dns_handler(struct ns_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;
  struct ns_dns_message msg;

  /* Pass low-level events to the user handler */
  nc->handler(nc, ev, ev_data);

  switch (ev) {
    case NS_RECV:
      if (!(nc->flags & NSF_UDP)) {
        mbuf_remove(&nc->recv_mbuf, 2);
      }
      if (ns_parse_dns(nc->recv_mbuf.buf, nc->recv_mbuf.len, &msg) == -1) {
        /* reply + recursion allowed + format error */
        memset(&msg, 0, sizeof(msg));
        msg.flags = 0x8081;
        ns_dns_insert_header(io, 0, &msg);
        if (!(nc->flags & NSF_UDP)) {
          uint16_t len = htons(io->len);
          mbuf_insert(io, 0, &len, 2);
        }
        ns_send(nc, io->buf, io->len);
      } else {
        /* Call user handler with parsed message */
        nc->handler(nc, NS_DNS_MESSAGE, &msg);
      }
      mbuf_remove(io, io->len);
      break;
  }
}

void ns_set_protocol_dns(struct ns_connection *nc) {
  nc->proto_handler = dns_handler;
}

#endif /* NS_DISABLE_DNS */
#ifdef NS_MODULE_LINES
#line 1 "src/dns-server.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifdef NS_ENABLE_DNS_SERVER


struct ns_dns_reply ns_dns_create_reply(struct mbuf *io,
                                        struct ns_dns_message *msg) {
  struct ns_dns_reply rep;
  rep.msg = msg;
  rep.io = io;
  rep.start = io->len;

  /* reply + recursion allowed */
  msg->flags |= 0x8080;
  ns_dns_copy_body(io, msg);

  msg->num_answers = 0;
  return rep;
}

int ns_dns_send_reply(struct ns_connection *nc, struct ns_dns_reply *r) {
  size_t sent = r->io->len - r->start;
  ns_dns_insert_header(r->io, r->start, r->msg);
  if (!(nc->flags & NSF_UDP)) {
    uint16_t len = htons(sent);
    mbuf_insert(r->io, r->start, &len, 2);
  }

  if (&nc->send_mbuf != r->io || nc->flags & NSF_UDP) {
    sent = ns_send(nc, r->io->buf + r->start, r->io->len - r->start);
    r->io->len = r->start;
  }
  return sent;
}

int ns_dns_reply_record(struct ns_dns_reply *reply,
                        struct ns_dns_resource_record *question,
                        const char *name, int rtype, int ttl, const void *rdata,
                        size_t rdata_len) {
  struct ns_dns_message *msg = (struct ns_dns_message *) reply->msg;
  char rname[512];
  struct ns_dns_resource_record *ans = &msg->answers[msg->num_answers];
  if (msg->num_answers >= NS_MAX_DNS_ANSWERS) {
    return -1; /* LCOV_EXCL_LINE */
  }

  if (name == NULL) {
    name = rname;
    rname[511] = 0;
    ns_dns_uncompress_name(msg, &question->name, rname, sizeof(rname) - 1);
  }

  *ans = *question;
  ans->kind = NS_DNS_ANSWER;
  ans->rtype = rtype;
  ans->ttl = ttl;

  if (ns_dns_encode_record(reply->io, ans, name, strlen(name), rdata,
                           rdata_len) == -1) {
    return -1; /* LCOV_EXCL_LINE */
  };

  msg->num_answers++;
  return 0;
}

#endif /* NS_ENABLE_DNS_SERVER */
#ifdef NS_MODULE_LINES
#line 1 "src/resolv.c"
/**/
#endif
/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#ifndef NS_DISABLE_RESOLVER


static const char *ns_default_dns_server = "udp://8.8.8.8:53";
NS_INTERNAL char ns_dns_server[256];

struct ns_resolve_async_request {
  char name[1024];
  int query;
  ns_resolve_callback_t callback;
  void *data;
  time_t timeout;
  int max_retries;

  /* state */
  time_t last_time;
  int retries;
};

/*
 * Find what nameserver to use.
 *
 * Return 0 if OK, -1 if error
 */
static int ns_get_ip_address_of_nameserver(char *name, size_t name_len) {
  int ret = 0;

#ifdef _WIN32
  int i;
  LONG err;
  HKEY hKey, hSub;
  char subkey[512], dhcpns[512], ns[512], value[128],
      *key = "SYSTEM\\ControlSet001\\Services\\Tcpip\\Parameters\\Interfaces";

  if ((err = RegOpenKey(HKEY_LOCAL_MACHINE, key, &hKey)) != ERROR_SUCCESS) {
    fprintf(stderr, "cannot open reg key %s: %d\n", key, err);
    ret = -1;
  } else {
    for (ret = -1, i = 0;
         RegEnumKey(hKey, i, subkey, sizeof(subkey)) == ERROR_SUCCESS; i++) {
      DWORD type, len = sizeof(value);
      if (RegOpenKey(hKey, subkey, &hSub) == ERROR_SUCCESS &&
          (RegQueryValueEx(hSub, "NameServer", 0, &type, value, &len) ==
               ERROR_SUCCESS ||
           RegQueryValueEx(hSub, "DhcpNameServer", 0, &type, value, &len) ==
               ERROR_SUCCESS)) {
        /*
         * See https://github.com/cesanta/fossa/issues/176
         * The value taken from the registry can be empty, a single
         * IP address, or multiple IP addresses separated by comma.
         * If it's empty, check the next interface.
         * If it's multiple IP addresses, take the first one.
         */
        char *comma = strchr(value, ',');
        if (value[0] == '\0') {
          continue;
        }
        if (comma != NULL) {
          *comma = '\0';
        }
        snprintf(name, name_len, "udp://%s:53", value);
        ret = 0;
        RegCloseKey(hSub);
        break;
      }
    }
    RegCloseKey(hKey);
  }
#elif !defined(NS_DISABLE_FILESYSTEM)
  FILE *fp;
  char line[512];

  if ((fp = fopen("/etc/resolv.conf", "r")) == NULL) {
    ret = -1;
  } else {
    /* Try to figure out what nameserver to use */
    for (ret = -1; fgets(line, sizeof(line), fp) != NULL;) {
      char buf[256];
      if (sscanf(line, "nameserver %255[^\n\t #]s", buf) == 1) {
        snprintf(name, name_len, "udp://%s:53", buf);
        ret = 0;
        break;
      }
    }
    (void) fclose(fp);
  }
#endif /* _WIN32 */

  return ret;
}

int ns_resolve_from_hosts_file(const char *name, union socket_address *usa) {
#ifndef NS_DISABLE_FILESYSTEM
  /* TODO(mkm) cache /etc/hosts */
  FILE *fp;
  char line[1024];
  char *p;
  char alias[256];
  unsigned int a, b, c, d;
  int len = 0;

  if ((fp = fopen("/etc/hosts", "r")) == NULL) {
    return -1;
  }

  for (; fgets(line, sizeof(line), fp) != NULL;) {
    if (line[0] == '#') continue;

    if (sscanf(line, "%u.%u.%u.%u%n", &a, &b, &c, &d, &len) == 0) {
      /* TODO(mkm): handle ipv6 */
      continue;
    }
    for (p = line + len; sscanf(p, "%s%n", alias, &len) == 1; p += len) {
      if (strcmp(alias, name) == 0) {
        usa->sin.sin_addr.s_addr = htonl(a << 24 | b << 16 | c << 8 | d);
        return 0;
      }
    }
  }

  fclose(fp);
#endif

  return -1;
}

static void ns_resolve_async_eh(struct ns_connection *nc, int ev, void *data) {
  time_t now = time(NULL);
  struct ns_resolve_async_request *req;
  struct ns_dns_message msg;

  req = (struct ns_resolve_async_request *) nc->user_data;

  switch (ev) {
    case NS_POLL:
      if (req->retries > req->max_retries) {
        req->callback(NULL, req->data);
        nc->flags |= NSF_CLOSE_IMMEDIATELY;
        break;
      }
      if (now - req->last_time > req->timeout) {
        ns_send_dns_query(nc, req->name, req->query);
        req->last_time = now;
        req->retries++;
      }
      break;
    case NS_RECV:
      if (ns_parse_dns(nc->recv_mbuf.buf, *(int *) data, &msg) == 0 &&
          msg.num_answers > 0) {
        req->callback(&msg, req->data);
      } else {
        req->callback(NULL, req->data);
      }
      nc->flags |= NSF_CLOSE_IMMEDIATELY;
      break;
  }
}

int ns_resolve_async(struct ns_mgr *mgr, const char *name, int query,
                     ns_resolve_callback_t cb, void *data) {
  static struct ns_resolve_async_opts opts;
  return ns_resolve_async_opt(mgr, name, query, cb, data, opts);
}

int ns_resolve_async_opt(struct ns_mgr *mgr, const char *name, int query,
                         ns_resolve_callback_t cb, void *data,
                         struct ns_resolve_async_opts opts) {
  struct ns_resolve_async_request *req;
  struct ns_connection *dns_nc;
  const char *nameserver = opts.nameserver_url;

  /* resolve with DNS */
  req = (struct ns_resolve_async_request *) NS_CALLOC(1, sizeof(*req));
  if (req == NULL) {
    return -1;
  }

  strncpy(req->name, name, sizeof(req->name));
  req->query = query;
  req->callback = cb;
  req->data = data;
  /* TODO(mkm): parse defaults out of resolve.conf */
  req->max_retries = opts.max_retries ? opts.max_retries : 2;
  req->timeout = opts.timeout ? opts.timeout : 5;

  /* Lazily initialize dns server */
  if (nameserver == NULL && ns_dns_server[0] == '\0' &&
      ns_get_ip_address_of_nameserver(ns_dns_server, sizeof(ns_dns_server)) ==
          -1) {
    strncpy(ns_dns_server, ns_default_dns_server, sizeof(ns_dns_server));
  }

  if (nameserver == NULL) {
    nameserver = ns_dns_server;
  }

  dns_nc = ns_connect(mgr, nameserver, ns_resolve_async_eh);
  if (dns_nc == NULL) {
    free(req);
    return -1;
  }
  dns_nc->user_data = req;

  return 0;
}

#endif /* NS_DISABLE_RESOLVE */
#ifdef NS_MODULE_LINES
#line 1 "src/coap.c"
/**/
#endif
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


#ifdef NS_ENABLE_COAP

void ns_coap_free_options(struct ns_coap_message *cm) {
  while (cm->options != NULL) {
    struct ns_coap_option *next = cm->options->next;
    NS_FREE(cm->options);
    cm->options = next;
  }
}

struct ns_coap_option *ns_coap_add_option(struct ns_coap_message *cm,
                                          uint32_t number, char *value,
                                          size_t len) {
  struct ns_coap_option *new_option =
      (struct ns_coap_option *) NS_CALLOC(1, sizeof(*new_option));

  new_option->number = number;
  new_option->value.p = value;
  new_option->value.len = len;

  if (cm->options == NULL) {
    cm->options = cm->options_tail = new_option;
  } else {
    /*
     * A very simple attention to help clients to compose options:
     * CoAP wants to see options ASC ordered.
     * Could be change by using sort in coap_compose
     */
    if (cm->options_tail->number <= new_option->number) {
      /* if option is already ordered just add it */
      cm->options_tail = cm->options_tail->next = new_option;
    } else {
      /* looking for appropriate position */
      struct ns_coap_option *current_opt = cm->options;
      struct ns_coap_option *prev_opt = 0;

      while (current_opt != NULL) {
        if (current_opt->number > new_option->number) {
          break;
        }
        prev_opt = current_opt;
        current_opt = current_opt->next;
      }

      if (prev_opt != NULL) {
        prev_opt->next = new_option;
        new_option->next = current_opt;
      } else {
        /* insert new_option to the beginning */
        new_option->next = cm->options;
        cm->options = new_option;
      }
    }
  }

  return new_option;
}

/*
 * Fills CoAP header in ns_coap_message.
 *
 * Helper function.
 */
static char *coap_parse_header(char *ptr, struct mbuf *io,
                               struct ns_coap_message *cm) {
  if (io->len < sizeof(uint32_t)) {
    cm->flags |= NS_COAP_NOT_ENOUGH_DATA;
    return NULL;
  }

  /*
   * Version (Ver):  2-bit unsigned integer.  Indicates the CoAP version
   * number.  Implementations of this specification MUST set this field
   * to 1 (01 binary).  Other values are reserved for future versions.
   * Messages with unknown version numbers MUST be silently ignored.
   */
  if (((uint8_t) *ptr >> 6) != 1) {
    cm->flags |= NS_COAP_IGNORE;
    return NULL;
  }

  /*
   * Type (T):  2-bit unsigned integer.  Indicates if this message is of
   * type Confirmable (0), Non-confirmable (1), Acknowledgement (2), or
   * Reset (3).
   */
  cm->msg_type = ((uint8_t) *ptr & 0x30) >> 4;
  cm->flags |= NS_COAP_MSG_TYPE_FIELD;

  /*
   * Token Length (TKL):  4-bit unsigned integer.  Indicates the length of
   * the variable-length Token field (0-8 bytes).  Lengths 9-15 are
   * reserved, MUST NOT be sent, and MUST be processed as a message
   * format error.
   */
  cm->token.len = *ptr & 0x0F;
  if (cm->token.len > 8) {
    cm->flags |= NS_COAP_FORMAT_ERROR;
    return NULL;
  }

  ptr++;

  /*
   * Code:  8-bit unsigned integer, split into a 3-bit class (most
   * significant bits) and a 5-bit detail (least significant bits)
   */
  cm->code_class = (uint8_t) *ptr >> 5;
  cm->code_detail = *ptr & 0x1F;
  cm->flags |= (NS_COAP_CODE_CLASS_FIELD | NS_COAP_CODE_DETAIL_FIELD);

  ptr++;

  /* Message ID:  16-bit unsigned integer in network byte order. */
  cm->msg_id = (uint8_t) *ptr << 8 | (uint8_t) * (ptr + 1);
  cm->flags |= NS_COAP_MSG_ID_FIELD;

  ptr += 2;

  return ptr;
}

/*
 * Fills token information in ns_coap_message.
 *
 * Helper function.
 */
static char *coap_get_token(char *ptr, struct mbuf *io,
                            struct ns_coap_message *cm) {
  if (cm->token.len != 0) {
    if (ptr + cm->token.len > io->buf + io->len) {
      cm->flags |= NS_COAP_NOT_ENOUGH_DATA;
      return NULL;
    } else {
      cm->token.p = ptr;
      ptr += cm->token.len;
      cm->flags |= NS_COAP_TOKEN_FIELD;
    }
  }

  return ptr;
}

/*
 * Returns Option Delta or Length.
 *
 * Helper function.
 */
static int coap_get_ext_opt(char *ptr, struct mbuf *io, uint16_t *opt_info) {
  int ret = 0;

  if (*opt_info == 13) {
    /*
     * 13:  An 8-bit unsigned integer follows the initial byte and
     * indicates the Option Delta/Length minus 13.
     */
    if (ptr < io->buf + io->len) {
      *opt_info = (uint8_t) *ptr + 13;
      ret = sizeof(uint8_t);
    } else {
      ret = -1; /* LCOV_EXCL_LINE */
    }
  } else if (*opt_info == 14) {
    /*
     * 14:  A 16-bit unsigned integer in network byte order follows the
     * initial byte and indicates the Option Delta/Length minus 269.
     */
    if (ptr + sizeof(uint8_t) < io->buf + io->len) {
      *opt_info = ((uint8_t) *ptr << 8 | (uint8_t) * (ptr + 1)) + 269;
      ret = sizeof(uint16_t);
    } else {
      ret = -1; /* LCOV_EXCL_LINE */
    }
  }

  return ret;
}

/*
 * Fills options in ns_coap_message.
 *
 * Helper function.
 *
 * General options format:
 * +---------------+---------------+
 * | Option Delta  | Option Length |  1 byte
 * +---------------+---------------+
 * \    Option Delta (extended)    \  0-2 bytes
 * +-------------------------------+
 * / Option Length  (extended)     \  0-2 bytes
 * +-------------------------------+
 * \         Option Value          \  0 or more bytes
 * +-------------------------------+
 */
static char *coap_get_options(char *ptr, struct mbuf *io,
                              struct ns_coap_message *cm) {
  uint16_t prev_opt = 0;

  if (ptr == io->buf + io->len) {
    /* end of packet, ok */
    return NULL;
  }

  /* 0xFF is payload marker */
  while (ptr < io->buf + io->len && (uint8_t) *ptr != 0xFF) {
    uint16_t option_delta, option_lenght;
    int optinfo_len;

    /* Option Delta:  4-bit unsigned integer */
    option_delta = ((uint8_t) *ptr & 0xF0) >> 4;
    /* Option Length:  4-bit unsigned integer */
    option_lenght = *ptr & 0x0F;

    if (option_delta == 15 || option_lenght == 15) {
      /*
       * 15:  Reserved for future use.  If the field is set to this value,
       * it MUST be processed as a message format error
       */
      cm->flags |= NS_COAP_FORMAT_ERROR;
      break;
    }

    ptr++;

    /* check for extended option delta */
    optinfo_len = coap_get_ext_opt(ptr, io, &option_delta);
    if (optinfo_len == -1) {
      cm->flags |= NS_COAP_NOT_ENOUGH_DATA; /* LCOV_EXCL_LINE */
      break;                                /* LCOV_EXCL_LINE */
    }

    ptr += optinfo_len;

    /* check or extended option lenght */
    optinfo_len = coap_get_ext_opt(ptr, io, &option_lenght);
    if (optinfo_len == -1) {
      cm->flags |= NS_COAP_NOT_ENOUGH_DATA; /* LCOV_EXCL_LINE */
      break;                                /* LCOV_EXCL_LINE */
    }

    ptr += optinfo_len;

    /*
     * Instead of specifying the Option Number directly, the instances MUST
     * appear in order of their Option Numbers and a delta encoding is used
     * between them.
     */
    option_delta += prev_opt;

    ns_coap_add_option(cm, option_delta, ptr, option_lenght);

    prev_opt = option_delta;

    if (ptr + option_lenght > io->buf + io->len) {
      cm->flags |= NS_COAP_NOT_ENOUGH_DATA; /* LCOV_EXCL_LINE */
      break;                                /* LCOV_EXCL_LINE */
    }

    ptr += option_lenght;
  }

  if ((cm->flags & NS_COAP_ERROR) != 0) {
    ns_coap_free_options(cm);
    return NULL;
  }

  cm->flags |= NS_COAP_OPTIONS_FIELD;

  if (ptr == io->buf + io->len) {
    /* end of packet, ok */
    return NULL;
  }

  ptr++;

  return ptr;
}

uint32_t ns_coap_parse(struct mbuf *io, struct ns_coap_message *cm) {
  char *ptr;

  memset(cm, 0, sizeof(*cm));

  if ((ptr = coap_parse_header(io->buf, io, cm)) == NULL) {
    return cm->flags;
  }

  if ((ptr = coap_get_token(ptr, io, cm)) == NULL) {
    return cm->flags;
  }

  if ((ptr = coap_get_options(ptr, io, cm)) == NULL) {
    return cm->flags;
  }

  /* the rest is payload */
  cm->payload.len = io->len - (ptr - io->buf);
  if (cm->payload.len != 0) {
    cm->payload.p = ptr;
    cm->flags |= NS_COAP_PAYLOAD_FIELD;
  }

  return cm->flags;
}

/*
 * Calculates extended size of given Opt Number/Length in coap message.
 *
 * Helper function.
 */
static size_t coap_get_ext_opt_size(uint32_t value) {
  int ret = 0;

  if (value >= 13 && value <= 0xFF + 13) {
    ret = sizeof(uint8_t);
  } else if (value > 0xFF + 13 && value <= 0xFFFF + 269) {
    ret = sizeof(uint16_t);
  }

  return ret;
}

/*
 * Splits given Opt Number/Length into base and ext values.
 *
 * Helper function.
 */
static int coap_split_opt(uint32_t value, uint8_t *base, uint16_t *ext) {
  int ret = 0;

  if (value < 13) {
    *base = value;
  } else if (value >= 13 && value <= 0xFF + 13) {
    *base = 13;
    *ext = value - 13;
    ret = sizeof(uint8_t);
  } else if (value > 0xFF + 13 && value <= 0xFFFF + 269) {
    *base = 14;
    *ext = value - 269;
    ret = sizeof(uint16_t);
  }

  return ret;
}

/*
 * Puts uint16_t (in network order) into given char stream.
 *
 * Helper function.
 */
static char *coap_add_uint16(char *ptr, uint16_t val) {
  *ptr = val >> 8;
  ptr++;
  *ptr = val & 0x00FF;
  ptr++;
  return ptr;
}

/*
 * Puts extended value of Opt Number/Length into given char stream.
 *
 * Helper function.
 */
static char *coap_add_opt_info(char *ptr, uint16_t val, size_t len) {
  if (len == sizeof(uint8_t)) {
    *ptr = val;
    ptr++;
  } else if (len == sizeof(uint16_t)) {
    ptr = coap_add_uint16(ptr, val);
  }

  return ptr;
}

/*
 * Verifies given ns_coap_message and calculates message size for it.
 *
 * Helper function.
 */
static uint32_t coap_calculate_packet_size(struct ns_coap_message *cm,
                                           size_t *len) {
  struct ns_coap_option *opt;
  uint32_t prev_opt_number;

  *len = 4; /* header */
  if (cm->msg_type > NS_COAP_MSG_MAX) {
    return NS_COAP_ERROR | NS_COAP_MSG_TYPE_FIELD;
  }
  if (cm->token.len > 8) {
    return NS_COAP_ERROR | NS_COAP_TOKEN_FIELD;
  }
  if (cm->code_class > 7) {
    return NS_COAP_ERROR | NS_COAP_CODE_CLASS_FIELD;
  }
  if (cm->code_detail > 31) {
    return NS_COAP_ERROR | NS_COAP_CODE_DETAIL_FIELD;
  }

  *len += cm->token.len;
  if (cm->payload.len != 0) {
    *len += cm->payload.len + 1; /* ... + 1; add payload marker */
  }

  opt = cm->options;
  prev_opt_number = 0;
  while (opt != NULL) {
    *len += 1; /* basic delta/length */
    *len += coap_get_ext_opt_size(opt->number);
    *len += coap_get_ext_opt_size((uint32_t) opt->value.len);
    /*
     * Current implementation performs check if
     * option_number > previous option_number and produces an error
     * TODO(alashkin): write design doc with limitations
     * May be resorting is more suitable solution.
     */
    if ((opt->next != NULL && opt->number > opt->next->number) ||
        opt->value.len > 0xFFFF + 269 ||
        opt->number - prev_opt_number > 0xFFFF + 269) {
      return NS_COAP_ERROR | NS_COAP_OPTIONS_FIELD;
    }
    *len += opt->value.len;
    opt = opt->next;
  }

  return 0;
}

uint32_t ns_coap_compose(struct ns_coap_message *cm, struct mbuf *io) {
  struct ns_coap_option *opt;
  uint32_t res, prev_opt_number;
  size_t prev_io_len, packet_size;
  char *ptr;

  res = coap_calculate_packet_size(cm, &packet_size);
  if (res != 0) {
    return res;
  }

  /* saving previous lenght to handle non-empty mbuf */
  prev_io_len = io->len;
  mbuf_append(io, NULL, packet_size);
  ptr = io->buf + prev_io_len;

  /*
   * since cm is verified, it is possible to use bits shift operator
   * without additional zeroing of unused bits
   */

  /* ver: 2 bits, msg_type: 2 bits, toklen: 4 bits */
  *ptr = (1 << 6) | (cm->msg_type << 4) | (cm->token.len);
  ptr++;

  /* code class: 3 bits, code detail: 5 bits */
  *ptr = (cm->code_class << 5) | (cm->code_detail);
  ptr++;

  ptr = coap_add_uint16(ptr, cm->msg_id);

  if (cm->token.len != 0) {
    memcpy(ptr, cm->token.p, cm->token.len);
    ptr += cm->token.len;
  }

  opt = cm->options;
  prev_opt_number = 0;
  while (opt != NULL) {
    uint8_t delta_base = 0, length_base = 0;
    uint16_t delta_ext, length_ext;

    size_t opt_delta_len =
        coap_split_opt(opt->number - prev_opt_number, &delta_base, &delta_ext);
    size_t opt_lenght_len =
        coap_split_opt((uint32_t) opt->value.len, &length_base, &length_ext);

    *ptr = (delta_base << 4) | length_base;
    ptr++;

    ptr = coap_add_opt_info(ptr, delta_ext, opt_delta_len);
    ptr = coap_add_opt_info(ptr, length_ext, opt_lenght_len);

    if (opt->value.len != 0) {
      memcpy(ptr, opt->value.p, opt->value.len);
      ptr += opt->value.len;
    }

    prev_opt_number = opt->number;
    opt = opt->next;
  }

  if (cm->payload.len != 0) {
    *ptr = 0xFF;
    ptr++;
    memcpy(ptr, cm->payload.p, cm->payload.len);
  }

  return 0;
}

uint32_t ns_coap_send_message(struct ns_connection *nc,
                              struct ns_coap_message *cm) {
  struct mbuf packet_out;
  int send_res;
  uint32_t compose_res;

  mbuf_init(&packet_out, 0);
  compose_res = ns_coap_compose(cm, &packet_out);
  if (compose_res != 0) {
    return compose_res; /* LCOV_EXCL_LINE */
  }

  send_res = ns_send(nc, packet_out.buf, (int) packet_out.len);
  mbuf_free(&packet_out);

  if (send_res == 0) {
    /*
     * in case of UDP ns_send tries to send immediately
     * and could return an error.
     */
    return NS_COAP_NETWORK_ERROR; /* LCOV_EXCL_LINE */
  }

  return 0;
}

uint32_t ns_coap_send_ack(struct ns_connection *nc, uint16_t msg_id) {
  struct ns_coap_message cm;
  memset(&cm, 0, sizeof(cm));
  cm.msg_type = NS_COAP_MSG_ACK;
  cm.msg_id = msg_id;

  return ns_coap_send_message(nc, &cm);
}

static void coap_handler(struct ns_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;
  struct ns_coap_message cm;
  uint32_t parse_res;

  memset(&cm, 0, sizeof(cm));

  nc->handler(nc, ev, ev_data);

  switch (ev) {
    case NS_RECV:
      parse_res = ns_coap_parse(io, &cm);
      if ((parse_res & NS_COAP_IGNORE) == 0) {
        if ((cm.flags & NS_COAP_NOT_ENOUGH_DATA) != 0) {
          /*
           * Since we support UDP only
           * NS_COAP_NOT_ENOUGH_DATA == NS_COAP_FORMAT_ERROR
           */
          cm.flags |= NS_COAP_FORMAT_ERROR; /* LCOV_EXCL_LINE */
        }                                   /* LCOV_EXCL_LINE */
        nc->handler(nc, NS_COAP_EVENT_BASE + cm.msg_type, &cm);
      }

      ns_coap_free_options(&cm);
      mbuf_remove(io, io->len);
      break;
  }
}
/*
 * Attach built-in CoAP event handler to the given connection.
 *
 * The user-defined event handler will receive following extra events:
 *
 * - NS_COAP_CON
 * - NS_COAP_NOC
 * - NS_COAP_ACK
 * - NS_COAP_RST
 */
int ns_set_protocol_coap(struct ns_connection *nc) {
  /* supports UDP only */
  if ((nc->flags & NSF_UDP) == 0) {
    return -1;
  }

  nc->proto_handler = coap_handler;

  return 0;
}

#endif /* NS_DISABLE_COAP */
