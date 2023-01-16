/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Util.h"

#include "../common.h"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../platform/Platform.h"
#include "../title/TitleScreen.h"
#include "zlib.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <random>

int32_t squaredmetres_to_squaredfeet(int32_t squaredMetres)
{
    // 1 metre squared = 10.7639104 feet squared
    // RCT2 approximates as 11
    return squaredMetres * 11;
}

int32_t metres_to_feet(int32_t metres)
{
    // 1 metre = 3.2808399 feet
    // RCT2 approximates as 3.28125
    return (metres * 840) / 256;
}

int32_t mph_to_kmph(int32_t mph)
{
    // 1 mph = 1.60934 kmph
    // RCT2 approximates as 1.609375
    return (mph * 1648) >> 10;
}

int32_t mph_to_dmps(int32_t mph)
{
    // 1 mph = 4.4704 decimeters/s
    return (mph * 73243) >> 14;
}

int32_t bitscanforward(int32_t source)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400) // Visual Studio 2005
    DWORD i;
    uint8_t success = _BitScanForward(&i, static_cast<uint32_t>(source));
    return success != 0 ? i : -1;
#elif defined(__GNUC__)
    int32_t success = __builtin_ffs(source);
    return success - 1;
#else
#    pragma message("Falling back to iterative bitscan forward, consider using intrinsics")
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (int32_t i = 0; i < 32; i++)
        if (source & (1u << i))
            return i;

    return -1;
#endif
}

int32_t bitscanforward(int64_t source)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400) && defined(_M_X64) // Visual Studio 2005
    DWORD i;
    uint8_t success = _BitScanForward64(&i, static_cast<uint64_t>(source));
    return success != 0 ? i : -1;
#elif defined(__GNUC__)
    int32_t success = __builtin_ffsll(source);
    return success - 1;
#else
#    pragma message("Falling back to iterative bitscan forward, consider using intrinsics")
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (int32_t i = 0; i < 64; i++)
        if (source & (1uLL << i))
            return i;

    return -1;
#endif
}

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#    include <cpuid.h>
#    define OpenRCT2_CPUID_GNUC_X86
#elif defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_X64) || defined(_M_IX86)) // VS2008
#    include <intrin.h>
#    include <nmmintrin.h>
#    define OpenRCT2_CPUID_MSVC_X86
#endif

#ifdef OPENRCT2_X86
static bool cpuid_x86(uint32_t* cpuid_outdata, int32_t eax)
{
#    if defined(OpenRCT2_CPUID_GNUC_X86)
    int ret = __get_cpuid(eax, &cpuid_outdata[0], &cpuid_outdata[1], &cpuid_outdata[2], &cpuid_outdata[3]);
    return ret == 1;
#    elif defined(OpenRCT2_CPUID_MSVC_X86)
    __cpuid(reinterpret_cast<int*>(cpuid_outdata), static_cast<int>(eax));
    return true;
#    else
    return false;
#    endif
}
#endif // OPENRCT2_X86

bool sse41_available()
{
#ifdef OPENRCT2_X86
    // SSE4.1 support is declared as the 19th bit of ECX with CPUID(EAX = 1).
    uint32_t regs[4] = { 0 };
    if (cpuid_x86(regs, 1))
    {
        return (regs[2] & (1 << 19));
    }
#endif
    return false;
}

bool avx2_available()
{
#ifdef OPENRCT2_X86
    // For GCC and similar use the builtin function, as cpuid changed its semantics in
    // https://github.com/gcc-mirror/gcc/commit/132fa33ce998df69a9f793d63785785f4b93e6f1
    // which causes it to ignore subleafs, but the new function is unavailable on
    // Ubuntu 18.04's toolchains.
#    if defined(OpenRCT2_CPUID_GNUC_X86) && (!defined(__FreeBSD__) || (__FreeBSD__ > 10))
    return __builtin_cpu_supports("avx2");
#    else
    // AVX2 support is declared as the 5th bit of EBX with CPUID(EAX = 7, ECX = 0).
    uint32_t regs[4] = { 0 };
    if (cpuid_x86(regs, 7))
    {
        bool avxCPUSupport = (regs[1] & (1 << 5)) != 0;
        if (avxCPUSupport)
        {
            // Need to check if OS also supports the register of xmm/ymm
            // This check has to be conditional, otherwise INVALID_INSTRUCTION exception.
            uint64_t xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
            avxCPUSupport = (xcrFeatureMask & 0x6) || false;
        }
        return avxCPUSupport;
    }
#    endif
#endif
    return false;
}

