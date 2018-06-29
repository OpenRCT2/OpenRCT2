/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <time.h>
#include "../common.h"

int32_t squaredmetres_to_squaredfeet(int32_t squaredMetres);
int32_t metres_to_feet(int32_t metres);
int32_t mph_to_kmph(int32_t mph);
int32_t mph_to_dmps(int32_t mph);

bool filename_valid_characters(const utf8 *filename);

char *path_get_directory(const utf8 *path);
const char *path_get_filename(const utf8 *path);
const char *path_get_extension(const utf8 *path);
void path_set_extension(utf8 *path, const utf8 *newExtension, size_t size);
void path_append_extension(utf8 *path, const utf8 *newExtension, size_t size);
void path_remove_extension(utf8 *path);
void path_end_with_separator(utf8 *path, size_t size);
bool writeentirefile(const utf8 * path, const void * buffer, size_t length);

bool sse41_available();
bool avx2_available();

int32_t bitscanforward(int32_t source);
void bitcount_init();
int32_t bitcount(uint32_t source);
bool strequals(const char *a, const char *b, int32_t length, bool caseInsensitive);
int32_t strcicmp(char const *a, char const *b);
int32_t strlogicalcmp(char const *a, char const *b);
utf8 * safe_strtrunc(utf8 * text, size_t size);
char *safe_strcpy(char * destination, const char * source, size_t num);
char *safe_strcat(char *destination, const char *source, size_t size);
char *safe_strcat_path(char *destination, const char *source, size_t size);
char *safe_strtrimleft(char *destination, const char *source, size_t size);
#if !(defined(_GNU_SOURCE) || (defined(__DARWIN_C_LEVEL) && __DARWIN_C_LEVEL >= 200809L))
char * strcasestr(const char * haystack, const char * needle);
#endif

bool utf8_is_bom(const char *str);
bool str_is_null_or_empty(const char *str);

void util_srand(int32_t source);
uint32_t util_rand();

uint8_t *util_zlib_deflate(const uint8_t *data, size_t data_in_size, size_t *data_out_size);
uint8_t *util_zlib_inflate(uint8_t *data, size_t data_in_size, size_t *data_out_size);

int8_t add_clamp_int8_t(int8_t value, int8_t value_to_add);
int16_t add_clamp_int16_t(int16_t value, int16_t value_to_add);
int32_t add_clamp_int32_t(int32_t value, int32_t value_to_add);
money32 add_clamp_money32(money32 value, money32 value_to_add);

uint8_t lerp(uint8_t a, uint8_t b, float t);
float flerp(float a, float b, float t);
uint8_t soft_light(uint8_t a, uint8_t b);

size_t strcatftime(char * buffer, size_t bufferSize, const char * format, const struct tm * tp);

#endif
