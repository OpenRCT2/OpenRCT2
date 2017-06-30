#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <ctype.h>
#include <time.h>
#include "../common.h"
#include "../core/Guard.hpp"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../title/TitleScreen.h"
#include "util.h"
#include "zlib.h"

sint32 squaredmetres_to_squaredfeet(sint32 squaredMetres)
{
    // 1 metre squared = 10.7639104 feet squared
    // RCT2 approximates as 11
    return squaredMetres * 11;
}

sint32 metres_to_feet(sint32 metres)
{
    // 1 metre = 3.2808399 feet
    // RCT2 approximates as 3.28125
    return (metres * 840) / 256;
}

sint32 mph_to_kmph(sint32 mph)
{
    // 1 mph = 1.60934 kmph
    // RCT2 approximates as 1.609375
    return (mph * 1648) >> 10;
}

sint32 mph_to_dmps(sint32 mph)
{
    // 1 mph = 4.4704 decimeters/s
    return (mph * 73243) >> 14;
}

bool filename_valid_characters(const utf8 *filename)
{
    for (sint32 i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':' || filename[i] == '?' ||
            filename[i] == '*' || filename[i] == '<' || filename[i] == '>' || filename[i] == '|')
            return false;
    }
    return true;
}

utf8 *path_get_directory(const utf8 *path)
{
    // Find the last slash or backslash in the path
    char *filename = strrchr(path, *PATH_SEPARATOR);

    // If the path is invalid (e.g. just a file name), return NULL
    if (filename == NULL)
        return NULL;

    char *directory = _strdup(path);
    safe_strtrunc(directory, strlen(path) - strlen(filename) + 2);

    return directory;
}

const char *path_get_filename(const utf8 *path)
{
    // Find last slash or backslash in the path
    char *filename = strrchr(path, *PATH_SEPARATOR);

    // Checks if the path is valid (e.g. not just a file name)
    if (filename == NULL)
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
const char *path_get_extension(const utf8 *path)
{
    // Get the filename from the path
    const char *filename = path_get_filename(path);

    // Try to find the most-right dot in the filename
    char *extension = strrchr(filename, '.');

    // When no dot was found, return a pointer to the null-terminator
    if (extension == NULL)
        extension = strrchr(filename, '\0');

    return extension;
}

void path_set_extension(utf8 *path, const utf8 *newExtension, size_t size)
{
    // Remove existing extension (check first if there is one)
    if (path_get_extension(path) < strrchr(path, '\0'))
        path_remove_extension(path);
    // Append new extension
    path_append_extension(path, newExtension, size);
}

void path_append_extension(utf8 *path, const utf8 *newExtension, size_t size)
{
    // Append a dot to the filename if the new extension doesn't start with it
    if (newExtension[0] != '.')
        safe_strcat(path, ".", size);

    // Append the extension to the path
    safe_strcat(path, newExtension, size);
}

void path_remove_extension(utf8 *path)
{
    // Find last dot in filename, and replace it with a null-terminator
    char *lastDot = strrchr(path_get_filename(path), '.');
    if (lastDot != NULL)
        *lastDot = '\0';
    else
        log_warning("No extension found. (path = %s)", path);
}

void path_end_with_separator(utf8 *path, size_t size) {
    size_t length = strnlen(path, size);
    if (length >= size - 1) return;

    if ((length == 0) || (path[length - 1] != *PATH_SEPARATOR))
        safe_strcat(path, PATH_SEPARATOR, size);
}

sint32 bitscanforward(sint32 source)
{
    #if defined(_MSC_VER) && (_MSC_VER >= 1400) // Visual Studio 2005
        DWORD i;
        uint8 success = _BitScanForward(&i, (uint32)source);
        return success != 0 ? i : -1;
    #elif defined(__GNUC__)
        sint32 success = __builtin_ffs(source);
        return success - 1;
    #else
    #pragma message "Falling back to iterative bitscan forward, consider using intrinsics"
    // This is a low-hanging optimisation boost, check if your compiler offers
    // any intrinsic.
    // cf. https://github.com/OpenRCT2/OpenRCT2/pull/2093
    for (sint32 i = 0; i < 32; i++)
        if (source & (1u << i))
            return i;

    return -1;
    #endif
}

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #include <cpuid.h>
    #define OpenRCT2_POPCNT_GNUC
#elif defined(_MSC_VER) && (_MSC_VER >= 1500) && (defined(_M_X64) || defined(_M_IX86)) // VS2008
    #include <nmmintrin.h>
    #define OpenRCT2_POPCNT_MSVC
#endif

static bool bitcount_popcnt_available()
{
    // POPCNT support is declared as the 23rd bit of ECX with CPUID(EAX = 1).
    #if defined(OpenRCT2_POPCNT_GNUC)
        // we could use __builtin_cpu_supports, but it requires runtime support from
        // the compiler's library, which clang doesn't have yet.
        uint32 eax, ebx, ecx = 0, edx; // avoid "maybe uninitialized"
        __get_cpuid(1, &eax, &ebx, &ecx, &edx);
        return (ecx & (1 << 23));
    #elif defined(OpenRCT2_POPCNT_MSVC)
        sint32 regs[4];
        __cpuid(regs, 1);
        return (regs[2] & (1 << 23));
    #else
        return false;
    #endif
}

static sint32 bitcount_popcnt(uint32 source)
{
    #if defined(OpenRCT2_POPCNT_GNUC)
        // use asm directly in order to actually emit the instruction : using
        // __builtin_popcount results in an extra call to a library function.
        sint32 rv;
        asm volatile ("popcnt %1,%0" : "=r"(rv) : "rm"(source) : "cc");
        return rv;
    #elif defined(OpenRCT2_POPCNT_MSVC)
        return _mm_popcnt_u32(source);
    #else
        openrct2_assert(false, "bitcount_popcnt() called, without support compiled in");
        return INT_MAX;
    #endif
}

static sint32 bitcount_lut(uint32 source)
{
    // https://graphics.stanford.edu/~seander/bithacks.html
    static const uint8 BitsSetTable256[256] =
    {
    #define B2(n) n,     n+1,     n+1,     n+2
    #define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
    #define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
    };
    return BitsSetTable256[source & 0xff] +
        BitsSetTable256[(source >> 8) & 0xff] +
        BitsSetTable256[(source >> 16) & 0xff] +
        BitsSetTable256[source >> 24];
}

static sint32(*bitcount_fn)(uint32);

void bitcount_init()
{
    bitcount_fn = bitcount_popcnt_available() ? bitcount_popcnt : bitcount_lut;
}

sint32 bitcount(uint32 source)
{
    return bitcount_fn(source);
}

bool strequals(const char *a, const char *b, sint32 length, bool caseInsensitive)
{
    return caseInsensitive ?
        _strnicmp(a, b, length) == 0 :
        strncmp(a, b, length) == 0;
}

/* case insensitive compare */
sint32 strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        sint32 d = tolower(*a) - tolower(*b);
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
sint32 strlogicalcmp(char const *a, char const *b) {
    for (;; a++, b++) {
        sint32 result = tolower(*a) - tolower(*b);
        bool both_numeric = *a >= '0' && *a <= '9' && *b >= '0' && *b <= '9';
        if (result != 0 || !*a || both_numeric) { // difference found || end of string
            if (both_numeric) { // a and b both start with a number
                // Get the numbers in the string at current positions
                sint32 na = 0 , nb = 0;
                for (; *a >= '0' && *a <= '9'; a++) { na *= 10; na += *a - '0'; }
                for (; *b >= '0' && *b <= '9'; b++) { nb *= 10; nb += *b - '0'; }
                // In case the numbers are the same
                if (na == nb)
                    continue;
                return na - nb;
            }
            else {
                return result;
            }
        }
    }
}

utf8 * safe_strtrunc(utf8 * text, size_t size)
{
    assert(text != NULL);

    if (size == 0) return text;

    const char *sourceLimit = text + size - 1;
    char *ch = text;
    char *last = text;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(ch, (const utf8 **)&ch)) != 0) {
        if (ch <= sourceLimit) {
            last = ch;
        } else {
            break;
        }
    }
    *last = 0;

    return text;
}

