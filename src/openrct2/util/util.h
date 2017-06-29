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

#ifndef _UTIL_H_
#define _UTIL_H_

#include <time.h>
#include "../common.h"
#include "../park_load_result_types.h"

sint32 squaredmetres_to_squaredfeet(sint32 squaredMetres);
sint32 metres_to_feet(sint32 metres);
sint32 mph_to_kmph(sint32 mph);
sint32 mph_to_dmps(sint32 mph);

bool filename_valid_characters(const utf8 *filename);

char *path_get_directory(const utf8 *path);
const char *path_get_filename(const utf8 *path);
const char *path_get_extension(const utf8 *path);
void path_set_extension(utf8 *path, const utf8 *newExtension, size_t size);
void path_append_extension(utf8 *path, const utf8 *newExtension, size_t size);
void path_remove_extension(utf8 *path);
void path_end_with_separator(utf8 *path, size_t size);
bool readentirefile(const utf8 *path, void **outBuffer, size_t *outLength);
bool writeentirefile(const utf8 * path, const void * buffer, size_t length);

sint32 bitscanforward(sint32 source);
void bitcount_init();
sint32 bitcount(uint32 source);
bool strequals(const char *a, const char *b, sint32 length, bool caseInsensitive);
sint32 strcicmp(char const *a, char const *b);
sint32 strlogicalcmp(char const *a, char const *b);
utf8 * safe_strtrunc(utf8 * text, size_t size);
char *safe_strcpy(char * destination, const char * source, size_t num);
char *safe_strcat(char *destination, const char *source, size_t size);
char *safe_strcat_path(char *destination, const char *source, size_t size);
char *safe_strtrimleft(char *destination, const char *source, size_t size);

bool utf8_is_bom(const char *str);
bool str_is_null_or_empty(const char *str);

void util_srand(sint32 source);
uint32 util_rand();

uint8 *util_zlib_deflate(const uint8 *data, size_t data_in_size, size_t *data_out_size);
uint8 *util_zlib_inflate(uint8 *data, size_t data_in_size, size_t *data_out_size);

sint8 add_clamp_sint8(sint8 value, sint8 value_to_add);
sint16 add_clamp_sint16(sint16 value, sint16 value_to_add);
sint32 add_clamp_sint32(sint32 value, sint32 value_to_add);
money32 add_clamp_money32(money32 value, money32 value_to_add);

size_t strcatftime(char * buffer, size_t bufferSize, const char * format, const struct tm * tp);

void handle_park_load_failure(park_load_result* result, const utf8* path);
#endif
