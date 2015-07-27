/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef LOCALISATION_H
#define LOCALISATION_H

#include "currency.h"
#include "format_codes.h"
#include "language.h"
#include "string_ids.h"

bool utf8_is_format_code(int codepoint);
bool utf8_should_use_sprite_for_codepoint(int codepoint);
int utf8_get_sprite_offset_for_codepoint(int codepoint);
int utf8_get_format_code_arg_length(int codepoint);

void format_string(char *dest, rct_string_id format, void *args);
void format_string_raw(char *dest, char *src, void *args);
void format_string_to_upper(char *dest, rct_string_id format, void *args);
void generate_string_file();
void error_string_quit(int error, rct_string_id format);
utf8 *get_string_end(const utf8 *text);
size_t get_string_size(const utf8 *text);
int get_string_length(const utf8 *text);

void user_string_clear_all();
rct_string_id user_string_allocate(int base, const char *text);
void user_string_free(rct_string_id id);
bool is_user_string_id(rct_string_id stringId);

utf8 *win1252_to_utf8_alloc(const char *src);
int win1252_to_utf8(utf8string dst, const char *src, int maxBufferLength);

wchar_t encoding_convert_gb2312_to_unicode(wchar_t gb2312);
wchar_t encoding_convert_big5_to_unicode(wchar_t big5);

#define MAX_USER_STRINGS 1024
#define USER_STRING_MAX_LENGTH 32

// Real name data
extern const char real_name_initials[16];
extern const char *real_names[1024];

#endif