char *safe_strcpy(char * destination, const char * source, size_t size)
{
    assert(destination != NULL);
    assert(source != NULL);

    if (size == 0) return destination;

    char * result = destination;

    bool truncated = false;
    const char *sourceLimit = source + size - 1;
    const char *ch = source;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
        if (ch <= sourceLimit) {
            destination = utf8_write_codepoint(destination, codepoint);
        } else {
            truncated = true;
        }
    }
    *destination = 0;

    if (truncated) {
        log_warning("Truncating string \"%s\" to %d bytes.", result, size);
    }
    return result;
}

char *safe_strcat(char *destination, const char *source, size_t size)
{
    assert(destination != NULL);
    assert(source != NULL);

    if (size == 0) {
        return destination;
    }

    char *result = destination;

    size_t i;
    for (i = 0; i < size; i++) {
        if (*destination == '\0') {
            break;
        } else {
            destination++;
        }
    }

    bool terminated = false;
    for (; i < size; i++) {
        if (*source != '\0') {
            *destination++ = *source++;
        } else {
            *destination = *source;
            terminated = true;
            break;
        }
    }

    if (!terminated) {
        result[size - 1] = '\0';
        log_warning("Truncating string \"%s\" to %d bytes.", result, size);
    }

    return result;
}

char *safe_strcat_path(char *destination, const char *source, size_t size)
{
    path_end_with_separator(destination, size);
    return safe_strcat(destination, source, size);
}

char *safe_strtrimleft(char *destination, const char *source, size_t size)
{
    while (*source == ' ') {
        source++;
    }
    return safe_strcpy(destination, source, size);
}

bool utf8_is_bom(const char *str)
{
    return str[0] == (char)(uint8)0xEF && str[1] == (char)(uint8)0xBB && str[2] == (char)(uint8)0xBF;
}