static bool bitcount_popcnt_available()
{
#ifdef OPENRCT2_X86
    // POPCNT support is declared as the 23rd bit of ECX with CPUID(EAX = 1).
    uint32_t regs[4] = { 0 };
    if (cpuid_x86(regs, 1))
    {
        return (regs[2] & (1 << 23));
    }
#endif
    return false;
}

static int32_t bitcount_popcnt(uint32_t source)
{
// Use CPUID defines to figure out calling style
#if defined(OpenRCT2_CPUID_GNUC_X86)
    // use asm directly in order to actually emit the instruction : using
    // __builtin_popcount results in an extra call to a library function.
    int32_t rv;
    asm volatile("popcnt %1,%0" : "=r"(rv) : "rm"(source) : "cc");
    return rv;
#elif defined(OpenRCT2_CPUID_MSVC_X86)
    return _mm_popcnt_u32(source);
#else
    openrct2_assert(false, "bitcount_popcnt() called, without support compiled in");
    return INT_MAX;
#endif
}

static int32_t bitcount_lut(uint32_t source)
{
    // https://graphics.stanford.edu/~seander/bithacks.html
    static constexpr const uint8_t BitsSetTable256[256] = {
#define B2(n) n, (n) + 1, (n) + 1, (n) + 2
#define B4(n) B2(n), B2((n) + 1), B2((n) + 1), B2((n) + 2)
#define B6(n) B4(n), B4((n) + 1), B4((n) + 1), B4((n) + 2)
        B6(0), B6(1), B6(1), B6(2)
    };
    return BitsSetTable256[source & 0xff] + BitsSetTable256[(source >> 8) & 0xff] + BitsSetTable256[(source >> 16) & 0xff]
        + BitsSetTable256[source >> 24];
}

static int32_t (*bitcount_fn)(uint32_t);

void bitcount_init()
{
    bitcount_fn = bitcount_popcnt_available() ? bitcount_popcnt : bitcount_lut;
}

int32_t bitcount(uint32_t source)
{
    return bitcount_fn(source);
}

/* Case insensitive logical compare */
// Example:
// - Guest 10
// - Guest 99
// - Guest 100
// - John v2.0
// - John v2.1
int32_t strlogicalcmp(const char* s1, const char* s2)
{
    for (;;)
    {
        if (*s2 == '\0')
            return *s1 != '\0';
        if (*s1 == '\0')
            return -1;
        if (!(isdigit(static_cast<unsigned char>(*s1)) && isdigit(static_cast<unsigned char>(*s2))))
        {
            if (toupper(*s1) != toupper(*s2))
                return toupper(*s1) - toupper(*s2);

            ++s1;
            ++s2;
        }
        else
        {
            char *lim1, *lim2;
            unsigned long n1 = strtoul(s1, &lim1, 10);
            unsigned long n2 = strtoul(s2, &lim2, 10);
            if (n1 > n2)
                return 1;
            if (n1 < n2)
                return -1;

            s1 = lim1;
            s2 = lim2;
        }
    }
}

char* safe_strcpy(char* destination, const char* source, size_t size)
{
    assert(destination != nullptr);
    assert(source != nullptr);

    if (size == 0)
        return destination;

    char* result = destination;

    bool truncated = false;
    const char* sourceLimit = source + size - 1;
    const char* ch = source;
    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(ch, &ch)) != 0)
    {
        if (ch <= sourceLimit)
        {
            destination = UTF8WriteCodepoint(destination, codepoint);
        }
        else
        {
            truncated = true;
        }
    }
    *destination = 0;

    if (truncated)
    {
        log_warning("Truncating string \"%s\" to %d bytes.", result, size);
    }
    return result;
}

