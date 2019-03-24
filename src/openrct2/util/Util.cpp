/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Util.h"

#include "../common.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../platform/platform.h"
#include "../title/TitleScreen.h"
#include "zlib.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>

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

bool filename_valid_characters(const utf8* filename)
{
    for (int32_t i = 0; filename[i] != '\0'; i++)
    {
        if (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':' || filename[i] == '?' || filename[i] == '*'
            || filename[i] == '<' || filename[i] == '>' || filename[i] == '|')
            return false;
    }
    return true;
}

utf8* path_get_directory(const utf8* path)
{
    // Find the last slash or backslash in the path
    char* filename = (char*)strrchr(path, *PATH_SEPARATOR);
    char* filename_posix = (char*)strrchr(path, '/');
    filename = filename < filename_posix ? filename_posix : filename;

    // If the path is invalid (e.g. just a file name), return NULL
    if (filename == nullptr)
    {
        return nullptr;
    }

    char* directory = _strdup(path);
    safe_strtrunc(directory, strlen(path) - strlen(filename) + 2);

    return directory;
}

const char* path_get_filename(const utf8* path)
{
    // Find last slash or backslash in the path
    char* filename = (char*)strrchr(path, *PATH_SEPARATOR);
    char* filename_posix = (char*)strchr(path, '/');
    filename = filename < filename_posix ? filename_posix : filename;

    // Checks if the path is valid (e.g. not just a file name)
    if (filename == nullptr)
    {
        // Return the input string to keep things working
        return path;
    }

    // Increase pointer by one, to get rid of the slashes
    filename++;

    return filename;
}

// Returns the extension (dot inclusive) from the given path, or the end of the
// string when no extension was found.
const char* path_get_extension(const utf8* path)
{
    // Get the filename from the path
    const char* filename = path_get_filename(path);

    // Try to find the most-right dot in the filename
    char* extension = (char*)strrchr(filename, '.');

    // When no dot was found, return a pointer to the null-terminator
    if (extension == nullptr)
        extension = (char*)strrchr(filename, '\0');

    return extension;
}

void path_set_extension(utf8* path, const utf8* newExtension, size_t size)
{
    // Remove existing extension (check first if there is one)
    if (path_get_extension(path) < strrchr(path, '\0'))
        path_remove_extension(path);
    // Append new extension
    path_append_extension(path, newExtension, size);
}

void path_append_extension(utf8* path, const utf8* newExtension, size_t size)
{
    // Skip to the dot if the extension starts with a pattern (starts with "*.")
    if (newExtension[0] == '*')
        newExtension++;

    // Append a dot to the filename if the new extension doesn't start with it
    if (newExtension[0] != '.')
        safe_strcat(path, ".", size);

    // Append the extension to the path
    safe_strcat(path, newExtension, size);
}

void path_remove_extension(utf8* path)
{
    // Find last dot in filename, and replace it with a null-terminator
    char* lastDot = (char*)strrchr(path_get_filename(path), '.');
    if (lastDot != nullptr)
        *lastDot = '\0';
    else
        log_warning("No extension found. (path = %s)", path);
}

void path_end_with_separator(utf8* path, size_t size)
{
    size_t length = strnlen(path, size);
    if (length >= size - 1)
        return;

    if ((length == 0) || ((path[length - 1] != *PATH_SEPARATOR) && path[length - 1] != '/'))
    {
        safe_strcat(path, PATH_SEPARATOR, size);
    }
}

int32_t bitscanforward(int32_t source)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400) // Visual Studio 2005
    DWORD i;
    uint8_t success = _BitScanForward(&i, (uint32_t)source);
    return success != 0 ? i : -1;
#elif defined(__GNUC__)
    int32_t success = __builtin_ffs(source);
    return success - 1;
#else
#    pragma message "Falling back to iterative bitscan forward, consider using intrinsics"
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (int32_t i = 0; i < 32; i++)
        if (source & (1u << i))
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
    __cpuid((int*)cpuid_outdata, (int)eax);
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
// which causes it to ignore subleafs, but the new function is unavailable on Ubuntu's
// prehistoric toolchains
#    if defined(OpenRCT2_CPUID_GNUC_X86) && (!defined(__FreeBSD__) || (__FreeBSD__ > 10))
    return __builtin_cpu_supports("avx2");