bool str_is_null_or_empty(const char *str)
{
    return str == NULL || str[0] == 0;
}

uint32 srand0, srand1, srand2, srand3;

void util_srand(sint32 source) {
    srand0 = source;
    srand1 = srand0 ^ (source >> 24);
    srand2 = srand1 ^ (source >> 16);
    srand3 = srand2 ^ (source >> 8);
}

uint32 util_rand() {
    uint32 temp = srand0 ^ (srand0 << 11);
    srand0 = srand1;
    srand1 = srand2;
    srand2 = srand3;
    srand3 = srand3 ^ (srand3 >> 19) ^ temp ^ (temp >> 8);
    return srand3;
}

#define CHUNK 128*1024
#define MAX_ZLIB_REALLOC 4*1024*1024

/**
 * @brief Inflates zlib-compressed data
 * @param data Data to be decompressed
 * @param data_in_size Size of data to be decompressed
 * @param data_out_size Pointer to a variable where output size will be written. If not 0, it will be used to set initial output buffer size.
 * @return Returns a pointer to memory holding decompressed data or NULL on failure.
 * @note It is caller's responsibility to free() the returned pointer once done with it.
 */
uint8 *util_zlib_inflate(uint8 *data, size_t data_in_size, size_t *data_out_size)
{
    sint32 ret = Z_OK;
    uLongf out_size = (uLong)*data_out_size;
    if (out_size == 0)
    {
        // Try to guesstimate the size needed for output data by applying the
        // same ratio it would take to compress data_in_size.
        out_size = (uLong)data_in_size * (uLong)data_in_size / compressBound((uLong)data_in_size);
        out_size = min(MAX_ZLIB_REALLOC, out_size);
    }
    uLongf buffer_size = out_size;
    uint8 *buffer = malloc(buffer_size);
    do {
        if (ret == Z_BUF_ERROR)
        {
            buffer_size *= 2;
            out_size = buffer_size;
            buffer = realloc(buffer, buffer_size);
        } else if (ret == Z_STREAM_ERROR) {
            log_error("Your build is shipped with broken zlib. Please use the official build.");
            free(buffer);
            return NULL;
        } else if (ret < 0) {
            log_error("Error uncompressing data.");
            free(buffer);
            return NULL;
        }
        ret = uncompress(buffer, &out_size, data, (uLong)data_in_size);
    } while (ret != Z_OK);
    buffer = realloc(buffer, out_size);
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
uint8 *util_zlib_deflate(const uint8 *data, size_t data_in_size, size_t *data_out_size)
{
    sint32 ret = Z_OK;
    uLongf out_size = (uLongf)*data_out_size;
    uLong buffer_size = compressBound((uLong)data_in_size);
    uint8 *buffer = malloc(buffer_size);
    do {
        if (ret == Z_BUF_ERROR)
        {
            buffer_size *= 2;
            out_size = buffer_size;
            buffer = realloc(buffer, buffer_size);
        } else if (ret == Z_STREAM_ERROR) {
            log_error("Your build is shipped with broken zlib. Please use the official build.");
            free(buffer);
            return NULL;
        }
        ret = compress(buffer, &out_size, data, (uLong)data_in_size);
    } while (ret != Z_OK);
    *data_out_size = out_size;
    buffer = realloc(buffer, *data_out_size);
    return buffer;
}

// Type-independent code left as macro to reduce duplicate code.
#define add_clamp_body(value, value_to_add, min_cap, max_cap) \
    if ((value_to_add > 0) && (value > (max_cap - value_to_add))) { \
        value = max_cap; \
    } \
    else if ((value_to_add < 0) && (value < (min_cap - value_to_add))) { \
        value = min_cap; \
    } \
    else { \
       value += value_to_add; \
    }

sint8 add_clamp_sint8(sint8 value, sint8 value_to_add)
{
    add_clamp_body(value, value_to_add, INT8_MIN, INT8_MAX);
    return value;
}

sint16 add_clamp_sint16(sint16 value, sint16 value_to_add)
{
    add_clamp_body(value, value_to_add, INT16_MIN, INT16_MAX);
    return value;
}

sint32 add_clamp_sint32(sint32 value, sint32 value_to_add)
{
    add_clamp_body(value, value_to_add, INT32_MIN, INT32_MAX);
    return value;
}

money32 add_clamp_money32(money32 value, money32 value_to_add)
{
    // This function is intended only for clarity, as money32
    // is technically the same as sint32
    assert_struct_size(money32, sizeof(sint32));
    return add_clamp_sint32(value, value_to_add);
}

#undef add_clamp_body

/**
 * strftime wrapper which appends to an existing string.
 */
size_t strcatftime(char * buffer, size_t bufferSize, const char * format, const struct tm * tp)
{
    size_t stringLen = strnlen(buffer, bufferSize);
    if (stringLen < bufferSize) {
        char * dst = buffer + stringLen;
        size_t dstMaxSize = bufferSize - stringLen;
        return strftime(dst, dstMaxSize, format, tp);
    }
    return 0;
}