char* safe_strcat(char* destination, const char* source, size_t size)
{
    assert(destination != nullptr);
    assert(source != nullptr);

    if (size == 0)
    {
        return destination;
    }

    char* result = destination;

    size_t i;
    for (i = 0; i < size; i++)
    {
        if (*destination == '\0')
        {
            break;
        }

        destination++;
    }

    bool terminated = false;
    for (; i < size; i++)
    {
        if (*source != '\0')
        {
            *destination++ = *source++;
        }
        else
        {
            *destination = *source;
            terminated = true;
            break;
        }
    }

    if (!terminated)
    {
        result[size - 1] = '\0';
        log_warning("Truncating string \"%s\" to %d bytes.", result, size);
    }

    return result;
}

uint32_t util_rand()
{
    thread_local std::mt19937 _prng(std::random_device{}());
    return _prng();
}

// Returns a random floating point number from the Standard Normal Distribution; mean of 0 and standard deviation of 1.
// TODO: In C++20 this can be templated, where the standard deviation is passed as a value template argument.
float util_rand_normal_distributed()
{
    thread_local std::mt19937 _prng{ std::random_device{}() };
    thread_local std::normal_distribution<float> _distributor{ 0.0f, 1.0f };
    return _distributor(_prng);
}

constexpr size_t CHUNK = 128 * 1024;

// Compress the source to gzip-compatible stream, write to dest.
// Mainly used for compressing the crashdumps
bool util_gzip_compress(FILE* source, FILE* dest)
{
    if (source == nullptr || dest == nullptr)
    {
        return false;
    }
    int ret, flush;
    size_t have;
    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    int windowBits = 15;
    int GZIP_ENCODING = 16;
    ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
    {
        log_error("Failed to initialise stream");
        return false;
    }
    do
    {
        strm.avail_in = uInt(fread(in, 1, CHUNK, source));
        if (ferror(source))
        {
            deflateEnd(&strm);
            log_error("Failed to read data from source");
            return false;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            if (ret == Z_STREAM_ERROR)
            {
                log_error("Failed to compress data");
                return false;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest))
            {
                deflateEnd(&strm);
                log_error("Failed to write data to destination");
                return false;
            }
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);
    deflateEnd(&strm);
    return true;
}

std::vector<uint8_t> Gzip(const void* data, const size_t dataLen)
{
    assert(data != nullptr);

    std::vector<uint8_t> output;
    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    {
        const auto ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK)
        {
            throw std::runtime_error("deflateInit2 failed with error " + std::to_string(ret));
        }
    }

    int flush = 0;
    const auto* src = static_cast<const Bytef*>(data);
    size_t srcRemaining = dataLen;
    do
    {
        const auto nextBlockSize = std::min(srcRemaining, CHUNK);
        srcRemaining -= nextBlockSize;

        flush = srcRemaining == 0 ? Z_FINISH : Z_NO_FLUSH;
        strm.avail_in = static_cast<uInt>(nextBlockSize);
        strm.next_in = const_cast<Bytef*>(src);
        do
        {
            output.resize(output.size() + nextBlockSize);
            strm.avail_out = static_cast<uInt>(nextBlockSize);
            strm.next_out = &output[output.size() - nextBlockSize];
            const auto ret = deflate(&strm, flush);
            if (ret == Z_STREAM_ERROR)
            {
                throw std::runtime_error("deflate failed with error " + std::to_string(ret));
            }
            output.resize(output.size() - strm.avail_out);
        } while (strm.avail_out == 0);

        src += nextBlockSize;
    } while (flush != Z_FINISH);
    deflateEnd(&strm);
    return output;
}

std::vector<uint8_t> Ungzip(const void* data, const size_t dataLen)
{
    assert(data != nullptr);

    std::vector<uint8_t> output;
    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    {
        const auto ret = inflateInit2(&strm, 15 | 16);
        if (ret != Z_OK)
        {
            throw std::runtime_error("inflateInit2 failed with error " + std::to_string(ret));
        }
    }

    int flush = 0;
    const auto* src = static_cast<const Bytef*>(data);
    size_t srcRemaining = dataLen;
    do
    {
        const auto nextBlockSize = std::min(srcRemaining, CHUNK);
        srcRemaining -= nextBlockSize;

        flush = srcRemaining == 0 ? Z_FINISH : Z_NO_FLUSH;
        strm.avail_in = static_cast<uInt>(nextBlockSize);
        strm.next_in = const_cast<Bytef*>(src);
        do
        {
            output.resize(output.size() + nextBlockSize);
            strm.avail_out = static_cast<uInt>(nextBlockSize);
            strm.next_out = &output[output.size() - nextBlockSize];
            const auto ret = inflate(&strm, flush);
            if (ret == Z_STREAM_ERROR)
            {
                throw std::runtime_error("deflate failed with error " + std::to_string(ret));
            }
            output.resize(output.size() - strm.avail_out);
        } while (strm.avail_out == 0);

        src += nextBlockSize;
    } while (flush != Z_FINISH);
    inflateEnd(&strm);
    return output;
}