#    else
    // AVX2 support is declared as the 5th bit of EBX with CPUID(EAX = 7, ECX = 0).
    uint32_t regs[4] = { 0 };
    if (cpuid_x86(regs, 7))
    {
        return (regs[1] & (1 << 5));
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

bool strequals(const char* a, const char* b, int32_t length, bool caseInsensitive)
{
    return caseInsensitive ? _strnicmp(a, b, length) == 0 : strncmp(a, b, length) == 0;
}

/* case insensitive compare */
int32_t strcicmp(char const* a, char const* b)
{
    for (;; a++, b++)
    {
        int32_t d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

/* Case insensitive logical compare */
// Example:
// - Guest 10
// - Guest 99
// - Guest 100
// - John v2.0
// - John v2.1
int32_t strlogicalcmp(char const* a, char const* b)
{
    for (;; a++, b++)
    {
        int32_t result = tolower(*a) - tolower(*b);
        bool both_numeric = *a >= '0' && *a <= '9' && *b >= '0' && *b <= '9';
        if (result != 0 || !*a || both_numeric)
        { // difference found || end of string
            if (both_numeric)
            { // a and b both start with a number
                // Get the numbers in the string at current positions
                int32_t na = 0, nb = 0;
                for (; *a >= '0' && *a <= '9'; a++)
                {
                    na *= 10;
                    na += *a - '0';
                }
                for (; *b >= '0' && *b <= '9'; b++)
                {
                    nb *= 10;
                    nb += *b - '0';
                }
                // In case the numbers are the same
                if (na == nb)
                    continue;
                return na - nb;
            }
            else
            {
                return result;
            }
        }
    }
}

utf8* safe_strtrunc(utf8* text, size_t size)
{
    assert(text != nullptr);

    if (size == 0)
        return text;

    const char* sourceLimit = text + size - 1;
    char* ch = text;
    char* last = text;
    uint32_t codepoint;
    while ((codepoint = utf8_get_next(ch, (const utf8**)&ch)) != 0)
    {
        if (ch <= sourceLimit)
        {
            last = ch;
        }
        else
        {
            break;
        }
    }
    *last = 0;

    return text;
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
    while ((codepoint = utf8_get_next(ch, &ch)) != 0)
    {
        if (ch <= sourceLimit)
        {
            destination = utf8_write_codepoint(destination, codepoint);
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
        else
        {
            destination++;
        }
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

char* safe_strcat_path(char* destination, const char* source, size_t size)
{
    path_end_with_separator(destination, size);
    if (source[0] == *PATH_SEPARATOR)
    {
        source = source + 1;
    }
    return safe_strcat(destination, source, size);
}

char* safe_strtrimleft(char* destination, const char* source, size_t size)
{
    while (*source == ' ')
    {
        source++;
    }
    return safe_strcpy(destination, source, size);
}

#if defined(_WIN32)
char* strcasestr(const char* haystack, const char* needle)
{
    const char* p1 = haystack;
    const char* p2 = needle;
    const char* r = *p2 == 0 ? haystack : nullptr;

    while (*p1 != 0 && *p2 != 0)
    {
        if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
        {
            if (r == nullptr)
                r = p1;
            p2++;
        }
        else
        {
            p2 = needle;
            if (r != nullptr)
                p1 = r + 1;

            if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
            {
                r = p1;
                p2++;
            }
            else
            {
                r = nullptr;
            }
        }

        p1++;
    }

    return *p2 == 0 ? (char*)r : nullptr;
}
#endif

bool utf8_is_bom(const char* str)
{
    return str[0] == (char)(uint8_t)0xEF && str[1] == (char)(uint8_t)0xBB && str[2] == (char)(uint8_t)0xBF;
}

bool str_is_null_or_empty(const char* str)
{
    return str == nullptr || str[0] == 0;
}

void util_srand(int32_t source)
{
    srand(source);
}

// Caveat: rand() might only return values up to 0x7FFF, which is the minimum specified in the C standard.
uint32_t util_rand()
{
    return rand();
}

#define CHUNK (128 * 1024)
#define MAX_ZLIB_REALLOC (4 * 1024 * 1024)

/**
 * @brief Inflates zlib-compressed data
 * @param data Data to be decompressed
 * @param data_in_size Size of data to be decompressed
 * @param data_out_size Pointer to a variable where output size will be written. If not 0, it will be used to set initial output
 * buffer size.
 * @return Returns a pointer to memory holding decompressed data or NULL on failure.
 * @note It is caller's responsibility to free() the returned pointer once done with it.
 */
uint8_t* util_zlib_inflate(uint8_t* data, size_t data_in_size, size_t* data_out_size)
{
    int32_t ret = Z_OK;
    uLongf out_size = (uLong)*data_out_size;
    if (out_size == 0)
    {
        // Try to guesstimate the size needed for output data by applying the
        // same ratio it would take to compress data_in_size.
        out_size = (uLong)data_in_size * (uLong)data_in_size / compressBound((uLong)data_in_size);
        out_size = std::min((uLongf)MAX_ZLIB_REALLOC, out_size);
    }
    uLongf buffer_size = out_size;
    uint8_t* buffer = (uint8_t*)malloc(buffer_size);
    do
    {
        if (ret == Z_BUF_ERROR)
        {
            buffer_size *= 2;
            out_size = buffer_size;
            buffer = (uint8_t*)realloc(buffer, buffer_size);
        }
        else if (ret == Z_STREAM_ERROR)
        {
            log_error("Your build is shipped with broken zlib. Please use the official build.");
            free(buffer);
            return nullptr;
        }
        else if (ret < 0)
        {
            log_error("Error uncompressing data.");
            free(buffer);
            return nullptr;
        }
        ret = uncompress(buffer, &out_size, data, (uLong)data_in_size);
    } while (ret != Z_OK);
    buffer = (uint8_t*)realloc(buffer, out_size);
    *data_out_size = out_size;
    return buffer;
}

/**
 * @brief Deflates input using zlib
 * @param data Data to be compressed
 * @param data_in_size Size of data to be compressed
 * @param data_out_size Pointer to a variable where output size will be written
 * @return Returns a pointer to memory holding compressed data or NULL on failure.
 * @note It is caller's responsibility to free() the returned pointer once done with it.
 */
uint8_t* util_zlib_deflate(const uint8_t* data, size_t data_in_size, size_t* data_out_size)
{
    int32_t ret = Z_OK;
    uLongf out_size = (uLongf)*data_out_size;
    uLong buffer_size = compressBound((uLong)data_in_size);
    uint8_t* buffer = (uint8_t*)malloc(buffer_size);
    do
    {
        if (ret == Z_BUF_ERROR)
        {
            buffer_size *= 2;
            out_size = buffer_size;
            buffer = (uint8_t*)realloc(buffer, buffer_size);
        }
        else if (ret == Z_STREAM_ERROR)
        {
            log_error("Your build is shipped with broken zlib. Please use the official build.");
            free(buffer);
            return nullptr;
        }
        ret = compress(buffer, &out_size, data, (uLong)data_in_size);
    } while (ret != Z_OK);
    *data_out_size = out_size;
    buffer = (uint8_t*)realloc(buffer, *data_out_size);
    return buffer;
}

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

money32 add_clamp_money32(money32 value, money32 value_to_add)
{
    // This function is intended only for clarity, as money32
    // is technically the same as int32_t
    assert_struct_size(money32, sizeof(int32_t));
    return add_clamp_int32_t(value, value_to_add);
}

#undef add_clamp_body

uint8_t lerp(uint8_t a, uint8_t b, float t)
{
    if (t <= 0)
        return a;
    if (t >= 1)
        return b;

    int32_t range = b - a;
    int32_t amount = (int32_t)(range * t);
    return (uint8_t)(a + amount);
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
    return (uint8_t)(std::clamp(fr, 0.0f, 1.0f) * 255.0f);
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