// Type-independent code left as macro to reduce duplicate code.
#define add_clamp_body(value, value_to_add, min_cap, max_cap)                                                                  \
    if ((value_to_add > 0) && (value > (max_cap - (value_to_add))))                                                            \
    {                                                                                                                          \
        value = max_cap;                                                                                                       \
    }                                                                                                                          \
    else if ((value_to_add < 0) && (value < (min_cap - (value_to_add))))                                                       \
    {                                                                                                                          \
        value = min_cap;                                                                                                       \
    }                                                                                                                          \
    else                                                                                                                       \
    {                                                                                                                          \
        value += value_to_add;                                                                                                 \
    }

int8_t add_clamp_int8_t(int8_t value, int8_t value_to_add)
{
    add_clamp_body(value, value_to_add, INT8_MIN, INT8_MAX);
    return value;
}

int16_t add_clamp_int16_t(int16_t value, int16_t value_to_add)
{
    add_clamp_body(value, value_to_add, INT16_MIN, INT16_MAX);
    return value;
}

int32_t add_clamp_int32_t(int32_t value, int32_t value_to_add)
{
    add_clamp_body(value, value_to_add, INT32_MIN, INT32_MAX);
    return value;
}

int64_t add_clamp_int64_t(int64_t value, int64_t value_to_add)
{
    add_clamp_body(value, value_to_add, INT64_MIN, INT64_MAX);
    return value;
}

money16 add_clamp_money16(money16 value, money16 value_to_add)
{
    // This function is intended only for clarity, as money16
    // is technically the same as int16_t
    assert_struct_size(money16, sizeof(int16_t));
    return add_clamp_int16_t(value, value_to_add);
}

money32 add_clamp_money32(money32 value, money32 value_to_add)
{
    // This function is intended only for clarity, as money32
    // is technically the same as int32_t
    assert_struct_size(money32, sizeof(int32_t));
    return add_clamp_int32_t(value, value_to_add);
}

money64 add_clamp_money64(money64 value, money64 value_to_add)
{
    // This function is intended only for clarity, as money64
    // is technically the same as int64_t
    assert_struct_size(money64, sizeof(int64_t));
    return add_clamp_int64_t(value, value_to_add);
}

#undef add_clamp_body

uint8_t lerp(uint8_t a, uint8_t b, float t)
{
    if (t <= 0)
        return a;
    if (t >= 1)
        return b;

    int32_t range = b - a;
    int32_t amount = static_cast<int32_t>(range * t);
    return static_cast<uint8_t>(a + amount);
}

float flerp(float a, float b, float f)
{
    return (a * (1.0f - f)) + (b * f);
}

uint8_t soft_light(uint8_t a, uint8_t b)
{
    float fa = a / 255.0f;
    float fb = b / 255.0f;
    float fr;
    if (fb < 0.5f)
    {
        fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
    }
    else
    {
        fr = (2 * fa * (1 - fb)) + (std::sqrt(fa) * ((2 * fb) - 1));
    }
    return static_cast<uint8_t>(std::clamp(fr, 0.0f, 1.0f) * 255.0f);
}

/**
 * strftime wrapper which appends to an existing string.
 */
size_t strcatftime(char* buffer, size_t bufferSize, const char* format, const struct tm* tp)
{
    size_t stringLen = strnlen(buffer, bufferSize);
    if (stringLen < bufferSize)
    {
        char* dst = buffer + stringLen;
        size_t dstMaxSize = bufferSize - stringLen;
        return strftime(dst, dstMaxSize, format, tp);
    }
    return 0;
}
